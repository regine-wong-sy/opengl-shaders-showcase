R"(
    #version 430 core

    layout(std430, binding = 0) buffer RayDistances {
        float distances[];
    };

    layout(std430, binding = 1) buffer RayTextureCoordinates {
        float coordinates[];
    };

    layout(std430, binding = 2) buffer RayTextureIds {
        int ids[];
    };

    uniform vec2 iResolution;

    out vec4 FragColor;
    vec4 checkerboardTexture(vec2 uv, int size)
    {
        uv = uv * size;
        float color = mod(floor(uv.x) + floor(uv.y), 2.0f) /5.0f;
        return vec4(color, color, color, 1.0);
    }

    void main() 
    {  
        float dist = distances[int(gl_FragCoord.x)];    // distance to wall
        int id = ids[int(gl_FragCoord.x)];              // texture id
        float u = coordinates[int(gl_FragCoord.x)];     // u texture coordinate


        // wall height distance
        // farther, smaller wall
        float wallHeight = iResolution.y / max(dist, 0.1f);
        
        float wallBottom = (iResolution.y - wallHeight) / 2.0f;
        float wallTop = wallBottom + wallHeight;
    
       if (gl_FragCoord.y < wallBottom) // with reflection
        {
            vec3 floorColor = vec3(0.6f, 0.6f, 0.6f);
            
            // reflection
            float reflection = wallBottom - gl_FragCoord.y;
            
            // smooth fade
            float range = wallHeight * 2.0;
            float fade = clamp(1.0 - (reflection / range), 0.0, 1.0);
            fade *= 0.15; // reflection strength
            
            vec3 reflectioncolor = vec3(0.15, 0.15, 0.15);
            
            // blend floor with reflection
            FragColor = vec4(mix(floorColor, reflectioncolor, fade), 1.0);
        } 

        else if (gl_FragCoord.y < wallTop) // Wall
        {
            // vertical texture coordinate
            float v = (gl_FragCoord.y - wallBottom) / wallHeight;
            
            // uv coordinates checkerboard
            vec2 walluv = vec2(u, v);

            vec4 checkerboard;
            if (id == 1) {
                checkerboard = checkerboardTexture(walluv, 2); // brown
            } else if(id == 2) {
                checkerboard = checkerboardTexture(walluv, 10); // red
            } else if(id == 3) {
                checkerboard = checkerboardTexture(walluv, 6); // green
            } else if(id == 4) {
                checkerboard = checkerboardTexture(walluv, 8); // blue
            } else {
                checkerboard = checkerboardTexture(walluv, 8); // gray
            }



            // wall colors
            vec3 wallcolor;
            if (id == 1) {
                wallcolor = vec3(0.6, 0.4, 0.2); // brown
            } else if (id == 2) {
                wallcolor = vec3(0.8, 0.2, 0.2); // red
            } else if (id == 3) {
                wallcolor = vec3(0.2, 0.8, 0.2); // green
            } else if (id == 4) {
                wallcolor = vec3(0.2, 0.2, 0.8); // blue
            } else if (id == 5) {
                wallcolor = vec3(0.5, 0.5, 0.5); // gray 
            } else {
                wallcolor = vec3(0.5, 0.5, 0.5); // default gray
            }
            
            // multiply wall color by checkerboard pattern
            vec3 finalwall = wallcolor * (0.5 + checkerboard.rgb);
            
            float brightness = clamp(1.0f / (1.0f + dist * 0.2f), 0.0f, 1.0f);
            FragColor = vec4(finalwall * brightness, 1.0);
        }
        else // Ceiling
        {
             FragColor = vec4(0.9f, 0.9f, 0.7f, 1.0f);
        }
    }
)"