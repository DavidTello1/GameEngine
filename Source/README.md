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
  - Prespective transformation's here (projection 3D -> 2D)
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
The vertex shader is a program on the graphics card that processes each vertex and its attributes as they appear in the vertex array. **Final** vertex position in **device coordinates**
