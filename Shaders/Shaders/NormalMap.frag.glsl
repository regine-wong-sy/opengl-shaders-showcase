R"(
    #version 420

    // This fragment shader implements normal mapping using 
    // the Blinn-Phong illumination model.

    in vec3 LightDirection;
    in vec3 ViewDirection;
    in vec2 TextureCoordinates;

    layout(location=0) out vec4 FragColor;

    layout(binding=0) uniform sampler2D ColorTex;
    layout(binding=1) uniform sampler2D NormalMapTex;

    struct Material 
    {
        vec3 Ka;            // Ambient reflectivity
        vec3 Kd;            // Diffuse reflectivity
        vec3 Ks;            // Specular reflectivity
        float shininess;    // Specular shininess factor
    };

    struct Light
    {
        vec3 position;      // Position of the light source in the world space
        vec3 La;            // Ambient light intensity
        vec3 Ld;            // Diffuse light intensity
        vec3 Ls;            // Specular light intensity
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
        vec3 normal = vec3(texture(NormalMapTex, TextureCoordinates));
        normal.xy = 2.0f * normal.xy - 1.0f;

        vec3 color = vec3(texture(ColorTex, TextureCoordinates));

        // Shade the color using the Blinn-Phong illumination model
        color = blinnPhong(color, normal, normalize(LightDirection), 
                                          normalize(ViewDirection));

        // Set with the gamma correction
        FragColor = vec4(pow(color, vec3(1.0f/2.2f)), 1.0f);
    }
)"