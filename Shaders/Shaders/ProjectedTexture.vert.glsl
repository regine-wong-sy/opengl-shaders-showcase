R"(
    #version 330 core

    // The shader transforms vertex positions and normals to view space, passes texture 
    // coordinates, and calculates projected texture coordinates for use in the fragment shader.

    layout(location=0) in vec3 VertexPosition;
    layout(location=1) in vec3 VertexNormal;
    layout(location=2) in vec2 VertexTexCoord;

    uniform mat4 M;     // Model
    uniform mat4 V;     // View
    uniform mat4 P;     // Projection

    out vec3 Position;  // Position in view space
    out vec3 Normal;    // Normal in view space
    out vec2 TexCoord;  // Texture coordinates
        
    uniform mat4 ProjectorMatrix; // Projection matrix for the projector
    out vec4 ProjTexCoord;  // Texture coordinates for the projected texture

    void main()
    {
        mat4 MV = V * M; // Model-View transform matrix
        mat3 N = mat3(transpose(inverse(MV))); // Normal transform matrix

        Normal = normalize(N * VertexNormal);

        vec4 VertexPositionInView = MV * vec4(VertexPosition, 1.0f);
        Position = vec3(VertexPositionInView);

        TexCoord = VertexTexCoord;

        gl_Position = P * VertexPositionInView;

        ProjTexCoord = ProjectorMatrix * M * vec4(VertexPosition, 1.0f);
    }
)"