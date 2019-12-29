#pragma once




class FrameBuffer
{
public:

	FrameBuffer();
	~FrameBuffer();

	unsigned int id;
	unsigned int tex;
	unsigned int depth;

	void GenerateFBO(int width, int height);

	void Remove();

};

