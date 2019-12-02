
#pragma once

struct Color
{
	float r, g, b, a;
	
	Color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f)
	{}

	Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a)
	{}

	void Set(float r, float g, float b, float a = 1.0f)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	float* operator & ()
	{
		return (float*)this;
	}

	void operator -= (float scalar)
	{
		this->r -= scalar;
		this->g -= scalar;
		this->b -= scalar;
		//this->a -= scalar;
		Clamp();
	}
	
	void Clamp()
	{
		this->r = (this->r < 0) ? 0 : this->r;
		this->g = (this->g < 0) ? 0 : this->g;
		this->b = (this->b < 0) ? 0 : this->b;
		this->a = (this->a < 0) ? 0 : this->a;

		this->r = (this->r > 255.0f) ? 255.0f : this->r;
		this->g = (this->g > 255.0f) ? 255.0f : this->g;
		this->b = (this->b > 255.0f) ? 255.0f : this->b;
		this->a = (this->a > 255.0f) ? 255.0f : this->a;

	}
};

extern Color Red;
extern Color Green;
extern Color Blue;
extern Color Cyan;
extern Color Yellow;
extern Color Black;
extern Color White; 
extern Color DarkGrey;
extern Color Grey;
extern Color LightGrey; 