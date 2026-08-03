#pragma once
#include "api/WorldData.h"
#include "api/Engine.h"
#include <string>

// ─────────────────────────────────────────────────────────────────────
// WorldSerializer — save/load the entire world to/from a JSON file.
//
//   WorldSerializer::save("assets/world.json", engine);
//   WorldSerializer::load("assets/world.json", engine);
//
// The serializer reads the Engine's current state (terrain + entities)
// and converts it to WorldData, then writes it as JSON.
// On load, it rebuilds the world from the JSON data.
// ─────────────────────────────────────────────────────────────────────
class WorldSerializer {
public:
    // Save the current engine state to a JSON file
    static bool save(const std::string& path, Engine& engine);

    // Load a JSON file and rebuild the world in the engine
    static bool load(const std::string& path, Engine& engine);

    // Export current engine state to WorldData (without writing to file)
    static WorldData exportWorld(Engine& engine);

    // Import WorldData into the engine (rebuilds everything)
    static void importWorld(Engine& engine, const WorldData& world);

private:
    // JSON writing helpers (manual, no external JSON library needed)
    static std::string worldToJson(const WorldData& world);
    static WorldData jsonToWorld(const std::string& json);

    // Entity data extraction from the ECS
    static EntityData extractEntity(Engine& engine, EntityID id);
};