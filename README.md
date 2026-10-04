# OpenGL Shaders Showcase

A C++ / OpenGL / GLSL framework with a set of real-time rendering demos, built for CSD2150 (Computer Graphics). Each demo opens in its own window.

Demos include:

- Ray Casting (2D-map, first-person style, compute + fragment shader versions)
- Ray Tracing
- Phong Shading
- Environment Mapping (night-sky and Pisa HDR cubemaps)
- Reflection and Refraction
- Normal Mapping (Ogre model)
- Projected Texture
- Portal
- Deferred Shading with Blur, Edge and Cartoon post-processing filters

## Try it (Windows, no install)

1. Download `Shaders-win64.zip` from the [latest release](../../releases/latest).
2. Unzip it and run `Shaders.exe` from inside the folder (keep the `Ogre`, `Pisa` and `NightSky` folders next to it).

Needs a GPU/driver with OpenGL 4.6 support.

### Controls

| Key | Action |
| --- | --- |
| Space | Toggle fullscreen |
| P | Save a screenshot of the active window |
| Tab | Cycle the object shown in the active demo |
| Enter | Copy the active window into its own window |
| Esc | Quit |

Some demos have extra keys (printed in the console window when they start).

## Build from source

Requirements: Visual Studio 2022 with the C++ desktop workload, plus [GLFW](https://www.glfw.org/), [GLEW](https://glew.sourceforge.net/) and [GLM](https://github.com/g-truc/glm).

The project expects them at:

```
C:\glfw\include   C:\glfw\lib-vc2022
C:\glew\include   C:\glew\lib\Release\x64
C:\glm
```

Adjust the include and library paths in `Shaders/Shaders.vcxproj` if yours differ. Open `Shaders.sln`, select **Release | x64**, build, then copy `glew32.dll` and the `Ogre`, `Pisa` and `NightSky` folders next to `Shaders.exe`.

## Structure

```
Shaders/*.cpp, *.h     framework (window manager, FBOs, meshes, lights, materials) and demos (main.cpp)
Shaders/Shaders/*.glsl vertex, fragment and compute shaders
Shaders/Ogre/          model + textures for normal mapping
Shaders/Pisa/          HDR cubemap
Shaders/NightSky/      skybox
```

Coursework for CSD2150, Singapore Institute of Technology / DigiPen.

