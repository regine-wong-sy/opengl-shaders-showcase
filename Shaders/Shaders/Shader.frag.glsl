//void main()
//{
//    FragColor = checkerboardTexture(gl_FragCoord.xy / iResolution.xy, 10);
//}
//)"

R"(
#version 330 core

uniform vec2 iResolution;
uniform int tab;
uniform float iTime;
uniform vec2 cursor;

layout(location=0) out vec4 FragColor;

vec4 checkerboardTexture(vec2 uv, int size)
{
    uv = uv * size;
    float color = mod(floor(uv.x) + floor(uv.y), 2.0f) / 5.0f; // x+y even:black  odd:white
    return vec4(color, color, color, 1.0f);
}

// Tab 0: moving checkerboard
vec4 tab0_visual(vec2 uv)
{
    vec2 movements = vec2(iTime * 0.1, iTime * 0.075); // horizontal n vertical movements
    vec2 animatedmoving = uv + movements; // shift coords
    return checkerboardTexture(animatedmoving, 10);
}

// Tab 1: color
vec4 tab1_visual(vec2 uv)
{
    // time based color
    float t = iTime * 0.5;
    
    // top right corner different colors
    vec3 color1a = vec3(1.0, 0.2, 0.3);
    vec3 color1b = vec3(1.0, 0.8, 0.2);
    vec3 color1c = vec3(0.8, 0.2, 1.0);
    
    float mix1 = sin(t) * 0.5 + 0.5; // smooth 0.0 and 1.0 since sin is -1 to 1
    float mix2 = sin(t + 2.0) * 0.5 + 0.5;
    vec3 color1 = mix(mix(color1a, color1b, mix1), color1c, mix2);
    
    // bottom left corner
    vec3 color2a = vec3(0.2, 0.5, 1.0);
    vec3 color2b = vec3(0.2, 1.0, 0.5);
    vec3 color2c = vec3(1.0, 0.3, 0.8);
    
    float mix3 = cos(t * 0.7) * 0.5 + 0.5;
    float mix4 = cos(t * 0.7 + 2.0) * 0.5 + 0.5;
    vec3 color2 = mix(mix(color2a, color2b, mix3), color2c, mix4);
    
    float blend = (uv.x + uv.y) * 0.5;
    
    vec3 final = mix(color2, color1, blend);
    
    return vec4(final, 1.0);
}

// Tab 2: highlight
vec4 tab2_visual(vec2 uv)
{
    vec4 baseColor = checkerboardTexture(gl_FragCoord.xy / iResolution.xy, 10);
    
    vec2 pixelPos = gl_FragCoord.xy;
    
    float dist = distance(pixelPos, cursor);
    
    float highlightrad = 30.0;
    vec3 highlightcolor = vec3(1.0, 1.0, 1.0);    
    float highlight = smoothstep(highlightrad, 0.0, dist);
    vec3 final = mix(baseColor.rgb, highlightcolor, highlight * 0.7);

    return vec4(final, 1.0);
}

// circle for tab
float drawCircle(vec2 pos, vec2 center, float radius)
{
    float dist = distance(pos, center);
    return dist < radius ? 1.0 : 0.0;
}

// tab indicators at bottom center
vec4 drawTabIndicators(vec2 pixelPos, vec4 baseColor)
{
    // indicators at bottom center
    float indicatorY = 20.0; // distance from bottom
    float centerX = iResolution.x/2;
    float spacing = 30.0; // space between circles
    float radius = 10.0; // radius
    float activerad = 13.0; // radius for current tab
    
    // positions for 3 tabs
    vec2 tab0Pos = vec2(centerX - spacing, indicatorY);
    vec2 tab1Pos = vec2(centerX, indicatorY);
    vec2 tab2Pos = vec2(centerX + spacing, indicatorY);
    
    vec3 final = baseColor.rgb;
    
    // tab 0
    float circle0 = drawCircle(pixelPos, tab0Pos, tab == 0 ? activerad : radius);
    vec3 color0 = tab == 0 ? vec3(1.0, 1.0, 1.0) : vec3(0.5, 0.5, 0.5);
    final = mix(final, color0, circle0);
    
    // tab 1
    float circle1 = drawCircle(pixelPos, tab1Pos, tab == 1 ? activerad : radius);
    vec3 color1 = tab == 1 ? vec3(1.0, 1.0, 1.0) : vec3(0.5, 0.5, 0.5);
    final = mix(final, color1, circle1);
    
    // tab 2
    float circle2 = drawCircle(pixelPos, tab2Pos, tab == 2 ? activerad : radius);
    vec3 color2 = tab == 2 ? vec3(1.0, 1.0, 1.0) : vec3(0.5, 0.5, 0.5);
    final = mix(final, color2, circle2);
    
    return vec4(final, 1.0);
}

void main()
{
    vec2 uv = gl_FragCoord.xy / iResolution.xy;
    vec4 color;
    
    // current tab stuffs
    if (tab == 0)
    {
        color = tab0_visual(uv);
    }
    else if (tab == 1)
    {
        color = tab1_visual(uv);
    }
    else if (tab == 2)
    {
        color = tab2_visual(uv);
    }
    else
    {
        color = vec4(1.0, 0.0, 1.0, 1.0);
    }
    
    //tab indicators last
    FragColor = drawTabIndicators(gl_FragCoord.xy, color);
}
)"