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

class ParticleSystem : public sf::Drawable, public sf::Transformable
{
public:

	ParticleSystem(unsigned int count) :
		m_particles(count),
		m_vertices(sf::Quads, count*4),
		m_lifetime(sf::seconds(3)),
		m_emitter(0, 0)
	{
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

			// update the position of the corresponding vertex
			p.position += p.velocity * elapsed.asSeconds();
			updateVerticesPos(i*4, p);

			// update the alpha (transparency) of the particle according to its lifetime
			float ratio = p.lifetime.asSeconds() / m_lifetime.asSeconds();
			updateVerticesColor(i*4, static_cast<sf::Uint8>(ratio * 255));
		}
	}

private:

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		// apply the transform
		states.transform *= getTransform();

		// our particles don't use a texture
		states.texture = NULL;

		// draw the vertex array
		target.draw(m_vertices, states);
	}

private:

	struct Particle
	{
		sf::Vector2f velocity;
		sf::Time lifetime;
		sf::Vector2f position;
		float size = 5;
	};

	void updateVerticesPos(int index , Particle &p) {
		float size = p.size / 2;
		m_vertices[index].position = p.position + sf::Vector2f(-size, -size);
		m_vertices[index+1].position = p.position + sf::Vector2f(size, -size);
		m_vertices[index+2].position = p.position + sf::Vector2f(size, size);
		m_vertices[index+3].position = p.position + sf::Vector2f(-size, size);
	}

	void updateVerticesColor(int index, sf::Uint8 alpha) {
		m_vertices[index].color.a = alpha;
		m_vertices[index + 1].color.a = alpha;
		m_vertices[index + 2].color.a = alpha;
		m_vertices[index + 3].color.a = alpha;
	}

	void resetParticle(std::size_t index)
	{
		// give a random velocity and lifetime to the particle
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dis(-135, -45);

		float angle = (dis(gen) * 3.14f / 180.f);
		float speed = (std::rand() % 50) + 50.f;
		m_particles[index].velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
		m_particles[index].lifetime = sf::milliseconds((std::rand() % 2000) + 1000);
		m_particles[index].position = m_emitter;

		// reset the position of the corresponding vertex
		//m_vertices[index].position = m_emitter;
	}

	std::vector<Particle> m_particles;
	sf::VertexArray m_vertices;
	sf::Time m_lifetime;
	sf::Vector2f m_emitter;
};

int main()
{
	// create the window
	sf::RenderWindow window(sf::VideoMode(1280, 720), "Particles");

	// create the particle system
	ParticleSystem particles(1000);

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
		particles.setEmitter(window.mapPixelToCoords(mouse));

		// update it
		sf::Time elapsed = clock.restart();
		particles.update(elapsed);

		// draw it
		window.clear();
		window.draw(particles);
		window.display();
	}

	return 0;
}