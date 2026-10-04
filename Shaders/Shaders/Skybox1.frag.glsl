//R"(
//    #version 420

    // Fragment shader for cube mapping with gamma correction.

  //  in vec3 Vec;

    //layout(binding=0) uniform samplerCube CubeMapTex;

    //layout(location=0) out vec4 FragColor;

   
    //from shader ass 1 lol
    //vec4 checkerboardTexture(vec2 uv, float size)
    //{
     //    uv = uv * size;
       //  float color = mod(floor(uv.x) + floor(uv.y), 2.0f);
   
    
         //return vec4(color, color, color, 1.0);
//    }

  //  vec2 vec2uv(vec3 v)
    //{
      //  vec3 dir = normalize(v);
       // vec3 absdir = abs(dir);

        //float t;
        //vec2 hit;

        // Z faces
        //if (absdir.z >= absdir.x && absdir.z >= absdir.y)
        //{
           
          //  if (dir.z < 0.0)
            //{
            //    t = -1.0 / dir.z;
            //}
            //else
            //{
             //   t =  1.0 / dir.z;
            //}
            //hit = dir.xy * t;

            //if (abs(hit.x) < 1.0 && abs(hit.y) < 1.0)
            //{
            //    if (dir.z > 0.0)
            //    hit.x = -hit.x;   // back face flip

                //convert mapping stuff
              //  return hit * 0.5 + 0.5;
            //}
//        }

        // same wif x
  //      if (absdir.x >= absdir.y)
    //    {
          
      //      if (dir.x < 0.0)
        //    {
          //      t = -1.0 / dir.x;
            //}
           // else
            //{
            //    t =  1.0 / dir.x;
            //}
            //2DDDD
            //hit = vec2(dir.z, dir.y) * t;

            //if (abs(hit.x) < 1.0 && abs(hit.y) < 1.0)
           // {
            //    if (dir.x > 0.0)
           //         hit.x = -hit.x;   // right face flip

             //   return hit * 0.5 + 0.5;
            //}
       // }
        //The function must calculate and return the texture coordinate where the given view ray (defined by vector v) intersects the side of the skybox.
       //If the view ray intersects the top or bottom sides of the cube, then the function must return vec2(0.0f, 0.0f).
       // Y face
      //  return vec2(0.0);
    //}
    //void main()
    //{
      //  vec2 uv = vec2uv(normalize(Vec));
    
     //   vec4 color = checkerboardTexture(uv, 10.0);
 
        // Gamma correction
        //checkboard vec4 alr
       // color.rgb = pow(color.rgb, vec3(1.0/2.2));

        //FragColor = color;
    //}
//)"

R"(
    #version 420

    // Fragment shader for cube mapping with gamma correction.

    in vec3 Vec;

    layout(binding=0) uniform samplerCube CubeMapTex;

    layout(location=0) out vec4 FragColor;

    void main() 
    {
        // Access the cube map texture
        vec3 color = vec3(texture(CubeMapTex, normalize(Vec)));

        // Gamma correction
        color = pow(color, vec3(1.0f/2.2f));

        FragColor = vec4(color, 1.0f);
    }
)"