R"(
    #version 330 core

    // Cartoon (cel-shaded) fragment shader. Quantizes the diffuse lighting
    // into discrete bands to produce a flat, 2D-animation style appearance.
    // Uses the same Blinn-Phong Light/Material layout as PhongShading so the
    // framework can bind the same uniforms for both passes.

    struct Light
    {
        vec3 position;      // Position in world space
        vec3 La;            // Ambient intensity
        vec3 Ld;            // Diffuse intensity
        vec3 Ls;            // Specular intensity (unused here)
    };

    struct Material
    {
        vec3 Ka;            // Ambient reflectivity
        vec3 Kd;            // Diffuse reflectivity
        vec3 Ks;            // Specular reflectivity (unused here)
        float shininess;    // Shininess (unused here)
    };

    in vec3 Position;   // Fragment position in view space
    in vec3 Normal;     // Fragment normal in view space

    uniform Light light[1];
    uniform Material material;
    uniform mat4 V;     // View matrix (to transform light position)

    layout(location=0) out vec4 FragColor;

    // Quantises a [0,1] diffuse coefficient into a fixed number of shading bands.
    float cartoon(float value)
    {
        float levels = 2.0f;
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

        float cos2   = max(dot(H, N), 0.0);
        vec3 spec    = light.Ls * material.Ks * cartoon(pow(cos2, material.shininess))* attenuation;

        return ambient + diffuse + spec;

    }

    void main()
    {
        vec3 color = blinnPhongCartoon(Position, Normal, light[0], material, V);
        FragColor = vec4(color, 1.0);
    }
)"
