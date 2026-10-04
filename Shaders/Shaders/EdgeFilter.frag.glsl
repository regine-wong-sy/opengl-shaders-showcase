R"(
    #version 420

    // This is a fragment shader that implements edge detection using the Sobel
    // operator for post-processing effects. It analyzes a rendered texture 
    // from a previous pass to identify edges by detecting significant 
    // luminance changes between neighboring pixels.

    layout(location=0) out vec4 FragColor;

    // Texture from the first pass
    layout(binding=2) uniform sampler2D Texture;

    // Parameters
    uniform float EdgeThreshold = 0.05f;
    uniform bool EdgeEnabled = false;

    float luminance(vec3 color) 
    {
        return dot(vec3(0.2126f, 0.7152f, 0.0722f), color);
    }

    void main()
    {
        ivec2 pix = ivec2(gl_FragCoord.xy);
        if (!EdgeEnabled)
        {
            FragColor = vec4(0.0, 0.0, 0.0, 1.0);
            return;
        }
        float s00 = luminance(texelFetchOffset(Texture, pix, 0, ivec2(-1, 1)).rgb);
        float s10 = luminance(texelFetchOffset(Texture, pix, 0, ivec2(-1, 0)).rgb);
        float s20 = luminance(texelFetchOffset(Texture, pix, 0, ivec2(-1,-1)).rgb);
        float s01 = luminance(texelFetchOffset(Texture, pix, 0, ivec2( 0, 1)).rgb);
        float s21 = luminance(texelFetchOffset(Texture, pix, 0, ivec2( 0,-1)).rgb);
        float s02 = luminance(texelFetchOffset(Texture, pix, 0, ivec2( 1, 1)).rgb);
        float s12 = luminance(texelFetchOffset(Texture, pix, 0, ivec2( 1, 0)).rgb);
        float s22 = luminance(texelFetchOffset(Texture, pix, 0, ivec2( 1,-1)).rgb);

        float sx = s00 + 2*s10 + s20 - (s02 + 2*s12 + s22);
        float sy = s00 + 2*s01 + s02 - (s20 + 2*s21 + s22);

        float g = sx*sx + sy*sy;

        FragColor = (g > EdgeThreshold) ? vec4(1.0f) : vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
)"