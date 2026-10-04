R"(
    #version 330 core

    struct Light
    {
        vec3 position;      // Position of the light source in the world space
    
        vec3 L;             // Light intensity (for PBR)

        vec3 La;            // Ambient light intensity (for Phong)
        vec3 Ld;            // Diffuse light intensity (for Phong)
        vec3 Ls;            // Specular light intensity (for Phong)
    };

    struct Material
    {
         // For PBR
        float rough;        // Roughness
        float metal;        // Metallic (1.0f) or dielectric (0.0f)
        vec3 color;         // Diffuse color for metal

        // For Phong
        vec3 Ka;            // Ambient reflectivity
        vec3 Kd;            // Diffuse reflectivity
        vec3 Ks;            // Specular reflectivity
        float shininess;    // Specular shininess factor

        float effect;
    };

    struct Fog
    {
        float minDist;
        float maxDist;
        vec4 color;
    };

    in vec3 Position;
    in vec3 Normal;
    in vec2 TexCoord;

    uniform Light light[1];
    uniform Material material;
    uniform Fog fog;
    uniform mat4 V;         // View transform matrix

    layout(location=0) out vec4 FragColor;

    const float PI = 3.14159265358979323846;

    //
    // The Microgeometry Normal Distribution Function, based on GGX/Trowbrodge-Reitz,
    // that describes the relative concentration of microfacet normals
    // in the direction H. It has an effect on the size and shape
    // of the specular highlight.
    //
    // Parameter is cosine of the angle between the normal and H which is the halfway vector of
    // both the light direction and the view direction
    //
    float ggxDistribution(float nDotH)
    {
        float alpha2 = material.rough * material.rough * material.rough * material.rough;
        float d = (nDotH * nDotH) * (alpha2 - 1.0f) + 1.0f;
        return alpha2 / (PI * d * d);
    }

    //
    // The Smith Masking-Shadowing Function describes the probability that microfacets with
    // a given normal are visible from both the light direction and the view direction.
    //
    // Parameter is cosine of the angle between the normal vector and the view direction.
    //
    float geomSmith(float nDotL)
    {
        float k = (material.rough + 1.0f) * (material.rough + 1.0f) / 8.0f;
        float denom = nDotL * (1.0f - k) + k;
        return 1.0f / denom;
    }

    //
    // Schlick approximation for Fresnel reflection that defines the probability of light
    // reflected from an optically flat surface.
    //
    // Parameter is cosine of the angle between the light direction vector and
    // the halfway vector of both the light direction and the view direction
    //
    vec3 schlickFresnel(float lDotH)
    {
        vec3 f0 = vec3(0.04f); // Dielectrics
        if (material.metal == 1.0f)
            f0 = material.color;
        return f0 + (1.0f - f0) * pow(1.0f - lDotH, 5);
    }

    //
    // Bidirectional Reflectance Distribution Function.
    // This is the common way to model reflectance based on microfacet theory.
    // This theory was developed to describe reflection from general, non-optically flat surfaces.
    // It models the surface as consisting of small facets that are optically flat (mirrors) and
    // are oriented in various directions. Only those that are oriented correctly to reflect toward
    // the viewer can contribute.
    //
    // Parameters are the position of a fragment and the surface normal in the view space.
    //
    vec3 microfacetModel(vec3 position, vec3 n)
    {
        vec3 diffuseBrdf = material.color;

        vec3 lightI = light[0].L;
        vec3 lightPositionInView = (V * vec4(light[0].position, 1.0f)).xyz;

        vec3 l = lightPositionInView - position;
        float dist = length(l);
        l = normalize(l);
        lightI *= 100 / (dist * dist); // Intensity is normalized, so scale up by 100 first

        vec3 v = normalize(-position);
        vec3 h = normalize(v + l);
        float nDotH = dot(n, h);
        float lDotH = dot(l, h);
        float nDotL = max(dot(n, l), 0.0f);
        float nDotV = dot(n, v);
        vec3 specBrdf = 0.25f * ggxDistribution(nDotH) * schlickFresnel(lDotH) *
                           geomSmith(nDotL) * geomSmith(nDotV);

        return (diffuseBrdf + PI * specBrdf) * lightI * nDotL;
    }

    // Cartoon shading quantization function
    float cartoon(float value)
    {
        float levels = 3.0f;
        float scaleFactor = 1.0f / levels;
        return floor(value * levels) * scaleFactor;
    }

    
      vec3 blinnPhongCartoon(vec3 position, vec3 normal, Light light, Material material, mat4 view)
    {
        vec3 light_toview = vec3(view * vec4(light.position, 1.0));

        vec3 lightVec = light_toview - position; 
        float dist = length(lightVec);
        float attenuation = 16.0f / (dist * dist);
        vec3 toLight = normalize(lightVec);


        vec3 V = normalize(-position);
        vec3 N = normalize(normal);
        vec3 H = normalize(toLight + V);

        vec3 ambient = light.La * material.Ka;

        float cos1   = max(dot(toLight, N), 0.0);
        vec3 diffuse = light.Ld * material.Kd * cartoon(cos1) * 2.0 * attenuation;

        //float cos2   = max(dot(H, N), 0.0);
        //vec3 spec    = light.Ls * material.Ks * cartoon(pow(cos2, material.shininess))* attenuation;

        return ambient + diffuse;

    }
    vec4 checkerboardTexture(vec2 uv, float size)
    {
        uv = uv * size;
        float color = mod(floor(uv.x) + floor(uv.y), 2.0);
        return vec4(color, color, color, 1.0);
    }

    float computeFog(float dist)
    {
        return clamp((dist - fog.minDist) / (fog.maxDist - fog.minDist), 0.0, 1.0);
    }

    void main()
    {
        float dist  = length(Position);
        vec3 color  = vec3(0.0);

        if (material.effect == 1.0f)
        {
            //look like cheese hahah
            // discard fragments inside each circle
            vec2 scaledUV = TexCoord * 5.0;

            // offset odd rows by 0.5 so pattern looking
            float row = floor(scaledUV.y);
            vec2 staggeredUV = scaledUV + vec2(mod(row, 2.0) * 0.5, 0.0);
            vec2 localPos = mod(staggeredUV, 1.0) - 0.5; // centre of cell, wrap 0,1

            // discard if inside the circle
            if (length(localPos) < 0.3)
                discard;

            // Probably need to inverse the normal here if not gl_FrontFacing
            vec3 n = gl_FrontFacing ? normalize(Normal) : normalize(-Normal);
            color  = microfacetModel(Position, n);
        }
        else if (material.effect == 2.0f)
        {
            // Silhouette outline: if N dot V is near 0 the fragment is at a silhouette edge
            vec3 N       = normalize(Normal);
            vec3 viewDir = normalize(-Position);
           
            color = blinnPhongCartoon(Position, N, light[0], material, V);
        }

        else if (material.effect == 3.0f)
        {
            vec3 shade   = microfacetModel(Position, normalize(Normal));
            vec3 texColor = checkerboardTexture(TexCoord, 50.0f).rgb * 0.7 + vec3(0.30);
            color = mix(shade, texColor, 0.5f);
        }
        else
        {
            color = microfacetModel(Position, normalize(Normal));
        }

        // Gamma correction
        color = pow(color, vec3(1.0f / 2.2f));

        FragColor = mix(vec4(color, 1.0), fog.color, computeFog(dist));
    }
)"
