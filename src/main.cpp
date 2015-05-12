#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/View.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Color.hpp>

#include <string>
#include <iostream>
#include <windows.h>

#include "Map.hpp"
#include "HeightEntry.hpp"
#include "HeightEntry.hpp"

int main(int argc, char** argv)
{
	const float VIEW_MOVE = 5.f;

	hgt::Map map;

	map.load("biometal.hgt");

	// window stuff
	sf::RenderWindow window({800, 600, 32}, "BZ HGT Editor", sf::Style::Titlebar | sf::Style::Close);
	sf::View view(map.getBounds());
	view.setViewport({0, 0, 1, 1});
	window.setView(view);
	window.setVerticalSyncEnabled(true);

	// text stuff
	//sf::Font font;
	//font.loadFromFile("arial.ttf");

	//sf::Text heightText("", font);

	// cursor
	sf::RectangleShape cursor({1, 1});
	cursor.setFillColor({0, 0, 0, 0});
	cursor.setOutlineColor(sf::Color::White);
	cursor.setOutlineThickness(1);
	cursor.setOrigin(cursor.getSize() / 2.f);
	cursor.setPosition(view.getCenter());

	const std::array<float, 9> zoomLevels = {0.1f, 0.25f, 0.5f, 0.75f, 1.f, 1.25f, 1.5f, 1.75f, 1.9f};
	int currentZoomLevel = 4;
	bool mouseInWindow = false;
	bool ctrlPressed = false;

	while(window.isOpen())
	{
		sf::Event event;
		while(window.pollEvent(event))
		{
			switch(event.type)
			{
				case sf::Event::Closed:
					window.close();
					break;
				case sf::Event::KeyPressed:
					switch(event.key.code)
					{
						case sf::Keyboard::LControl:
							ctrlPressed = true;
							break;
						case sf::Keyboard::Left:
							view.move(-VIEW_MOVE, 0);
							break;
						case sf::Keyboard::Right:
							view.move(VIEW_MOVE, 0);
							break;
						case sf::Keyboard::Up:
							view.move(0, -VIEW_MOVE);
							break;
						case sf::Keyboard::Down:
							view.move(0, VIEW_MOVE);
							break;
						default:
							break;
					}
					break;
				case sf::Event::KeyReleased:
					switch(event.key.code)
					{
						case sf::Keyboard::LControl:
							ctrlPressed = false;
							break;
						case sf::Keyboard::Num1:
							cursor.setSize({1, 1});
							cursor.setOrigin(cursor.getSize() / 2.f);
							break;
						case sf::Keyboard::Num2:
							cursor.setSize({2, 2});
							cursor.setOrigin(cursor.getSize() / 2.f);
							break;
						case sf::Keyboard::Num3:
							cursor.setSize({3, 3});
							cursor.setOrigin(cursor.getSize() / 2.f);
							break;
						case sf::Keyboard::Num4:
							cursor.setSize({4, 4});
							cursor.setOrigin(cursor.getSize() / 2.f);
							break;
						case sf::Keyboard::Num5:
							cursor.setSize({5, 5});
							cursor.setOrigin(cursor.getSize() / 2.f);
							break;
						case sf::Keyboard::Num6:
							cursor.setSize({6, 6});
							cursor.setOrigin(cursor.getSize() / 2.f);
							break;
						case sf::Keyboard::Num7:
							cursor.setSize({7, 7});
							cursor.setOrigin(cursor.getSize() / 2.f);
							break;
						case sf::Keyboard::Num8:
							cursor.setSize({8, 8});
							cursor.setOrigin(cursor.getSize() / 2.f);
							break;
						case sf::Keyboard::Num9:
							cursor.setSize({9, 9});
							cursor.setOrigin(cursor.getSize() / 2.f);
							break;
						case sf::Keyboard::Num0:
							cursor.setSize({10, 10});
							cursor.setOrigin(cursor.getSize() / 2.f);
							break;
						default:
							break;
					}
					break;
				case sf::Event::MouseEntered:
					mouseInWindow = true;
					break;
				case sf::Event::MouseLeft:
					mouseInWindow = false;
					break;
				case sf::Event::MouseMoved:
				{
					if(mouseInWindow)
					{
						sf::Vector2f mousePos = {static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y)};
						cursor.setPosition(mousePos);

						//heightText.setString(std::to_string(map.getEntry(static_cast<unsigned int>(mousePos.x), static_cast<unsigned int>(mousePos.y)).getHeightInt()));
					}
					break;
				}
				case sf::Event::MouseWheelScrolled:
				{
					if(ctrlPressed)
					{
						int prevZoomLevel = currentZoomLevel;
						std::cout << "previous: " << currentZoomLevel << '\n';
						currentZoomLevel += event.mouseWheelScroll.delta > 0 ? -1 : 1;
						std::cout << "currrent: " << currentZoomLevel << '\n';

						// bound the zoom level
						if(currentZoomLevel < 0 || static_cast<unsigned int>(currentZoomLevel) >= zoomLevels.size())
						{
							// undo the invalid change
							currentZoomLevel -= event.mouseWheelScroll.delta > 0 ? -1 : 1;
							std::cout << "undone: " << currentZoomLevel << '\n';
							break;
						}

						view.zoom(1.f + zoomLevels[currentZoomLevel] - zoomLevels[prevZoomLevel]);

						// move the view's center towards the mouse position
						sf::Vector2f mousePos = window.mapPixelToCoords({event.mouseWheelScroll.x, event.mouseWheelScroll.y});
						sf::Vector2f diff = mousePos - view.getCenter();
						float scale = std::sqrt(diff.x * diff.x + diff.y * diff.y) / 2;
						view.setCenter(view.getCenter() + diff / scale);
					}
					else
					{
						
					}
					break;
				}
				default:
					break;
			}
		}

		window.clear();
		window.setView(view);
		window.draw(map);
		window.setView(window.getDefaultView());
		window.draw(cursor);
		//window.draw(heightText);
		window.setView(view);
		window.display();
	}

	return 0;
}
