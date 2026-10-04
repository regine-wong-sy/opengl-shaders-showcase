R"(
    #version 330 core

    // This fragment shader computes per‑pixel lighting using the Phong 
    // illumination model. It takes a single light source and material 
    // properties, transforms lighting into view space, and outputs ambient, 
    // diffuse, and specular contributions for realistic shaded rendering.

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

    in vec3 Position;       // In view space
    in vec3 Normal;         // In view space

    uniform Light light[1];
    uniform Material material;
    uniform mat4 V;         // View transform matrix

    layout(location=0) out vec4 FragColor;

vec3 phong(vec3 position, vec3 normal, Light light, Material material, mat4 view)
{
    // light position to view space
    vec3 light_toview = vec3(view * vec4(light.position, 1.0));
    
    
    vec3 L = normalize(light_toview - position);  // Light direction
    vec3 V = normalize(-position);                  // View direction
    vec3 N = normalize(normal);                     // Normal
    
    //product of Ia  and ka
    vec3 ambient = light.La * material.Ka;
    
    // Diffuse component
    // D = I * cos θ
    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = light.Ld * material.Kd * NdotL;
    
    // Specular Light Contribution
    // bling-phong
    vec3 specular = vec3(0.0);
    if (NdotL > 0.0) {
        vec3 H = normalize(L + V);
        float NdotH = max(dot(N, H), 0.0);
        specular = light.Ls * material.Ks * pow(NdotH, material.shininess);
    }
    
    return ambient + diffuse + specular;
}

    void main() 
    {
        FragColor = vec4(phong(Position, normalize(Normal), light[0], material, V), 1.0f);
    }
)"