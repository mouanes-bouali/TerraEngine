# Solum Engine — TDD

## 1. Abstract
- What, why, unique, proves

## 2. Problem Statement
- What breaks in Unity/Godot for this genre
- The 4 specific problems

## 3. Requirements
- 3.1 Functional Requirements (FR table)
- 3.2 Non-Functional Requirements (NFR table)
- 3.3 Constraints (hardware, platform, build time)

## 4. Software Engineering Principles
- 4.1 Separation of Concerns
- 4.2 Dependency Rule
- 4.3 Interface Contracts (one per module)
- 4.4 Open/Closed (add types, never change core)
- 4.5 Testability (every module isolated)
- 4.6 Design Patterns (one line each, where used)

## 5. Use Cases
- 5.1 Use Case Diagram
- 5.2 UC01: Player places building
- 5.3 UC02: Enemy wave spawns
- 5.4 UC03: Tile destroyed

## 6. Architecture
- 6.1 Layer Diagram
- 6.2 Subsystem Map
- 6.3 Module Interfaces
- 6.4 Data Flow Per Frame
- 6.5 Event Flow

## 7. Architecture Decision Records
- ADR-001: Language — C++17
- ADR-002: Rendering — OpenGL 3.3
- ADR-003: Pathfinding — Flow Field
- ADR-004: Memory — Object Pools
- ADR-005: Entity System — Sparse Set ECS
- ADR-006: Tiles — Stateful Structs
- ADR-007: AI — Priority Tables
- ADR-008: Build Mode — Data-Driven

## 8. Unique Systems
Each system has:
- Problem
- Existing solution + why it fails
- Your solution
- SE principles applied
- Interface contract
- Code snippet
- Visual/diagram

Systems:
- 8.1 Elastic Tile Physics
- 8.2 Flow Field Pathfinding
- 8.3 Living Tile State Machine
- 8.4 Data-Driven Build Mode
- 8.5 Game Phase Manager

## 9. Engineering Knowledge Applied
- Table: concept → where in Solum

## 10. Testing
- 10.1 Unit Test Table
- 10.2 Integration Tests
- 10.3 Performance Benchmark
- 10.4 Traceability Matrix (FR/NFR → system → test)

## 11. Challenges & Solutions
- One bullet per challenge
- Problem → root cause → fix

## 12. Conclusion
- Requirements met (table)
- What I learned
- What I'd do differently
- Future work

## References
## Appendices