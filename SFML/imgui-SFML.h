#ifndef IMGUI_SFML_BACKEND
#define IMGUI_SFML_BACKEND
#include <vector>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

namespace ImGui
{
    namespace ImImpl
    {
        static sf::Clock ImImpl_timeElapsed;
        static bool ImImpl_mousePressed[2] = { false, false };
        static sf::RenderWindow* ImImpl_window;
        static sf::Texture ImImpl_fontTex;
        static void ImImpl_RenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count);
    }
    namespace SFML
    {
        static void SetWindow(sf::RenderWindow& window){ImImpl::ImImpl_window=&window;}
        static void ProcessEvent(sf::Event &event)
        {
            switch(event.type)
            {
                case sf::Event::MouseButtonPressed:
                {
                    ImImpl::ImImpl_mousePressed[event.mouseButton.button]=true;
                    break;
                }
                case sf::Event::MouseButtonReleased:
                {
                    ImImpl::ImImpl_mousePressed[event.mouseButton.button]=false;
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

        static void InitImGui()
        {
            ImGuiIO& io = ImGui::GetIO();
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
            io.KeyMap[ImGuiKey_A] = sf::Keyboard::A;
            io.KeyMap[ImGuiKey_C] = sf::Keyboard::C;
            io.KeyMap[ImGuiKey_V] = sf::Keyboard::V;
            io.KeyMap[ImGuiKey_X] = sf::Keyboard::X;
            io.KeyMap[ImGuiKey_Y] = sf::Keyboard::Y;
            io.KeyMap[ImGuiKey_Z] = sf::Keyboard::Z;

            io.RenderDrawListsFn = ImImpl::ImImpl_RenderDrawLists;

            // Load font texture
            unsigned char* pixels;
            int width, height;
            ImFont* font = io.Fonts->AddFontDefault();
            io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
            ImImpl::ImImpl_fontTex.create(width, height);
            ImImpl::ImImpl_fontTex.update(pixels);
            io.Fonts->TexID = (void*)&ImImpl::ImImpl_fontTex;
            ImImpl::ImImpl_timeElapsed.restart();
        }

        static void UpdateImGui()
        {
            ImGuiIO& io = ImGui::GetIO();
            int w, h;
            sf::Vector2u size = ImImpl::ImImpl_window->getSize();
            w=size.x; h=size.y;
            io.DisplaySize = ImVec2((float)w, (float)h);
            static double time = 0.0f;
            const double current_time = ImImpl::ImImpl_timeElapsed.getElapsedTime().asSeconds();
            io.DeltaTime = (float)(current_time - time);
            time = current_time;
            sf::Vector2i mouse = sf::Mouse::getPosition(*ImImpl::ImImpl_window);
            io.MousePos = ImVec2((float)mouse.x, (float)mouse.y);
            io.MouseDown[0] = ImImpl::ImImpl_mousePressed[0] || sf::Mouse::isButtonPressed(sf::Mouse::Left);
            io.MouseDown[1] = ImImpl::ImImpl_mousePressed[1] || sf::Mouse::isButtonPressed(sf::Mouse::Right);
            ImGui::NewFrame();
        }
    }

    namespace ImImpl
    {
        static void ImImpl_RenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count)
        {
            if (cmd_lists_count == 0)
                return;

            sf::RenderStates states;
            states.blendMode = sf::BlendMode(sf::BlendMode::SrcAlpha, sf::BlendMode::OneMinusSrcAlpha);

            for (int n = 0; n < cmd_lists_count; n++)
            {
                const ImDrawList* cmd_list = cmd_lists[n];
                const unsigned char* vtx_buffer_ = (const unsigned char*)(&(cmd_list->vtx_buffer.front()));
                sf::Vertex* vtx_buffer = (sf::Vertex*)vtx_buffer_;
                int i=cmd_list->vtx_buffer.size()-1;
                while(i>=0)
                {
                    vtx_buffer[i].texCoords.x*=ImImpl::ImImpl_fontTex.getSize().x;
                    vtx_buffer[i].texCoords.y*=ImImpl::ImImpl_fontTex.getSize().y;
                    --i;
                }
                int vtx_offset = 0;
                for (size_t cmd_i = 0; cmd_i < cmd_list->commands.size(); cmd_i++)
                {
                    const ImDrawCmd* pcmd = &cmd_list->commands.at(cmd_i);
                    states.texture = (sf::Texture*)pcmd->texture_id;
                    ImImpl::ImImpl_window->draw(&vtx_buffer[vtx_offset], pcmd->vtx_count, sf::Triangles, states);
                    vtx_offset += pcmd->vtx_count;
                }
            }
        }
    }
}
#endif