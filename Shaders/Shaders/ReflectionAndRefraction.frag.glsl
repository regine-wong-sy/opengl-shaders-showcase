R"(
    #version 420

    // Given per-fragment reflection and refraction direction vectors (in world 
    // space), this shader samples a cubemap for each and linearly blends the 
    // two using a material reflection factor. It applies gamma correction and 
    // outputs an RGBA color

    // Material properties for reflection and refraction
    struct Material 
    {
        vec3 color;             
        float reflectionFactor; // The light reflection factor
        float eta;              // The ratio of indices of refraction
    };

    layout(binding=0) uniform samplerCube CubeMapTex;

    layout(location=0) out vec4 FragColor;

    in vec3 ReflectDir;
    in vec3 RefractDir;

    uniform Material material;

    //from shader ass 1 lol
    vec4 checkerboardTexture(vec2 uv, float size)
    {
         uv = uv * size;
         float color = mod(floor(uv.x) + floor(uv.y), 2.0f);
   
    
         return vec4(color, color, color, 1.0);
    }


 
    vec2 vec2uv(vec3 v)
    {
        vec3 dir = normalize(v);
        vec3 absdir = abs(dir);

        float t;
        vec2 hit;

        // Z faces
        if (absdir.z >= absdir.x && absdir.z >= absdir.y)
        {
           
            if (dir.z < 0.0)
            {
                t = -1.0 / dir.z;
            }
            else
            {
                t =  1.0 / dir.z;
            }
            hit = dir.xy * t;

            if (abs(hit.x) < 1.0 && abs(hit.y) < 1.0)
            {
                if (dir.z > 0.0)
                hit.x = -hit.x;   // back face flip

                //convert mapping stuff
                return hit * 0.5 + 0.5;
            }
        }

        // same wif x
        if (absdir.x >= absdir.y)
        {
          
            if (dir.x < 0.0)
            {
                t = -1.0 / dir.x;
            }
            else
            {
                t =  1.0 / dir.x;
            }
            //2DDDD
            hit = vec2(dir.z, dir.y) * t;

            if (abs(hit.x) < 1.0 && abs(hit.y) < 1.0)
            {
                if (dir.x > 0.0)
                    hit.x = -hit.x;   // right face flip

                return hit * 0.5 + 0.5;
            }
        }
        //The function must calculate and return the texture coordinate where the given view ray (defined by vector v) intersects the side of the skybox.
       //If the view ray intersects the top or bottom sides of the cube, then the function must return vec2(0.0f, 0.0f).
       // Y face
        return vec2(0.0);
    }




    void main()
    {
        
        vec2 reflectuv = vec2uv(ReflectDir);
        vec2 refractuv = vec2uv(RefractDir);

        vec3 reflectColor = checkerboardTexture(reflectuv, 10.0).rgb;
        vec3 refractColor = checkerboardTexture(refractuv, 10.0).rgb;

        vec3 color = mix(refractColor, reflectColor, material.reflectionFactor);

        // Gamma correction
        color = pow(color, vec3(1.0f/2.2f));
    
        FragColor = vec4(color, 1.0f);
    }
)"
