#include <iostream>
#include <SFML/Graphics.hpp>
#define TINYC2_IMPL
#include <tinyc2.h>
#include <AnimatedSprite.h>
#include <Player.h>
#include <Input.h>
#include <Debug.h>

using namespace std;

void draw(const c2Ray& ray, RenderWindow& window, sf::Color color = sf::Color::White)
{
	sf::Vertex shape[] = {
		sf::Vertex(sf::Vector2f(ray.p.x, ray.p.y), color),
		sf::Vertex(sf::Vector2f(ray.p.x + (ray.d.x * ray.t), ray.p.y + (ray.d.y * ray.t)), color)
	};

	window.draw(shape, 2, sf::Lines);
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

	sf::CircleShape mouseCircle(30);
	mouseCircle.setFillColor(sf::Color::Red);
	mouseCircle.setOrigin(30, 30);

	sf::Sprite capsule;
	capsule.setTexture(capsule_texture);
	capsule.setPosition(300, 10);

	sf::Sprite polygon;
	polygon.setTexture(polygon_texture);
	polygon.setPosition(650, 10);

	sf::CircleShape circle(50);
	circle.setFillColor(sf::Color::Blue);
	circle.setOrigin(50, 50);
	circle.setPosition(700, 300);

	sf::RectangleShape colision_output;
	colision_output.setSize(sf::Vector2f(50, 50));
	colision_output.setPosition(375, 550);
	colision_output.setFillColor(sf::Color::Red);

	//Setup mouse AABB
	c2AABB aabb_mouse;
	aabb_mouse.min = c2V(mouse.getPosition().x, mouse.getPosition().y);
	aabb_mouse.max = c2V(mouse.getGlobalBounds().width, mouse.getGlobalBounds().height);

	//mouse circle colisions
	c2Circle circle_mouse;
	circle_mouse.p = c2V(mouseCircle.getPosition().x, mouseCircle.getPosition().y);
	circle_mouse.r = mouseCircle.getRadius();

	mouse.setPosition(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
	c2Ray ray_mouse;
	ray_mouse.p = c2V(window.mapPixelToCoords(sf::Mouse::getPosition(window)).x, window.mapPixelToCoords(sf::Mouse::getPosition(window)).y);
	ray_mouse.d = c2Norm(c2V(0, -1));
	ray_mouse.t = 50;


	// Setup Players Default Animated Sprite
	AnimatedSprite animated_sprite(sprite_sheet);
	animated_sprite.addFrame(sf::IntRect(3, 3, 84, 84));
	animated_sprite.addFrame(sf::IntRect(88, 3, 84, 84));
	animated_sprite.addFrame(sf::IntRect(173, 3, 84, 84));
	animated_sprite.addFrame(sf::IntRect(258, 3, 84, 84));
	animated_sprite.addFrame(sf::IntRect(343, 3, 84, 84));
	animated_sprite.addFrame(sf::IntRect(428, 3, 84, 84));

	//changing poition  for science
	//animated_sprite.setPosition(85, 85);

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
	c2MakePoly(&poly_player);

	c2Ray ray_player;
	ray_player.p = c2V(50, 300);
	ray_player.d = c2Norm(c2V(0, 1));
	ray_player.t = 50;

	c2Circle circle_player;
	circle_player.p = c2V(circle.getPosition().x, circle.getPosition().y);
	circle_player.r = circle.getRadius();

	// Setup the Player
	Player player(animated_sprite);
	Input input;

	// Collision result
	int result = 0;
	bool collision = false;
	int currShape = 1;

	// Start the game loop
	while (window.isOpen())
	{
		//std::cout << animated_sprite.getPosition().x << std::endl;
		// Move Sprite Follow Mouse
		mouse.setPosition(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
		mouseCircle.setPosition(window.mapPixelToCoords(sf::Mouse::getPosition(window)));

		// Update mouse AABB
		aabb_mouse.min = c2V(mouse.getPosition().x, mouse.getPosition().y);
		aabb_mouse.max = c2V(mouse.getPosition().x + mouse.getGlobalBounds().width, mouse.getPosition().y + mouse.getGlobalBounds().width);

		circle_mouse.p = c2V(mouseCircle.getPosition().x, mouseCircle.getPosition().y);

		ray_mouse.p = c2V(window.mapPixelToCoords(sf::Mouse::getPosition(window)).x, window.mapPixelToCoords(sf::Mouse::getPosition(window)).y);

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
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
				{
					currShape = 1;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
				{
					currShape = 2;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
				{
					currShape = 3;
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
		collision = false;

		if (currShape ==1)
		{
			result = c2AABBtoAABB(aabb_mouse, aabb_player);
			if (result){
				cout << "aabb" << endl;
				collision = true;
			}

			result = c2AABBtoPoly(aabb_mouse, &poly_player, NULL);
			if (result) {
				cout << "poly" << endl;
				collision = true;
			}

			c2Manifold manifold;
			c2AABBtoCapsuleManifold(aabb_mouse, capsule_player, &manifold);
			if (manifold.count > 0) {
				cout << "capsule" << endl;
				collision = true;
			}

			c2Raycast cast;
			result = c2RaytoAABB(ray_player, aabb_mouse, &cast);
			if (result) {
				cout << "ray" << endl;
				collision = true;
			}
		}
		
		if (currShape == 2)
		{
			result = c2CircletoCircle(circle_player, circle_mouse);
			if (result) {
				cout << "circle" << endl;
				collision = true;
			}

			result = c2CircletoAABB(circle_mouse, aabb_player);
			if (result) {
				cout << "aabb" << endl;
				collision = true;
			}

			c2Raycast cast;
			result = c2RaytoCircle(ray_player, circle_mouse, &cast);
			if (result) {
				cout << "ray" << endl;
				collision = true;
			}

			result = c2CircletoCapsule(circle_mouse, capsule_player);
			if (result) {
				cout << "capsule" << endl;
				collision = true;
			}

			result = c2CircletoPoly(circle_mouse, &poly_player, NULL);
			if (result) {
				cout << "poly" << endl;
				collision = true;
			}
		}

		if (currShape == 3)
		{
			c2Raycast cast;
			result = c2RaytoCircle(ray_mouse, circle_player, &cast);
			if (result) {
				cout << "circle" << endl;
				collision = true;
			}

			result = c2RaytoAABB(ray_mouse, aabb_player, &cast);
			if (result) {
				cout << "aabb" << endl;
				collision = true;
			}

			result = c2RaytoCapsule(ray_mouse, capsule_player, &cast);
			if (result) {
				cout << "capsule" << endl;
				collision = true;
			}

			
			result = c2RaytoPoly(ray_mouse, &poly_player, NULL, &cast);
			//result = c2CastRay(ray_mouse, &poly_player, NULL, C2_POLY, &cast);
			if (result) {
				cout << "poly" << endl;
				collision = true;
			}
		}

		if (collision == false)
		{
			cout << "" << endl;
			colision_output.setFillColor(sf::Color::Red);
		}
		else
		{
			colision_output.setFillColor(sf::Color::Green);
		}

		// Clear screen
		window.clear();

		// Draw the Players Current Animated Sprite
		window.draw(player.getAnimatedSprite());
		window.draw(capsule);
		draw(ray_player, window);
		window.draw(polygon);
		window.draw(circle);

		window.draw(colision_output);

		if (currShape == 1)
		{
			window.draw(mouse);
		}
		if (currShape == 2)
		{
			window.draw(mouseCircle);
		}
		if (currShape == 3)
		{
			draw(ray_mouse, window);
		}

		// Update the window
		window.display();
	}


	

	return EXIT_SUCCESS;
};


