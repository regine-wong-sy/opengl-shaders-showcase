/*****************************************************************************
\file main.cpp
\author Vadim Surov (vsurov@digipen.edu)
\co-author regine wong (w.shiyuregine@digipen.edu)
\par Course: CSD2150
\par Assignment: 12 (Portfolio)
\date 03/29/2026 (MM/DD/YYYY)
\brief Application entry point: initializes GLFW/Windows, constructs demo windows
       and rendering passes, registers callbacks, and runs the render loop.
*******************************************************************************/
#include <iostream> // input/output

#include "windowmanager.h"
#include "trianglemeshes.h"

const int NOMARGIN = 0;      // To set no margin for a window
static int tab = 0;   // tracks which tab is on (0, 1, or 2)
const int TABS = 3;      // total num of tabs
static glm::vec2 cursorPos = glm::vec2(0.0f, 0.0f);

const int MARGIN = 2;        // Defines margin size for windows
const int GAP = 10;          // Gap between windows
const int SIZEOFGRID = 9;   // Size of grid (number of windows per row/column)

namespace RayCasting
{
    // Map dimensions
    const int MAP_WIDTH = 16;
    const int MAP_HEIGHT = 16;

    int worldMap[MAP_WIDTH][MAP_HEIGHT] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,2,2,2,0,0,3,3,3,0,0,0,1},
        {1,0,0,0,2,0,0,0,0,0,0,3,0,0,0,1},
        {1,0,0,0,2,0,0,0,0,0,0,3,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,5,0,0,0,0,0,0,4,0,0,0,1},
        {1,0,0,0,5,0,0,0,0,0,0,4,0,0,0,1},
        {1,0,0,0,5,5,5,0,0,4,4,4,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    // Create and bind the storage
    void createBuffers(cg::ManagedWindow* pWindow)
    {
        if (!pWindow || !pWindow->pWindow)
            return;

        // Make the window's OpenGL context current before touching GL objects.
        glfwMakeContextCurrent(pWindow->pWindow);

        // Create SSBO for ray distances
        if (pWindow->context.gluintparam[0] == 0)  // Distance SSBO
            glGenBuffers(1, &pWindow->context.gluintparam[0]);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, pWindow->context.gluintparam[0]);
        glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(pWindow->width *
            sizeof(float)), nullptr, GL_DYNAMIC_DRAW);

        // Create SSBO for ray texture coordinates
        if (pWindow->context.gluintparam[1] == 0)  // Texture coordinates SSBO
            glGenBuffers(1, &pWindow->context.gluintparam[1]);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, pWindow->context.gluintparam[1]);
        glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(pWindow->width *
            sizeof(float)), nullptr, GL_DYNAMIC_DRAW);

        // Create SSBO for texture ids
        if (pWindow->context.gluintparam[2] == 0)  // Texture ids SSBO
            glGenBuffers(1, &pWindow->context.gluintparam[2]);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, pWindow->context.gluintparam[2]);
        glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(pWindow->width *
            sizeof(int)), nullptr, GL_DYNAMIC_DRAW);

        // Create SSBO for the map
        if (pWindow->context.gluintparam[3] == 0)  // Map SSBO
            glGenBuffers(1, &pWindow->context.gluintparam[3]);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, pWindow->context.gluintparam[3]);
        glBufferData(GL_SHADER_STORAGE_BUFFER, MAP_WIDTH * MAP_HEIGHT * sizeof(int),
            worldMap, GL_STATIC_DRAW);

        // Unbind to avoid accidental state leaks
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    // Resize the distance buffer when the window size changes
    void resizeBuffers(cg::ManagedWindow* pWindow)
    {
        if (!pWindow || !pWindow->pWindow)
            return;

        // Make the window's OpenGL context current before touching GL objects.
        glfwMakeContextCurrent(pWindow->pWindow);

        // Ensure SSBO exists for ray distances; create if not
        if (pWindow->context.gluintparam[0] == 0) // Distance SSBO
            glGenBuffers(1, &pWindow->context.gluintparam[0]);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, pWindow->context.gluintparam[0]);
        glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(pWindow->width *
            sizeof(float)), nullptr, GL_DYNAMIC_DRAW);

        // Ensure SSBO exists for ray texture coordinates; create if not
        if (pWindow->context.gluintparam[1] == 0) // Texture coordinates SSBO
            glGenBuffers(1, &pWindow->context.gluintparam[1]);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, pWindow->context.gluintparam[1]);
        glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(pWindow->width *
            sizeof(float)), nullptr, GL_DYNAMIC_DRAW);

        // Ensure SSBO exists for ray texture ids; create if not
        if (pWindow->context.gluintparam[2] == 0) // Texture ids SSBO
            glGenBuffers(1, &pWindow->context.gluintparam[2]);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, pWindow->context.gluintparam[2]);
        glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(pWindow->width *
            sizeof(int)), nullptr, GL_DYNAMIC_DRAW);

        // Unbind to avoid accidental state leaks
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    // Compute distances using ray casting
    void computeDistances(cg::ManagedWindow* pWindow)
    {
        if (!pWindow || !pWindow->pWindow)
            return;

        // Ensure the correct OpenGL context is current. Dispatching compute work requires
        // the program to be created/used on the same current context.
        glfwMakeContextCurrent(pWindow->pWindow);

        // Use the compute program (now guaranteed to be built on the current context)
        pWindow->computeShader.use();
        pWindow->computeShader.setUniform("playerPos", pWindow->context.vec2param[0]);
        pWindow->computeShader.setUniform("playerDir", pWindow->context.vec2param[1]);
        pWindow->computeShader.setUniform("mapWidth", MAP_WIDTH);
        pWindow->computeShader.setUniform("mapHeight", MAP_HEIGHT);
        pWindow->computeShader.setUniform("numRays", pWindow->width);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, pWindow->context.gluintparam[0]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, pWindow->context.gluintparam[1]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, pWindow->context.gluintparam[2]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, pWindow->context.gluintparam[3]);

        // Dispatch compute threads (num work groups = window width; shader should use 
        // local size accordingly)
        glDispatchCompute(pWindow->width, 1, 1);

        // Ensure writes are visible to subsequent stages
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

        pWindow->computeShader.disuse();
    }
}
bool clickontab(int xpos, int ypos, int windowWidth, int windowHeight, int tabIndex)
{
    float indicatorY = 20.0f; // distance from bottom (same as shader)
    float centerX = windowWidth / 2.0f;
    float spacing = 30.0f;
    float radius = 10.0f;

    // tab position
    float tabX = centerX + (tabIndex - 1) * spacing; // -1, 0, 1 for tabs 0, 1, 2
    float tabY = indicatorY;

    // mouse bottom
    float mouseFlipped = static_cast<float>(windowHeight - ypos);

    // check distance from tab center
    float distancex = xpos - tabX;
    float distancey = mouseFlipped - tabY;
    float dist = sqrt(distancex * distancex + distancey * distancey);

    return dist <= radius;
}


// Entry point of the application
int main(int argc, char** argv)
{
    int retcode = 0; // Return code for the application

    // Create a window manager instance
    auto& windowManager = cg::WindowManager::getInstance();

    try
    {
        // Initialize GLFW
        if (!glfwInit())
            throw std::runtime_error("Failed to initialize GLFW.");

        // Debug context setup for OpenGL if in debug mode
#ifdef _DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

        // Configure OpenGL context properties
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        GLFWwindow* pMainWindow = nullptr; // Pointer to the main window
        int i = 0;
        // Add the main window with its properties
        pMainWindow = windowManager.addWindow({
            cg::State::SHOW,        // State (SHOW, HIDE)
            nullptr,                // Parent window (nullptr for top-level window)
            DEFAULT,                // Viewport (DEFAULT, {GAP}, {WIDTH, HEIGHT}, 
            //   {GRIDELEMENT, SIZEOFGRID, GAP}, {X, Y, WIDTH, HEIGHT})
"Shaders Framework",    // Title
NONE,                   // Background color (NONE, BLACK, GREY, WHITE, RED, GREEN, 
//   BLUE, {R,G,B,A})
NONE,                   // Textures ({filename})
NONE,                   // Compute shader
{                       // Rendering passes
    {                       // Pass 0
{ NOMARGIN },           // Viewport
NONE,                   // Background color
DISABLE,                // Depth test
{ { QUAD } },           // Objects
NONE,                   // Cameras
NONE,                   // Lights
DEFAULT,                // Rendering target
{                       // Vertex and fragment shaders
    #include "Shaders/Shader.vert.glsl"
    ,
    #include "Shaders/Shader.frag.glsl"  
}
}
},

// FocusCallback - called when the window gains or loses focus
[](cg::ManagedWindow* pWindow, int focused)
{
    if (focused)
    {
        std::cout << std::endl;
        std::cout << "Shaders Framework (version "
            << __DATE__ << " " << __TIME__ << ")";
        std::cout << std::endl << R"(
   This framework provides a solid foundation for exploring the capabilities of
   modern real-time 2D and 3D graphics for developing interactive simulations
   and games. It harnesses the power of the graphics card and advanced shader
   technology to deliver smooth, visually immersive experiences.
Controls:
   - Resize: Drag the window edges to adjust the application size.
   - Fullscreen: Press Space to toggle between windowed and fullscreen mode.
   - Save Screenshot: Press 'P' to save a screenshot of the active window.
   - Exit: Press Esc to close the application.
)";
                }
            },

            // InitCallback - called for initializing shaders across
            // the entire rendering process or individual passes when required
            [](cg::ManagedWindow* pWindow, int pass)
            {
                if (pass == -1) // Initial setup for the entire rendering process
                {
                    pWindow->passes[0].renderShader.use();
                    pWindow->passes[0].renderShader.setUniform("tab", tab);
                    pWindow->passes[0].renderShader.setUniform("iTime", 0.0f);
                    pWindow->passes[0].renderShader.setUniform("cursor", cursorPos);
                }
                else if (pass == 0) // called each frame for pass 0
                {
                    pWindow->passes[0].renderShader.use();
                    pWindow->passes[0].renderShader.setUniform("tab", tab);

                    // update time for animations
                    float time = static_cast<float>(glfwGetTime());
                    pWindow->passes[0].renderShader.setUniform("iTime", time);
                    pWindow->passes[0].renderShader.setUniform("cursor", cursorPos);
                }
            },

            // SizeCallback - called when the window is resized
            [](cg::ManagedWindow* pWindow, int width, int height)
            {
                 auto& windowManager = cg::WindowManager::getInstance();
                 windowManager.resizeChildren(pWindow->pWindow, width, height);
            },

            // KeyCallback - called when a key is pressed
            [](cg::ManagedWindow* pWindow, int key)
            {
                switch (key)
                {
                    case GLFW_KEY_TAB:
                        // next tab
                        tab = (tab + 1) % TABS;

                        // update the shader for tab
                        pWindow->passes[0].renderShader.use();
                        pWindow->passes[0].renderShader.setUniform("tab", tab);
                        auto& windowManager = cg::WindowManager::getInstance();
                        // hide the Ray Casting window on tab 2 n 3
                        for (auto& window : windowManager.windows) {
                            if (!window.pWindow) continue;
                            if (window.title == "Ray Casting") {
                                if (tab == 0) {
                                    glfwShowWindow(window.pWindow);
                                }
                                else {
                                    glfwHideWindow(window.pWindow);
                                }
                            }
                            if (window.title == "Ray Tracing") {

                                if (tab == 0) {
                                    glfwShowWindow(window.pWindow);
                                }
                                else {
                                    glfwHideWindow(window.pWindow);
                                }
                            }
                            if (window.title == "Phong Shading") {

                                if (tab == 0) {
                                    glfwShowWindow(window.pWindow);
                                }
                                else {
                                    glfwHideWindow(window.pWindow);
                                }
                            }
                            if (window.title == "Environment Mapping") {

                                if (tab == 0) {
                                    glfwShowWindow(window.pWindow);
                                }
                                else {
                                    glfwHideWindow(window.pWindow);
                                }
                            }


                            if (window.title == "Assignment 8 (NightSky)") {

                                if (tab == 0) {
                                    glfwShowWindow(window.pWindow);
                                }
                                else {
                                    glfwHideWindow(window.pWindow);
                                }
                            }
                            if (window.title == "Assignment 9 (Cartoon)") {

                                if (tab == 0) {
                                    glfwShowWindow(window.pWindow);
                                }
                                else {
                                    glfwHideWindow(window.pWindow);
                                }
                            }

                            if (window.title == "Assignment 10 (Portal)") {

                                if (tab == 0) {
                                    glfwShowWindow(window.pWindow);
                                }
                                else {
                                    glfwHideWindow(window.pWindow);
                                }
                            }
                            if (window.title == "Assignment 11 (Ogres)") {

                                if (tab == 0) {
                                    glfwShowWindow(window.pWindow);
                                }
                                else {
                                    glfwHideWindow(window.pWindow);
                                }
                            }

                            if (window.title == "Assignment 1") {

                                if (tab == 0) {
                                    glfwShowWindow(window.pWindow);
                                }
                                else {
                                    glfwHideWindow(window.pWindow);
                                }
                            }




                        }
                        return false; // do not propagate the event

                }
                return true; // Propagate the event by default
            },

            // MousebuttonCallback - called when a mouse button is pressed
            [](cg::ManagedWindow* pWindow, int button, int xpos, int ypos)
            {
                    if (button == GLFW_MOUSE_BUTTON_LEFT)
                    {
                        // if clicked on any tab indicator
                        for (int i = 0; i < TABS; i++)
                        {
                            if (clickontab(xpos, ypos, pWindow->passes[0].width, pWindow->passes[0].height, i))
                            {
                                tab = i;

                                // same as key tab
                                pWindow->passes[0].renderShader.use();
                                pWindow->passes[0].renderShader.setUniform("tab", tab);
                                // hide the Ray Casting window on tab 2 n 3
                                auto& windowManager = cg::WindowManager::getInstance();
                                for (auto& window : windowManager.windows) {
                                    if (!window.pWindow) continue;
                                    if (window.title == "Ray Casting") {
                                        if (tab == 0) {
                                            glfwShowWindow(window.pWindow);
                                        }
                                        else {
                                            glfwHideWindow(window.pWindow);
                                        }
                                    }
                                    if (window.title == "Ray Tracing") {

                                        if (tab == 0) {
                                            glfwShowWindow(window.pWindow);
                                        }
                                        else {
                                            glfwHideWindow(window.pWindow);
                                        }
                                    }
                                    if (window.title == "Phong Shading") {

                                        if (tab == 0) {
                                            glfwShowWindow(window.pWindow);
                                        }
                                        else {
                                            glfwHideWindow(window.pWindow);
                                        }
                                    }
                                    if (window.title == "Environment Mapping") {

                                        if (tab == 0) {
                                            glfwShowWindow(window.pWindow);
                                        }
                                        else {
                                            glfwHideWindow(window.pWindow);
                                        }
                                    }
                                    if (window.title == "Assignment 8 (NightSky)") {

                                        if (tab == 0) {
                                            glfwShowWindow(window.pWindow);
                                        }
                                        else {
                                            glfwHideWindow(window.pWindow);
                                        }
                                    }
                                    if (window.title == "Assignment 9 (Cartoon)") {

                                        if (tab == 0) {
                                            glfwShowWindow(window.pWindow);
                                        }
                                        else {
                                            glfwHideWindow(window.pWindow);
                                        }
                                    }
                                    if (window.title == "Assignment 10 (Portal)") {

                                        if (tab == 0) {
                                            glfwShowWindow(window.pWindow);
                                        }
                                        else {
                                            glfwHideWindow(window.pWindow);
                                        }
                                    }
                                    if (window.title == "Assignment 11 (Ogres)") {

                                        if (tab == 0) {
                                            glfwShowWindow(window.pWindow);
                                        }
                                        else {
                                            glfwHideWindow(window.pWindow);
                                        }
                                    }

                                    if (window.title == "Assignment 1") {

                                        if (tab == 0) {
                                            glfwShowWindow(window.pWindow);
                                        }
                                        else {
                                            glfwHideWindow(window.pWindow);
                                        }
                                    }

                                }
                                break;
                            }
                        }
                    }
            },

            // ScrollCallback - called when the mouse wheel is scrolled
            [](cg::ManagedWindow* pWindow, double xoffset, double yoffset, int mods)
            {
            },

            // CursorposCallback - called when the mouse is moved
            [](cg::ManagedWindow* pWindow, int xpos, int ypos, double xoffset, double yoffset, int mods)
            {
                //flip y
                cursorPos = glm::vec2(static_cast<float>(xpos), static_cast<float>(pWindow->passes[0].height - ypos));

                // similar to key tab but mouse
                pWindow->passes[0].renderShader.use();
                pWindow->passes[0].renderShader.setUniform("cursor", cursorPos);
        }
            });

        // child window for Ray Casting demo
        if (!GLEW_ARB_compute_shader)
            std::cout << "Compute shaders not supported!" << std::endl;
        else
        {
                        windowManager.addWindow({
                cg::State::SHOW,            // State
                pMainWindow,                // Parent window
                { i++, SIZEOFGRID, GAP },  
                // Viewport (DEFAULT, {GAP}, {WIDTH, HEIGHT}, 
    //   {GRIDELEMENT, SIZEOFGRID, GAP}, {X, Y, WIDTH, HEIGHT})
"Assignment 1",    // Title
NONE,                   // Background color (NONE, BLACK, GREY, WHITE, RED, GREEN, 
//   BLUE, {R,G,B,A})
NONE,                   // Textures ({filename})
NONE,                   // Compute shader
{                       // Rendering passes
    {                       // Pass 0
{ NOMARGIN },           // Viewport
NONE,                   // Background color
DISABLE,                // Depth test
{ { QUAD } },           // Objects
NONE,                   // Cameras
NONE,                   // Lights
DEFAULT,                // Rendering target
{                       // Vertex and fragment shaders
    #include "Shaders/Shader.vert.glsl"
    ,
    #include "Shaders/Shader.frag.glsl"  
}
}
},

// FocusCallback - called when the window gains or loses focus
[](cg::ManagedWindow* pWindow, int focused)
{
    if (focused)
    {
        std::cout << std::endl;
        std::cout << "Shaders Framework (version "
            << __DATE__ << " " << __TIME__ << ")";
        std::cout << std::endl << R"(
   This framework provides a solid foundation for exploring the capabilities of
   modern real-time 2D and 3D graphics for developing interactive simulations
   and games. It harnesses the power of the graphics card and advanced shader
   technology to deliver smooth, visually immersive experiences.
Controls:
   - Resize: Drag the window edges to adjust the application size.
   - Fullscreen: Press Space to toggle between windowed and fullscreen mode.
   - Save Screenshot: Press 'P' to save a screenshot of the active window.
   - Exit: Press Esc to close the application.
)";
                }
            },

                // InitCallback - called for initializing shaders across
                // the entire rendering process or individual passes when required
                [](cg::ManagedWindow* pWindow, int pass)
                {
                    if (pass == -1) // Initial setup for the entire rendering process
                    {
                        
                        pWindow->context.intparam[0] = tab;
                        pWindow->passes[0].renderShader.use();
                        pWindow->passes[0].renderShader.setUniform("tab", tab);
                        pWindow->passes[0].renderShader.setUniform("iTime", 0.0f);
                        pWindow->passes[0].renderShader.setUniform("cursor", cursorPos);
                    }
                    else if (pass == 0) // called each frame for pass 0
                    {
                        
                        int currentTab = pWindow->context.intparam[0];
                        pWindow->passes[0].renderShader.use();
                        pWindow->passes[0].renderShader.setUniform("tab", currentTab);

                        // update time for animations
                        float time = static_cast<float>(glfwGetTime());
                        pWindow->passes[0].renderShader.setUniform("iTime", time);
                        pWindow->passes[0].renderShader.setUniform("cursor", cursorPos);
                    }
                },

                // SizeCallback - called when the window is resized
                [](cg::ManagedWindow* pWindow, int width, int height)
                {
                     auto& windowManager = cg::WindowManager::getInstance();
                     windowManager.resizeChildren(pWindow->pWindow, width, height);
                },

                // KeyCallback - called when a key is pressed
                [](cg::ManagedWindow* pWindow, int key)
                {
                    switch (key)
                    {
                        case GLFW_KEY_TAB:
                            if (!pWindow->pParentWindow)
                            {
                               
                                pWindow->context.intparam[0] = (pWindow->context.intparam[0] + 1) % TABS;
                                pWindow->passes[0].renderShader.use();
                                pWindow->passes[0].renderShader.setUniform("tab", pWindow->context.intparam[0]);
                            }
                            else
                            {
                                pWindow->context.intparam[0] = (pWindow->context.intparam[0] + 1) % TABS;
                                pWindow->passes[0].renderShader.use();
                                pWindow->passes[0].renderShader.setUniform("tab", pWindow->context.intparam[0]);
                            }
                            return false; // do not propagate the event

                    }
                    return true; // Propagate the event by default
                },

                // MousebuttonCallback - called when a mouse button is pressed
                [](cg::ManagedWindow* pWindow, int button, int xpos, int ypos)
                {
                    if (button == GLFW_MOUSE_BUTTON_LEFT)
                    {
                        for (int i = 0; i < TABS; i++)
                        {
                            if (clickontab(xpos, ypos, pWindow->passes[0].width, pWindow->passes[0].height, i))
                            {
                                pWindow->context.intparam[0] = i;
                                pWindow->passes[0].renderShader.use();
                                pWindow->passes[0].renderShader.setUniform("tab", pWindow->context.intparam[0]);
                                break;
                            }
                        }
                    }
                },

                // ScrollCallback - called when the mouse wheel is scrolled
                [](cg::ManagedWindow* pWindow, double xoffset, double yoffset, int mods)
                {
                },

                // CursorposCallback - called when the mouse is moved
                [](cg::ManagedWindow* pWindow, int xpos, int ypos, double xoffset, double yoffset, int mods)
                {
                    //flip y
                    cursorPos = glm::vec2(static_cast<float>(xpos), static_cast<float>(pWindow->passes[0].height - ypos));

                    // similar to key tab but mouse
                    pWindow->passes[0].renderShader.use();
                    pWindow->passes[0].renderShader.setUniform("cursor", cursorPos);
                },
                // MouseclickCallback
                nullptr,

                // MousedoubleclickCallback - double-click to copy this window into the main view
                [](cg::ManagedWindow* pWindow, int xpos, int ypos)
                {
                    cg::WindowManager::getInstance().copyWindow(*pWindow);
                 }

                });

            windowManager.addWindow({
                cg::State::SHOW,            // State
                pMainWindow,                // Parent window
                { i++, SIZEOFGRID, GAP },     // Viewport
                "Ray Casting",              // Title
                BLACK,                      // Background color
                NONE,                       // Textures
                {                           // Compute shader
                    #include "Shaders/RayCasting.comp.glsl"
                },
                {                           // Rendering passes
                    {                           // Pass 0
                        { MARGIN },                 // Viewport
                        NONE,                       // Background color
                        DISABLE,                    // Depth test
                        { { QUAD } },               // Objects
                        NONE,                       // Cameras
                        NONE,                       // Lights
                        DEFAULT,                    // Rendering target
                        {                           // Vertex and fragment shaders
                            #include "Shaders/PassThrough.vert.glsl"
                            ,
                            #include "Shaders/RayCasting.frag.glsl"
                        },
                    }
                },

                // FocusCallback - called when the window gains or loses focus
                [](cg::ManagedWindow* pWindow, int focused)
                {
                    if (focused)
                    {
                        std::cout << std::endl << R"(
Assignment 2 (Ray Casting)
   This is an implementation of the Ray Casting method. This rendering method
   serves as the foundation for many classic 'first-person shooter'-style games.
   The scene is defined as a 2D map, and the player's position is tracked within it.
   The GPU is also used to calculate distances to walls during rendering, enabling
   efficient real-time visualization.
Controls:
   - Move Player: Use W, A, S, and D keys to move.
   - Change Orientation: Drag left or right with the mouse to rotate the view.
)";
                    }
                },

                // InitCallback - called for initializing shaders across
                // the entire rendering process or individual passes when required
                [](cg::ManagedWindow* pWindow, int pass)
                {
                    if (pass == -1) // Initial setup
                    {
                        pWindow->context.vec2param[0] = glm::vec2{ 8.0f, 8.0f }; // Pos
                        pWindow->context.vec2param[1] = glm::vec2{ 0.0f, -1.0f }; // Dir

                        RayCasting::createBuffers(pWindow);
                        RayCasting::computeDistances(pWindow);


                    }
                    else if (pass == 0) // Each frame before rendering
                    {
                        // Compute ray distances for this frame
                        RayCasting::computeDistances(pWindow);

                        // Update render shader
                        pWindow->passes[0].renderShader.use();
                        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, pWindow->context.gluintparam[0]);
                        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, pWindow->context.gluintparam[1]);
                        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, pWindow->context.gluintparam[2]);
                         }
                     },

                // SizeCallback - called when the window is resized
                [](cg::ManagedWindow* pWindow, int width, int height)
                {
                    RayCasting::resizeBuffers(pWindow);
                    RayCasting::computeDistances(pWindow);
                },

                // KeyCallback - called when a key is pressed
                [](cg::ManagedWindow* pWindow, int key)
                {
                    switch (key)
                    {
                    case 'W':
                        pWindow->context.vec2param[0] += pWindow->context.vec2param[1] /
                            glm::vec2(5.0f, 5.0f);
                        RayCasting::computeDistances(pWindow);

                        return false; // Do not propagate the event

                    case 'A':
                        pWindow->context.vec2param[0] -=
                            glm::vec2(-pWindow->context.vec2param[1].y,
                            pWindow->context.vec2param[1].x) / glm::vec2(5.0f, 5.0f);
                        RayCasting::computeDistances(pWindow);

                        return false; // Do not propagate the event

                    case 'S':
                        pWindow->context.vec2param[0] -= pWindow->context.vec2param[1] /
                            glm::vec2(5.0f, 5.0f);
                        RayCasting::computeDistances(pWindow);

                        return false; // Do not propagate the event

                    case 'D':
                        pWindow->context.vec2param[0] += glm::vec2(
                            -pWindow->context.vec2param[1].y,
                            pWindow->context.vec2param[1].x) / glm::vec2(5.0f, 5.0f);
                        RayCasting::computeDistances(pWindow);

                        return false; // Do not propagate the event
                    }

                    return true; // Propagate the event by default
                },

                // MousebuttonCallback - called when a mouse button is pressed
                nullptr,

                // ScrollCallback - called when the mouse wheel is scrolled
                nullptr,

                // CursorposCallback - called when the mouse is moved
            [](cg::ManagedWindow* pWindow, int xpos, int ypos, double xoffset, double yoffset, int mods)
            {
                    // rotate direction by +/-5 degrees based on xoffset
                    if (glm::abs(xoffset) > glm::abs(yoffset))
                    {
                        if (glm::abs(xoffset) > 1.0)
                        {
                            const float angleDeg = (xoffset < 0.0) ? -5.0f : 5.0f;
                            const float angle = glm::radians(angleDeg);
                            const float c = glm::cos(angle);
                            const float s = glm::sin(angle);
                            glm::mat2 rot = glm::mat2(c, -s, s, c);
                            pWindow->context.vec2param[1] = glm::normalize(rot *
                                pWindow->context.vec2param[1]);
                            RayCasting::computeDistances(pWindow);
                        }
                    }
                },
                // MouseclickCallback
                nullptr,

                // MousedoubleclickCallback - double-click to copy this window into the main view
                [](cg::ManagedWindow* pWindow, int xpos, int ypos)
                {
                    cg::WindowManager::getInstance().copyWindow(*pWindow);
                 }



                });



            // Child window for Ray Tracing demo
            windowManager.addWindow({
                cg::State::SHOW,            // State
                pMainWindow,                // Parent window
                { i++, SIZEOFGRID, GAP },   // Viewport
                "Ray Tracing",              // Title
                BLACK,                      // Background color
                NONE,                       // Textures
                NONE,                       // Compute shader
                {                           // Rendering passes
                    {                           // Pass 0
                        { MARGIN },                 // Viewport
                        NONE,                       // Background color
                        DISABLE,                    // Depth test
                        { { QUAD } },               // Objects
                        NONE,                       // Cameras
                        NONE,                       // Lights
                        DEFAULT,                    // Rendering target
                        {                           // Vertex and fragment shaders
                            #include "Shaders/PassThrough.vert.glsl"
                            ,
                            #include "Shaders/RayTracing.frag.glsl"
                        },
                    }
                },

                // FocusCallback - called when the window gains or loses focus
                [](cg::ManagedWindow* pWindow, int focused)
                {
                    if (focused)
                    {
                        std::cout << std::endl << R"(
Assignment 3 (Ray Tracing)
   This demo illustrates the use of ray tracing to render spheres representing
   the Sun, Earth, and Moon, with the Sun and Moon orbiting around the Earth.
   Shading is computed based on surface normals and the Sun's position,
   producing realistic lighting effects.
Controls:
   - This demo does not have interactive controls.

)";
                }
            },

                // InitCallback - called for initializing shaders
                [](cg::ManagedWindow* pWindow, int pass)
                {
                    if (pass == -1) // Initial setup
                    {
                        pWindow->context.intparam[0] = 1;
                        pWindow->passes[0].renderShader.use();
                        pWindow->passes[0].renderShader.setUniform("lightingMethod", pWindow->context.intparam[0]);
                    }
                    else if (pass == 0) // Each frame before rendering
                    {
                        pWindow->passes[0].renderShader.use();
                        pWindow->passes[0].renderShader.setUniform("lightingMethod", pWindow->context.intparam[0]);
                    }
                },

                // SizeCallback - called when the window is resized
                nullptr,

                // KeyCallback - called when a key is pressed
                nullptr,

                // MousebuttonCallback - called when a mouse button is pressed
                nullptr,

                // ScrollCallback - called when the mouse wheel is scrolled
                nullptr,

                // CursorposCallback - called when the mouse is moved
                nullptr,

                // MouseclickCallback
                nullptr,

                // MousedoubleclickCallback - double-click to copy this window into the main view
                [](cg::ManagedWindow* pWindow, int xpos, int ypos)
                {
                    cg::WindowManager::getInstance().copyWindow(*pWindow);
                 }

               
                });


        }
        windowManager.addWindow({
                        cg::State::SHOW,            // State
                        pMainWindow,                // Parent window
                        { i++, SIZEOFGRID, GAP },     // Viewport
                        "Phong Shading",            // Title
                        BLACK,                      // Background color
                        NONE,                       // Textures
                        NONE,                       // Compute shader
                        {                           // Rendering passes
                            {                           // Pass 0
                                { MARGIN },                 // Viewport
                                BLACK,                      // Background color
                                ENABLE,                     // Depth test
                                DEMOOBJECTS,                // Objects
                                {     cg::Camera(cg::CameraType::ORBITING,
                                      glm::vec3{0.0f, 0.0f, 15.0f},  // Camera position
                                      glm::vec3{0.0f, 0.0f, 0.0f},   // Look straight ahead (same Y level)
                                      45.0f, 0.5f, 200.0f) },                // Cameras
                                { DEFAULT },                // Lights
                                DEFAULT,                    // Rendering target
                                {                           // Vertex and fragment shaders
                                    #include "Shaders/ViewSpaceTransform.vert.glsl"
                                    ,
                                    #include "Shaders/PhongShading.frag.glsl"
                                }
                            }
                        },

            // FocusCallback - called when the window gains or loses focus
            [](cg::ManagedWindow* pWindow, int focused)
            {
                if (focused)
                {
                    std::cout << std::endl << R"(
Assignment 4 (Blinn-Phong Shading)
This demo showcases the Blinn-Phong shading technique, which produces smooth,
   realistic surfaces by interpolating vertex normals per pixel and applying
   a lighting model that combines ambient, diffuse, and specular components.
   This approach creates sharp, well-defined highlights, in contrast to
   the softer, blurred highlights seen with Gouraud shading.
Controls:
   - Orbit Object: Drag with the left mouse button; scroll to zoom.
   - Orbit Light: Hold Shift and drag; scroll to adjust distance.
)";
                    }
                },

            // InitCallback - called for initializing shaders and setup
            [](cg::ManagedWindow* pWindow, int pass)
            {

                if (pass == -1) // Initial setup
                {

                    pWindow->passes[0].objects[0].instances[0].isVisible = true;  // PLANE
                    pWindow->passes[0].objects[1].instances[0].isVisible = true;  // TORUS
                    pWindow->passes[0].objects[2].instances[0].isVisible = true;  // CUBE
                    pWindow->passes[0].objects[3].instances[0].isVisible = true;  // SPHERE
                    pWindow->passes[0].objects[4].instances[0].isVisible = true;  // TEAPOT

                    pWindow->passes[0].lights[0].L[0] = glm::vec3(0.1f, 0.1f, 0.1f);  // La to 0.1

                    float radius = 4.0f;  // Distance from origin

                    // TORUS 0 degrees (positive X)
                    pWindow->passes[0].objects[1].instances[0].modeltransform =
                        glm::translate(glm::mat4(1.0f), glm::vec3(radius, 0.0f, 0.0f));

                    // CUBE 90 degrees (positive Z)
                    pWindow->passes[0].objects[2].instances[0].modeltransform =
                        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, radius));

                    // SPHERE 180 degrees (negative X)
                    pWindow->passes[0].objects[3].instances[0].modeltransform =
                        glm::translate(glm::mat4(1.0f), glm::vec3(-radius, 0.0f, 0.0f));

                    // TEAPOT 270 degrees (negative Z)
                    pWindow->passes[0].objects[4].instances[0].modeltransform =
                        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -radius));


                }
                else if (pass == 0) // Each frame update
                {

                    float time = static_cast<float>(glfwGetTime());
                    float radius = 4.0f;
                    // TORUS
                    float torusAngle = time * 0.5f; // Speed
                    pWindow->passes[0].objects[1].instances[0].modeltransform =
                        glm::translate(glm::mat4(1.0f), glm::vec3(radius * cos(torusAngle), 0.0f, radius * sin(torusAngle))) *
                        glm::rotate(glm::mat4(1.0f), torusAngle, glm::vec3(0.0f, 1.0f, 0.0f));

                    // CUBE
                    float cubeAngle = time * 0.8f;
                    glm::vec3 cubeAxis = glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f));
                    pWindow->passes[0].objects[2].instances[0].modeltransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, radius)) * glm::rotate(glm::mat4(1.0f), cubeAngle, cubeAxis);

                    // SPHERE
                    float scaleAmount = 1.0f + 0.5f * sin(time * 2.0f); // Oscillate between 0.5 and 1.5
                    pWindow->passes[0].objects[3].instances[0].modeltransform =
                        glm::translate(glm::mat4(1.0f), glm::vec3(-radius, 0.0f, 0.0f)) *
                        glm::scale(glm::mat4(1.0f), glm::vec3(scaleAmount));

                    // TEAPOT
                    float teapotAngle = time * 1.5f;
                    pWindow->passes[0].objects[4].instances[0].modeltransform =
                        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -radius)) * glm::rotate(glm::mat4(1.0f), teapotAngle, glm::vec3(0.0f, 1.0f, 0.0f));
                }
            },

            // SizeCallback - called when the window is resized
            nullptr,

            // KeyCallback - called when a key is pressed
            nullptr,

            // MousebuttonCallback - called when a mouse button is pressed
            nullptr,

            // ScrollCallback - called when the mouse wheel is scrolled
            [](cg::ManagedWindow* pWindow, double xoffset, double yoffset, int mods)
            {
                int shiftPressed = glfwGetKey(pWindow->pWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
                    glfwGetKey(pWindow->pWindow, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
                if (shiftPressed)
                {

                    if (!pWindow->passes[0].lights.empty())
                    {
                        for (auto& light : pWindow->passes[0].lights)
                        {
                            light.onScroll(xoffset, yoffset);
                        }
                    }
                }
                else
                {

                    for (auto& camera : pWindow->passes[0].cameras)
                    {
                        camera.onScroll(xoffset, yoffset);
                    }
                }
            },

            // CursorposCallback - called when the mouse is moved
            [](cg::ManagedWindow* pWindow, int xpos, int ypos, double xoffset, double yoffset, int mods)
            {
                int shiftPressed = glfwGetKey(pWindow->pWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
                    glfwGetKey(pWindow->pWindow, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;

                if (pWindow->context.input.lbutton_down && (xoffset != 0.0 || yoffset != 0.0))
                {
                    if (shiftPressed)
                    {
                        if (!pWindow->passes[0].lights.empty())
                        {
                            for (auto& light : pWindow->passes[0].lights)
                            {
                                light.onCursor(xoffset, yoffset);
                            }
                        }
                    }
                    else
                    {
                        if (!pWindow->passes[0].cameras.empty())
                        {
                            for (auto& camera : pWindow->passes[0].cameras)
                            {
                                camera.onCursor(xoffset, yoffset);
                            }
                        }
                    }
                }
            },
            // MouseclickCallback
            nullptr,

            // MousedoubleclickCallback - double-click to copy this window into the main view
            [](cg::ManagedWindow* pWindow, int xpos, int ypos)
            {
                cg::WindowManager::getInstance().copyWindow(*pWindow);
                 }

            });


        windowManager.addWindow({
          cg::State::SHOW,            // State
          pMainWindow,                // Parent window
          { i++, SIZEOFGRID, GAP },   // Viewport (updated to use grid positioning)
          "Environment Mapping",      // Title
          BLACK,                      // Background color
          //{                           // Textures
          //    {
          //        "Pisa/pisa_posx.hdr", "Pisa/pisa_negx.hdr",
          //        "Pisa/pisa_posy.hdr", "Pisa/pisa_negy.hdr",
          //        "Pisa/pisa_posz.hdr", "Pisa/pisa_negz.hdr"
          //    }
          //},
          NONE,
          NONE,                       // Compute shader
          {                           // Rendering passes
              {                           // Pass 0 - Skybox
                  { MARGIN },                 // Viewport
                  NONE,                       // Background color
                  DISABLE,                    // Depth test (disabled for skybox)
                  { { SKYBOX } },             // Objects
                  { { { 3.0f, 2.0f, 0.0f } } },       // Cameras
                  NONE,                       // Lights
                  DEFAULT,                    // Rendering target
                  {                           // Vertex and fragment shaders
                      #include "Shaders/Skybox.vert.glsl"
                      ,
                      #include "Shaders/Skybox.frag.glsl"
                  }
              },
              {                           // Pass 1 - Objects with environment mapping
                  { MARGIN },                 // Viewport
                  NONE,                       // Background color
                  ENABLE,                     // Depth test
                  DEMOOBJECTS_ENVIRONMENTMAPPING,    // Objects
                  { { { 3.0f, 2.0f, 0.0f } } },        // Cameras
                  NONE,                       // Lights
                  DEFAULT,                    // Rendering target
                  {                           // Vertex and fragment shaders
                      #include "Shaders/ReflectionAndRefraction.vert.glsl"
                      ,
                      #include "Shaders/ReflectionAndRefraction.frag.glsl"
                  }
              }
          },

            // FocusCallback - called when the window gains or loses focus
            [](cg::ManagedWindow* pWindow, int focused)
            {
                if (focused)
                {
                    std::cout << std::endl << R"(
Assignment 5 (Environment Mapping)
This demo showcases the environment mapping technique that uses cube maps to 
simulate reflective and transparent surfaces along with rendering the surrounding 
environment. The cube map is applied to an object's surface to approximate how 
the environment would appear in its reflection. For transparent objects, light 
rays passing through are refracted slightly, creating a realistic distortion effect.
Controls:
   - Orbit Object: Drag with the left mouse button; scroll to zoom.
   - Change Reflection Factor: Hold Ctrl and scroll the mouse wheel.
   - Change Ratio Of Indices Of Refraction: Hold Alt and scroll the mouse wheel.
   - key 1,2,3 to display sphere, torus, and cube
)";
                }
            },

            // InitCallback - called for initializing shaders and setup
            [](cg::ManagedWindow* pWindow, int pass)
            {

                if (pass == -1) // Initial setup
                {
                    pWindow->context.floatparam[0] = 0.7f;  // Reflection factor
                    pWindow->context.floatparam[1] = 1.5f;  // Ratio of indices of refraction

                    pWindow->passes[1].renderShader.setUniform("material.reflectionFactor", pWindow->context.floatparam[0]);
                    pWindow->passes[1].renderShader.setUniform("material.eta", pWindow->context.floatparam[1]);

                    //object visibility
                    pWindow->passes[1].objects[0].instances[0].isVisible = true;  // S
                    pWindow->passes[1].objects[1].instances[0].isVisible = false; // T
                    pWindow->passes[1].objects[2].instances[0].isVisible = false; // C
                }
                else if (pass == 1) // Each frame update
                {
                    pWindow->passes[1].renderShader.use();
                    pWindow->passes[1].renderShader.setUniform("material.reflectionFactor", pWindow->context.floatparam[0]);
                    pWindow->passes[1].renderShader.setUniform("material.eta", pWindow->context.floatparam[1]);
                }
            },

            // SizeCallback - called when the window is resized
            nullptr,

            // KeyCallback - called when a key is pressed
            [](cg::ManagedWindow* pWindow, int key)
            {
                switch (key)
                {
                case '1':
                    pWindow->context.intparam[0] = 0;
                    pWindow->passes[1].objects[0].instances[0].isVisible = true;  // S
                    pWindow->passes[1].objects[1].instances[0].isVisible = false; // T
                    pWindow->passes[1].objects[2].instances[0].isVisible = false; // C

                    return false;

                case '2':
                    pWindow->context.intparam[0] = 1;
                    pWindow->passes[1].objects[0].instances[0].isVisible = false; // S
                    pWindow->passes[1].objects[1].instances[0].isVisible = true;  // T
                    pWindow->passes[1].objects[2].instances[0].isVisible = false; // C

                    return false;

                case '3': // Switch to cube
                    pWindow->context.intparam[0] = 2;
                    pWindow->passes[1].objects[0].instances[0].isVisible = false; // S
                    pWindow->passes[1].objects[1].instances[0].isVisible = false; // T
                    pWindow->passes[1].objects[2].instances[0].isVisible = true;  // C

                    return false;
                }
                return true;
            },


            // MousebuttonCallback - called when a mouse button is pressed
            nullptr,

            // ScrollCallback - called when the mouse wheel is scrolled
            [](cg::ManagedWindow* pWindow, double xoffset, double yoffset, int mods)
            {

                if (mods == GLFW_MOD_CONTROL)
                {
                    //how many steps lol
                    pWindow->context.floatparam[0] += (yoffset > 0.0 ? 0.05f : -0.05f);
                    pWindow->context.floatparam[0] = glm::clamp(pWindow->context.floatparam[0], 0.0f, 1.0f);

                    for (auto& object : pWindow->passes[1].objects)
                    {
                        object.instances[0].material.params["material.reflectionFactor"] = { pWindow->context.floatparam[0] };
                    }
                }
                else if (mods == GLFW_MOD_ALT)
                {
                    //eta = n_air / n_glass stuffs
                    pWindow->context.floatparam[1] += (yoffset > 0.0 ? 0.01f : -0.01f);
                    pWindow->context.floatparam[1] = glm::clamp(pWindow->context.floatparam[1], 0.1f, 1.0f);


                    for (auto& object : pWindow->passes[1].objects)
                    {
                        object.instances[0].material.params["material.eta"] = { pWindow->context.floatparam[1] };
                    }


                }
                else
                {
                    //cameraaaaaaaaaaaaaaaaaaaa
                    pWindow->passes[0].cameras[0].onScroll(xoffset, yoffset);
                    pWindow->passes[1].cameras[0].onScroll(xoffset, yoffset);
                }
             },

            // CursorposCallback - called when the mouse is moved
            [](cg::ManagedWindow* pWindow, int xpos, int ypos, double xoffset, double yoffset, int mods)
            {
                //moving cameraaaaaaaaaaaaaaaa
                pWindow->passes[0].cameras[0].onCursor(xoffset, yoffset);
                pWindow->passes[1].cameras[0].onCursor(xoffset, yoffset);
            },
            // MouseclickCallback
            nullptr,

            // MousedoubleclickCallback - double-click to copy this window into the main view
            [](cg::ManagedWindow* pWindow, int xpos, int ypos)
            {
                cg::WindowManager::getInstance().copyWindow(*pWindow);
            }

            });

#define FOG { 0.0f, 0.0f, 0.0f, 1.0f }

        windowManager.addWindow({
                    cg::State::SHOW,            // State
                    pMainWindow,                // Parent window
                    { i++, SIZEOFGRID, GAP },                    // Viewport
                    "Assignment 8 (NightSky)",  // Title
                    BLACK,                      // Background color
                    {                           // Textures
                        cg::Texture{
                            "NightSky/night_negx.png", "NightSky/night_posx.png",
                            "NightSky/night_posy.png", "NightSky/night_negy.png",
                            "NightSky/night_posz.png", "NightSky/night_negz.png"
                        }
                    },
                    NONE,                       // Compute shader
                    {                           // Rendering passes
                        {                           // Pass 0
                            { MARGIN },                 // Viewport
                            NONE,                       // Background color
                            ENABLE,                     // Depth test
                            { { SKYBOX } },             // Objects
                            { {{ 6.0f, 0.0f, 4.0f }} }, // Cameras
                            NONE,                       // Lights
                            DEFAULT,                    // Rendering target
                            {                           // Vertex and fragment shaders
                                #include "Shaders/Skybox.vert.glsl"
                                ,
                                #include "Shaders/Nightsky.frag.glsl"
                            }
                        },
                        {                           // Pass 1
                            { MARGIN },                 // Viewport
                            NONE,                       // Background color
                            ENABLE,                     // Depth test
                             {                           // Objects
                                {
                                    PLANE,
                                    MATERIAL_CHECKERBOARD,
                                    glm::translate(glm::scale(glm::mat4(1.0f), {3.0f, 1.0f, 3.0f}), {0.0f, -1.0f, 0.0f})
                                },
                                {
                                    SPHERE,
                                    MATERIAL_PBR_PHONG,
                                    glm::translate(glm::mat4(1.0f), {3.0f, 0.0f, 0.0f})
                                },
                                {
                                    CUBE,
                                    MATERIAL_DISCARD,
                                    glm::translate(glm::mat4(1.0f), {-3.0f, 0.0f, 0.0f})
                                },
                                {
                                    TEAPOT,
                                    MATERIAL_CARTOON,
                                    glm::translate(glm::mat4(1.0f), {0.0f, -0.5f, 0.0f})
                                }
                            },
                            { {{ 6.0f, 0.0f, 4.0f }} }, // Cameras
                            { {{ 4.0f, 3.0f, 4.0f }} }, // Lights
                            DEFAULT,                    // Rendering target
                            {                           // Vertex and fragment shaders
                                #include "Shaders/ViewSpaceTransform.vert.glsl" 
                                ,
                                #include "Shaders/Miscellaneous.frag.glsl" 
                            }
                        }
                    },



            // FocusCallback - called when the window gains or loses focus
            [](cg::ManagedWindow* pWindow, int focused)
            {
                if (focused)
            {
                std::cout << std::endl << R"(
Assignment 8 (NightSky)
   This demo combines multiple rendering techniques into a single multi-pass
   process. It integrates the skybox and procedural textures from the previous
   assignments with new features, including fog, cartoon style shading, and
   fragment discarding. Two types of camera movement are also demonstrated:
   orbiting and walking.
Controls:
   - Orbit Object: Drag with the left mouse button; scroll to zoom.
   - Orbit Light: Hold Shift and drag; scroll to adjust distance.
   - Rendering Options: Press W or O to switch the camera movement types.
)";
                    }
                },

            // InitCallback - called for initializing shaders across
            // the entire rendering process or individual passes when required
            [](cg::ManagedWindow* pWindow, int pass)
            {
                if (pass == -1)
                {
                    // Fog parameters
                    pWindow->context.floatparam[0] = 8.0f;   // fog.minDist
                    pWindow->context.floatparam[1] = 20.0f;  // fog.maxDist
                    pWindow->context.vec4param[0] = FOG;     // fog.color

                    pWindow->passes[1].renderShader.use();
                    pWindow->passes[1].renderShader.setUniform("fog.minDist", pWindow->context.floatparam[0]);
                    pWindow->passes[1].renderShader.setUniform("fog.maxDist", pWindow->context.floatparam[1]);
                    pWindow->passes[1].renderShader.setUniform("fog.color", pWindow->context.vec4param[0]);


                    // Spotlight parameters
                    pWindow->passes[1].renderShader.setUniform("light[0].Direction", glm::vec3(0.0f, -1.0f, 0.0f));
                    pWindow->passes[1].renderShader.setUniform("light[0].Exponent", 1.0f);
                    pWindow->passes[1].renderShader.setUniform("light[0].Cutoff", 0.45f);
                }
            },

            // SizeCallback - called when the window is resized
            nullptr,

            // KeyCallback - called when a key is pressed
            [](cg::ManagedWindow* pWindow, int key)
            {
                switch (key)
                {
                case 'W':
                    std::cout << "Option W - Walking camera" << std::endl;
                    pWindow->passes[0].cameras[0].setType(cg::CameraType::WALKING);
                    pWindow->passes[1].cameras[0].setType(cg::CameraType::WALKING);

                    return false; // Do not propagate the event

                case 'O':
                    std::cout << "Option O - Orbiting camera" << std::endl;
                    pWindow->passes[0].cameras[0].setType(cg::CameraType::ORBITING);
                    pWindow->passes[1].cameras[0].setType(cg::CameraType::ORBITING);

                    return false; // Do not propagate the event
                }
                return true;
            },



            // MousebuttonCallback - called when a mouse button is pressed
            nullptr,

            // ScrollCallback - called when the mouse wheel is scrolled
            [](cg::ManagedWindow* pWindow, double xoffset, double yoffset, int mods)
            {
                if (mods == GLFW_MOD_SHIFT)
                {
                    // Shift n scroll = adjust light distance
                    pWindow->passes[1].lights[0].onScroll(xoffset, yoffset);
                }
                else
                {
                    pWindow->passes[0].cameras[0].onScroll(xoffset, yoffset);
                    pWindow->passes[1].cameras[0].onScroll(xoffset, yoffset);
                }
            },


            // CursorposCallback - called when the mouse is moved
            [](cg::ManagedWindow* pWindow, int xpos, int ypos,
             double xoffset, double yoffset, int mods)
          {
              if (mods == GLFW_MOD_SHIFT)
              {

                  pWindow->passes[1].lights[0].onCursor(xoffset, yoffset);
              }
              else
              {

                  pWindow->passes[0].cameras[0].onCursor(xoffset, yoffset);
                  pWindow->passes[1].cameras[0].onCursor(xoffset, yoffset);
              }
          },
          // MouseclickCallback
          nullptr,

          // MousedoubleclickCallback - double-click to copy this window into the main view
          [](cg::ManagedWindow* pWindow, int xpos, int ypos)
          {
              cg::WindowManager::getInstance().copyWindow(*pWindow);
                 }

            });















        GLuint texture0 = 0;  // Phong render
        GLuint texture1 = 0;  // Blur vertical
        GLuint texture2 = 0;  // Blur horizontal
        GLuint texture3 = 0;  // Edge filter
        GLuint texture4 = 0;  // Cartoon render
        windowManager.addWindow({
            cg::State::SHOW,                // State
            pMainWindow,                // Parent window
            { i++, SIZEOFGRID, GAP },                       // Viewport
            "Assignment 9 (Cartoon)",                  // Title
            BLACK,                          // Background color
            {                               // Textures
                { texture0, GL_TEXTURE0 },
                { texture1, GL_TEXTURE1 },
                { texture2, GL_TEXTURE2 },
                { texture3, GL_TEXTURE3 },
                { texture4, GL_TEXTURE4 }
            },
            NONE,                           // Compute shader
            {                               // Rendering passes
                {                               // Pass 0
                    { MARGIN },                     // Viewport
                    BLACK,                          // Background color
                    ENABLE,                         // Depth test
                    {                           // Objects
                        { TEAPOT, MATERIAL_OBJECT, glm::translate(glm::mat4(1.0f), {0.0f, -0.5f, 0.0f}) },
                        { PLANE,  MATERIAL_PLANE,  glm::translate(glm::mat4(1.0f), { 0.0f, -1.1f, 0.0f }) }
                    },
                    { DEFAULT },                    // Cameras
                    { DEFAULT },                    // Lights
                    {                               // Rendering target
                        cg::FBO::Type::Texture,
                        GL_TEXTURE0,
                        &texture0
                    },
                    {                               // Vertex and fragment shaders
                        #include "Shaders/ViewSpaceTransform.vert.glsl"
                        ,
                        #include "Shaders/PhongShading.frag.glsl"
                    }
                },
                {                               // Pass 1
                    { MARGIN },                     // Viewport
                    NONE,                           // Background color
                    DISABLE,                        // Depth test
                    { { QUAD } },                   // Objects
                    NONE,                           // Cameras
                    NONE,                           // Lights
                    {                               // Rendering target
                        cg::FBO::Type::Texture,
                        GL_TEXTURE1,
                        &texture1
                    },
                    {                               // Vertex and fragment shaders
                        #include "Shaders/PassThrough.vert.glsl"
                        ,
                        #include "Shaders/BlurFilter.1.frag.glsl"
                    }
                },
                {                               // Pass 2 - Gaussian blur (horizontal)
                    { MARGIN },                     // Viewport
                    NONE,                           // Background color
                    DISABLE,                        // Depth test
                    { { QUAD } },                   // Objects
                    NONE,                           // Cameras
                    NONE,                           // Lights
                    {                               // Rendering target
                        cg::FBO::Type::Texture,
                        GL_TEXTURE2,
                        &texture2
                    },
                    {                               // Vertex and fragment shaders
                        #include "Shaders/PassThrough.vert.glsl"
                        ,
                        #include "Shaders/BlurFilter.2.frag.glsl"
                    }
                },
                {                               // Pass 3 - Edge filter (Sobel)
                    { MARGIN },                     // Viewport
                    NONE,                           // Background color
                    DISABLE,                        // Depth test
                    { { QUAD } },                   // Objects
                    NONE,                           // Cameras
                    NONE,                           // Lights
                    {                               // Rendering target
                        cg::FBO::Type::Texture,
                        GL_TEXTURE3,
                        &texture3
                    },
                    {                               // Vertex and fragment shaders
                        #include "Shaders/PassThrough.vert.glsl"
                        ,
                        #include "Shaders/EdgeFilter.frag.glsl"

                    }
                },
                {                               // Pass 4 - Cartoon render
                    { MARGIN },                     // Viewport
                    WHITE,                          // Background color
                    ENABLE,                         // Depth test
                    {                           // Objects
                        { TEAPOT, MATERIAL_OBJECT, glm::translate(glm::mat4(1.0f), {0.0f, -0.5f, 0.0f}) },
                        { PLANE,  MATERIAL_PLANE, glm::translate(glm::mat4(1.0f), { 0.0f, -1.1f, 0.0f })}
                    },
                    { DEFAULT },                    // Cameras
                    { DEFAULT },                    // Lights
                    {                               // Rendering target
                        cg::FBO::Type::Texture,
                        GL_TEXTURE4,
                        &texture4
                    },
                    {                               // Vertex and fragment shaders
                        #include "Shaders/ViewSpaceTransform.vert.glsl"
                        ,
                        #include "Shaders/Cartoon.frag.glsl"
                    }
                },
                {                               // Pass 5 - Composite (cartoon + edges)
                    { MARGIN },                     // Viewport
                    NONE,                           // Background color
                    DISABLE,                        // Depth test
                    { { QUAD } },                   // Objects
                    NONE,                           // Cameras
                    NONE,                           // Lights
                    DEFAULT,                        // Rendering target (screen)
                    {                               // Vertex and fragment shaders
                        #include "Shaders/PassThrough.vert.glsl"
                        ,
                        #include "Shaders/Composite.frag.glsl"
                    }
                }
            },

            // FocusCallback - called when the window gains or loses focus
            [](cg::ManagedWindow* pWindow, int focused)
            {
                if (focused)
            {
                std::cout << std::endl << R"(
Assignment 9 (Cartoon)
   This demo showcases a multi-pass cartoon-style rendering pipeline that creates
   non-photorealistic effects with crisp edge outlines. The process applies a
   Gaussian blur filter to reduce noise, followed by Sobel edge detection to
   identify object boundaries. The scene is rendered twice: once with standard
   Blinn-Phong shading for edge analysis, and again with cartoon-style shading
   for the final cell-shaded appearance. The final pass composites the edge
   outlines with the cartoon shading to produce a stylized 2D animation effect.
Controls:
   - Orbit Object: Drag with the left mouse button; scroll to zoom.
   - Orbit Light: Hold Shift and drag; scroll to adjust distance.
   - Blur Filter: Press 1 to enable and 2 to disable.
   - Edge Filter: Press 3 to enable and 4 to disable.
   - Edge Threshold: Hold Alt and scroll to adjust.
   - Alpha Factor: Hold Control and scroll to adjust.
)";
                    }
                },

            // InitCallback - called for initializing shaders across
            // the entire rendering process or individual passes when required
            [](cg::ManagedWindow* pWindow, int pass)
            {
                // Initialize context values ONLY on the first pass (pass 0)
                if (pass == -1)
                {

                    pWindow->context.floatparam[2] = 0.1f; // Edge threshold
                    pWindow->context.floatparam[3] = 0.0f; // Alpha factor
                    pWindow->context.intparam[0] = 0; // blur off
                    pWindow->context.intparam[1] = 1; // edge on
                    return;
                }
                pWindow->passes[pass].renderShader.use();
                switch (pass)
                {
                case 3:
                    pWindow->passes[pass].renderShader.setUniform("EdgeThreshold", pWindow->context.floatparam[2]);
                    pWindow->passes[pass].renderShader.setUniform("EdgeEnabled", (bool)pWindow->context.intparam[1]);
                    break;

                case 4: // black bg when edge disabled, white bg when enabled
                    if ((bool)pWindow->context.intparam[1])
                        pWindow->passes[4].backgroundColor = {1.0f, 1.0f, 1.0f, 1.0f};
                    else
                        pWindow->passes[4].backgroundColor = {0.0f, 0.0f, 0.0f, 1.0f};
                    break;

                case 5:
                    pWindow->passes[pass].renderShader.setUniform("AlphaFactor", pWindow->context.floatparam[3]);
                    pWindow->passes[pass].renderShader.setUniform("BlurEnabled", (bool)pWindow->context.intparam[0]);
                    pWindow->passes[pass].renderShader.setUniform("EdgeEnabled", (bool)pWindow->context.intparam[1]);
                    break;

                default:
                    if (pass == 1 || pass == 2)
                        pWindow->passes[pass].renderShader.setUniform("BlurEnabled", (bool)pWindow->context.intparam[0]);
                    break;
                }
            },
            // SizeCallback - called when the window is resized
            nullptr,

            // KeyCallback - called when a key is pressed
            [](cg::ManagedWindow* pWindow, int key)
            {
                switch (key)
                {
                case '1':
                    std::cout << "Blur filter enabled" << std::endl;
                    pWindow->context.intparam[0] = 1;

                    pWindow->passes[1].renderShader.use();
                    pWindow->passes[1].renderShader.setUniform("BlurEnabled", true);

                    pWindow->passes[2].renderShader.use();
                    pWindow->passes[2].renderShader.setUniform("BlurEnabled", true);
                    return false;

                case '2':
                    std::cout << "Blur filter disabled" << std::endl;
                    pWindow->context.intparam[0] = 0;

                    pWindow->passes[1].renderShader.use();
                    pWindow->passes[1].renderShader.setUniform("BlurEnabled", false);

                    pWindow->passes[2].renderShader.use();
                    pWindow->passes[2].renderShader.setUniform("BlurEnabled", false);
                    return false;

                case '3':
                    std::cout << "Edge filter enabled" << std::endl;
                    pWindow->context.intparam[1] = 1;

                    pWindow->passes[3].renderShader.use();
                    pWindow->passes[3].renderShader.setUniform("EdgeEnabled", true);
                    return false;

                case '4':
                    std::cout << "Edge filter disabled" << std::endl;
                    pWindow->context.intparam[1] = 0;

                    pWindow->passes[3].renderShader.use();
                    pWindow->passes[3].renderShader.setUniform("EdgeEnabled", false);
                    return false;
                }
                return true;
            },



            // MousebuttonCallback - called when a mouse button is pressed
            nullptr,

            // ScrollCallback - called when the mouse wheel is scrolled
            [](cg::ManagedWindow* pWindow, double xoffset, double yoffset, int mods)
            {
                if (mods == GLFW_MOD_SHIFT)
                {

                    pWindow->passes[0].lights[0].onScroll(xoffset, yoffset);
                    pWindow->passes[4].lights[0].onScroll(xoffset, yoffset);
                }
                else if (mods == GLFW_MOD_ALT)
                {

                    pWindow->context.floatparam[2] += (yoffset > 0.0 ? 0.005f : -0.05f);
                    pWindow->context.floatparam[2] = glm::clamp(pWindow->context.floatparam[2], 0.0f, 1.0f);


                    pWindow->passes[3].renderShader.use();
                    pWindow->passes[3].renderShader.setUniform("EdgeThreshold", pWindow->context.floatparam[2]);
                    std::cout << "EdgeThreshold: " << pWindow->context.floatparam[2] << "\n";
                }
                else if (mods == GLFW_MOD_CONTROL)
                {

                    pWindow->context.floatparam[3] += (yoffset > 0.0 ? 0.05f : -0.05f);
                    pWindow->context.floatparam[3] = glm::clamp(pWindow->context.floatparam[3], 0.0f, 1.0f);


                    pWindow->passes[5].renderShader.use();
                    pWindow->passes[5].renderShader.setUniform("AlphaFactor", pWindow->context.floatparam[3]);
                    std::cout << "AlphaFactor: " << pWindow->context.floatparam[3] << "\n";
                }
                else
                {
                    pWindow->passes[0].cameras[0].onScroll(xoffset, yoffset);
                    pWindow->passes[4].cameras[0].onScroll(xoffset, yoffset);
                }
            },


            // CursorposCallback - called when the mouse is moved
            [](cg::ManagedWindow* pWindow, int xpos, int ypos,
             double xoffset, double yoffset, int mods)
          {
              if (mods == GLFW_MOD_SHIFT)
              {
                  pWindow->passes[0].lights[0].onCursor(xoffset, yoffset);
                  pWindow->passes[4].lights[0].onCursor(xoffset, yoffset);
              }
              else
              {
                  pWindow->passes[0].cameras[0].onCursor(xoffset, yoffset);
                  pWindow->passes[4].cameras[0].onCursor(xoffset, yoffset);
              }
          },
          // MouseclickCallback
          nullptr,

          // MousedoubleclickCallback - double-click to copy this window into the main view
          [](cg::ManagedWindow* pWindow, int xpos, int ypos)
          {
              cg::WindowManager::getInstance().copyWindow(*pWindow);
                 }


            });


            windowManager.addWindow({
              cg::State::SHOW,            // State
              pMainWindow,                // Parent window
              { i++, SIZEOFGRID, GAP },   // Viewport (updated to use grid positioning)
              "Assignment 10 (Portal)",      // Title
              BLACK,                      // Background color
              {                           // Textures
                  {
                      "Pisa/pisa_posx.hdr", "Pisa/pisa_negx.hdr",
                      "Pisa/pisa_posy.hdr", "Pisa/pisa_negy.hdr",
                      "Pisa/pisa_posz.hdr", "Pisa/pisa_negz.hdr"
                  }
              },
              NONE,                       // Compute shader
              {                           // Rendering passes
                  {                           // Pass 0 - Skybox (cubemap background)
                      { MARGIN },                 // Viewport
                      NONE,                       // Background color
                      DISABLE,                    // Depth test (disabled for skybox)
                      { { SKYBOX } },             // Objects
                      { { { -4.0f, 0.0f, 0.0f }, 50.0f } },       // Cameras
                      NONE,                       // Lights
                      DEFAULT,                    // Rendering target
                      {                           // Vertex and fragment shaders
                          #include "Shaders/Skybox1.vert.glsl"
                          ,
                          #include "Shaders/Skybox1.frag.glsl"
                      }
                  },
                  {                           // Pass 1 - Portal quad
                      { MARGIN },                 // Viewport
                      NONE,                       // Background color
                      ENABLE,                     // Depth test enabled for 3D placement
                      {                           // Objects: a quad scaled to portal size
                          {
                              QUAD,
                              { },
                              glm::scale(
                                  glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
                                  glm::vec3(1.5f)
                              ),
                              VISIBLE
                          }
                      },
                      { { { -8.0f, 0.0f, 0.0f }, 50.0f } },       // Same camera position as skybox
                      NONE,                       // Lights
                      DEFAULT,                    // Rendering target
                      {                           // Vertex and fragment shaders
                          #include "Shaders/ProjectedTexture.vert.glsl"
                          ,
                          #include "Shaders/ProjectedTexture.frag.glsl"
                      }
                  },
              },

                // FocusCallback - called when the window gains or loses focus
                [](cg::ManagedWindow* pWindow, int focused)
                {
                    if (focused)
                    {
                        std::cout << std::endl << R"(
Assignment 10 (Portal)
   This demo showcases the integration of cube mapping and projected texture
   techniques with dynamic texture rendering. Cube mapping is used to render
   the skybox, while projected textures visualize a gate (portal) to another
   dimension or location in the world.
Controls:
   - Rotating And Moving Camera: Drag with the left mouse button and scroll
     the mouse wheel.
   - Open Portal: Click the left mouse button at the desired location to
     place the new portal.
)";
                }
            },

                // InitCallback - called for initializing shaders and setup
                [](cg::ManagedWindow* pWindow, int pass)
                {
                    if (pass == -1) // Initial setup
                    {
                        pWindow->context.floatparam[0] = 0.7f;  // Reflection factor
                        pWindow->context.floatparam[1] = 1.5f;  // Ratio of indices of refraction

                        // Portal starts at the origin; stored so click can move it
                        pWindow->context.vec4param[0] = glm::vec4(0.0f);

                        // Start both cameras in walking mode
                        pWindow->passes[0].cameras[0].setType(cg::CameraType::WALKING);
                        pWindow->passes[1].cameras[0].setType(cg::CameraType::WALKING);

                        // Seed iTime on the portal shader
                        pWindow->passes[1].renderShader.use();
                        pWindow->passes[1].renderShader.setUniform("iTime", 0.0f);

                        glm::mat4 projView = glm::lookAt(
                            glm::vec3(3.0f, 7.0f, 7.0f),
                            glm::vec3(-2.0f, -4.0f, 0.0f),
                            glm::vec3(0.0f, 1.0f, 0.0f));
                        glm::mat4 projProj = glm::perspective(glm::radians(30.0f), 1.0f, 0.2f, 1000.0f);
                        glm::mat4 bias = glm::scale(
                            glm::translate(glm::mat4(1.0f), glm::vec3(0.5f)),
                            glm::vec3(0.5f));

                        pWindow->passes[1].renderShader.use();
                        pWindow->passes[1].renderShader.setUniform("ProjectorMatrix", bias * projProj * projView);
                    }
                    else if (pass == 0)
                    {
                        glDisable(GL_BLEND);
                    }
                    else if (pass == 1)
                    {
                        glEnable(GL_BLEND);
                        glBlendFunc(GL_ONE, GL_ONE);

                        pWindow->passes[1].renderShader.use();
                        pWindow->passes[1].renderShader.setUniform("iTime",
                            static_cast<float>(glfwGetTime()));
                    }
                },

                // SizeCallback - called when the window is resized
                nullptr,

                // KeyCallback - called when a key is pressed
                [](cg::ManagedWindow* pWindow, int key)
                {
                    return true;
                },


                // MousebuttonCallback - left click places portal following the mouse
                [](cg::ManagedWindow* pWindow, int button, int xpos, int ypos)
                {
                    if (button != GLFW_MOUSE_BUTTON_LEFT)
                        return;

                    // Read depth buffer under mouse
                    float depth = 1.0f;
                    glReadPixels(xpos, pWindow->height - ypos, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

                    // depth == 1.0 means no geometry was hit
                    // depth = f*(d-n) / ((f-n)*d), n=0.5, f=200, d=8
                    if (depth >= 1.0f)
                        depth = 200.0f * (8.0f - 0.5f) / ((200.0f - 0.5f) * 8.0f);

                    // Convert to world coordinates
                    glm::vec3 pos = glm::unProject(
                        glm::vec3(xpos, pWindow->height - ypos, depth),
                        pWindow->passes[1].cameras[0].getLookAt(),
                        pWindow->passes[1].cameras[0].getPerspective(
                            static_cast<float>(pWindow->passes[1].width)
                            / pWindow->passes[1].height),
                        glm::vec4(0, 0, pWindow->width, pWindow->height)
                    );

                    glm::vec3 camPos = pWindow->passes[1].cameras[0].getPosition();

                    // Rotate portal to face the camera
                    glm::vec3 toCam = camPos - pos;
                    float angle = atan2f(toCam.x, toCam.z);

                    pWindow->passes[1].objects[0].instances[0].modeltransform =
                        glm::translate(glm::mat4(1.0f), pos) *
                        glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f)) *
                        glm::scale(glm::mat4(1.0f), glm::vec3(1.5f));
                },

                // ScrollCallback - called when the mouse wheel is scrolled
                [](cg::ManagedWindow* pWindow, double xoffset, double yoffset, int mods)
                {
                    // both cameras together so skybox and portal stay in sync
                    pWindow->passes[0].cameras[0].onScroll(xoffset, yoffset);
                    pWindow->passes[1].cameras[0].onScroll(xoffset, yoffset);
                },

                // CursorposCallback - called when the mouse is moved
                [](cg::ManagedWindow* pWindow, int xpos, int ypos, double xoffset, double yoffset, int mods)
                {
                    // both cameras together so skybox and portal stay in sync
                    pWindow->passes[0].cameras[0].onCursor(xoffset, yoffset);
                    pWindow->passes[1].cameras[0].onCursor(xoffset, yoffset);
                },
                // MouseclickCallback
                nullptr,

                // MousedoubleclickCallback - double-click to copy this window into the main view
                [](cg::ManagedWindow* pWindow, int xpos, int ypos)
                {
                    cg::WindowManager::getInstance().copyWindow(*pWindow);
                 }

             });
#define ANINSTANCE(x, z) { MATERIAL_OBJECT, glm::translate(glm::mat4(1.0f), {x, 0.0f, z}) }

#define INSTANCES(x)\
                            ANINSTANCE(x, -20.0f-5.0f),\
                            ANINSTANCE(x, -18.0f-5.0f),\
                            ANINSTANCE(x, -16.0f-5.0f),\
                            ANINSTANCE(x, -14.0f-5.0f),\
                            ANINSTANCE(x, -12.0f-5.0f),\
                            ANINSTANCE(x, -10.0f-5.0f),\
                            ANINSTANCE(x,  -8.0f-5.0f),\
                            ANINSTANCE(x,  -6.0f-5.0f),\
                            ANINSTANCE(x,  -4.0f-5.0f),\
                            ANINSTANCE(x,  -2.0f-5.0f),\
                            ANINSTANCE(x,   0.0f-5.0f),\
                            ANINSTANCE(x,   2.0f-5.0f),\
                            ANINSTANCE(x,   4.0f-5.0f),\
                            ANINSTANCE(x,   6.0f-5.0f),\
                            ANINSTANCE(x,   8.0f-5.0f),\
                            ANINSTANCE(x,  10.0f-5.0f),\
                            ANINSTANCE(x,  12.0f-5.0f),\
                            ANINSTANCE(x,  14.0f-5.0f),\
                            ANINSTANCE(x,  16.0f-5.0f),\
                            ANINSTANCE(x,  18.0f-5.0f),\
                            ANINSTANCE(x,  20.0f-5.0f)

                //stack overflow warning :( even tho i alr did the properties adjustment T_T
                cg::Object ogres("Ogre/bs_ears.obj");
                ogres.instances.clear();
                for (float x = -10.0f; x <= 10.0f; x += 2.0f)
                    for (float z = -15.0f; z <= 15.0f; z += 2.0f)
                        //smaller ogres
                        ogres.instances.push_back({ MATERIAL_OBJECT, glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)) });

             windowManager.addWindow({
                 cg::State::SHOW,            // State
                  pMainWindow,                // Parent window
                 { i++, SIZEOFGRID, GAP },                    // Viewport
                 "Assignment 11 (Ogres)",         // Title
                 BLACK,                      // Background color
                 {                           // Textures (GL_TEXTURE0-2 reserved for G-buffer)
                     cg::Texture{ { "Ogre/ogre_diffuse.png"   }, GL_TEXTURE3 },
                     cg::Texture{ { "Ogre/ogre_normalmap.png" }, GL_TEXTURE4 }
                 },
                 NONE,                       // Compute shader
                 {                           // Rendering passes
                     {                           // Pass 0 - geometry pass (writes G-buffer)
                         { MARGIN },                 // Viewport
                         BLACK,                      // Background color (must be black so gLight==0 marks background)
                         ENABLE,                     // Depth test
                         { ogres },                  // Objects
                         { {{ 0.0f, 5.0f, 5.0f }} }, // Cameras
                         {DEFAULT},                   // Lights
                         { cg::FBO::Type::Deferred }, // G-buffer render target
                         {                               // Vertex and fragment shaders
                             #include "Shaders/NormalMap.vert.glsl"
                             ,
                             #include "Shaders/DeferredShading.0.frag.glsl"
                         }
                     },
                     {                           // Pass 1 - lighting pass (reads G-buffer)
                         { MARGIN },                 // Viewport
                         NONE,                        // Background color
                         DISABLE,                     // No depth test
                         { { QUAD, MATERIAL_OBJECT } }, // Fullscreen quad
                         {DEFAULT},                   // Camera
                         {DEFAULT},                   // Lights
                         DEFAULT,                     // Default render target
                         {                               // Vertex and fragment shaders
                             #include "Shaders/PassThrough.vert.glsl"
                             ,
                             #include "Shaders/DeferredShading.1.frag.glsl"
                         }
                     }
                 },

                 // FocusCallback
                 [](cg::ManagedWindow* pWindow, int focused)
                 {
                     if (focused)
                     {
                         std::cout << std::endl << R"(
Assignment 11 (Ogres)
   This demo showcases both normal mapping and deferred shading techniques.
   Normal mapping enhances the visual complexity of rendered objects without
   increasing geometric detail. Deferred shading improves rendering
   efficiency by avoiding multiple shading calculations for the same pixel,
   making it particularly effective for scenes with a large number of
   objects, as demonstrated here.
Controls:
   - Orbit Object: Drag with the left mouse button; scroll to zoom.
   - Orbit Light: Hold Shift and drag; scroll to adjust distance.
   - Rendering Options: Press 1-3 to change the rendering mode.
)";
                     }
                 },

                 // InitCallback
                [](cg::ManagedWindow* pWindow, int pass)
                {
                    if (pass == 1)
                    {
                        pWindow->passes[1].renderShader.use();
                        pWindow->passes[1].renderShader.use();
                        pWindow->passes[1].renderShader.setUniform("CameraView", pWindow->passes[0].cameras[0].getLookAt());
                    }
                },

                 // SizeCallback
                 nullptr,

                 // KeyCallback - called when a key is pressed
                 [](cg::ManagedWindow* pWindow, int key)
                 {
                     if (key == GLFW_KEY_1 || key == GLFW_KEY_2 || key == GLFW_KEY_3)
                     {
                         int option = key - GLFW_KEY_0;
                         pWindow->passes[1].renderShader.use();
                         pWindow->passes[1].renderShader.setUniform("Option", option);
                     }
                     return true;
                 },

                 // MousebuttonCallback
                 nullptr,

                 // ScrollCallback - scroll zooms camera; Shift+scroll adjusts light distance
                 [](cg::ManagedWindow* pWindow, double xoffset, double yoffset, int mods)
                 {
                     if (mods & GLFW_MOD_SHIFT) 
                     {
                         pWindow->passes[0].lights[0].onScroll(xoffset, yoffset);
                     }
                     else 
                     {
                         pWindow->passes[0].cameras[0].onScroll(xoffset, yoffset);
                     }
                 },

                 // CursorposCallback - drag orbits camera; Shift+drag orbits light
                 [](cg::ManagedWindow* pWindow, int xpos, int ypos, double xoffset, double yoffset, int mods)
                 {
                     if (mods & GLFW_MOD_SHIFT)
                     {
                         pWindow->passes[0].lights[0].onCursor(xoffset, yoffset);
                     }
                     else
                     {
                         pWindow->passes[0].cameras[0].onCursor(xoffset, yoffset);
                     }
                 },

                 // MouseclickCallback
                 nullptr,

                 // MousedoubleclickCallback - double-click to copy this window into the main view
                 [](cg::ManagedWindow* pWindow, int xpos, int ypos)
                 {
                     cg::WindowManager::getInstance().copyWindow(*pWindow);
                 }

                 });
                    
        windowManager.show();

        if (pMainWindow)
            glfwFocusWindow(pMainWindow);

        windowManager.render();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        retcode = -1;
    }

    // Explicit cleanup before GLFW termination
    windowManager.cleanup();

    // Clean up and terminate GLFW
    glfwTerminate();

    return retcode;
}