R"(
    #version 420

    // Fragment shader: portal/dimension shader adapted from Shadertoy.
    // UVs come from the vertex shader (TexCoord) so the effect maps
    // onto the quad's surface in 3D world space.

    in vec2 TexCoord;

    layout(location=0) out vec4 FragColor;

    uniform float iTime;

    // Applies smooth displacement to the circumference of the circle.
    float variation(vec2 v1, vec2 v2, float strength, float speed)
    {
        return sin(
            dot(normalize(v1), normalize(v2)) * strength + iTime * speed
        ) / 100.;
    }

    // Draws a circle with smooth variation to its circumference over time.
    vec3 paintCircle(vec2 uv, vec2 center, float rad, float width, float index)
    {
        vec2  diff  = center - uv;
        float len   = length(diff);
        float scale = rad;
        float mult  = mod(index, 2.0) == 0.0 ? 1.0 : -1.0;

        len += variation(diff, vec2(rad * mult, 1.0), 7.0 * scale, 2.0);
        len -= variation(diff, vec2(1.0, rad * mult), 7.0 * scale, 2.0);

        float circle = smoothstep((rad - width) * scale, rad * scale, len)
                     - smoothstep(rad * scale, (rad + width) * scale, len);
        return vec3(circle);
    }

    // Smooth RGB rainbow using cosine palette — t in [0,1] cycles through all hues.
    vec3 rainbow(float t)
    {
        return 0.5 + 0.5 * cos(6.28318 * (t + vec3(0.0, 0.333, 0.667)));
    }

    // A ring = wider faded circle + bright inner circle (grayscale, tinted in main).
    vec3 paintRing(vec2 uv, vec2 center, float radius, float index)
    {
        vec3 color  = paintCircle(uv, center, radius, 0.075, index);
        color      += paintCircle(uv, center, radius, 0.015, index);
        return color;
    }

    void main()
    {
        // Use the quad's texture coordinates [0,1] as the 2D UV for the portal effect
        vec2 uv = TexCoord;

        const float numRings         = 20.0;
        const vec2  center           = vec2(0.5);
        const float spacing          = 1.0 / numRings;
        const float slow             = 30.0;
        const float cycleDur         = 1.0;
        const float tunnelElongation = 0.25;

        float radius = mod(iTime / slow, cycleDur);
        vec3  color  = vec3(0.0);

        // Smooth black border around edges
        float border = 0.25;
        vec2  bl     = smoothstep(0.0, border, uv);
        vec2  tr     = smoothstep(0.0, border, 1.0 - uv);
        float edges  = bl.x * bl.y * tr.x * tr.y;

        // Push in the sides to make the warp square
        uv.x *= 1.5;
        uv.x -= 0.25;

        for (float i = 0.0; i < numRings; i++)
        {
            float r = mod(radius + i * spacing, cycleDur);
            color += paintRing(uv, center, tunnelElongation * log(r), i);
            color += paintRing(uv, center, log(r), i);
        }

        color = mix(color, vec3(0.0), 1.0 - edges);
        color = mix(color, vec3(0.0), distance(uv, center));

        // Start at the original blue-cyan, then cycling RGB
        vec3 originalColor = vec3(0.3, 0.85, 1.0);
        float blend = smoothstep(0.0, 5.0, iTime); // transition over 5 seconds
        color *= mix(originalColor, rainbow(iTime * 0.1), blend);

        FragColor = vec4(color, 1.0);
    }
)"
