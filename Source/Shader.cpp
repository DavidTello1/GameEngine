#include "Shader.h"

const GLchar* Shader::vertex_source = 
R"glsl(

    #version 150 core

    in vec3 position;
	in vec3 normal;
	in vec4 color;
	in vec2 texcoord;

	out vec3 Normal;
	out vec4 Color;
	out vec2 Texcoord;

    void main()
    {
        gl_Position = vec4(position, 1.0);
		Normal = normal;
		Color = color;
		Texcoord = texcoord;
    }
)glsl";

const GLchar* Shader::fragment_source = R"glsl(
    #version 150 core

	uniform sampler2D tex;

	in vec3 Normal;
	in vec4 Color;
	in vec2 Texcoord;

	out vec4 outColor;

	void main()
	{
		outColor = texture(tex, Texcoord);
	}
)glsl";