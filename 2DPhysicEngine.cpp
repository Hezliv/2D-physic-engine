#include <SFML/Graphics.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include <math.h>
#include <vector>

# define M_PI 3.141592653589793238462643383279502884L
using namespace std;
using namespace sf;



class IMouseTouchable
{
public:
	virtual Vector2f GetCenter() = 0;
	virtual Vector2f GetSize() = 0;
	virtual void AddForce(Vector2f force) = 0;
};

class AABB // it's border for collisions
{

};

class ResultOfCollision //!!!!!!!!!!!!! to finish writing accessors
{
	Vector2f normal;
	float penetration;
	Vector2f contact_point;
public:
	void SetNormal(Vector2f normal)
	{
		this->normal = normal;
	}
};

class Collider 
{
public:
	virtual ~Collider() = default;

	virtual bool Intersects(Collider& other, Vector2f self_pos, Vector2f other_pos, ResultOfCollision& res) = 0;

	virtual AABB GetAABB(Vector2f pos) = 0;

	enum class Type {Circle, Box};

	virtual Type GetType() const = 0;
};

class PhysBody 
{
protected:
	Collider* collider;
	float mass;
	float inv_mass;
	float height;
	float width;
	Vector2f position;
	Vector2f velocity = { 0, 0 }; // speed
	Vector2f acceleration = { 0, 0 }; //ускорение
	float resilience = 0.8f; // коэф упругость
	Vector2f force_accum = { 0, 0 };
	Vector2f normal;
	float angle; // angle rotatio in radians
	float angular_velocity; // speed with which object rotates

public:
	virtual ~PhysBody() = default;

	void AddForce(Vector2f force)
	{
		force_accum += force;
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

	void SetPosition(Vector2f position)
	{
		this->position = position;
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

	Vector2f GetCenter()
	{
		return position + Vector2f(width / 2.0f, height / 2.0f);
	}
};

class SoftBody : public PhysBody // soft body like jelly
{

};

class RigidBody : public PhysBody // tough object
{	
public:
	RigidBody(float width, float height, Vector2f position)
	{
		height = height;
		width = width;
		this->collider->position = position;
		mass = (0.5 * height * height * M_PI) / 1000;
		inv_mass = 1 / mass;
	}
	
	Vector2f GetPosition()
	{
		return position;
	}

	void SetRotation(float radians)
	{

	}
};

class Sensor : public PhysBody // it's transparency, but it indicates if some body passed through
{

};


// rewrite method KeepInBounds because of problem with stucking circle objects
// organize code, transfer methods which useful for circle objects to their own class
//rewrite interaction

class MouseInteraction {
	vector<IMouseTouchable*> objects;
	Vector2u window_size;
public:
	MouseInteraction(Vector2u window_size) : window_size(window_size) {}

	void MousePush(Vector2f mouse_pos, float radius, float strength, bool version = 1)
	{
		for (auto i : objects)
		{
			Vector2f center = i->GetCenter();
			Vector2f diff;
			if (version)
			{
				diff = center - Vector2f(mouse_pos);
			}
			else
			{
				diff = Vector2f(mouse_pos) - center;
			}
			float dist_sq = diff.x * diff.x + diff.y * diff.y;

			if (dist_sq < radius * radius && dist_sq > 0)
			{
				float dist = sqrt(dist_sq);
				Vector2f force = (diff / dist) * (strength / dist);

				if (i->GetCenter().y + i->GetSize().y / 2 != window_size.y && force.y > 0)
					force.y = 0;

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

class PhysicEngine
{
	Vector2f gravity = { 0, 600.0f }; //for monitor would be better use 980 raither our 9.8 for gravity, here I use 400 for more smooth but not like in moon(300.0f)
	Vector2u size; // size to which will rules of physic engine works
	float resistance = 0.95f; // ground resistance
	float air_resistance = 0.01f;
public:

	PhysicEngine(Vector2u size) : size(size) {}

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

	void SetSize(Vector2u size)
	{
		this->size = size;
	}

	void CollisionSolve()
	{

	}

	void Step()
	{

	}

	void BoundCollision(PhysBody* obj, float dt)
	{
		if (obj->GetCenter().x + (obj->GetHeight() / 2) >= size.x)
		{
			obj->SetPosition({ size.x - (obj->GetHeight() / 2), obj->GetPosition().y });
			if (obj->GetVelocity().x > 0) obj->SetVelocity({ -obj->GetVelocity().x * obj->GetResilience(), obj->GetVelocity().y });
		}

		else if (obj->GetCenter().x + (obj->GetHeight() / 2) <= 0)
		{
			position.x = width / 2;
			if (velocity.x < 0) velocity.x = -velocity.x * resilience;
		}

		if (obj->GetCenter().y + (obj->GetHeight() / 2) >= size.y)
		{
			obj->SetPosition({ obj->GetPosition().x, size.y - (obj->GetHeight() / 2) });
			if (obj->GetVelocity().y > 0)
			{
				obj->SetVelocity({ obj->GetVelocity().x, -obj->GetVelocity().y * obj->GetResilience() });
				float rolling_frict = 0.95f;
				velocity.x *= pow(rolling_frict, dt * 10.0f);
			}
			if (abs(velocity.y) < 5.0f) velocity.y = 0;
		}

		else if (obj->GetCenter().y + (obj->GetHeight() / 2) <= 0)
		{
			position.y = height / 2;
			if (velocity.y < 0) velocity.y = -velocity.y * resilience;

		}
	}

	float Distance(Vector2f first, Vector2f second)
	{
		return sqrt(pow(first.x - second.x, 2) + pow(first.y - second.y, 2));
	}

	void Integration(PhysicEngine* eng, float dt)
	{
		if (dt > 0.1f) dt = 0.1f;

		Vector2f drag_force = -velocity * eng->GetAirResistance();
		Vector2f Ftotal = force_accum + (eng->GetGravity() * mass) + drag_force;

		acceleration = Ftotal / mass;

		velocity += acceleration * dt;
		angular_velocity = velocity.x / (width / 2.0f);
		angle += angular_velocity * dt;

		position += velocity * dt;

		force_accum = { 0, 0 };
	}
};


int main() 
{
	Vector2u window_size = { 1000, 700 };
	RenderWindow window = RenderWindow(VideoMode(window_size), "PhysicEngine");
	PhysicEngine eng = PhysicEngine(window_size);

	Ball* ball1 = new Ball(20, {100, 10});
	ball1->SetColor(Color::White);
	Ball* ball2 = new Ball(60, { 100, 300 });
	ball1->SetColor(Color::White);
	Clock clock;

	MouseInteraction m(window_size);
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
			Vector2i mouse_pos = Mouse::getPosition(window);
			m.MousePush(Vector2f(mouse_pos.x, mouse_pos.y), 200.0f, 100000.0f);
		}

		else if (Mouse::isButtonPressed(Mouse::Button::Right))
		{
			Vector2i mouse_pos = Mouse::getPosition(window);
			m.MousePush(Vector2f(mouse_pos.x, mouse_pos.y), window_size.x, 1000000.0f, 0);
		}

		ball1->ResolveCollision(ball2);

		ball1->Update(&eng, dt);
		ball2->Update(&eng, dt);

		window.draw(ball1->GetVisual());
		window.draw(ball2->GetVisual());
		window.display();
	}
}
