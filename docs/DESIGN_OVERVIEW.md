# Solum Engine — Design Overview

## 1. What It Is
- An Game engine for a top down survival game
-using high performance 
## 2. The Problem
-already exisitng big game engines have a poor performance for a game that needs many objects

## 3. Architecture
- the engine uses a ECS and DOD architecture for a readable  and performance boost by avoiding creating objects that are resources heavy for the CPU -objects are scatrred in the memeory and the CPU needs to fetch the in the memory where the objects is pointed to.So we represent it like this:
-Entites :
any object is just an entityID number.we can check all the entities by going through an entites list only.

-Components:
The behaviours of the entities, represented as data (Transform component saves the object place,rotatio and scale in the 3D world )
implemented using Structs. Each component saves entites IDs that implemented it, and consists of Dense and spare arrays for a complexity of O(1) fetching.

-Systems:
they implmenet the needed behaviours absed on the existing componenets(movement system moves entites with transform component)




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