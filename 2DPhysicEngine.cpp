#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <vector>
using namespace std;
using namespace sf;

class PhysBody abstract 
{
	Vector2f position;
	Vector2f velocity = { 0, 0 };
	Vector2f acceleration = { 0, 0 };
	float resilience = 0.8f;
	float mass;
};

class AABB : public PhysBody
{

};

class RigitBody : public PhysBody
{
	float radius;

};

class PhysicEngine
{
	vector<PhysBody*> objects;
	Vector2f gravity = { 0, 9.8f };

public:
	void KeepInBound()
	{

	}

	void ApplyGravity(float dt)
	{

	}

	void ApplyForce(Vector2f force)
	{

	}
};