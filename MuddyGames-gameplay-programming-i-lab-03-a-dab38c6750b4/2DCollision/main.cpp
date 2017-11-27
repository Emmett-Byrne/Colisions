#include <iostream>
#include <SFML/Graphics.hpp>
#define TINYC2_IMPL
#include <tinyc2.h>
#include <AnimatedSprite.h>
#include <Player.h>
#include <Input.h>
#include <Debug.h>

using namespace std;

void draw(const c2Capsule& capsule, RenderWindow& window, sf::Color color = sf::Color::White)
{
	sf::CircleShape circle(capsule.r);
	circle.setOrigin(capsule.r, capsule.r);
	circle.setFillColor(sf::Color::Transparent);
	circle.setOutlineColor(color);
	circle.setOutlineThickness(1.0f);

	sf::Vertex topLine[] = {
		sf::Vertex(sf::Vector2f(capsule.a.x, capsule.a.y + capsule.r), color),
		sf::Vertex(sf::Vector2f(capsule.b.x, capsule.b.y + capsule.r), color)
	};

	sf::Vertex bottomLine[] = {
		sf::Vertex(sf::Vector2f(capsule.a.x, capsule.a.y - capsule.r), color),
		sf::Vertex(sf::Vector2f(capsule.b.x, capsule.b.y - capsule.r), color)
	};

	circle.setPosition(capsule.a.x, capsule.a.y);
	window.draw(circle);
	circle.setPosition(capsule.b.x, capsule.b.y);
	window.draw(circle);

	window.draw(topLine, 2, sf::Lines);
	window.draw(bottomLine, 2, sf::Lines);
}

int main()
{
	// Create the main window
	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");

	// Load a sprite to display
	sf::Texture sprite_sheet;
	if (!sprite_sheet.loadFromFile("assets\\grid.png")) {
		DEBUG_MSG("Failed to load file");
		return EXIT_FAILURE;
	}

	// Load a mouse texture to display
	sf::Texture mouse_texture;
	if (!mouse_texture.loadFromFile("assets\\mouse.png")) {
		DEBUG_MSG("Failed to load file");
		return EXIT_FAILURE;
	}

	sf::Texture capsule_texture;
	if (!capsule_texture.loadFromFile("assets\\capsule.png")) {
		DEBUG_MSG("Failed to load file");
		return EXIT_FAILURE;
	}

	sf::Texture polygon_texture;
	if (!polygon_texture.loadFromFile("assets\\polygon.png")) {
		DEBUG_MSG("Failed to load file");
		return EXIT_FAILURE;
	}

	// Setup a mouse Sprite
	sf::Sprite mouse;
	mouse.setTexture(mouse_texture);

	sf::Sprite capsule;
	capsule.setTexture(capsule_texture);
	capsule.setPosition(150, 10);

	sf::Sprite polygon;
	polygon.setTexture(polygon_texture);
	polygon.setPosition(150, 10);

	//Setup mouse AABB
	c2AABB aabb_mouse;
	aabb_mouse.min = c2V(mouse.getPosition().x, mouse.getPosition().y);
	aabb_mouse.max = c2V(mouse.getGlobalBounds().width, mouse.getGlobalBounds().width);

	// Setup Players Default Animated Sprite
	AnimatedSprite animated_sprite(sprite_sheet);
	animated_sprite.addFrame(sf::IntRect(3, 3, 84, 84));
	animated_sprite.addFrame(sf::IntRect(88, 3, 84, 84));
	animated_sprite.addFrame(sf::IntRect(173, 3, 84, 84));
	animated_sprite.addFrame(sf::IntRect(258, 3, 84, 84));
	animated_sprite.addFrame(sf::IntRect(343, 3, 84, 84));
	animated_sprite.addFrame(sf::IntRect(428, 3, 84, 84));

	// Setup Players AABB
	c2AABB aabb_player;
	aabb_player.min = c2V(animated_sprite.getPosition().x, animated_sprite.getPosition().y);
	aabb_player.max = c2V(animated_sprite.getGlobalBounds().width / animated_sprite.getFrames().size(), 
		animated_sprite.getGlobalBounds().height / animated_sprite.getFrames().size());


	c2Capsule capsule_player;
	capsule_player.a = c2V(capsule.getPosition().x + capsule.getGlobalBounds().height / 2, capsule.getPosition().y + capsule.getGlobalBounds().height / 2);
	capsule_player.b = c2V(capsule.getPosition().x + capsule.getGlobalBounds().width - capsule.getGlobalBounds().height / 2, capsule.getPosition().y + capsule.getGlobalBounds().height / 2);
	capsule_player.r = capsule.getGlobalBounds().height / 2;

	c2Poly poly_player;
	poly_player.count = 4;
	poly_player.verts[0] = c2V(polygon.getPosition().x + 110, polygon.getPosition().y);
	poly_player.verts[1] = c2V(polygon.getPosition().x, polygon.getPosition().y + 65);
	poly_player.verts[2] = c2V(polygon.getPosition().x + 60, polygon.getPosition().y + 140);
	poly_player.verts[3] = c2V(polygon.getPosition().x + 150, polygon.getPosition().y + 60);



	// Setup the Player
	Player player(animated_sprite);
	Input input;

	// Collision result
	int result = 0;
	
	// Start the game loop
	while (window.isOpen())
	{
		// Move Sprite Follow Mouse
		mouse.setPosition(window.mapPixelToCoords(sf::Mouse::getPosition(window)));

		// Update mouse AABB
		aabb_mouse.min = c2V(mouse.getPosition().x, mouse.getPosition().y);
		aabb_mouse.max = c2V(mouse.getGlobalBounds().width, mouse.getGlobalBounds().width);

		// Process events
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				// Close window : exit
				window.close();
				break;
			case sf::Event::KeyPressed:
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
				{
					input.setCurrent(Input::Action::LEFT);
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
				{
					input.setCurrent(Input::Action::RIGHT);
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
				{
					input.setCurrent(Input::Action::UP);
				}
				break;
			default:
				input.setCurrent(Input::Action::IDLE);
				break;
			}
		}

		// Handle input to Player
		player.handleInput(input);

		// Update the Player
		player.update();

		// Check for collisions
		result = c2AABBtoAABB(aabb_mouse, aabb_player);
		cout << ((result != 0) ? ("Collision") : "") << endl;
		if (result){
			player.getAnimatedSprite().setColor(sf::Color(255,0,0));
		}
		else {
			player.getAnimatedSprite().setColor(sf::Color(0, 255, 0));
		}

		result = c2AABBtoPoly(aabb_mouse, &poly_player, NULL);
		cout << ((result != 0) ? ("Collision") : "") << endl;
		if (result) {
			player.getAnimatedSprite().setColor(sf::Color(255, 0, 0));
		}
		else {
			player.getAnimatedSprite().setColor(sf::Color(0, 255, 0));
		}

		/*result = c2AABBtoCapsule(aabb_mouse, capsule_player);
		cout << ((result != 0) ? ("Collision") : "") << endl;
		if (result) {
			player.getAnimatedSprite().setColor(sf::Color(255, 0, 0));
		}
		else {
			player.getAnimatedSprite().setColor(sf::Color(0, 255, 0));
		}*/

		// Clear screen
		window.clear();

		// Draw the Players Current Animated Sprite
		window.draw(player.getAnimatedSprite());
		window.draw(capsule);
		//draw(capsule_player, window);
		window.draw(polygon);
		window.draw(mouse);

		// Update the window
		window.display();
	}


	

	return EXIT_SUCCESS;
};


