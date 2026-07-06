# Implementation Details: Solum Engine (Full Project Scope)
## Outline – Titles Only

---

## 1. Core Systems Architecture

### 1.1 Entity-Component-System (ECS)
- Sparse-set design (Dense + Sparse arrays)
- O(1) add/remove/lookup
- Free-list for ID recycling
- 64-bit Entity Signatures (bitset filtering)

### 1.2 Component Storage
- ComponentPool<T> (dense array, sparse array, entity map)
- Swap-back removal (O(1) deletion)
- Hot/Cold data separation

### 1.3 Systems & The Game Loop
- Fixed timestep accumulator
- ISystem interface (update method)
- System execution order (Input → AI → Movement → Collision → Combat → Render)
- Event Queue (Observer pattern)

### 1.4 Data-Oriented Design (Hot/Cold Split)
- CTransform (hot) – separate from CHealth (cold)
- Cache-friendly dense iteration
- Memory layout optimization

### 1.5 Design Patterns Applied
- Object Pool (free-list)
- Strategy (IRenderer)
- Observer (EventQueue)
- Builder (EntityBuilder)
- Game Loop (fixed timestep)
- Command (Input)
- State (AI finite states)
- Spatial Partition (grid)
- Dirty Flag (tile deformation)

---

## 2. Rendering Pipeline

### 2.1 Renderer Architecture
- IRenderer interface (abstraction)
- IOpenGLRenderer (concrete OpenGL 3.3 backend)

### 2.2 Shader System
- Runtime loading (.vert / .frag files)
- Compilation & linking with error logging
- Uniform setters (float, int, bool, vec, mat4)
- RAII resource management

### 2.3 Matrix Transformations
- GLM math library
- Model matrix (Scale → Rotate → Translate)
- View matrix (camera lookAt)
- Projection matrix (orthographic / perspective)
- Vertex shader transform

### 2.4 Textures
- stb_image.h loader (JPG, PNG, BMP)
- Texture units and samplers
- Mipmapping
- Wrapping modes (REPEAT, CLAMP, MIRROR)
- Filtering (LINEAR / NEAREST)
- Texture atlas (future)

### 2.5 Instanced Rendering
- glDrawElementsInstanced
- Per-instance data (model matrices via vertex attributes)
- Tile map rendering (10,000+ tiles)

### 2.6 Camera System
- Position, rotation, FOV, aspect ratio
- follow mode
- Zoom (scroll wheel)

---

## 3. World & Terrain

### 3.1 Tile Map
- TileMap class (grid management)
- Chunk system (future)
- Tile types (Grass, Dirt, Water, Stone, Forest)
- Tile properties (walkable, buildable)

### 3.2 Tile Deformation
- Vertex shader height displacement
- Dynamic terrain changes (craters, hills)
- Dirty flag system (rebuild only modified tiles)
- CPU → GPU height map uniform

### 3.3 Obstacles & Path Blockers
- Bushes, rocks, trees blocking movement
- Clear time mechanics

---

## 4. Game Prototype (Survival RTS)

### 4.1 Game-Specific Components
- CPlayer
- CEnemy
- CHealth (current/max)
- CCombat (damage, cooldown, range)
- CAIState (Idle, Chase, Attack, Flee)
- CResource (type, amount)
- CBuilder (work speed, cooldown)
- CBuilding (type, construction progress)

### 4.2 Game-Specific Systems
- sAI (state machines)
- sCombat (damage processing)
- sBuilding (construction, upgrades)
- sResourceGather (collection)
- sWaveSpawner (night cycles)

### 4.3 Flow-Field Pathfinding
- Integration (cost propagation from target)
- Flow vector generation
- O(tiles) per frame, O(1) per entity
- Handles 500+ enemies simultaneously

### 4.4 Entity Building (Fluent API)
- EntityBuilder.with<CTransform>().with<CHealth>().build()

### 4.5 AI Priorities
- Melee: nearest builder → walls → buildings → Inhibitor
- Ranged: defensive buildings → walls → resources → Inhibitor
- Tanks: walls → buildings → Inhibitor (high HP)

### 4.6 Day/Night Cycle
- Day: Resource gathering, building
- Night: Monster waves spawn
- Win condition: survive X nights

### 4.7 Building System
- Walls (first defense)
- Archer Tower (ranged damage)
- Resource Depot (storage cap)
- Barracks (train builders)
- Upgrade Center (unlock advanced)

---

## 5. Tooling & Development

### 5.1 Build System (CMake)
- C++20 standard
- Dependency management (SFML, OpenGL, GLM, GLAD, stb_image, ImGui)
- MSYS2 MINGW64 / Linux support
- Asset copying (POST_BUILD)

### 5.2 Version Control (Git)
- Conventional Commits (feat, fix, refactor, chore, docs)
- Atomic commits
- Imperative tense
- Body explains why

### 5.3 Debugging & Profiling (ImGui)
- Entity viewer
- Performance overlay (FPS, frame time)
- Tile editor
- Console (commands)
- Camera controls

### 5.4 Unit Testing
- Custom lightweight test harness
- ECS core coverage (EntityManager, ComponentPool)
- System tests (future)

---

## 6. Performance Targets

- 60 FPS stable
- 50,000 active entities (sparse)
- 10,000 rendered tiles (instanced)
- 500 enemies with flow-field AI
- < 256 MB RAM
- > 90% cache hit rate
- < 30 second build time

---

## 7. Challenges & Solutions

- GLAD linker errors
- Asset path resolution
- stb_image load failures
- OpenGL context activation
- AI performance collapse
- Tile deformation redraws
- ECS iteration speed
- Instancing shader limits
- Memory fragmentation

---

## 8. Future Work

- Job System (multithreading)
- Networking (multiplayer)
- Audio System (OpenAL)
- Scripting (Lua)
- Asset hot-reload
- Shadow Mapping
- Particle System
- Save/Load serialization

---

## 9. References

- *Game Programming Patterns* – Robert Nystrom
- *Data-Oriented Design* – Richard Fabian
- LearnOpenGL.com – Joey de Vries
- stb_image.h – Sean Barrett
- GLM Documentation – g-truc
- Flow-Field Pathfinding (GDC talks)