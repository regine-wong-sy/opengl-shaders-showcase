R"(
    #version 430 core

    layout(local_size_x = 1, local_size_y = 1) in;

    layout(std430, binding = 0) buffer RayDistances {
        float distances[];
    };

    layout(std430, binding = 1) buffer RayTextureCoordinates {
        float coordinates[];
    };

    layout(std430, binding = 2) buffer RayTextureIds {
        int ids[];
    };

    layout(std430, binding = 3) buffer WorldMap {
        int map[];
    };

    uniform vec2 playerPos;
    uniform vec2 playerDir;
    uniform int mapWidth;
    uniform int mapHeight;
    uniform int numRays;

    void main() 
    {
        uint rayIndex = gl_GlobalInvocationID.x;

        // Calculate ray direction based on ray index
        float x = (rayIndex * 2.0f - numRays) / numRays;
        vec2 target = playerDir + vec2(-playerDir.y, playerDir.x) * x;
        vec2 rayDir = normalize(target);
        vec2 rayPos = playerPos;
    
        float distance = 0.0f;
        int id = 0;     // Wall/texture id
        const float maxDistance = 20.0f;
        const float stepSize = 0.001f;
    
        // DDA-like algorithm
        while (true) 
        {
            // Step along the ray; increment distance
            rayPos += rayDir * stepSize;
            distance += stepSize;
        
            // Map coordinates
            int mapX = int(floor(rayPos.x));
            int mapY = int(floor(rayPos.y));
        
            // Check for out of bounds
            if (mapX < 0 || mapX >= mapWidth || mapY < 0 
                || mapY >= mapHeight || distance >= maxDistance) 
            {
                distance = maxDistance;
                break;
            }
        
            if (map[mapY * mapWidth + mapX] != 0) // Hit a wall
            {
                id = map[mapY * mapWidth + mapX];
                break;
            }
        }
    
        // Fix fish-eye effect
        distances[rayIndex] = distance * dot(playerDir, rayDir);

        // Calculate texture coordinates
        coordinates[rayIndex] = clamp(fract(rayPos.x + rayPos.y), 0.0f, 0.9999f);

        // Set texture id
        ids[rayIndex] = id;
    }
)"