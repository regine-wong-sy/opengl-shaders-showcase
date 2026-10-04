R"(
    #version 420

    // This is a fragment shader that implements the second pass of a two-pass 
    // Gaussian blur filter. It applies a horizontal blur by sampling pixels 
    // along the X-axis using the same precomputed Gaussian weights as the 
    // first pass.

    layout(location=0) out vec4 FragColor;

    //layout(binding=0) uniform sampler2D Texture0;
    layout(binding=1) uniform sampler2D Texture1;
    uniform bool BlurEnabled = false;

    // Precomputed normalized Gaussian weights. 
    // We need only half of all weights because they are symmetric about zero.
    float Weight[5] = float[5]( 0.158435f, 0.148836f, 0.123389f, 
                                0.0902733f, 0.0582848f );
   

    // Applies the weighted horizontally sum of the Gaussian blur filter 
    // to store the result in yet another texture. 
    void main() 
    {
        ivec2 pix = ivec2(gl_FragCoord.xy);
        if (!BlurEnabled)
        {
            FragColor = texelFetch(Texture1, pix, 0);
            return;
        }
        vec4 sum = texelFetch(Texture1, pix, 0);
        
        sum *= Weight[0];
        sum += texelFetchOffset(Texture1, pix, 0, ivec2(-4,0)) * Weight[4];
        sum += texelFetchOffset(Texture1, pix, 0, ivec2(-3,0)) * Weight[3];
        sum += texelFetchOffset(Texture1, pix, 0, ivec2(-2,0)) * Weight[2];
        sum += texelFetchOffset(Texture1, pix, 0, ivec2(-1,0)) * Weight[1];
        sum += texelFetchOffset(Texture1, pix, 0, ivec2( 1,0)) * Weight[1];
        sum += texelFetchOffset(Texture1, pix, 0, ivec2( 2,0)) * Weight[2];
        sum += texelFetchOffset(Texture1, pix, 0, ivec2( 3,0)) * Weight[3];
        sum += texelFetchOffset(Texture1, pix, 0, ivec2( 4,0)) * Weight[4];

        FragColor = sum;
    }
)"