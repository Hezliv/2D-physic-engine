#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
using namespace sf;
using namespace std;

namespace My {

	struct Shape : public sf::Drawable
	{
		virtual ~Shape() = default;
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
		virtual Shape& SetPosition(int x, int y) = 0;

	};

	class Essence abstract
	{
	protected:
		float height;
		float width;
		Color color;
		int x;
		int y;
	};

	class Window
	{
		RenderWindow* window;
		unsigned int height;
		unsigned int width;
		int x;
		int y;
		void SetWindow(unsigned int height, unsigned int width, string title, Color color = Color::Black)
		{
			this->height = height;
			this->width = width;
			window = new sf::RenderWindow(sf::VideoMode(sf::Vector2u(width, height)), title);
		}
	public:
		Window(unsigned int height, unsigned int width)
		{
			SetWindow(height, width, "2D Physics Engine");
		}

		sf::RenderWindow& GetWindow()
		{
			return *window;
		}

		void Draw(My::Shape* shape)
		{
			window->draw(*shape);
		}

		bool IsOpen()
		{
			return window->isOpen();
		}

		void Close()
		{
			window->close();
		}

		void Clear(Color color = Color::Black)
		{
			window->clear(color);
		}

		void Display()
		{
			window->display();
		}

		std::optional<sf::Event> PollEvent()
		{
			return window->pollEvent();
		}
	};

	class Circle : public Essence, public Shape
	{
		sf::CircleShape circle;


		void SetCircle(float radius, Color color)
		{
			this->color = color;
			this->height = radius;
			this->width = radius;
			circle.setFillColor(color);
			circle.setRadius(radius);
		}

	public:
		Circle(float radius, Color color)
		{
			SetCircle(radius, color);
		}
		Circle()
		{
			SetCircle(5, Color::Red);
		}
		
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override
		{
			target.draw(circle, states);
		}

		My::Shape& SetPosition(int x, int y) override
		{
			circle.setPosition(sf::Vector2f(x, y));
			this->x = x;
			this->y = y;
			return *this;
		}

		sf::Vector2f GetPosition()
		{
			return sf::Vector2f(x, y);
		}
	};

	class Square : public Essence, public Shape
	{
		sf::RectangleShape square;
	public:
		void SetCircle(float height, float width, Color color)
		{
			this->color = color;
			this->height = height;
			this->width = width;
			square.setFillColor(color);
			square.setSize({ width, height });
		}

		void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
			target.draw(square, states);
		}

		My::Shape& SetPosition(int x, int y) override {
			square.setPosition(sf::Vector2f(x, y));
			return *this;
		}

		sf::Vector2f GetPosition()
		{
			return sf::Vector2f(x, y);
		}
	};
}

class PhysicEngine {
	My::Window window = My::Window(100, 100);
	float g = 10;
	float jump = 10;

public:
	PhysicEngine(float g, float jump) : g(g), jump(jump) {}
	PhysicEngine() : PhysicEngine(10, 5) {}

	void SetWindow(int height, int width)
	{
		window = My::Window(height, width);
	}

	void Start()
	{
		vector<My::Shape*> shapes;
		shapes.push_back(new My::Circle(2, Color::Magenta));
		shapes[0]->SetPosition(10, 10);
		shapes.push_back(new My::Circle());
		shapes[1]->SetPosition(100, 100);
		shapes.push_back(new My::Circle(10, Color::Blue));
		shapes[2]->SetPosition(200, 200);


		while (window.IsOpen())
		{
			while (std::optional<Event> event = window.PollEvent())
			{
				if (event->is<Event::Closed>())
					window.Close();
			}

			window.Clear();


			for (vector<My::Shape*>::iterator it = shapes.begin(); it != shapes.end(); it++)
			{
				window.Draw(*it);
			}

			window.Display();
		}
	}

};
int main()
{
	PhysicEngine eng;
	eng.SetWindow(600, 600);
	eng.Start();
    
}
