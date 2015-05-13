#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/View.hpp>

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Text.hpp>

#include <iostream>
#include <string>

#include "Map.hpp"
#include "HeightEntry.hpp"
#include "HeightEntry.hpp"

void modifyHeight(const sf::FloatRect& selection, hgt::Map& map, int heightChange);

void changeCursorSize(sf::RectangleShape& cursor, unsigned int size);

int main(int argc, char** argv)
{
	std::vector<std::string> args(argv + 1, argv + argc);

	if(args.size() != 1)
	{
		std::cout << "Incorrect number of args! Must be 1! (path to hgt file you wish to edit)\n";
		return 1;
	}

	hgt::Map map;

	map.load(args[0]);

	// window stuff
	sf::View view(map.getBounds());
	view.setViewport({0, 0, 1, 1});

	sf::RenderWindow window({800, 600, 32}, "BZ HGT Editor", sf::Style::Titlebar | sf::Style::Close);
	window.setVerticalSyncEnabled(true);
	window.setView(view);

	// texture to draw the vertex array to for simplifying zooming
	sf::RenderTexture renTex;
	renTex.create(static_cast<unsigned int>(map.getBounds().width), static_cast<unsigned int>(map.getBounds().height));

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

	// vars
	const std::array<float, 9> zoomLevels = {0.1f, 0.25f, 0.5f, 0.75f, 1.f, 1.25f, 1.5f, 1.75f, 1.9f};
	int currentZoomLevel = 4;

	const float VIEW_MOVE = 5.f;

	const int HEIGHT_FACTOR_NORMAL = hgt::HEIGHT_MAX / 45;	// 91
	const int HEIGHT_FACTOR_PRECISE = 1;
	int heightFactor = HEIGHT_FACTOR_NORMAL;

	bool needsDrawn = true;
	bool mouseInWindow = false;

	bool ctrlPressed = false;
	bool shiftPressed = false;

	float cursorSize = 1;

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
						// modifiers
						case sf::Keyboard::LControl:
							ctrlPressed = true;
							break;
						case sf::Keyboard::LShift:
							shiftPressed = true;
							break;
						case sf::Keyboard::LAlt:
							heightFactor = HEIGHT_FACTOR_PRECISE;
							break;
						// view movement
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
						// modifiers
						case sf::Keyboard::LControl:
							ctrlPressed = false;
							break;
						case sf::Keyboard::LShift:
							shiftPressed = false;
							break;
						case sf::Keyboard::LAlt:
							heightFactor = HEIGHT_FACTOR_NORMAL;
							break;
						// instant max/min terrain
						case sf::Keyboard::M:
							modifyHeight(cursor.getGlobalBounds(), map, hgt::HEIGHT_MAX + 1);
							needsDrawn = true;
							break;
						case sf::Keyboard::N:
							modifyHeight(cursor.getGlobalBounds(), map, -hgt::HEIGHT_MAX - 1);
							needsDrawn = true;
							break;
						// cursor sizing
						case sf::Keyboard::Num1:
							changeCursorSize(cursor, 1);
							break;
						case sf::Keyboard::Num2:
							changeCursorSize(cursor, 2);
							break;
						case sf::Keyboard::Num3:
							changeCursorSize(cursor, 3);
							break;
						case sf::Keyboard::Num4:
							changeCursorSize(cursor, 4);
							break;
						case sf::Keyboard::Num5:
							changeCursorSize(cursor, 5);
							break;
						case sf::Keyboard::Num6:
							changeCursorSize(cursor, 6);
							break;
						case sf::Keyboard::Num7:
							changeCursorSize(cursor, 7);
							break;
						case sf::Keyboard::Num8:
							changeCursorSize(cursor, 8);
							break;
						case sf::Keyboard::Num9:
							changeCursorSize(cursor, 9);
							break;
						case sf::Keyboard::Num0:
							changeCursorSize(cursor, 10);
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
						window.setView(view);
						sf::Vector2f mousePos = window.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y});
						cursor.setPosition(mousePos);

						//heightText.setString(std::to_string(map.getEntry(static_cast<unsigned int>(mousePos.x), static_cast<unsigned int>(mousePos.y)).getHeightInt()));

						window.setView(window.getDefaultView());
					}
					break;
				}
				case sf::Event::MouseWheelScrolled:
					if(ctrlPressed)
					{
						int prevZoomLevel = currentZoomLevel;
						currentZoomLevel += event.mouseWheelScroll.delta > 0 ? -1 : 1;

						// bound the zoom level
						if(currentZoomLevel < 0 || static_cast<unsigned int>(currentZoomLevel) >= zoomLevels.size())
						{
							// undo the invalid change
							currentZoomLevel -= event.mouseWheelScroll.delta > 0 ? -1 : 1;
							break;
						}

						view.zoom(zoomLevels[currentZoomLevel] * std::pow(zoomLevels[prevZoomLevel], -1));

						// move the view's center towards the mouse position
						sf::Vector2i mousePos = {event.mouseWheelScroll.x, event.mouseWheelScroll.y};
						sf::Vector2i diff = mousePos - sf::Vector2i(400, 300);	// center of window
						float length = static_cast<float>(std::sqrt(diff.x * diff.x + diff.y * diff.y));

						// only move the center if the mouse is a large enough distance away
						if(length > 50.f)
						{
							float scale = length / 30.f;
							view.setCenter(view.getCenter() + static_cast<sf::Vector2f>(diff) * event.mouseWheelScroll.delta / scale);
						}
					}
					else if(shiftPressed)
					{
						cursorSize += event.mouseWheelScroll.delta > 0 ? 1 : -1;

						if(cursorSize < 1)
						{
							cursorSize = 1;
						}

						cursor.setSize({static_cast<float>(cursorSize), static_cast<float>(cursorSize)});
						cursor.setOrigin(cursor.getSize() / 2.f);
					}
					else
					{
						modifyHeight(cursor.getGlobalBounds(), map, static_cast<int>(event.mouseWheelScroll.delta) * heightFactor);
						needsDrawn = true;
					}
					break;
				default:
					break;
			}
		}

		map.update();

		// render map to texture
		// why? Well, since the map is just working with vertices, and a vertex just refers to 1 pixel,
		// they don't scale very well for zooming in and out!
		// so, by rendering to a texture, and then drawing that... we get scaling pixels!
		if(needsDrawn)
		{
			renTex.clear();
			renTex.draw(map);
			renTex.display();
			needsDrawn = false;
		}

		sf::Sprite sprite(renTex.getTexture());

		window.clear();
		window.setView(view);
		window.draw(sprite);
		window.draw(cursor);
		window.setView(window.getDefaultView());
		//window.draw(heightText);
		window.display();
	}

	std::string input;

	std::cout << "Save? (y or n): ";
	std::getline(std::cin, input);

	if(std::tolower(input[0], std::locale()) == 'y')
	{
		map.write(args[0]);
	}

	return 0;
}

void modifyHeight(const sf::FloatRect& selection, hgt::Map& map, int heightChange)
{
	// +-'s are for not including the border
	int left = static_cast<int>(selection.left) + 2;
	int right = left + static_cast<int>(selection.width) - 2;
	int top = static_cast<int>(selection.top) + 2;
	int bottom = top + static_cast<int>(selection.height) - 2;

	// bound area to modify to map size
	if(left < 0)
	{
		left = 0;
	}
	if(right > static_cast<int>(map.getBounds().width))
	{
		right = static_cast<int>(map.getBounds().width) - 1;
	}
	if(top < 0)
	{
		top = 0;
	}
	if(bottom > static_cast<int>(map.getBounds().height))
	{
		bottom = static_cast<int>(map.getBounds().height) - 1;
	}

	for(int x = left; x < right; x++)
	{
		for(int y = top; y < bottom; y++)
		{
			hgt::HeightEntry& entry = map.getEntry(x, y);
			
			if(heightChange > hgt::HEIGHT_MAX)
			{
				entry.setHeight(hgt::HEIGHT_MAX);
			}
			else if(heightChange < -hgt::HEIGHT_MAX)
			{
				entry.setHeight(hgt::HEIGHT_MIN);
			}
			else
			{
				entry.setHeight(entry.getHeightInt() + heightChange);

				// check for limit-crossing, fix if so
				int height = entry.getHeightInt();

				if(height < hgt::HEIGHT_MIN)
				{
					entry.setHeight(hgt::HEIGHT_MIN);
				}
				else if(height > hgt::HEIGHT_MAX)
				{
					entry.setHeight(hgt::HEIGHT_MAX);
				}
			}
		}
	}
}

void changeCursorSize(sf::RectangleShape& cursor, unsigned int size)
{
	cursor.setSize({static_cast<float>(size), static_cast<float>(size)});
	cursor.setOrigin(cursor.getSize() / 2.f);
}
