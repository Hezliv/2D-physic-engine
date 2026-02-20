#include <SFML/Graphics.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include <math.h>
#include <vector>

# define M_PI 3.141592653589793238462643383279502884L
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
		mass = (0.5 * height * height * M_PI) / 1000;
	}
};

class Ball { // decide what do with class wrapper
public:
	Vector2f position;
	RigidBody* physics;
	CircleShape* visual;
	Vector2f normal;

	Vector2f GetCenter()
	{

	}

	void SetPosition(Vector2f& pos)
	{
		position = pos;
	}

};

// rewrite method KeepInBounds because of problem with stucking circle objects
// optimize the Distance method
// organize code, transfer methods which useful for circle objects to thir own class

class PhysicEngine
{
	vector<PhysBody*> objects;
	Vector2f gravity = { 0, 500.0f }; //for monitor would be better use 980 raither our 9.8 for gravity, here I use 400 for more smooth but not like in moon(300.0f)
	Vector2u size;
	float resistance = 0.2f;
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
				if (Distance(Vector2f( objects[i]->GetCenter().x, objects[i]->GetCenter().y), Vector2f(objects[j]->GetCenter().x, objects[j]->GetCenter().y)) < (objects[i]->width / 2 + objects[j]->width / 2)) // only for circles
				{
					float mass = 1 / objects[i]->mass + 1 / objects[j]->mass; // вес

					Vector2f pos_diff = objects[i]->GetCenter() - objects[j]->GetCenter(); // разница между расстояниями
					Vector2f velocity_diff = objects[i]->velocity - objects[j]->velocity; // разница скорости

					float dist = Distance(objects[i]->GetCenter(), objects[j]->GetCenter());
					float e = min(objects[i]->resilience, objects[j]->resilience);
					if (dist < 0.001f) dist = 0.001f;
					Vector2f normal = pos_diff / dist;
					
					float scalar = velocity_diff.x * normal.x + velocity_diff.y * normal.y;
					float impulse = -(1 + e) * scalar / mass;
					
					Vector2f jm = normal * impulse;
					
					//cout << objects[i]->velocity.x << " " << objects[i]->velocity.y << "\n";
					//cout << objects[j]->velocity.x << " " << objects[j]->velocity.y << "\n";

					objects[i]->velocity += jm / objects[i]->mass; // обмен скоростями
					objects[j]->velocity -= jm / objects[j]->mass;

					float penetration = (objects[i]->height / 2 + objects[j]->height / 2) - Distance(objects[i]->GetCenter(), objects[j]->GetCenter());

					float percent = 0.8f;
					float slop = 0.01f;

					Vector2f correction = normal * (max(penetration - slop, 0.0f) / (1 / objects[i]->mass + 1 / objects[j]->mass) * percent);

					objects[i]->position += correction / objects[i]->mass; // обмен скоростями
					objects[j]->position -= correction / objects[j]->mass;
				}
			}
		}
	}

	void PositionCorrection(PhysBody* obj1, PhysBody* obj2)// redo 
	{
		
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

	vector<PhysBody*> GetObjects()
	{
		return objects;
	}
};

class MouseInteraction {
public:
	void MouseVoid(vector<PhysBody*>& objects, Vector2f mouse_pos, float radius, float strength)
	{
		for (auto i : objects)
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
	circle2.setFillColor(Color::White);
	circle2.setPosition({ 100, 100 });
	CircleShape circle3(20);
	circle3.setFillColor(Color::White);
	circle3.setPosition({ 300, 100 });
	CircleShape circle4(30);
	circle4.setFillColor(Color::White);
	circle4.setPosition({ 700, 100 });
	CircleShape circle5(10);
	circle5.setFillColor(Color::White);
	circle5.setPosition({ 500, 100 });
	Clock clock;
	RigidBody circle_body1(circle1.getRadius(), circle1.getPosition());
	RigidBody circle_body2(circle2.getRadius(), circle2.getPosition());
	RigidBody circle_body3(circle3.getRadius(), circle3.getPosition());
	RigidBody circle_body4(circle4.getRadius(), circle4.getPosition());
	RigidBody circle_body5(circle5.getRadius(), circle5.getPosition());

	MouseInteraction m;
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
			auto temp = eng.GetObjects();
			m.MouseVoid(temp, Vector2f(mouse.x, mouse.y), 100.0f, 1000000.0f);
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