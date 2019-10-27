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
- Option to automatically focus to window hovered *may cause some problems (WIP)*
- Add/Remove Components from Inspector panel
- Hierarchy panel has trees
- Rename GameObjects from Hierarchy panel (Double-click | Right-click)
- Extensive primitives creation options
- GPU detection & Memory statistics
- Bounding Box for each mesh (with editable options in Configuration->Scene panel)
- Bounding Box for parent of meshes *WIP*

## Features
- Multiple game objects can be selected (with Left CTRL) to make texture loading process faster
- Selected game objects can be deleted with button _Delete_ or with the key _SUPR_
- If a game object is selected inside _Hierarchy_, the new gameobjects will be created as childs of it
- All loading actions can be automatically made drag and dropping the file directly to the editor

## Model loading instructions
### Tank
- First load the _tank.fbx_ model. 
- Load _tankTracks.dds_ texture to the meshes 1,2
- Load any _tank#.dds_ texture to the meshes 3,4,5,6
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
