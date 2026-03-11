#include <SFML/Graphics.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include <math.h>
#include <vector>
#include <tuple>

# define M_PI 3.141592653589793238462643383279502884L
using namespace std;
using namespace sf;

class AABB // it's border for collisions
{

};

class ResultOfCollision
{
	Vector2f normal;
	float penetration; // the distance over which they intersect
	Vector2f contact_point;
public:
	void SetNormal(Vector2f normal)
	{
		this->normal = normal;
	}

	void SetPenetration(float penetration) 
	{
		this->penetration = penetration;
	}

	void SetContactPoint(Vector2f contact_point)
	{
		this->contact_point = contact_point;
	}

	Vector2f GetNormal()
	{
		return normal;
	}

	float GetPenetration() 
	{
		return penetration;
	}

	Vector2f GetContactPoint()
	{
		return contact_point;
	}
};

class Collider  // it's borders to object
{
protected:
	float height;
	float width;
	Vector2f position; // center of shape
public:
	virtual ~Collider() = default;

	virtual bool Intersects(Collider* other) = 0;

	virtual AABB GetAABB(Vector2f pos) = 0;

	enum class Type {Circle, Box};

	virtual Type GetType() const = 0;

	void SetHeight(float height)
	{
		this->height = height;
	}

	void SetWidth(float width)
	{
		this->width = width;
	}

	void SetPosition(Vector2f position)
	{
		this->position = position;
	}

	Vector2f GetPosition()
	{
		return position;
	}

	float GetHeight()
	{
		return height;
	}

	float GetWidth()
	{
		return width;
	}
};

class CircleCollider : public Collider
{
	float radius;
public:
	CircleCollider(float radius) : radius(radius) {}

	virtual Type GetType() const override
	{
		return Type::Circle;
	}

	virtual bool Intersects(Collider* other) override
	{
		float distance_radius = this->radius + (other->GetWidth() / 2);
		float distance_centers = sqrt(pow(this->GetPosition().x - other->GetPosition().x, 2) + pow(this->GetPosition().y - other->GetPosition().y, 2));
		
		//cout << distance_radius << "  " << distance_centers << "\n";
		return distance_radius < distance_centers ? false : true;
	}

	virtual AABB GetAABB(Vector2f pos) override 
	{
		return AABB();
	}

	float GetWidth()
	{
		return radius * 2;
	}

	float GetHeight()
	{
		return radius * 2;
	}
};

class BoxCollider : public Collider 
{
public:
	virtual Type GetType() const override
	{
		return Type::Box;
	}
};

class PhysBody 
{
protected:
	Collider* collider;
	float mass;
	float inv_mass;
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

	void SetAngle(float angle)
	{
		this->angle = angle;
	}

	void SetAngularVelocity(float angular_velocity)
	{
		this->angular_velocity = angular_velocity;
	}

	void SetCollider(Collider* collider)
	{
		this->collider = collider;
	}

	Collider* GetCollider()
	{
		return collider;
	}

	float GetAngle()
	{
		return angle;
	}

	float GetAngular_velocity()
	{
		return angular_velocity;
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
		return position /*+ Vector2f(collider->GetWidth() / 2.0f, collider->GetHeight() / 2.0f)*/;
	}
};

class SoftBody : public PhysBody // soft body like jelly
{

};

class RigidBody : public PhysBody // tough object
{	
public:
	RigidBody(Collider* collider, float width, float height, Vector2f position)
	{
		this->collider = collider;
		collider->SetHeight(height);
		collider->SetWidth(width);
		collider->SetPosition(position);
		this->position = position;

		mass = (0.5 * height * height * M_PI) / 1000;
		inv_mass = 1 / mass;
	}

	~RigidBody() {}
	
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

class MouseInteraction {
	vector<PhysBody*> objects;
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

				if (i->GetCenter().y + i->GetCollider()->GetHeight() / 2 != window_size.y && force.y > 0)
					force.y = 0;

				i->AddForce(force);

			}
		}
	}

	void AddObject(PhysBody* phys_obj)
	{
		objects.push_back(phys_obj);
	}

	void DeleteObject(PhysBody* obj)
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

class Entity
{
	PhysBody* body;
	Shape* visual;
public:
	Entity(Shape* visual, PhysBody* body) : body(body), visual(visual) {}
	Entity() : Entity(nullptr, nullptr) {}

	void SetVisual(Shape* visual)
	{
		this->visual = visual;
	}

	Shape* GetVisual()
	{
		return visual;
	}

	void SetPhysBody(PhysBody* body)
	{
		this->body = body;
	}

	PhysBody* GetPhysBody()
	{
		return body;
	}
};

class PhysicEngine
{
	vector<PhysBody*> bodies;
	Vector2f gravity = { 0, 600.0f }; //for monitor would be better use 980 raither our 9.8 for gravity, here I use 400 for more smooth but not like in moon(300.0f)
	Vector2u size; // size of engine
	float resistance = 0.98f; // ground resistance
	float air_resistance = 0.01f;
public:

	PhysicEngine(Vector2u size) : size(size) {}

	void SetGravity(Vector2f gravity)
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

	void CollisionSolve(Entity* obj1, Entity* obj2, float dt)
	{
		if (obj1->GetPhysBody()->GetCollider()->Intersects(obj2->GetPhysBody()->GetCollider()))
		{
			Vector2f delta = obj2->GetPhysBody()->GetPosition() - obj1->GetPhysBody()->GetPosition();
			float distance = sqrt(delta.x * delta.x + delta.y * delta.y);
			if (distance > 0)
			{
				Vector2f collision_normal = delta / distance;
				Vector2f collision_normal2 = -collision_normal;
				float push = (obj1->GetPhysBody()->GetCollider()->GetWidth() / 2) + (obj2->GetPhysBody()->GetCollider()->GetWidth() / 2) - distance;
				
				Vector2f v1 = obj1->GetPhysBody()->GetVelocity();
				Vector2f v2 = obj2->GetPhysBody()->GetVelocity();

				float dot1 = v1.x * collision_normal .x + v1.y * collision_normal.y;
				float dot2 = v2.x * collision_normal2.x + v2.y * collision_normal2.y;

				Vector2f reflect_velocity1 = v1 - 2.0f * dot1 * collision_normal;
				Vector2f reflect_velocity2 = v2 - 2.0f * dot2 * collision_normal2;

				obj1->GetPhysBody()->SetVelocity(reflect_velocity1 * obj1->GetPhysBody()->GetResilience());
				obj2->GetPhysBody()->SetVelocity(reflect_velocity2 * obj2->GetPhysBody()->GetResilience());

				cout << obj1->GetPhysBody()->GetVelocity().x << " " << obj1->GetPhysBody()->GetVelocity().y << "    " << obj2->GetPhysBody()->GetVelocity().x << " " << obj2->GetPhysBody()->GetVelocity().y << "\n";
				
				obj1->GetPhysBody()->SetPosition(obj1->GetPhysBody()->GetPosition() - collision_normal * (push / 2));
				obj2->GetPhysBody()->SetPosition(obj2->GetPhysBody()->GetPosition() - collision_normal2 * (push / 2));
			}		
		}
	}

	void Step(vector<Entity*>& obj, float dt)
	{
		for(int i = 0; i < obj.size(); i++)
		{
			Integration(obj[i]->GetPhysBody(), dt);
			BoundCollision(obj[i]->GetPhysBody(), dt);
			if(i < obj.size() - 1)
				CollisionSolve(obj[i], obj[i + 1], dt);
			obj[i]->GetVisual()->setPosition(obj[i]->GetPhysBody()->GetPosition());
		}
	}

	void Render(vector<Entity*>& obj,RenderWindow* window)
	{
		for (auto i : obj)
		{
			window->draw(*i->GetVisual());
		}
	}

	void BoundCollision(PhysBody* obj, float dt)
	{
		if (obj->GetCenter().x + (obj->GetCollider()->GetWidth() / 2) >= size.x)
		{
			obj->SetPosition({ size.x - (obj->GetCollider()->GetWidth() / 2), obj->GetPosition().y});
			if (obj->GetVelocity().x > 0) obj->SetVelocity({ -obj->GetVelocity().x * obj->GetResilience(), obj->GetVelocity().y });
		}

		else if (obj->GetCenter().x - (obj->GetCollider()->GetWidth() / 2) <= 0)
		{
			obj->SetPosition({ obj->GetCollider()->GetWidth() / 2, obj->GetPosition().y });
			if (obj->GetVelocity().x < 0) obj->SetVelocity({ -obj->GetVelocity().x * obj->GetResilience(), obj->GetVelocity().y });
		}

		if (obj->GetCenter().y + (obj->GetCollider()->GetHeight() / 2) >= size.y)
		{
			obj->SetPosition({ obj->GetPosition().x, size.y - (obj->GetCollider()->GetHeight() / 2) });
			if (obj->GetVelocity().y > 0)
			{
				obj->SetVelocity({ obj->GetVelocity().x, -obj->GetVelocity().y * obj->GetResilience() });
				float rolling_frict = 0.98f;
				obj->SetVelocity({ obj->GetVelocity().x * pow(rolling_frict, dt * 10.0f), obj->GetVelocity().y });
			}
			if (abs(obj->GetVelocity().y) < 5.0f) obj->SetVelocity({ obj->GetVelocity().x, 0 });
		}

		else if (obj->GetCenter().y - (obj->GetCollider()->GetHeight() / 2) <= 0)
		{
			obj->SetPosition({ obj->GetPosition().x, obj->GetCollider()->GetHeight() / 2 });
			if (obj->GetVelocity().y < 0) obj->SetVelocity({ obj->GetVelocity().x, -obj->GetVelocity().y * obj->GetResilience() });

		}
	}

	float Distance(Vector2f first, Vector2f second)
	{
		return sqrt(pow(first.x - second.x, 2) + pow(first.y - second.y, 2));
	}

	void Integration(PhysBody* obj, float dt)
	{
		if (dt > 0.1f) dt = 0.1f;

		Vector2f drag_force = -obj->GetVelocity() * GetAirResistance();
		Vector2f Ftotal = obj->GetForce() + (GetGravity() * obj->GetMass()) + drag_force;

		obj->SetAcceleration(Ftotal * obj->GetInvMass());

		obj->SetVelocity(obj->GetVelocity() + obj->GetAcceleration() * dt);
		obj->SetAngularVelocity(obj->GetVelocity().x / (obj->GetCollider()->GetWidth() / 2.0f));
		obj->SetAngle(obj->GetAngle() + obj->GetAngular_velocity() * dt);

		obj->SetPosition(obj->GetPosition() + obj->GetVelocity() * dt);

		obj->GetCollider()->SetPosition(obj->GetPosition());
		obj->SetForce({ 0, 0 });
	}
};


int main() 
{
	RenderWindow window = RenderWindow(VideoMode({1000, 700}), "PhysicEngine");
	PhysicEngine eng = PhysicEngine(window.getSize());

	CircleShape* circle1 = new CircleShape(20);
	circle1->setFillColor(Color::White);
	circle1->setPosition({ 100, 20 });
	circle1->setOrigin({circle1->getRadius(), circle1->getRadius() });
	CircleShape* circle2 = new CircleShape(30);
	circle2->setFillColor(Color::White);
	circle2->setPosition({ 100, 200 });
	circle2->setOrigin({ circle2->getRadius(), circle2->getRadius() });
	Collider* collid1 = new CircleCollider(circle1->getRadius());
	Collider* collid2 = new CircleCollider(circle2->getRadius());
	RigidBody* circle_body1 = new RigidBody(collid1, circle1->getRadius() * 2, circle1->getRadius() * 2, circle1->getPosition());
	RigidBody* circle_body2 = new RigidBody(collid2, circle2->getRadius() * 2, circle2->getRadius() * 2, circle2->getPosition());
	Entity* entity1 = new Entity(circle1, circle_body1);
	Entity* entity2 = new Entity(circle2, circle_body2);

	vector<Entity*> ent = { entity1, entity2 };
	Clock clock;

	MouseInteraction m(window.getSize());
	for (auto i : ent)
		m.AddObject(i->GetPhysBody());

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
			m.MousePush(Vector2f(mouse_pos.x, mouse_pos.y), window.getSize().x, 1000000.0f, 0);
		}

		eng.Step(ent, dt);
		eng.Render(ent, &window);
		window.display();
	
	}

	
}
