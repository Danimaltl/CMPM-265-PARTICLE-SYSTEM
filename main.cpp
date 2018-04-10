//Dan Cochran
//Somewhat based on https://www.sfml-dev.org/tutorials/2.4/graphics-vertex-array.php and https://www.gamasutra.com/view/feature/131565/building_an_advanced_particle_.php

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>

#include <random>
#include <iostream>

#ifndef PI
#define PI  3.14159265f
#endif

class ParticleSystem : public sf::Drawable, public sf::Transformable
{
public:

	ParticleSystem(unsigned int count) :
		m_particles(count),
		m_vertices(sf::Quads, count * 4),
		//testsquare(sf::Quads, 4),
		m_lifetime(sf::seconds(3)),
		m_emitter(0, 0)
	{
		m_texture.loadFromFile("fire.png");
	}

	void setEmitter(sf::Vector2f position)
	{
		m_emitter = position;
	}

	void update(sf::Time elapsed)
	{
		for (std::size_t i = 0; i < m_particles.size(); ++i)
		{
			// update the particle lifetime
			Particle& p = m_particles[i];
			p.lifetime -= elapsed;

			// if the particle is dead, respawn it
			if (p.lifetime <= sf::Time::Zero)
				resetParticle(i);

			// update the alpha (transparency) of the particle according to its lifetime
			float ratio = p.lifetime.asSeconds() / m_lifetime.asSeconds();
			//updateVerticesAlpha(i*4, static_cast<sf::Uint8>(ratio * 255));
			p.color.g = static_cast<sf::Uint8>(ratio * 255);
			p.color.a = static_cast<sf::Uint8>(ratio * 255);
			updateVerticesColor(i*4, p.color);

			// update the position of the corresponding vertex
			p.position += (p.velocity - p.velocity * ratio) * elapsed.asSeconds();
			p.size = baseSize/2 + ratio * baseSize;
			p.rotation += (PI / 4) * elapsed.asSeconds();
			updateVerticesPosition(i * 4, p);
		}

		//testRotation += (PI / 4) * elapsed.asSeconds();
		//sf::Vector2f testPosition(640, 320);
		//testTransform.rotate(3, testPosition);
		//float testSize = 100;
		//testSize = testSize / 2;
		//testsquare[0].position = sf::Vector2f(-testSize, -testSize);
		//testsquare[1].position = sf::Vector2f(testSize, -testSize);
		//testsquare[2].position = sf::Vector2f(testSize, testSize);
		//testsquare[3].position = sf::Vector2f(-testSize, testSize);

		//rotateVector(testsquare[0].position, testRotation);
		//rotateVector(testsquare[1].position, testRotation);
		//rotateVector(testsquare[2].position, testRotation);
		//rotateVector(testsquare[3].position, testRotation);

		//testsquare[0].position += testPosition;
		//testsquare[1].position += testPosition;
		//testsquare[2].position += testPosition;
		//testsquare[3].position += testPosition;

	}

private:

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		// apply the transform
		states.transform *= getTransform();

		// our particles don't use a texture
		states.texture = &m_texture;
		//states.texture = NULL;

		// draw the vertex array
		target.draw(m_vertices, states);
		//target.draw(testsquare, states);
	}

private:

	struct Particle
	{
		sf::Vector2f velocity;
		sf::Time lifetime = sf::milliseconds((std::rand() % 2000) + 1000);
		sf::Vector2f position = sf::Vector2f(-20,-20);
		float rotation = 0;
		float size = 25;
		sf::Color color = sf::Color::Red;
		sf::Transform transform;
	};

	void updateVerticesPosition(int index , Particle &p) {
		float size = p.size / 2;

		//m_vertices[index].position = p.position + sf::Vector2f(-size, -size);
		//m_vertices[index + 1].position = p.position + sf::Vector2f(size, -size);
		//m_vertices[index + 2].position = p.position + sf::Vector2f(size, size);
		//m_vertices[index + 3].position = p.position + sf::Vector2f(-size, size);

		m_vertices[index].position = sf::Vector2f(-size, -size);
		m_vertices[index+1].position = sf::Vector2f(size, -size);
		m_vertices[index+2].position = sf::Vector2f(size, size);
		m_vertices[index+3].position = sf::Vector2f(-size, size);

		rotateVector(m_vertices[index].position, p.rotation);
		rotateVector(m_vertices[index+1].position, p.rotation);
		rotateVector(m_vertices[index+2].position, p.rotation);
		rotateVector(m_vertices[index+3].position, p.rotation);

		m_vertices[index].position += p.position;
		m_vertices[index + 1].position += p.position;
		m_vertices[index + 2].position += p.position;
		m_vertices[index + 3].position += p.position;
	}

	void updateVerticesColor(int index, sf::Color color) {
		m_vertices[index].color = color;
		m_vertices[index + 1].color = color;
		m_vertices[index + 2].color = color;
		m_vertices[index + 3].color = color;

		m_vertices[index].texCoords = sf::Vector2f(0, 0);
		m_vertices[index+1].texCoords = sf::Vector2f(250, 0);
		m_vertices[index+2].texCoords = sf::Vector2f(250, 170);
		m_vertices[index+3].texCoords = sf::Vector2f(0, 170);

	}

	void engareVector(sf::Vector2f &v, float r) {
		v.x = (v.x * cosf(r)) - (v.y * sinf(r));
		v.y = (v.x * sinf(r)) + (v.y * cosf(r));
	}

	void rotateVector(sf::Vector2f &v, float r) {
		float oldX = v.x;
		float oldY = v.y;
		
		v.x = (oldX * cosf(r)) - (oldY * sinf(r));
		v.y = (oldX * sinf(r)) + (oldY * cosf(r));
	}

	void resetParticle(std::size_t index)
	{
		// give a random velocity and lifetime to the particle
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> disAngle(-130, -50);
		std::uniform_int_distribution<int> disEmitter(-200, 200);

		float angle = (disAngle(gen) * 3.14f / 180.f);
		float speed = (std::rand() % 150) + 150.f;
		m_particles[index].velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
		m_particles[index].lifetime = sf::milliseconds((std::rand() % 2000) + 1000);
		m_particles[index].position = m_emitter; //+ sf::Vector2f(disEmitter(gen),0);
		// reset the position of the corresponding vertex
		//m_vertices[index].position = m_emitter;
	}

	std::vector<Particle> m_particles;
	sf::VertexArray m_vertices;
	sf::Time m_lifetime;
	sf::Vector2f m_emitter;
	float baseSize = 100;
	sf::Texture m_texture;

	//sf::VertexArray testsquare;
	//sf::Transform testTransform;
	//float testRotation;
};

int main()
{
	sf::Text fpsText;
	sf::Font font;
	if (!font.loadFromFile("arial.ttf")) {
		std::cout << "Couldn't load font!" << std::endl;
	}
	fpsText.setFont(font);
	fpsText.setCharacterSize(50);
	fpsText.setFillColor(sf::Color::Green);
	fpsText.setPosition(10, 10);

	bool spacePressed = false;
	unsigned int particleCount = 100;
	// create the window
	sf::RenderWindow window(sf::VideoMode(1280, 720), "Particles");
	//window.setFramerateLimit(60);

	// create the particle system
	ParticleSystem particles1(particleCount);
	ParticleSystem particles2(particleCount);
	ParticleSystem particles3(particleCount);
	ParticleSystem particles4(particleCount);

	// create a clock to track the elapsed time
	sf::Clock clock;

	// run the main loop
	while (window.isOpen())
	{
		// handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		// make the particle system emitter follow the mouse
		sf::Vector2i mouse = sf::Mouse::getPosition(window);
		particles1.setEmitter(sf::Vector2f(256,720));
		particles2.setEmitter(sf::Vector2f(512, 720));
		particles3.setEmitter(sf::Vector2f(768, 720));
		particles4.setEmitter(sf::Vector2f(1024, 720));

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			if (!spacePressed) {
				particleCount *= 2;
				particles1 = ParticleSystem(particleCount);
				particles2 = ParticleSystem(particleCount);
				particles3 = ParticleSystem(particleCount);
				particles4 = ParticleSystem(particleCount);
				std::cout << "particleCount: " << particleCount << std::endl;
			}
			spacePressed = true;
		}
		else {
			spacePressed = false;
		}

		// update it
		sf::Time elapsed = clock.restart();
		fpsText.setString(std::to_string(static_cast<int>(std::round(1.0f /elapsed.asSeconds()))));
		particles1.update(elapsed);
		particles2.update(elapsed);
		particles3.update(elapsed);
		particles4.update(elapsed);

		// draw it
		window.clear();
		window.draw(particles1);
		window.draw(particles2);
		window.draw(particles3);
		window.draw(particles4);
		window.draw(fpsText);
		window.display();
	}

	return 0;
}