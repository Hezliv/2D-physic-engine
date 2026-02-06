#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <vector>
using namespace std;
using namespace sf;

class PhysBody abstract 
{
public:
	Vector2f position;
	Vector2f velocity = { 0, 0 }; // speed
	Vector2f acceleration = { 0, 0 }; //ускорение
	float resilience = 0.8f; // коэф упругость
	Vector2f force_accum = { 0, 0 };
	float mass;
	float height;
	float width;

	void AddForce(Vector2f force)
	{
		force_accum += force;
	}
};

class AABB : public PhysBody
{

};

class RigidBody : public PhysBody
{
public:
	RigidBody(float radius, Vector2f position)
	{
		height = radius * 2;
		width = radius * 2;
		this->position = position;
		mass = 2.0f;
	}
};

class PhysicEngine
{
	vector<PhysBody*> objects;
	Vector2f gravity = { 0, 98 };
	Vector2u size;
public:
	void KeepInBound()
	{
		for(auto i : objects)
		{
			if (i->position.y + i->height >= size.y)
			{
				i->position.y = size.y - i->height;
				i->velocity.y = -i->velocity.y * i->resilience;
			}
			if (i->position.y + i->height < 0)
			{
				i->position.y = 0;
				i->velocity.y = i->velocity.y * i->resilience;
			}
			if (i->position.x + i->width >= size.x)
			{
				i->position.x = size.x - i->width;
				i->velocity.x = -i->velocity.x * i->resilience;
			}
			if (i->position.x + i->width < 0)
			{
				i->position.x = 0;
				i->velocity.x = i->velocity.x * i->resilience;
			}
		}
	}

	void AddObj(PhysBody* obj)
	{
		objects.push_back(obj);
	}

	void ApplyGravity(float dt)
	{
		if (dt > 0.1f) dt = 0.1f;
		for (auto i : objects)
		{
			Vector2f Ftotal = i->force_accum + (gravity * i->mass);

			// 2. Ускорение: a = F / m
			i->acceleration = Ftotal / i->mass;

			// 3. Скорость: v = v + a * dt
			i->velocity += i->acceleration * dt;

			// 4. Позиция: p = p + v * dt
			i->position += i->velocity * dt;

			// 5. ОЧИСТКА сил для следующего кадра
			i->force_accum = { 0, 0 };
		}
		KeepInBound();
	}

	void ApplyForce(Vector2f force)
	{

	}

	void SetSize(Vector2u size)
	{
		this->size = size;
	}
};

int main() 
{
	RenderWindow* window = new RenderWindow(VideoMode({ 800, 600 }), "2d physics");

	PhysicEngine eng;
	eng.SetSize(window->getSize());
	CircleShape circle1(20);
	circle1.setFillColor(Color::White);
	circle1.setPosition({ 100, 10 });
	CircleShape circle2(20);
	circle2.setFillColor(Color::Red);
	circle2.setPosition({ 200, 50 });
	CircleShape circle3(20);
	circle3.setFillColor(Color::Blue);
	circle3.setPosition({ 300, 100 });
	Clock clock;
	RigidBody circle_body1(circle1.getRadius(), circle1.getPosition());
	RigidBody circle_body2(circle2.getRadius(), circle2.getPosition());
	RigidBody circle_body3(circle3.getRadius(), circle3.getPosition());

	eng.AddObj(&circle_body1);
	eng.AddObj(&circle_body2);
	eng.AddObj(&circle_body3);
	while (window->isOpen())
	{
		float dt = clock.restart().asSeconds();
		window->clear();
		
		while (optional<Event> event = window->pollEvent())
		{
			if (event->is<Event::Closed>())
				window->close();
		}
		eng.ApplyGravity(dt);

		circle1.setPosition(circle_body1.position);
		circle2.setPosition(circle_body2.position);
		circle3.setPosition(circle_body3.position);
		window->draw(circle1);
		window->draw(circle2);
		window->draw(circle3);
		window->display();
	}
}