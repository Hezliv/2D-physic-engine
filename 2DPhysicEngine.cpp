#include <SFML/Graphics.hpp>
#include <SFML/Window/Mouse.hpp>
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

	virtual ~PhysBody() = default;
	void AddForce(Vector2f force)
	{
		force_accum += force;
	}

	Vector2f GetCenter()
	{
		return position + Vector2f(width / 2.0f, height / 2.0f);
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

class Ball { // decide what do with class wrapper
public:
	RigidBody* physics;
	CircleShape* visual;
};

class PhysicEngine
{
	vector<PhysBody*> objects;
	Vector2f gravity = { 0, 400.0f }; // для монитора лучше испльзовать 980 весто реальных 9.8
	Vector2u size;
	float resistance = 0.3f;
	float air_resistance = 0.1f;
public:
	void KeepInBound()
	{
		for(auto i : objects)
		{
			if (abs(i->velocity.x) < 0.1f) i->velocity.x = 0;
			if (abs(i->velocity.y) < 0.1f) i->velocity.y = 0;

			if (i->position.y + i->height >= size.y)
			{
				i->position.y = size.y - i->height;
				i->velocity.y = -i->velocity.y * i->resilience;
				i->velocity.x *= resistance;
			}
			if (i->position.y + i->height < i->height)
			{
				i->position.y = 0;
				i->velocity.y = -i->velocity.y * i->resilience;
			}
			if (i->position.x + i->width >= size.x)
			{
				i->position.x = size.x - i->width;
				i->velocity.x = -i->velocity.x * i->resilience;
				i->velocity.x *= 0.95f;
			}
			if (i->position.x + i->width < i->width)
			{
				i->position.x = 0;
				i->velocity.x = -i->velocity.x * i->resilience; 	

			}
		}
	}

	void AddObj(PhysBody* obj)
	{
		objects.push_back(obj);
	}

	void ApplyGravity(PhysBody* obj)
	{
		obj->acceleration = -gravity;
	}

	void Update(float dt)
	{
		if (dt > 0.1f) dt = 0.1f;
		for (auto i : objects)
		{
			Vector2f drag_force = -i->velocity * air_resistance;
			Vector2f Ftotal = i->force_accum + (gravity * i->mass) + drag_force;
	
			i->acceleration = Ftotal / i->mass;

			i->velocity += i->acceleration * dt;

			i->position += i->velocity * dt;
		
			i->force_accum = { 0, 0 };
		}
		KeepInBound();
	}

	void ResolveCollision()
	{
		
		for(int i = 0; i < objects.size(); i++)
		{
			for(int j = i + 1; j < objects.size(); j++)
			{			
				if (Distance(objects[i]->position, objects[j]->position) < (objects[i]->width / 2 + objects[j]->width / 2)) // only for circles
				{
					//cout << objects[j]->mass << "\n";
					//cout << objects[i]->position.y << " " << objects[j]->position.y << "\n";
					float mass = objects[i]->mass + objects[j]->mass; // вес

					Vector2f pos_diff = objects[i]->position - objects[j]->position; // разница между расстояниями
					Vector2f velocity_diff = objects[i]->velocity - objects[j]->velocity; // разница скорости

					float prod = velocity_diff.x * (objects[i]->position.x - objects[j]->position.x) + velocity_diff.y * (objects[i]->position.y - objects[j]->position.y);
					float dist_sq = pos_diff.x * pos_diff.x + pos_diff.y * pos_diff.y;
					if (dist_sq < 0.000001f) continue;
					float collision = (1.8f * prod) / (mass * dist_sq);

					objects[i]->velocity -= collision * objects[j]->mass * pos_diff; // обмен скоростями
					objects[j]->velocity += collision * objects[i]->mass * pos_diff;
				}
			}
		}
	}

	void ApplyForce(Vector2f force)
	{

	}

	void SetSize(Vector2u size)
	{
		this->size = size;
	}

	float Distance(Vector2f first, Vector2f second) // rewrite function
	{
		return sqrtf((pow((first.x - second.x), 2) + pow((first.y - second.y), 2)));
	}

	void MouseVoid(Vector2f mouse_pos, float radius, float strength)
	{
		for(auto i : objects)
		{
			Vector2f center = i->GetCenter();
			Vector2f diff = center - mouse_pos;

			float dist_sq = diff.x * diff.x + diff.y * diff.y;

			if (dist_sq < radius * radius && dist_sq > 0)
			{
				float dist = sqrt(dist_sq);
				Vector2f force = (diff / dist) * (strength / dist);

				i->AddForce(force);
			}
		}
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
	circle2.setPosition({ 100, 100 });
	CircleShape circle3(20);
	circle3.setFillColor(Color::Blue);
	circle3.setPosition({ 300, 100 });
	CircleShape circle4(30);
	circle4.setFillColor(Color::Green);
	circle4.setPosition({ 700, 100 });
	CircleShape circle5(10);
	circle5.setFillColor(Color::Yellow);
	circle5.setPosition({ 500, 100 });
	Clock clock;
	RigidBody circle_body1(circle1.getRadius(), circle1.getPosition());
	RigidBody circle_body2(circle2.getRadius(), circle2.getPosition());
	RigidBody circle_body3(circle3.getRadius(), circle3.getPosition());
	RigidBody circle_body4(circle4.getRadius(), circle4.getPosition());
	RigidBody circle_body5(circle5.getRadius(), circle5.getPosition());
	circle_body1.mass = 1.0f;
	circle_body2.mass = 2.0f;
	circle_body3.mass = 3.0f;
	circle_body4.mass = 4.0f;
	circle_body5.mass = 5.0f;

	eng.AddObj(&circle_body1);
	eng.AddObj(&circle_body2);
	eng.AddObj(&circle_body3);
	eng.AddObj(&circle_body4);
	eng.AddObj(&circle_body5);
	while (window->isOpen())
	{
		float dt = clock.restart().asSeconds();
		window->clear();
		
		while (optional<Event> event = window->pollEvent())
		{
			if (event->is<Event::Closed>())
				window->close();
		}
		eng.Update(dt);
		eng.ResolveCollision();
		if (Mouse::isButtonPressed(Mouse::Button::Left))
		{
			Vector2i mouse = Mouse::getPosition(*window);

			eng.MouseVoid(Vector2f(mouse.x, mouse.y), 40.0f, 1000000.0f);
		}

		circle1.setPosition(circle_body1.position);
		circle2.setPosition(circle_body2.position);
		circle3.setPosition(circle_body3.position);
		circle4.setPosition(circle_body4.position);
		circle5.setPosition(circle_body5.position);
		window->draw(circle1);
		window->draw(circle2);
		window->draw(circle3);
		window->draw(circle4);
		window->draw(circle5);
		window->display();
	}
}