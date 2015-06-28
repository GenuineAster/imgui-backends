SFML Backend How-To
=======

To use the SFML backend:

- Copy the contents of `imconfig-events-SFML.h` and `imconfig-rendering-SFML.h` to your `imconfig.h` file.
- Include `imgui-events-SFML.h` and `imgui-rendering-SFML.h`, *after* having included `imgui.h`
- Set the window to use for events with `ImGui::SFML::SetWindow(win)`
- Set the render target to use for renddering with `ImGui::SFML::SetRenderTarget(win)`
- Initialize the SFML rendering backend with `ImGui::SFML::InitImGuiRendering()`
- Initialize the SFML events backend with `ImGui::SFML::InitImGuiEvents()`
- At the beginning of every frame:
  - Update the events backend with `ImGui::SFML::UpdateImGui()`
  - Update the rendering backend with `ImGui::SFML::UpdateImGuiRendering()`
- For each event, send the event to the SFML events backend with `ImGui::SFML::ProcessEvent(my_event);`
- At the end of your rendering code, e.g before clearing, draw your GUI with `ImGui::Render()`, as explained by the imgui how-to.

Example code:

```c++
#include "imgui.h"
#include "imgui-events-SFML.h"
#include "imgui-rendering-SFML.h"
#include <SFML/Graphics.hpp>

int main() {
	sf::RenderWindow win{{800, 600}, "window"};
	win.setFramerateLimit(60);
	ImGui::SFML::SetRenderTarget(win);
	ImGui::SFML::InitImGuiRendering();
	ImGui::SFML::SetWindow(win);
	ImGui::SFML::InitImGuiEvents();
	while(win.isOpen()) {
		ImGui::SFML::UpdateImGui();
		ImGui::SFML::UpdateImGuiRendering();
		sf::Event e;
		while(win.pollEvent(e)) {
			ImGui::SFML::ProcessEvent(e);
			if(e.type == sf::Event::Closed) {
				win.close();
			}
		}
		ImGuiIO &io = ImGui::GetIO();
		bool tmp = true;
		ImGui::Begin("window", &tmp);
		ImGui::Text("Hello, world!");
		ImGui::Button("Is this working?!");
		ImGui::End();
		win.clear(sf::Color::Blue);
		ImGui::Render();
		win.display();
	}
}
```

