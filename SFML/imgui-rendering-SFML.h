#ifndef IMGUI_SFML_RENDERING_BACKEND
#define IMGUI_SFML_RENDERING_BACKEND
#include <vector>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <iostream>
namespace ImGui
{
    namespace ImImpl
    {
        static sf::RenderTarget* ImImpl_rtarget;
	static sf::Texture ImImpl_fontTex;	
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
		    if(!states.texture) std::cout<<"Invalid texture!"<<std::endl;
                    ImImpl::ImImpl_rtarget->draw(&vtx_buffer[vtx_offset], pcmd->vtx_count, sf::Triangles, states);
                    vtx_offset += pcmd->vtx_count;
                }
            }
        }
    }
    namespace SFML
    {
        static void SetRenderTarget(sf::RenderTarget& target){ImImpl::ImImpl_rtarget=&target;}
        static void InitImGuiRendering()
        {
            ImGuiIO& io = ImGui::GetIO();
	    io.DisplaySize = ImVec2(ImImpl::ImImpl_rtarget->getSize().x, ImImpl::ImImpl_rtarget->getSize().y);
            io.RenderDrawListsFn = ImImpl::ImImpl_RenderDrawLists;
	    unsigned char* pixels;
	    int width, height;
            io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
            ImImpl::ImImpl_fontTex.create(width, height);
            ImImpl::ImImpl_fontTex.update(pixels);
            io.Fonts->TexID = (void*)&ImImpl::ImImpl_fontTex;
	    io.Fonts->ClearInputData();
	    io.Fonts->ClearTexData();
        }
        static void UpdateImGuiRendering()
	{
	    ImGuiIO& io = ImGui::GetIO();
	    io.DisplaySize = ImVec2(ImImpl::ImImpl_rtarget->getSize().x, ImImpl::ImImpl_rtarget->getSize().y);
	}
    }
}
#endif
