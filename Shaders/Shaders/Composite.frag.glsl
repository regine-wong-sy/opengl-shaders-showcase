R"(
    #version 420

    // Final composite pass: blends the cartoon-shaded render (Texture0) with
    // the Sobel edge map (Texture1). Edge pixels (white in the edge map) are
    // darkened towards black; the strength is controlled by AlphaFactor.

    layout(binding=4) uniform sampler2D Texture0;  // Cartoon render
    layout(binding=3) uniform sampler2D Texture1;  // Edge map (white=edge, black=no edge)

    uniform float AlphaFactor = 0.0f;  // 0 = full base, 1 = white background with black edges only
    uniform bool BlurEnabled = false;
    uniform bool EdgeEnabled = false;

    layout(location=0) out vec4 FragColor;

    // Same Gaussian weights used by the blur passes
    float w[5] = float[5](0.158435, 0.148836, 0.123389, 0.0902733, 0.0582848);

    // 2D separable Gaussian blur applied inline to the cartoon texture
    vec4 blurCartoon(ivec2 pix)
    {
        vec4 sum = vec4(0.0);
        for (int y = -4; y <= 4; y++)
            for (int x = -4; x <= 4; x++)
                sum += texelFetch(Texture0, pix + ivec2(x, y), 0) * w[abs(x)] * w[abs(y)];
        return sum;
    }

    void main()
    {
        ivec2 pix    = ivec2(gl_FragCoord.xy);
        vec4 cartoon = texelFetch(Texture0, pix, 0);
        vec4 edge    = texelFetch(Texture1, pix, 0);

        vec4 base = (BlurEnabled && !EdgeEnabled) ? blurCartoon(pix) : cartoon;

        vec4 background = mix(base, vec4(1.0, 1.0, 1.0, 1.0), AlphaFactor);

        FragColor = mix(background, vec4(0.0, 0.0, 0.0, 1.0), edge.r);
    }
)"
