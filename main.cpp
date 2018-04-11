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

float StaticZero(float p) {
	return 0;
}

float StaticOne(float p) {
	return 1;
}

float Linear(float p) {
	return p;
}

float QuadraticEaseIn(float p)
{
	return p * p;
}

float QuadraticEaseOut(float p)
{
	return -(p * (p - 2));
}

// Modeled after quarter-cycle of sine wave
float SineEaseIn(float p)
{
	return sinf((p - 1) * PI) + 1;
}

// Modeled after quarter-cycle of sine wave (different phase)
float SineEaseOut(float p)
{
	return sinf(p * PI);
}

// Modeled after half sine wave
float SineEaseInOut(float p)
{
	return 0.5 * (1 - cosf(p * PI));
}

// Modeled after shifted quadrant IV of unit circle
float CircularEaseIn(float p)
{
	return 1 - sqrt(1 - (p * p));
}

// Modeled after shifted quadrant II of unit circle
float CircularEaseOut(float p)
{
	return sqrt((2 - p) * p);
}


class ParticleSystem : public sf::Drawable, public sf::Transformable
{
public:

	ParticleSystem(unsigned int count, int angle, sf::Texture *texture, float(*sizeM)(float), float(*speedM)(float), float(*rotationM)(float)) :
		m_particles(count),
		m_vertices(sf::Quads, count * 4),
		//testsquare(sf::Quads, 4),
		m_lifetime(sf::seconds(3)),
		m_emitter(0, 0),
		m_texture(texture),
		emitAngle(angle),
		sizeMod(sizeM),
		speedMod(speedM),
		rotationMod(rotationM)
	{
		setRate(currentCount);
	}

	void setEmitter(sf::Vector2f position)
	{
		m_emitter = position;
	}

	void setRate(unsigned int count)
	{
		m_particles.resize(count);
		currentCount = count;
		m_vertices.resize(count * 4);
		for (std::size_t i = 0; i < m_particles.size(); i++) {
			if (i < count) {
				m_particles[i].alive = true;
			}
			else {
				m_particles[i] = Particle();
			}
		}
	}

	void update(sf::Time elapsed)
	{
		for (int i = 0; i < currentCount; ++i)
		{
			// update the particle lifetime
			Particle& p = m_particles[i];
			if (p.alive) {
				p.lifetime -= elapsed;

				// if the particle is dead, respawn it
				if (p.lifetime <= sf::Time::Zero)
					resetParticle(i);

				// update the alpha (transparency) of the particle according to its lifetime
				float ratio = 1 - p.lifetime.asSeconds() / m_lifetime.asSeconds();
				//updateVerticesAlpha(i*4, static_cast<sf::Uint8>(ratio * 255));
				p.color.g = static_cast<sf::Uint8>((1 - ratio) * 255);
				p.color.a = static_cast<sf::Uint8>((1 - ratio) * 255);
				updateVerticesColor(i * 4, p.color);

				// update the position of the corresponding vertex
				p.position += p.velocity * speedMod(ratio) * elapsed.asSeconds();
				p.size = sizeMod(ratio) * baseSize;
				p.rotation += (2 * PI) * rotationMod(ratio) * elapsed.asSeconds();
				updateVerticesPosition(i * 4, p);
			}
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
		states.texture = m_texture;
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
		sf::Vector2f position = sf::Vector2f(-100,-100);
		float rotation = 0;
		float size = 25;
		sf::Color color = sf::Color::Red;
		sf::Transform transform;
		bool alive = false;
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
		m_vertices[index+1].texCoords = sf::Vector2f(256, 0);
		m_vertices[index+2].texCoords = sf::Vector2f(256, 256);
		m_vertices[index+3].texCoords = sf::Vector2f(0, 256);

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
		std::uniform_int_distribution<int> disAngle(-emitAngle/2 - 90, emitAngle/2 - 90);
		std::uniform_int_distribution<int> disEmitter(-200, 200);

		float angle = (disAngle(gen) * 3.14f / 180.f);
		float speed = 400;//(std::rand() % 150) + 150.f;
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
	int emitAngle = 90;
	sf::Texture *m_texture;
	unsigned int currentCount = 100;
	float(*sizeMod)(float);
	float(*speedMod)(float);
	float(*rotationMod)(float);

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

	sf::Text countText;
	countText.setFont(font);
	countText.setCharacterSize(50);
	countText.setFillColor(sf::Color::Blue);
	countText.setPosition(480, 10);

	sf::Text label1;
	label1.setFont(font);
	label1.setCharacterSize(50);
	label1.setFillColor(sf::Color::Blue);
	label1.setPosition(128, 400);

	sf::Text label2;
	label2.setFont(font);
	label2.setCharacterSize(50);
	label2.setFillColor(sf::Color::Blue);
	label2.setPosition(386, 480);

	sf::Text label3;
	label3.setFont(font);
	label3.setCharacterSize(50);
	label3.setFillColor(sf::Color::Blue);
	label3.setPosition(640, 400);

	sf::Text label4;
	label4.setFont(font);
	label4.setCharacterSize(50);
	label4.setFillColor(sf::Color::Blue);
	label4.setPosition(896, 480);

	bool upPressed = false;
	bool downPressed = false;
	unsigned int particleCount = 100;

	countText.setString("Particle count per system: " + std::to_string(particleCount));
	
	// create the window
	sf::RenderWindow window(sf::VideoMode(1280, 720), "Particles");
	//window.setFramerateLimit(60);

	sf::Texture texture1;
	texture1.loadFromFile("Textures/Flame_01.png");

	sf::Texture texture2;
	texture2.loadFromFile("Textures/Star_33.png");

	sf::Texture texture3;
	texture3.loadFromFile("Textures/Trail_01.png");

	sf::Texture texture4;
	texture4.loadFromFile("Textures/Decal_07.png");


	// create the particle system                     //size            //speed          //rotation
	ParticleSystem particles1(10000, 20, &texture1, QuadraticEaseIn, QuadraticEaseIn, QuadraticEaseIn);
	label1.setString("Quadratic on all");
	ParticleSystem particles2(10000, 90, &texture2, Linear, Linear, Linear);
	label2.setString("Linear on all");
	ParticleSystem particles3(10000, 180, &texture3, StaticOne, StaticOne, StaticZero);
	label3.setString("Static values");
	ParticleSystem particles4(10000, 360, &texture4, SineEaseOut, Linear, SineEaseOut);
	label4.setString("SineEaseOut \n size and rotation");

	particles1.setEmitter(sf::Vector2f(256, 360));
	particles2.setEmitter(sf::Vector2f(512, 360));
	particles3.setEmitter(sf::Vector2f(768, 360));
	particles4.setEmitter(sf::Vector2f(1024, 360));

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

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			if (!upPressed) {
				particleCount *= 2;
				particles1.setRate(particleCount);
				particles2.setRate(particleCount);
				particles3.setRate(particleCount);
				particles4.setRate(particleCount);
				countText.setString("Particle count per system: " + std::to_string(particleCount));
			}
			upPressed = true;
		}
		else {
			upPressed = false;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			if (!downPressed) {
				particleCount /= 2;
				particles1.setRate(particleCount);
				particles2.setRate(particleCount);
				particles3.setRate(particleCount);
				particles4.setRate(particleCount);
				countText.setString("Particle count per system: " + std::to_string(particleCount));
			}
			downPressed = true;
		}
		else {
			downPressed = false;
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
		window.draw(countText);
		window.draw(label1);
		window.draw(label2);
		window.draw(label3);
		window.draw(label4);
		window.display();
	}

	return 0;
}