# Assignment 1 - 15%
https://github.com/ponspack9/GameEngine

## License
Copyright 2019. Òscar Pons and David Tello

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## Controls
### Camera
- While right clicking, "WASD" to freely move around, SPACE to go upwards, CTRL+SPACE to go downwards (Holding left SHIFT doubles speed)
- Zoom using the mouse wheel
- 
## Overview
For our first assignment, we aim to deliver a simple geometry viewer supporting drag and drop and orbital
camera. The final release should load the provided model (baker_house.zip) plus other two you created.

## Submission rules
The delivery must be a Build zipped in its folder inside “First Assignment” named after your engine:
1. The build should be compiled in Release with all (and only) the material needed for execution
(including two other sample geometries to load).
2. There must be a text file called “README.md” containing info about the engine, author, a link to the
github repository and license. Any special instructions for execution should be included in this file,
as well as any system that you think could add to the innovation grade.
3. The repository under github.com must contain a copy of the build under the Release section. The
Release should be using a tag named “Assignment1.x” (x in case you make several deliveries).
4. Your own two models must be in FBX format with one channel diffuse texture in DDS format.
5. It should accept drag&drop of FBX files and DDS textures (only to show the last dropped) from
anywhere in the Hard Drive.
6. It should feature Unity-like camera controls:

- ~~While Right clicking, “WASD” fps-like movement and free look around must be enabled. SPACE to go upwards, LCTRL + SPACE to go downwards~~
- ~~Mouse wheel should zoom in and out~~-> **zoom is moving with Z axis? or resizing the viewport?**
- Alt+Left click should orbit the object
- Pressing “f” should focus the camera around the geometry
- ~~Holding SHIFT duplicates movement speed~~
- ~~Have a console window that should LOG the geometry loading process from ASSIMP~~
- There must be a configuration window containing at least:
- A graph for the frames per second
- Configuration for all variables on each module (renderer, window, input and textures)
- There must be information output with FPS graph, memory consumption, hardware detection
and software versions (SDL, OpenGL, Devil).

9. The camera must adapt to the size of the geometry after being dropped to move far or close
depending on the scale of the geometry.
10. There must be a general menu with: 
~~- option to quit~~
~~- visit the github page~~ 
- info about theengine (“About”). 
- It should also be able to turn on/off editor windows
11. A properties window with three sections: transformation, geometry and texture. All should give
information about the current loaded meshes and texture (triangle count, texture size, position,
rotation and scale).
12. If the FBX file contains multiple meshes, the system must be able to load all of them at the center of
the scene. It should only need to load one texture at a time.
13. You should have an option to show current geometry with checkers texture.
The zip must be submitted before October 20th 23:58 (folder closes automatically).

## Grading Criteria
To accept a submission for grading, it must comply with:
1. It followed the submission rules stated above.
2. The final .zip file cannot be bigger than 30 Mb.
3. The code compiles and uses only english.
4. It should be original. Since most of the functionality will be the same, this will be monitored closely.
5. The release did not crash while testing.
6. The FBX provided can load and renders correctly.
Once accepted, the criteria is as follows:
- 10%: Repository & Commit structure (small commits with clear description)
- 60%: Code structure / Good choice of containers / Const-correctness and use of pointers/references
- 30%: Unity-like camera controls & Editor tools (main menu, console, editor info and configuration,
properties)
Note: In case of a great imbalance in work between team members, teacher can decide to downgrade an
individual score.
Note: Remember that any experimental system could potentially impact the innovation grade. If you think
you have some system that falls in this category, clearly mention them in the README.
To achieve the maximum grade on each area the code is expected to be: const-correct, correct choice of
STL containers, efficient, simple and readable

## Innovation

Win64, DLL, octree...
