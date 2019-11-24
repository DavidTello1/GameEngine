# Davos Game Engine
[Davos Game Engine](https://github.com/ponspack9/GameEngine) is a 3D Game Engine made by [Oscar Pons](https://github.com/ponspack9) and [David Tello](https://github.com/DavidTello1) for the Video Games degree in Universitat Politècnica de Catalunya (UPC).

## Camera Controls
- While Right clicking, “WASD” fps-like movement and free look around.
- SPACE to go upwards, LCTRL + SPACE to go downwards
- Mouse wheel to zoom in and out
- Alt + Right-click to orbit the object
- Pressing “f” focuses the camera around the geometry closest side (_WIP phase when parent is selected_)
- Holding SHIFT duplicates movement speed

## Innovation
- 

## Work division
- Óscar: quadtrees, mousepicking, camera FOV, frustum culling, time management
- David: inspector properties, own file format, reference counting, resource management

## Model loading instructions

### T90
- First load the _T90.fbx_ model. 
- Load _T90-Tracks.dds_ texture to the meshes 3,4
- Load any _T90-#.dds_ texture to the meshes 1,2,5
### Building
- First load the _Building.fbx_ model. 
- Load _Base.dds_ texture to the mesh 1
- Load any _Out.dds_ texture to the mesh 2

## License
MIT License

Copyright (c) 2019 Oscar Pons and David Tello

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
