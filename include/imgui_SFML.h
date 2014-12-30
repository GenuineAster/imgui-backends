#include <vector>
#include "imgui.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>

static sf::Clock timeElapsed;
static bool mousePressed[2] = { false, false };
#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
static sf::RenderWindow* iwindow;
static sf::Texture fontTex;


static void ImImpl_RenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count)
{
    if (cmd_lists_count == 0)
        return;

    sf::RenderStates states(&fontTex);
    states.blendMode = sf::BlendMode(sf::BlendMode::SrcAlpha, sf::BlendMode::OneMinusSrcAlpha);

    for (int n = 0; n < cmd_lists_count; n++)
    {
        const ImDrawList* cmd_list = cmd_lists[n];
        const unsigned char* vtx_buffer_ = (const unsigned char*)(&(cmd_list->vtx_buffer.front()));
        sf::Vertex* vtx_buffer = (sf::Vertex*)vtx_buffer_;
        int i=cmd_list->vtx_buffer.size()-1;
        while(i>=0)
        {
            vtx_buffer[i].texCoords.x*=fontTex.getSize().x;
            vtx_buffer[i].texCoords.y*=fontTex.getSize().y;
            --i;
        }
        int vtx_offset = 0;
        for (size_t cmd_i = 0; cmd_i < cmd_list->commands.size(); cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->commands.at(cmd_i);
            iwindow->draw(&vtx_buffer[vtx_offset], pcmd->vtx_count, sf::Triangles, states);
            vtx_offset += pcmd->vtx_count;
        }
    }
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

void InitImGui()
{
    ImGuiIO& io = ImGui::GetIO();
    io.PixelCenterOffset = 0.0f;
    io.KeyMap[ImGuiKey_Tab] = sf::Keyboard::Tab;
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
    const void* png_data;
    unsigned int png_size;
    ImGui::GetDefaultFontData(NULL, NULL, &png_data, &png_size);
    fontTex.loadFromMemory(png_data, png_size);
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
    io.MousePos = ImVec2((float)mouse.x, (float)mouse.y);
    io.MouseDown[0] = mousePressed[0] || sf::Mouse::isButtonPressed(sf::Mouse::Left);
    io.MouseDown[1] = mousePressed[1] || sf::Mouse::isButtonPressed(sf::Mouse::Right);
    ImGui::NewFrame();
}
