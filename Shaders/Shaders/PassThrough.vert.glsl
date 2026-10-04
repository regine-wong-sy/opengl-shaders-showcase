R"(
    #version 330 core

    // This is the simplest vertex shader: it takes vertex positions and 
    // passes them to the GPU pipeline without transformations (no model, 
    // view, or projection matrices applied).

    layout(location=0) in vec3 VertexPosition;

    void main() 
    {
        gl_Position = vec4(VertexPosition.x, VertexPosition.y, 0.0f, 1.0f);
    }
)"