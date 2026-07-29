//What is this project
A costume 3D game engine built for a 3D top-down survival game,optimized for rendering thousands of objects at 60 fps

Why im building the project
existing engines(unity, unreal) are designed for general use.
they waste memory and Cpu on features the game doesn't need.By building a costume engine, I control exactly what runs each frame (no garbage collection), no unnecessary physics, no editor overhead.

3/How
**Research**
Studied exisitng ECS (Unity DOTS, EnTT) to understand the patterns and 
pitfalls.  
**Architecture**
Applied Entity-Component-System(ECS) for a cache-friendly data layout and Data Oriented Design for predictable performance, finally applying software engineering principals(Design patterns, SOLID) for a maintainable, scaleable, testable and readable
**Implementation**

**Implementation** — Built modular systems that operate on 
   component data independently (low coupling) and does one job (high cohesion):
   - Entity Manager (entity lifecycle)
   - Render System (GPU instancing)
   - Terrain System (procedural generation)
   - Physics System (planned)
   - UI System (ImGui overlay)

**Challenges & Solutions**

| Challenge | Solution |
|-----------|----------|
| Learning modern C++ syntax | Studied C++20 features (concepts, templates) through small practice projects |
| Linking external libraries | Used CMake with FetchContent for automatic dependency management |
| Building extensible systems | Designed around interfaces (IRenderer, IInput) so implementations can be swapped |


