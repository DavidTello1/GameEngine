# OpenGL basics

## Pipeline

### Vertices / Vertexs
These are the points from which shapes like triangles will later be constructed. (Array) Attributes:
  - 3D position in the world 
  - Texture coordinates
  - My attribute
 
### Vertex shader
Vertex shader: small program running on your graphics card that processes every one of the input vertices individually.
Primitives... Triangle, triangle strips, line strips... -> **Shape assembly**
  - Transformation's here (projection 3D -> 2D)
  - Color coordinates
  - Texture coordinates
  
### Geometry shader
Optional. It takes the primitives from the shape assembly stage as input and can either pass a primitive through down to the rest of the pipeline, modify it first, completely discard it or even replace it with other primitive(s)

### Fragment shader
The rasterizer turns the visible parts of the shapes into pixel-sized **fragments**. The vertex attributes coming from the vertex shader or geometry shader are interpolated and passed as input to the fragment shader for each fragment.

The _fragment shader_ processes each individual fragment along with its interpolated attributes and should output the final color.
  - Shading
  - Lighting
  - Special effects
  
### Tests and blending
Finally, the end result is composed from all these shape fragments by blending them together and performing depth and stencil testing.

### Device coordinates
Data of graphics card -> vertex attributes. **World position**: attribute that will determine where the objects and shapes end up on your screen in the end. After pipeline the coordinates are transformed into **device coordinates** [-1,1]

--- 

## Vertex input
Uploading vertex data to the graphics card. **Vertex Buffer Object (VBO)**
```c++
float vertices[] = {
     0.0f,  0.5f, // Vertex 1 (X, Y)
     0.5f, -0.5f, // Vertex 2 (X, Y)
    -0.5f, -0.5f  // Vertex 3 (X, Y)
};
```
- Creating VBO:
```c++
GLuint vbo;
glGenBuffers(1, &vbo); // Generate 1 buffer
```
- Making it active: This statement makes the VBO we just created the active array buffer. 
```c++
glBindBuffer(GL_ARRAY_BUFFER, vbo);
```
 Now that it's active we can copy the vertex data to it.
 ```c++
 glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
```
Notice that this function doesn't refer to the id of our VBO, but instead to the active array buffer
Usage of vertex data:
- GL_STATIC_DRAW: The vertex data will be uploaded once and drawn many times (e.g. the world).
- GL_DYNAMIC_DRAW: The vertex data will be created once, changed from time to time, but drawn many times more than that.
- GL_STREAM_DRAW: The vertex data will be uploaded once and drawn once.
---
## Shaders

### Vertex shader
The vertex shader is a program on the graphics card that processes each vertex and its attributes as they appear in the vertex array. Its duty is to output the final vertex position in device coordinates and to output any data the fragment shader requires. That's why the **3D transformation** should take place here. The fragment shader depends on attributes like the color and texture coordinates. It must output the **Final** vertex position, in **device coordinates**(auto).

```c++
#version 150 core

// vec# and mat# work with floats
in vec2 position;

void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
}
```

### Fragment shader
The output from the vertex shader is interpolated over all the pixels on the screen covered by a primitive. These _pixels_ are called **fragments** and this is what the fragment shader operates on. It has one mandatory **output**, the **final color** of a fragment.
It's up to you to write the code for computing this color **from vertex colors**, **texture** coordinates and **any other data** coming from the vertex shader.
Color range [0,1]

```c++
#version 150 core

out vec4 outColor;

void main()
{
    outColor = vec4(1.0, 1.0, 1.0, 1.0);
}
```

## Compiling shaders
- Source code as string</br>
  - Compiling shaders is easy once you have loaded the source code (either from file or as a hard-coded string). You can easily include your shader source in the C++ code through C++11 raw string literals:

```c++
const char* vertexSource = R"glsl(
    #version 150 core

    in vec2 position;

    void main()
    {
        gl_Position = vec4(position, 0.0, 1.0);
    }
)glsl";
```

- Creating shader
  - Simply pass a reference to shader functions. The last parameter can contain an array of source code string lengths, passing _NULL_  makes it stop at the null terminator.

```c++
GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
glShaderSource(vertexShader, 1, &vertexSource, NULL);

//Compiling shader
glCompileShader(vertexShader);
```

- Checking if a shader compiled successfully
```c++
GLint status;
glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);

if (status == GL_TRUE) // Nice, compiled successfully
// Compile log
char buffer[512];
glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
```

- **Fragment shader** is compiled the same way
```c++
GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
glCompileShader(fragmentShader);
```

## Combining shaders
**Attach** both shaders into a programm
```c++
GLuint shaderProgram = glCreateProgram();
glAttachShader(shaderProgram, vertexShader);
glAttachShader(shaderProgram, fragmentShader);
```
Since a fragment shader is allowed to write to multiple buffers, you need to explicitly specify which output is written to which buffer. This needs to happen before linking the program. The default output is 0.
```c++
glBindFragDataLocation(shaderProgram, 0, "outColor");
```
> Use glDrawBuffers when rendering to multiple buffers, because only the first output will be enabled by default.

After attaching both the fragment and vertex shaders, the connection is made by **linking** the program.
```c++
glLinkProgram(shaderProgram);
```
To start using the shaders in the program:
```c++
glUseProgram(shaderProgram);
```
Only **one** program can be **active** at a time.

> A shader object can be **deleted** with glDeleteShader, but it will not actually be removed before it has been detached from all programs with glDetachShader.

### Linking vertex data and attributes
Although we have our vertex data and shaders now, OpenGL still doesn't know how the attributes are formatted and ordered. You first need to retrieve a reference to the position input in the vertex shader:
```c++
GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
```
The **location** is a number depending on the order of the input definitions.</br>
The following function will also **store the VBO that is currently bound to _GL_ARRAY_BUFFER_ and will be stored in the currently bound VAO**. That means that you don't have to explicitly bind the correct VBO when the actual drawing functions are called. This also implies that you can use a different VBO for each attribute.</br>
With the reference to the input, you can specify how the data for that input is retrieved from the array:
```c++
glVertexAttribPointer(input reference, # of values, type of values, values to be normalized, stride, offset);
glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

glEnableVertexAttribArray(posAttrib);
```
- **stride**: how many bytes are between each position attribute in the array. The value 0 means that there is no data in between. This is currently the case as the position of each vertex is immediately followed by the position of the next vertex.
- **offset**: how many bytes from the start of the array the attribute occurs. Since there are no other attributes, this is 0 as well.

## Vertex Array Objects (VAO)
VAOs store all of the links between the attributes and your VBOs with raw vertex data.
```c++
GLuint vao;
glGenVertexArrays(1, &vao);

glBindVertexArray(vao);
```
