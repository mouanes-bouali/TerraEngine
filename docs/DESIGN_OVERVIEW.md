# Solum Engine — Design Overview

## 1. What It Is
- An Game engine for a top down survival game
-using high performance 

### 2.1 Functional Requirements
These define **what** the engine must do:

### 2.2 Non-Functional Requirements
These define **how well** the engine must perform:

## 2. The Problem
-already exisitng big game engines have a poor performance for a game that needs many objects
  


### The Solution:
Implement the most suitable architecture : ECS(Entity Component-System)and DOD(Data Oriented Design)

## 3. Architecture

### 3.1 Entity-Component-System (ECS)

- The Engine uses ECS for a clear and readable code, seperating the code into 4 layers: Entity, Component, Systems and Render  
-Entites :
any object in the program is just an entityID number(unit32_t).Managed by Entity manager(create, detroy, check and add components), and Entity Builder for a clear and ez API for creating entities .

-Components:
The behaviours of the entities, represented as data stucts (Transform component saves the object place, rotation and scale in the 3D world ).
 Each component saves entites IDs that implemented it, and consists of Dense and spare arrays for a complexity of O(1) fetching.

-Systems:
they implmenet the needed behaviours based on the existing componenets(movement system moves entites with transform component)

-CameraSystem:
it owns the data of the view, it's work is to hundle the inputs and change the camera data depending on that, like moving camera in defrent directions, zoom in/out...
it's responsible about the camera data so any other system like the Renderer System can get the camera config at runtime.






### 3.2 Data-Oriented Design (DOD)

The engine follows Data-Oriented Design rules to maximize performance. the CPU cash operates on 64bytes cash lines and the rules works on maximizing the cash hits to nearly 90%. it helps a lot in the performance as the CPU fetching time from the memory is slow so we try to save data in a way it prefers.

#### 3.2.1 The Rules:
-

## 4. Key Decisions
- One line per ADR
- Format: what → why

## 5. Unique Systems
- One bullet per system
- Format: name → what makes it unique

## 6. SE Principles Applied
- One line per principle
- Format: principle → where in Solum

## 7. Tech Stack
- Table: tool → why

## 8. Performance
- Key numbers only
- 60 FPS, 500 entities, zero malloc

## 9. Links
- Full