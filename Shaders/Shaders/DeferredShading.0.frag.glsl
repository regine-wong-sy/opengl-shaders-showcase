R"(
    #version 420 core

    // This fragment shader implements the first (geometry) pass for deferred 
    // shading that stores G-buffer data: position and normalized normal vector

    in vec3 LightDirection;
    in vec3 ViewDirection;
    in vec2 TexCoord;

    layout(location=1) out vec3 gLight;
    layout(location=2) out vec3 gView;
    layout(location=3) out vec2 gTexCoord;

    void main()
    {
        gLight    = normalize(LightDirection);
        gView     = normalize(ViewDirection);
        gTexCoord = TexCoord;
    }
)"
