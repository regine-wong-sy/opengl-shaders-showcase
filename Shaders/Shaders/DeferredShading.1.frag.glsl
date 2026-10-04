R"(
    #version 420

    // This fragment shader implements the second (lighting) pass for deferred
    // shading that applies lighting calculations using the stored surface data

    layout(location=0) out vec4 FragColor;

    layout(binding=0) uniform sampler2D gLight;
    layout(binding=1) uniform sampler2D gView;
    layout(binding=2) uniform sampler2D gTexCoord;
    layout(binding=3) uniform sampler2D ColorTex;
    layout(binding=4) uniform sampler2D NormalMapTex;

    struct Light
    {
        vec3 position;      // Position of the light source in the world space
        vec3 La;            // Ambient light intensity
        vec3 Ld;            // Diffuse light intensity
        vec3 Ls;            // Specular light intensity
    };

    struct Material
    {
        vec3 Ka;            // Ambient reflectivity
        vec3 Kd;            // Diffuse reflectivity
        vec3 Ks;            // Specular reflectivity
        float shininess;    // Specular shininess factor
    };

    uniform Material material;
    uniform Light light[1];

    uniform int Option = 1;

    // Blinn-Phong illumination with light and view direction vectors given as parameters.
    // All vector parameters must be normalized.
    vec3 blinnPhong(vec3 color, vec3 normal, vec3 lightDir, vec3 viewDir)
    {
        vec3 ambient = light[0].La * material.Ka * color;

        float lDotN = max(dot(lightDir, normal), 0.0f);
        vec3 diffuse = color * light[0].Ld * material.Kd * lDotN;

        vec3 spec = vec3(0.0f);
        if (lDotN > 0.0f)
        {
            vec3 halfway = normalize(viewDir + lightDir);
            spec = light[0].Ls * material.Ks *
                     pow(max(dot(halfway, normal), 0.0f), material.shininess);
        }
        return ambient + diffuse + spec;
    }

    void main()
    {
        vec3 lightDir = vec3(texelFetch(gLight,    ivec2(gl_FragCoord), 0));
        vec3 viewDir  = vec3(texelFetch(gView,     ivec2(gl_FragCoord), 0));
        vec2 uv       = vec2(texelFetch(gTexCoord, ivec2(gl_FragCoord), 0));

        if (Option == 2)
        {
            // Visualise tangent-space light direction remapped to [0,1]
            // Background pixels have lightDir==vec3(0), so 0*0.5+0.5 = grey
            FragColor = vec4(lightDir * 0.5f + 0.5f, 1.0f);
        }
        else if (Option == 3)
        {
            FragColor = vec4(viewDir * 0.5f + 0.5f, 1.0f);
        }
        else
        {
            // Blinn-Phong shading function for deferred shading with 
            // additional check to handle background pixels with zero normals
            if (any(notEqual(lightDir, vec3(0.0f))))
            {
                vec3 normal = vec3(texture(NormalMapTex, uv));
                normal.xy = 2.0f * normal.xy - 1.0f;

                vec3 color = vec3(texture(ColorTex, uv));
                color = blinnPhong(color, normal, lightDir, viewDir);

                FragColor = vec4(pow(color, vec3(1.0f / 2.2f)), 1.0f);
            }
            else
            {
                FragColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
            }
        }
    }
)"
