R"(
    #version 330 core

    uniform vec2 iResolution;   // Viewport resolution (in pixels)
    uniform float iTime;        // Time in seconds for animation

    layout(location=0) out vec4 FragColor;

    struct Light
    {
        vec3 position;
    };

    struct Ray
    {
        vec3 origin;
        vec3 direction;   // Normalized
    };

    struct Sphere
    {
        vec3 center;
        float radius;
        vec3 color;
    };

    bool intersectSphere(Ray ray, Sphere sphere, out float t)
    {
        //slide 9
        // O-S
        vec3 os = ray.origin - sphere.center;
        float a = dot(ray.direction, ray.direction);
        float b = 2.0 * dot(os, ray.direction);
        float c = dot(os, os) - sphere.radius * sphere.radius;
        //slide 10
        //Since our direction vector is normalized, its length is 1. Consequently, the value of A is also 1
        float discriminant = b * b - 4.0 * c;  // 4.0 * a * c becomes 4.0 * c
    
        if (discriminant < 0.0)
        {
            return false;
        }
        //slide 10
        float sqrtDiscriminant = sqrt(discriminant);
        float t0 = (-b - sqrtDiscriminant) / 2.0;  // 2.0 * a becomes 2.0
        float t1 = (-b + sqrtDiscriminant) / 2.0;
    
        //slide 11
        //if there are two solutions, we select the smallest non?negative value of t as the intersection point
        if (t0 > 0.0)
        {
            t = t0;
            return true;
        }
        else if (t1 > 0.0)
        {
            t = t1;
            return true;
        }
        //intersections behind the camera
        return false;
    }

    //Calculate the shade value and use it to set the traced color
    vec3 calculateShade(vec3 RayPoint, vec3 normal, vec3 sphereColor, Light light)
    {
        // Calculate direction to light
        vec3 lightDir = normalize(light.position - RayPoint);
   
        //slide 17
        //We set the intensity as vec3(0.1f, 0.1f, 0.1f) as minimum for all points on the surface.

        vec3 ambient = vec3(0.1, 0.1, 0.1);
        
        //slide 18 to 21
        //A dot B = |A||B| cos(?)
        float diffuse = max(dot(normal, lightDir), 0.0);
        //Diffuse Light Contribution
        vec3 diffuseContribution = vec3(diffuse);
        // Combine lighting
        vec3 shade = ambient + diffuseContribution;
        
        return sphereColor * shade;
    }

    void main()
    {
        vec2 uv = (gl_FragCoord.xy - 0.5 * iResolution.xy) / iResolution.y;
        vec3 direction = normalize(vec3(uv, -2.0)); 

        Ray ray = Ray(vec3(0.0, 0.0, 8.0), direction);
        
        // Earth (1)
        Sphere earth;
        earth.center = vec3(0.0, 0.0, 0.0);
        earth.radius = 0.8;
        earth.color = vec3(0.2, 0.4, 0.8);
        
        // Sun (2) orbits earth n light source
        float sunOrbitRad = 6.0;
        float sunOrbitSpeed = 0.8;
        float sunLoc = -iTime * sunOrbitSpeed + 2.5;
        vec3 sunPos = vec3( sunOrbitRad * cos(sunLoc), 1.5, sunOrbitRad * sin(sunLoc) );
        
        Sphere sun;
        sun.center = sunPos;
        sun.radius = 0.8;
        sun.color = vec3(1.0, 0.9, 0.2);
        
        // Moon (3) orbits earth
        float moonOrbitRad = 2.0;
        float moonOrbitSpeed = 2.0;
        float moonLoc = iTime * moonOrbitSpeed;
       // vec3 moonPos = vec3(moonOrbitRad * sin(moonLoc), moonOrbitRad * sin(moonLoc),- 1.0);
        vec3 moonPos = vec3(moonOrbitRad * sin(moonLoc),sin(moonLoc), moonOrbitRad * cos(moonLoc) * -1.0);
        
        Sphere moon;
        moon.center = moonPos;
        moon.radius = 0.2;
        moon.color = vec3(0.7, 0.7, 0.7);
        
        // Light source
        Light light;
        light.position = sunPos;
        
        // Ray tracing find closest intersection
        vec3 finalColor = vec3(0.1, 0.1, 0.1);
        //render the one closest to the camera
        float closestDist = 100000; //big number
        int ObjectIntersect = -1;  // -1: none, 0: earth, 1: sun, 2: moon
        float Intersected;
        
        // Sun intersection
        if (intersectSphere(ray, sun, Intersected) && Intersected < closestDist)
        {
            closestDist = Intersected;
            ObjectIntersect = 1;
        }
        
        // Moon intersection
        if (intersectSphere(ray, moon, Intersected) && Intersected < closestDist)
        {
            closestDist = Intersected;
            ObjectIntersect = 2;
        }
        
        // Earth intersection
        if (intersectSphere(ray, earth, Intersected) && Intersected < closestDist)
        {
            closestDist = Intersected;
            ObjectIntersect = 0;
        }
        
        // render closest object
        if (ObjectIntersect == 0)  // Earth
        {
            vec3 EarthRay = ray.origin + ray.direction * closestDist;
            vec3 normal = normalize(EarthRay - earth.center);
            finalColor = calculateShade(EarthRay, normal, earth.color, light);
        }
        else if (ObjectIntersect == 1)  // Sun no shade
        {
            finalColor = sun.color;
        }
        else if (ObjectIntersect == 2)  // Moon
        {
            //same as earth
            vec3 MoonRay = ray.origin + ray.direction * closestDist;
            vec3 normal = normalize(MoonRay - moon.center);
            finalColor = calculateShade(MoonRay, normal, moon.color, light);
        }
        FragColor = vec4(finalColor, 1.0);
    }
)"