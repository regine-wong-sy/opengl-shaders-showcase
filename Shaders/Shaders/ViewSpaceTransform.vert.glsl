R"(
    #version 330 core

    /*
       Transforms position and normal vectors into view space and passes them, 
       along with texture coordinates, to the fragment shader. 
       It uses the inverse-transpose of the normal matrix to correctly handle 
       cases involving non-uniform scaling during transformation.
    */

    layout(location=0) in vec3 VertexPosition;
    layout(location=1) in vec3 VertexNormal;
    layout(location=2) in vec2 VertexTexCoord;
    layout(location=3) in vec4 VertexTangent;  // w = handedness (+1 or -1)

    uniform mat4 M; // Model
    uniform mat4 V; // View
    uniform mat4 P; // Projection

    out vec3 Position;  // Position in view space
    out vec3 Normal;    // Normal in view space
    out vec2 TexCoord;  // Texture coordinates
    out vec3 Tangent;   // Tangent in view space
    out vec3 Bitangent; // Bitangent in view space

    void main()
    {
        mat4 MV = V * M; // Model-View transform
        mat3 N = mat3(transpose(inverse(MV))); // Normal transform matrix

        Normal    = normalize(N * VertexNormal);
        Tangent   = normalize(N * vec3(VertexTangent));
        Bitangent = normalize(cross(Normal, Tangent) * VertexTangent.w);

        vec4 VertexPositionInView = MV * vec4(VertexPosition, 1.0f);

        Position = vec3(VertexPositionInView);

        TexCoord = VertexTexCoord;

        gl_Position = P * VertexPositionInView;
    }
)"