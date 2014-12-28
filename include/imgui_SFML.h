#ifndef IMGUI_SFML_H
#define IMGUI_SFML_H
#include <imgui.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>

static sf::Clock timeElapsed;
static GLuint fontTex;
static bool mousePressed[2] = { false, false };
#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))


// This function was taken from https://github.com/ocornut/imgui/blob/master/examples/opengl_example/main.cpp
// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
// If text or lines are blurry when integrating ImGui in your engine:
// - in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
// - try adjusting ImGui::GetIO().PixelCenterOffset to 0.5f or 0.375f
static void ImImpl_RenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count)
{
    if (cmd_lists_count == 0)
        return;

    // We are using the OpenGL fixed pipeline to make the example code simpler to read!
    // A probable faster way to render would be to collate all vertices from all cmd_lists into a single vertex buffer.
    // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, vertex/texcoord/color pointers.
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    // Setup texture
    glBindTexture(GL_TEXTURE_2D, fontTex);
    glEnable(GL_TEXTURE_2D);

    // Setup orthographic projection matrix
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0f, width, height, 0.0f, -1.0f, +1.0f);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Render command lists
    for (int n = 0; n < cmd_lists_count; n++)
    {
        const ImDrawList* cmd_list = cmd_lists[n];
        const unsigned char* vtx_buffer = (const unsigned char*)&cmd_list->vtx_buffer.front();
        glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, pos)));
        glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, uv)));
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, col)));

        int vtx_offset = 0;
        for (size_t cmd_i = 0; cmd_i < cmd_list->commands.size(); cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->commands[cmd_i];
            glScissor((int)pcmd->clip_rect.x, (int)(height - pcmd->clip_rect.w), (int)(pcmd->clip_rect.z - pcmd->clip_rect.x), (int)(pcmd->clip_rect.w - pcmd->clip_rect.y));
            glDrawArrays(GL_TRIANGLES, vtx_offset, pcmd->vtx_count);
            vtx_offset += pcmd->vtx_count;
        }
    }

    // Restore modified state
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();
}

void imgui_process_event(sf::Event &event)
{
    switch(event.type)
    {
        case sf::Event::MouseButtonPressed:
        {
            mousePressed[event.mouseButton.button]=true;
            break;
        }
        case sf::Event::MouseButtonReleased:
        {
            mousePressed[event.mouseButton.button]=false;
            break;
        }
        case sf::Event::MouseWheelMoved:
        {
            ImGuiIO& io = ImGui::GetIO();
            io.MouseWheel += (float)event.mouseWheel.delta;
            break;
        }
        case sf::Event::KeyPressed:
        {
            ImGuiIO& io = ImGui::GetIO();
            io.KeysDown[event.key.code]=true;
            io.KeyCtrl=event.key.control;
            io.KeyShift=event.key.shift;
            break;
        }
        case sf::Event::KeyReleased:
        {
            ImGuiIO& io = ImGui::GetIO();
            io.KeysDown[event.key.code]=false;
            io.KeyCtrl=event.key.control;
            io.KeyShift=event.key.shift;
            break;
        }
        case sf::Event::TextEntered:
        {
            if(event.text.unicode > 0 && event.text.unicode < 0x10000)
                ImGui::GetIO().AddInputCharacter(event.text.unicode);
            break;
        }
        default: break;
    }
}

void InitGL()
{
}

void InitImGui()
{
    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = 1.0f/60.0f;                                    // Time elapsed since last frame, in seconds (in this sample app we'll override this every frame because our time step is variable)
    io.PixelCenterOffset = 0.0f;                                  // Align OpenGL texels
    io.KeyMap[ImGuiKey_Tab] = sf::Keyboard::Tab;                       // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
    io.KeyMap[ImGuiKey_LeftArrow] = sf::Keyboard::Left;
    io.KeyMap[ImGuiKey_RightArrow] = sf::Keyboard::Right;
    io.KeyMap[ImGuiKey_UpArrow] = sf::Keyboard::Up;
    io.KeyMap[ImGuiKey_DownArrow] = sf::Keyboard::Down;
    io.KeyMap[ImGuiKey_Home] = sf::Keyboard::Home;
    io.KeyMap[ImGuiKey_End] = sf::Keyboard::End;
    io.KeyMap[ImGuiKey_Delete] = sf::Keyboard::Delete;
    io.KeyMap[ImGuiKey_Backspace] = sf::Keyboard::BackSpace;
    io.KeyMap[ImGuiKey_Enter] = sf::Keyboard::Return;
    io.KeyMap[ImGuiKey_Escape] = sf::Keyboard::Escape;
    io.KeyMap[ImGuiKey_A] = sf::Keyboard::Left;
    io.KeyMap[ImGuiKey_C] = sf::Keyboard::Left;
    io.KeyMap[ImGuiKey_V] = sf::Keyboard::Left;
    io.KeyMap[ImGuiKey_X] = sf::Keyboard::Left;
    io.KeyMap[ImGuiKey_Y] = sf::Keyboard::Left;
    io.KeyMap[ImGuiKey_Z] = sf::Keyboard::Left;

    io.RenderDrawListsFn = ImImpl_RenderDrawLists;

    // Load font texture
    glGenTextures(1, &fontTex);
    glBindTexture(GL_TEXTURE_2D, fontTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    const void* png_data;
    unsigned int png_size;
    ImGui::GetDefaultFontData(NULL, NULL, &png_data, &png_size);
    int tex_x, tex_y, tex_comp;
    void* tex_data = stbi_load_from_memory((const unsigned char*)png_data, (int)png_size, &tex_x, &tex_y, &tex_comp, 0);
    IM_ASSERT(tex_data != NULL);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_x, tex_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
    stbi_image_free(tex_data);
    timeElapsed.restart();
}

void UpdateImGui(sf::Window &window)
{
    ImGuiIO& io = ImGui::GetIO();
    int w, h;
    sf::Vector2u size = window.getSize();
    w=size.x; h=size.y;
    io.DisplaySize = ImVec2((float)w, (float)h);
    static double time = 0.0f;
    const double current_time = timeElapsed.getElapsedTime().asSeconds();
    io.DeltaTime = (float)(current_time - time);
    time = current_time;
    sf::Vector2i mouse = sf::Mouse::getPosition(window);
    io.MousePos = ImVec2((float)mouse.x, (float)mouse.y);                                          // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
    io.MouseDown[0] = mousePressed[0] || sf::Mouse::isButtonPressed(sf::Mouse::Left);
    io.MouseDown[1] = mousePressed[1] || sf::Mouse::isButtonPressed(sf::Mouse::Right);
    ImGui::NewFrame();
}
#endif