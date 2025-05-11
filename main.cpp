#include "header.h"
#include <SDL2/SDL.h>

/*
NOTE : You are free to change the code as you wish, the main objective is to make the
       application work and pass the audit.

       It will be provided the main function with the following functions :

       - `void systemWindow(const char *id, ImVec2 size, ImVec2 position)`
            This function will draw the system window on your screen
       - `void memoryProcessesWindow(const char *id, ImVec2 size, ImVec2 position)`
            This function will draw the memory and processes window on your screen
       - `void networkWindow(const char *id, ImVec2 size, ImVec2 position)`
            This function will draw the network window on your screen
*/

// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h> // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h> // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h> // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
#include <glad/gl.h> // Initialize with gladLoadGL(...) or gladLoaderLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE      // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/Binding.h> // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE        // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h> // Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// systemWindow, display information for the system monitorization
void systemWindow(const char *id, ImVec2 size, ImVec2 position)
{
    ImGui::Begin(id);
    ImGui::SetWindowSize(id, size);
    ImGui::SetWindowPos(id, position);

    // Display system information
    ImGui::Text("Operating System:");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", getDetailedOSInfo().c_str());
    
    // Hostname
    ImGui::Text("Computer Name:");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", getHostname().c_str());
    
    ImGui::Text("Logged in User:");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", getLoggedInUser().c_str());
    
    ImGui::Text("CPU Model:");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", CPUinfo().c_str());

    // Performance Monitoring Tabs
    static PerformanceData cpuData, fanData, thermalData;
    static bool initialized = false;

    if (!initialized) {
        initPerformanceData(cpuData);
        initPerformanceData(fanData);
        initPerformanceData(thermalData);
        initialized = true;
    }

    // Update data
    updatePerformanceData(cpuData, getCPUUsage());
    updatePerformanceData(fanData, getFanSpeed());
    updatePerformanceData(thermalData, getCPUTemperature());

    ImGui::Separator();
    if (ImGui::BeginTabBar("PerformanceTabs")) {
        // CPU Tab
        if (ImGui::BeginTabItem("CPU")) {
            ImGui::Text("CPU Usage: %.1f%%", cpuData.currentValue);
            
            ImGui::Checkbox("Pause", &cpuData.isPaused);
            ImGui::SliderInt("FPS", &cpuData.fps, 1, 60);
            ImGui::SliderFloat("Y Scale", &cpuData.yScale, 0.1f, 2.0f);
            
            ImVec2 graphSize(ImGui::GetWindowWidth() - 20, 200);
            ImGui::PlotLines("##CPU", cpuData.values.data(), cpuData.values.size(), 
                           0, NULL, 0.0f, 100.0f * cpuData.yScale, graphSize);
            
            ImGui::EndTabItem();
        }

        // Fan Tab
        if (ImGui::BeginTabItem("Fan")) {
            ImGui::Text("Fan Speed: %d RPM", (int)fanData.currentValue);
            ImGui::Text("Status: %s", fanData.currentValue > 0 ? "Active" : "Inactive");
            ImGui::Text("Level: %d%%", (int)((fanData.currentValue * 100) / 5000));
            
            ImGui::Checkbox("Pause", &fanData.isPaused);
            ImGui::SliderInt("FPS", &fanData.fps, 1, 60);
            ImGui::SliderFloat("Y Scale", &fanData.yScale, 0.1f, 2.0f);
            
            ImVec2 graphSize(ImGui::GetWindowWidth() - 20, 200);
            ImGui::PlotLines("##Fan", fanData.values.data(), fanData.values.size(), 
                           0, NULL, 0.0f, 5000.0f * fanData.yScale, graphSize);
            
            ImGui::EndTabItem();
        }

        // Thermal Tab
        if (ImGui::BeginTabItem("Thermal")) {
            ImGui::Text("Temperature: %.1f°C", thermalData.currentValue);
            
            ImGui::Checkbox("Pause", &thermalData.isPaused);
            ImGui::SliderInt("FPS", &thermalData.fps, 1, 60);
            ImGui::SliderFloat("Y Scale", &thermalData.yScale, 0.1f, 2.0f);
            
            ImVec2 graphSize(ImGui::GetWindowWidth() - 20, 200);
            ImGui::PlotLines("##Thermal", thermalData.values.data(), thermalData.values.size(), 
                           0, NULL, 0.0f, 100.0f * thermalData.yScale, graphSize);
            
            ImGui::EndTabItem();
        }
        
        ImGui::EndTabBar();
    }

    ImGui::End();
}

// memoryProcessesWindow, display information for the memory and processes information
void memoryProcessesWindow(const char *id, ImVec2 size, ImVec2 position)
{
    ImGui::Begin(id);
    ImGui::SetWindowSize(id, size);
    ImGui::SetWindowPos(id, position);

    // student TODO : add code here for the memory and process information

    ImGui::End();
}

// network, display information network information
void networkWindow(const char *id, ImVec2 size, ImVec2 position)
{
    ImGui::Begin(id);
    ImGui::SetWindowSize(id, size);
    ImGui::SetWindowPos(id, position);

    static vector<NetworkStats> networkStats = getAllNetworkStats();
    static time_t lastUpdate = 0;
    time_t currentTime = time(nullptr);
    
    // Update network stats every second
    if (currentTime - lastUpdate >= 1) {
        networkStats = getAllNetworkStats();
        lastUpdate = currentTime;
    }

    // Display network interfaces and their IPv4 addresses
    ImGui::Text("Network Interfaces:");
    for (const auto& stats : networkStats) {
        ImGui::Text("%s: %s", stats.interface.c_str(), stats.ipv4.c_str());
    }

    ImGui::Separator();

    if (ImGui::BeginTabBar("NetworkTabs")) {
        // RX Statistics Tab
        if (ImGui::BeginTabItem("RX Statistics")) {
            if (ImGui::BeginTable("RXTable", 8, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Interface");
                ImGui::TableSetupColumn("Bytes");
                ImGui::TableSetupColumn("Packets");
                ImGui::TableSetupColumn("Errors");
                ImGui::TableSetupColumn("Drops");
                ImGui::TableSetupColumn("FIFO");
                ImGui::TableSetupColumn("Frame");
                ImGui::TableSetupColumn("Multicast");
                ImGui::TableHeadersRow();

                for (const auto& stats : networkStats) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", stats.interface.c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", formatBytes(stats.rx_bytes).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text("%llu", stats.rx_packets);
                    ImGui::TableNextColumn();
                    ImGui::Text("%llu", stats.rx_errs);
                    ImGui::TableNextColumn();
                    ImGui::Text("%llu", stats.rx_drop);
                    ImGui::TableNextColumn();
                    ImGui::Text("%llu", stats.rx_fifo);
                    ImGui::TableNextColumn();
                    ImGui::Text("%llu", stats.rx_frame);
                    ImGui::TableNextColumn();
                    ImGui::Text("%llu", stats.rx_multicast);
                }
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }

        // TX Statistics Tab
        if (ImGui::BeginTabItem("TX Statistics")) {
            if (ImGui::BeginTable("TXTable", 8, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Interface");
                ImGui::TableSetupColumn("Bytes");
                ImGui::TableSetupColumn("Packets");
                ImGui::TableSetupColumn("Errors");
                ImGui::TableSetupColumn("Drops");
                ImGui::TableSetupColumn("FIFO");
                ImGui::TableSetupColumn("Collisions");
                ImGui::TableSetupColumn("Carrier");
                ImGui::TableHeadersRow();

                for (const auto& stats : networkStats) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", stats.interface.c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", formatBytes(stats.tx_bytes).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text("%llu", stats.tx_packets);
                    ImGui::TableNextColumn();
                    ImGui::Text("%llu", stats.tx_errs);
                    ImGui::TableNextColumn();
                    ImGui::Text("%llu", stats.tx_drop);
                    ImGui::TableNextColumn();
                    ImGui::Text("%llu", stats.tx_fifo);
                    ImGui::TableNextColumn();
                    ImGui::Text("%llu", stats.tx_colls);
                    ImGui::TableNextColumn();
                    ImGui::Text("%llu", stats.tx_carrier);
                }
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }

        // Visual Usage Tab
        if (ImGui::BeginTabItem("Visual Usage")) {
            const float maxGB = 2.0f;  // Maximum display value in GB
            const float maxBytes = maxGB * 1024 * 1024 * 1024;  // Convert to bytes

            // RX Progress Bars
            ImGui::Text("RX Usage:");
            for (const auto& stats : networkStats) {
                float progress = static_cast<float>(stats.rx_bytes) / maxBytes;
                progress = std::min(progress, 1.0f);  // Cap at 100%
                
                ImGui::Text("%s: %s", stats.interface.c_str(), formatBytes(stats.rx_bytes).c_str());
                ImGui::ProgressBar(progress, ImVec2(-1, 0));
                ImGui::Text("2.00 GB");
            }

            ImGui::Separator();

            // TX Progress Bars
            ImGui::Text("TX Usage:");
            for (const auto& stats : networkStats) {
                float progress = static_cast<float>(stats.tx_bytes) / maxBytes;
                progress = std::min(progress, 1.0f);  // Cap at 100%
                
                ImGui::Text("%s: %s", stats.interface.c_str(), formatBytes(stats.tx_bytes).c_str());
                ImGui::ProgressBar(progress, ImVec2(-1, 0));
            }
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

// Main code
int main(int, char **)
{
    // Setup SDL
    // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
    // depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window *window = SDL_CreateWindow("System Monitor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
    bool err = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress) == 0; // glad2 recommend using the windowing library loader instead of the (optionally) bundled one.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
    bool err = false;
    glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
    bool err = false;
    glbinding::initialize([](const char *name) { return (glbinding::ProcAddress)SDL_GL_GetProcAddress(name); });
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // render bindings
    ImGuiIO &io = ImGui::GetIO();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // background color
    // note : you are free to change the style of the application
    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        {
            ImVec2 mainDisplay = io.DisplaySize;
            memoryProcessesWindow("== Memory and Processes ==",
                                  ImVec2((mainDisplay.x / 2) - 20, (mainDisplay.y / 2) + 30),
                                  ImVec2((mainDisplay.x / 2) + 10, 10));
            // --------------------------------------
            systemWindow("== System ==",
                         ImVec2((mainDisplay.x / 2) - 10, (mainDisplay.y / 2) + 30),
                         ImVec2(10, 10));
            // --------------------------------------
            networkWindow("== Network ==",
                          ImVec2(mainDisplay.x - 20, (mainDisplay.y / 2) - 60),
                          ImVec2(10, (mainDisplay.y / 2) + 50));
        }

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
