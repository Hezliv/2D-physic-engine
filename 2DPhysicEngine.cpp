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
protected:
	Vector2f position;
	Vector2f velocity = { 0, 0 }; // speed
	Vector2f acceleration = { 0, 0 }; //ускорение
	float resilience = 0.8f; // коэф упругость
	Vector2f force_accum = { 0, 0 };
	Vector2f normal;
	float angle; // angle rotatio in radians
	float angular_velocity; // speed with which object rotates
	
	float mass;
	float inv_mass;
	float height;
	float width;

public:
	virtual ~PhysBody() = default;

	void AddForce(Vector2f force)
	{
		force_accum += force;
	}

	void SetPosition(Vector2f position)
	{
		this->position = position;
	}

	void SetVelocity(Vector2f velocity)
	{
		this->velocity = velocity;
	}

	void SetAcceleration(Vector2f acceleration)
	{
		this->acceleration = acceleration;
	}

	void SetResilience(float resilience)
	{
		this->resilience = resilience;
	}

	void SetForce(Vector2f force)
	{
		this->force_accum = force;
	}

	void SetNormal(Vector2f normal)
	{
		this->normal = normal;
	}

	void SetMass(float mass)
	{
		this->mass = mass;
	}

	void SetInvMass(float inv_mass)
	{
		this->inv_mass = inv_mass;
	}

	void SetHeight(float height)
	{
		this->height = height;
	}

	void SetWidth(float width)
	{
		this->width = width;
	}

	Vector2f GetPosition()
	{
		return position;
	}

	Vector2f GetVelocity()
	{
		return velocity;
	}

	Vector2f GetAcceleration()
	{
		return acceleration;
	}

	float GetResilience()
	{
		return resilience;
	}
	
	Vector2f GetForce()
	{
		return force_accum;
	}

	Vector2f GetNormal()
	{
		return normal;
	}

	float GetMass()
	{
		return mass;
	}

	float GetInvMass()
	{
		return inv_mass;
	}

	float GetHeight()
	{
		return height;
	}

	float GetWidth()
	{
		return width;
	}

	Vector2f GetCenter()
	{
		return position + Vector2f(width / 2.0f, height / 2.0f);
	}
};

class PhysicEngine
{
	//vector<PhysBody*> objects;
	Vector2f gravity = { 0, 500.0f }; //for monitor would be better use 980 raither our 9.8 for gravity, here I use 400 for more smooth but not like in moon(300.0f)
	Vector2u size;
	float resistance = 0.98f;
	float air_resistance = 0.01f;
public:

	PhysicEngine(Vector2u size) : size(size) {}
	/*void AddObj(PhysBody* obj)
	{
		objects.push_back(obj);
	}*/

	/*void ApplyGravity(PhysBody* obj)
	{
		obj->acceleration = -gravity;
	}*/

	void SetGravity(Vector2f& gravity)
	{
		this->gravity = gravity;
	}

	void SetSize(Vector2u& size)
	{
		this->size = size;
	}

	void SetResistance(float resistance)
	{
		this->resistance = resistance;
	}

	void SetAirResistance(float air_resistance)
	{
		this->air_resistance = air_resistance;
	}

	Vector2f GetGravity()
	{
		return gravity;
	}

	Vector2u GetSize()
	{
		return size;
	}

	float GetResistance()
	{
		return resistance;
	}

	float GetAirResistance()
	{
		return air_resistance;
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

	/*vector<PhysBody*> GetObjects()
	{
		return objects;
	}*/
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

	float Distance(Vector2f first, Vector2f second) 
	{
		return sqrt(pow(first.x - second.x, 2) + pow(first.y - second.y, 2));
	}

	void KeepInBound(PhysicEngine* engine)
	{
		if (position.x + (width / 2) >= engine->GetSize().x)
		{
			position.x = engine->GetSize().x - (width / 2);
			if (velocity.x > 0) velocity.x = -velocity.x * resilience;
		}

		else if (position.x - (width / 2) <= 0)
		{
			position.x = width / 2;
			if (velocity.x < 0) velocity.x = -velocity.x * resilience;
		}

		if (position.y + (height / 2) >= engine->GetSize().y)
		{
			position.y = engine->GetSize().y - (height / 2);
			if (velocity.y > 0)
			{
				velocity.y = -velocity.y * resilience;
				velocity.x *= engine->GetResistance();
			}
			if (abs(velocity.y) < 5.0f) velocity.y = 0;
		}

		else if (position.y - (height / 2) <= 0)
		{
			position.y = height / 2;
			if (velocity.y < 0) velocity.y = -velocity.y * resilience;
			
		}
	}

	void Update(PhysicEngine* eng, float dt)
	{
		if (dt > 0.1f) dt = 0.1f;
		
		Vector2f drag_force = -velocity * eng->GetAirResistance();
		Vector2f Ftotal = force_accum + (eng->GetGravity() * mass) + drag_force;

		acceleration = Ftotal / mass;

		velocity += acceleration * dt;
		
		position += velocity * dt;
		
		KeepInBound(eng);

		force_accum = { 0, 0 };
	}

	void ResolveCollision(RigidBody* obj) // rewrite with inv_mass
	{
		if (Distance(GetCenter(), obj->GetCenter()) < (this->width / 2 + obj->width / 2)) // only for circles
		{
			float m = 1 / mass + 1 / obj->mass; // вес

			Vector2f pos_diff = GetCenter() - obj->GetCenter(); // разница между расстояниями
			Vector2f velocity_diff = velocity - obj->velocity; // разница скорости

			float dist = Distance(GetCenter(), obj->GetCenter());
			float e = min(resilience, obj->resilience);
			if (dist < 0.001f) dist = 0.001f;
			normal = pos_diff / dist;

			float scalar = velocity_diff.x * normal.x + velocity_diff.y * normal.y;
			float impulse = -(1 + e) * scalar / m;

			Vector2f jm = normal * impulse;


			velocity += jm / mass; // обмен скоростями
			obj->velocity -= jm / obj->mass;

			float penetration = (height / 2 + obj->height / 2) - Distance(GetCenter(), obj->GetCenter());

			float percent = 0.8f;
			float slop = 0.01f;

			Vector2f correction = normal * (max(penetration - slop, 0.0f) / (1 / mass + 1 / obj->mass) * percent);

			position += correction / mass; // обмен скоростями
			obj->position -= correction / obj->mass;
		}

	}

	Vector2f GetPosition()
	{
		return position;
	}
};

class IMouseTouchable
{
public:
	virtual Vector2f GetCenter() = 0;
	
	virtual void AddForce(Vector2f force) = 0;
}; 

class Ball : public IMouseTouchable { // decide what do with class wrapper
	RigidBody* physics;
	CircleShape* visual;
	Color color;

public:
	Ball(float radius, Vector2f position)
	{
		physics = new RigidBody(radius, position);
		visual = new CircleShape();
		visual->setPosition(position);
		visual->setRadius(radius);
		visual->setOrigin({radius, radius});
	}

	Vector2f GetCenter()
	{
		return physics->GetCenter();
	}

	void SetRadius(float radius)
	{
		physics->SetHeight(radius);
		physics->SetWidth(radius);
		visual->setRadius(radius);
	}

	float GetRadius()
	{
		return physics->GetHeight();
	}

	void SetColor(Color color)
	{
		this->color = color;
		visual->setFillColor(color);
	}

	Color GetColor()
	{
		return color;
	}

	Vector2f GetPosition()
	{
		return physics->GetPosition();	
	}

	CircleShape& GetVisual()
	{
		return *visual;
	}

	void Update(PhysicEngine* eng, float dt)
	{
		physics->Update(eng, dt);
		visual->setPosition(physics->GetPosition());
	}

	void ResolveCollision(Ball* ball)
	{
		physics->ResolveCollision(ball->physics);
		visual->setPosition(physics->GetPosition());
		ball->visual->setPosition(ball->physics->GetPosition());
	}

	void AddForce(Vector2f force)
	{
		physics->AddForce(force);
		visual->setPosition(physics->GetPosition());
	}
};

// rewrite method KeepInBounds because of problem with stucking circle objects
// organize code, transfer methods which useful for circle objects to their own class
//rewrite interaction

class MouseInteraction {
	vector<IMouseTouchable*> objects;
public:
	void MouseVoid(Vector2f mouse_pos, float radius, float strength)
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

	void AddObject(IMouseTouchable* phys_obj)
	{
		objects.push_back(phys_obj);
	}

	void DeleteObject(IMouseTouchable* obj)
	{
		for (int i = 0; i < objects.size(); i++)
		{
			if (objects[i] == obj)
			{
				objects.erase(objects.begin() + i);
				break;
			}
		}
	}
};

int main() 
{
	Vector2u size = { 1000, 700 };
	RenderWindow window = RenderWindow(VideoMode(size), "PhysicEngine");
	PhysicEngine eng = PhysicEngine(size);

	Ball* ball1 = new Ball(20, {100, 10});
	ball1->SetColor(Color::White);
	Ball* ball2 = new Ball(20, { 100, 30 });
	ball1->SetColor(Color::White);
	Clock clock;

	MouseInteraction m;
	m.AddObject(ball1);
	m.AddObject(ball2);

	while (window.isOpen())
	{
		window.clear();
		float dt = clock.restart().asSeconds();
		while (optional<Event> event = window.pollEvent())
		{
			if (event->is<Event::Closed>())
				window.close();	
			
		}

		if (Mouse::isButtonPressed(Mouse::Button::Left))
		{
			Vector2i mouse = Mouse::getPosition(window);
			m.MouseVoid(Vector2f(mouse.x, mouse.y), 100.0f, 1000000.0f);
		}
		ball1->ResolveCollision(ball2);

		ball1->Update(&eng, dt);
		ball2->Update(&eng, dt);

		window.draw(ball1->GetVisual());
		window.draw(ball2->GetVisual());
		window.display();
	}
}
