#include "api/WorldSerializer.h"
#include "api/EntityBuilder.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

// ── JSON helpers (manual, no external library) ──

static std::string esc(const std::string& s) {
    std::string out;
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\t': out += "\\t";  break;
            default:   out += c;      break;
        }
    }
    return out;
}

static std::string fmt(float v) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(4) << v;
    return ss.str();
}

// ── Export: read engine state → WorldData ──

EntityData WorldSerializer::extractEntity(Engine& engine, EntityID id)
{
    EntityData ed;
    ed.id = id;

    auto& scene = engine.scene();

    // Tag (name + type)
    if (scene.has<CTag>(id)) {
        auto& tag = scene.get<CTag>(id);
        ed.name = tag.name;
        ed.type = tag.type;
    }

    // Transform
    if (scene.has<CTransform>(id)) {
        auto& t = scene.get<CTransform>(id);
        ed.transform = {t.x, t.y, t.z, t.rotationX, t.rotationY, t.rotationZ, t.scaleX, t.scaleY, t.scaleZ};
    }

    // Mesh/Renderable — store the mesh NAME so we can re-map the GPU handle on load
    if (scene.has<CRenderable>(id)) {
        auto& r = scene.get<CRenderable>(id);
        MeshRefData md;
        md.handle = r.meshHandle;
        md.textureId = r.textureId;
        md.r = r.color.r; md.g = r.color.g; md.b = r.color.b; md.a = r.color.a;
        // Look up the mesh name from the library (handle 0 = cube, handle 1+ = loaded)
        auto& lib = engine.meshLibrary();
        if (r.meshHandle >= 1 && r.meshHandle < lib.count() + 1) {
            md.name = lib.get(r.meshHandle - 1).name;
        } else {
            md.name = "cube";
        }
        ed.mesh = md;
    }

    // Health
    if (scene.has<CHealth>(id)) {
        auto& h = scene.get<CHealth>(id);
        ed.health = HealthData{h.current, h.max};
    }

    // Player
    if (scene.has<CPlayer>(id)) {
        auto& p = scene.get<CPlayer>(id);
        ed.player = PlayerData{p.jumpSpeed, p.moveSpeed};
    }

    // Gravity
    if (scene.has<CGravity>(id)) {
        auto& g = scene.get<CGravity>(id);
        ed.gravity = GravityData{g.strength, g.weight};
    }

    // Collider
    if (scene.has<CCollider>(id)) {
        auto& c = scene.get<CCollider>(id);
        ed.collider = ColliderData{c.radius, c.offsetY};
    }

    return ed;
}

WorldData WorldSerializer::exportWorld(Engine& engine)
{
    WorldData world;
    world.name = "Solum World";
    world.version = "1.0";

    // Terrain data
    auto& terrain = engine.terrain().internal();
    world.terrain.resolution = terrain.terrainConfig.gridSize;
    world.terrain.heightScale = terrain.terrainConfig.heightScale;
    world.terrain.mapScale = 2.0f;  // TODO: get from TerrainAPI
    world.terrain.roughness = 0.5f;
    world.terrain.amplitude = 10.0f;

    // Heightmap data
    if (terrain.hasHeightmap()) {
        auto& hm = terrain.internalHeightmap();
        world.terrain.heightmap = hm.heights;
    }

    // Entities (skip entity 0 = terrain)
    auto& scene = engine.scene();
    for (EntityID i = 1; i < scene.entityCount(); ++i) {
        if (scene.exists(i)) {
            world.entities.push_back(extractEntity(engine, i));
        }
    }
    world.nextEntityId = scene.entityCount();

    return world;
}

// ── Import: WorldData → rebuild engine state ──

void WorldSerializer::importWorld(Engine& engine, const WorldData& world)
{
    // Clear existing entities (except terrain entity 0)
    auto& scene = engine.scene();
    for (EntityID i = 1; i < scene.entityCount(); ++i) {
        if (scene.exists(i)) {
            scene.destroy(i);
        }
    }

    // Rebuild terrain from heightmap
    auto& terrain = engine.terrain();
    if (!world.terrain.heightmap.empty()) {
        auto& hm = terrain.internal().internalHeightmap();
        int res = world.terrain.resolution;
        hm.resolution = res;
        hm.heights = world.terrain.heightmap;
        terrain.setHeightScale(world.terrain.heightScale);
        terrain.setMapScale(world.terrain.mapScale);
        terrain.upload();
    }

    // Recreate all entities
    for (const auto& ed : world.entities) {
        auto builder = scene.create()
            .setPosition(ed.transform.x, ed.transform.y, ed.transform.z)
            .setRotation(ed.transform.rotationX, ed.transform.rotationY, ed.transform.rotationZ)
            .setScale(ed.transform.scaleX, ed.transform.scaleY, ed.transform.scaleZ)
            .setTag(ed.name, ed.type);

        if (ed.mesh.has_value()) {
            // Re-map the GPU handle by mesh NAME (handles change across restarts)
            uint32_t newHandle = 0;  // default: cube
            auto& lib = engine.meshLibrary();
            if (ed.mesh->name.empty() || ed.mesh->name == "cube") {
                newHandle = 0;
            } else {
                for (size_t mi = 0; mi < lib.count(); ++mi) {
                    if (lib.get(mi).name == ed.mesh->name) {
                        newHandle = static_cast<uint32_t>(mi + 1);  // +1 because cube = 0
                        break;
                    }
                }
            }
            builder.setMesh(newHandle)
                   .setColor(ed.mesh->r, ed.mesh->g, ed.mesh->b, ed.mesh->a);
            if (ed.mesh->textureId >= 0)
                builder.setTexture(ed.mesh->textureId);
        }

        if (ed.health.has_value())
            builder.tagHealth(ed.health->current, ed.health->max);

        if (ed.player.has_value()) {
            builder.makePlayer(ed.player->jumpSpeed, ed.player->moveSpeed);
            if (!ed.gravity.has_value())
                builder.addGravity(9.8f, 1.0f);
            if (!ed.collider.has_value())
                builder.addCollider(1.0f, 0.0f);
        }

        if (ed.gravity.has_value())
            builder.addGravity(ed.gravity->strength, ed.gravity->weight);

        if (ed.collider.has_value())
            builder.addCollider(ed.collider->radius, ed.collider->offsetY);

        builder.build();
    }

    std::cout << "World imported: " << world.entities.size() << " entities\n";
}

// ── JSON serialization (manual) ──

std::string WorldSerializer::worldToJson(const WorldData& world)
{
    std::ostringstream ss;
    ss << "{\n";
    ss << "  \"version\": \"" << esc(world.version) << "\",\n";
    ss << "  \"name\": \"" << esc(world.name) << "\",\n";
    ss << "  \"nextEntityId\": " << world.nextEntityId << ",\n";

    // Terrain
    ss << "  \"terrain\": {\n";
    ss << "    \"resolution\": " << world.terrain.resolution << ",\n";
    ss << "    \"heightScale\": " << fmt(world.terrain.heightScale) << ",\n";
    ss << "    \"mapScale\": " << fmt(world.terrain.mapScale) << ",\n";
    ss << "    \"roughness\": " << fmt(world.terrain.roughness) << ",\n";
    ss << "    \"amplitude\": " << fmt(world.terrain.amplitude) << ",\n";
    ss << "    \"heightmap\": [";
    for (size_t i = 0; i < world.terrain.heightmap.size(); ++i) {
        if (i > 0) ss << ",";
        if (i % 20 == 0) ss << "\n      ";
        ss << fmt(world.terrain.heightmap[i]);
    }
    ss << "\n    ]\n";
    ss << "  },\n";

    // Entities
    ss << "  \"entities\": [\n";
    for (size_t i = 0; i < world.entities.size(); ++i) {
        const auto& ed = world.entities[i];
        ss << "    {\n";
        ss << "      \"id\": " << ed.id << ",\n";
        ss << "      \"name\": \"" << esc(ed.name) << "\",\n";
        ss << "      \"type\": \"" << esc(ed.type) << "\",\n";
        ss << "      \"isActive\": " << (ed.isActive ? "true" : "false") << ",\n";
        ss << "      \"transform\": {"
           << "\"x\":" << fmt(ed.transform.x)
           << ",\"y\":" << fmt(ed.transform.y)
           << ",\"z\":" << fmt(ed.transform.z)
           << ",\"rotationX\":" << fmt(ed.transform.rotationX)
           << ",\"rotationY\":" << fmt(ed.transform.rotationY)
           << ",\"rotationZ\":" << fmt(ed.transform.rotationZ)
           << ",\"scaleX\":" << fmt(ed.transform.scaleX)
           << ",\"scaleY\":" << fmt(ed.transform.scaleY)
           << ",\"scaleZ\":" << fmt(ed.transform.scaleZ)
           << "}";

        if (ed.mesh.has_value()) {
            ss << ",\n      \"mesh\": {"
               << "\"handle\":" << ed.mesh->handle
               << ",\"name\":\"" << esc(ed.mesh->name) << "\""
               << ",\"textureId\":" << ed.mesh->textureId
               << ",\"r\":" << fmt(ed.mesh->r)
               << ",\"g\":" << fmt(ed.mesh->g)
               << ",\"b\":" << fmt(ed.mesh->b)
               << ",\"a\":" << fmt(ed.mesh->a)
               << "}";
        }
        if (ed.health.has_value()) {
            ss << ",\n      \"health\": {"
               << "\"current\":" << ed.health->current
               << ",\"max\":" << ed.health->max
               << "}";
        }
        if (ed.player.has_value()) {
            ss << ",\n      \"player\": {"
               << "\"jumpSpeed\":" << fmt(ed.player->jumpSpeed)
               << ",\"moveSpeed\":" << fmt(ed.player->moveSpeed)
               << "}";
        }
        if (ed.gravity.has_value()) {
            ss << ",\n      \"gravity\": {"
               << "\"strength\":" << fmt(ed.gravity->strength)
               << ",\"weight\":" << fmt(ed.gravity->weight)
               << "}";
        }
        if (ed.collider.has_value()) {
            ss << ",\n      \"collider\": {"
               << "\"radius\":" << fmt(ed.collider->radius)
               << ",\"offsetY\":" << fmt(ed.collider->offsetY)
               << "}";
        }
        ss << "\n    }";
        if (i + 1 < world.entities.size()) ss << ",";
        ss << "\n";
    }
    ss << "  ]\n";
    ss << "}\n";
    return ss.str();
}

// ── JSON parsing (simple manual parser) ──
// This is a minimal parser — handles the format we write above.

static std::string extractStr(const std::string& json, const std::string& key) {
    std::string search = "\"" + key + "\": \"";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return "";
    pos += search.size();
    size_t end = json.find("\"", pos);
    if (end == std::string::npos) return "";
    return json.substr(pos, end - pos);
}

static float extractFloat(const std::string& json, const std::string& key) {
    std::string search = "\"" + key + "\":";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return 0.0f;
    pos += search.size();
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\n')) pos++;
    return std::stof(json.substr(pos));
}

static int extractInt(const std::string& json, const std::string& key) {
    return static_cast<int>(extractFloat(json, key));
}

static bool extractBool(const std::string& json, const std::string& key) {
    std::string search = "\"" + key + "\": ";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return false;
    pos += search.size();
    return json.substr(pos, 4) == "true";
}

static std::vector<float> extractFloatArray(const std::string& json, const std::string& key) {
    std::vector<float> result;
    std::string search = "\"" + key + "\": [";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return result;
    pos += search.size();
    while (pos < json.size() && json[pos] != ']') {
        while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\n' || json[pos] == '\t')) pos++;
        if (pos >= json.size() || json[pos] == ']') break;
        size_t end = pos;
        while (end < json.size() && json[end] != ',' && json[end] != ']') end++;
        std::string numStr = json.substr(pos, end - pos);
        // trim whitespace
        while (!numStr.empty() && (numStr.back() == ' ' || numStr.back() == '\n')) numStr.pop_back();
        if (!numStr.empty()) result.push_back(std::stof(numStr));
        pos = end;
        if (pos < json.size() && json[pos] == ',') pos++;
    }
    return result;
}

WorldData WorldSerializer::jsonToWorld(const std::string& json)
{
    WorldData world;
    world.version = extractStr(json, "version");
    world.name = extractStr(json, "name");
    world.nextEntityId = extractInt(json, "nextEntityId");

    // Terrain
    world.terrain.resolution = extractInt(json, "resolution");
    world.terrain.heightScale = extractFloat(json, "heightScale");
    world.terrain.mapScale = extractFloat(json, "mapScale");
    world.terrain.roughness = extractFloat(json, "roughness");
    world.terrain.amplitude = extractFloat(json, "amplitude");
    world.terrain.heightmap = extractFloatArray(json, "heightmap");

    // Entities (simple extraction — finds each entity block)
    size_t pos = json.find("\"entities\": [");
    if (pos != std::string::npos) {
        pos += 13;
        while (pos < json.size()) {
            size_t blockStart = json.find('{', pos);
            if (blockStart == std::string::npos) break;
            size_t blockEnd = json.find('}', blockStart);
            if (blockEnd == std::string::npos) break;
            std::string block = json.substr(blockStart, blockEnd - blockStart + 1);

            EntityData ed;
            ed.id = extractInt(block, "id");
            ed.name = extractStr(block, "name");
            ed.type = extractStr(block, "type");
            ed.isActive = extractBool(block, "isActive");
            ed.transform.x = extractFloat(block, "x");
            ed.transform.y = extractFloat(block, "y");
            ed.transform.z = extractFloat(block, "z");
            ed.transform.rotationX = extractFloat(block, "rotationX");
            ed.transform.rotationY = extractFloat(block, "rotationY");
            ed.transform.rotationZ = extractFloat(block, "rotationZ");
            ed.transform.scaleX = extractFloat(block, "scaleX");
            ed.transform.scaleY = extractFloat(block, "scaleY");
            ed.transform.scaleZ = extractFloat(block, "scaleZ");

            if (block.find("\"mesh\"") != std::string::npos) {
                MeshRefData m;
                m.handle = extractInt(block, "handle");
                m.name = extractStr(block, "name");
                m.textureId = extractInt(block, "textureId");
                m.r = extractFloat(block, "r");
                m.g = extractFloat(block, "g");
                m.b = extractFloat(block, "b");
                m.a = extractFloat(block, "a");
                ed.mesh = m;
            }
            if (block.find("\"health\"") != std::string::npos) {
                HealthData h;
                h.current = extractInt(block, "current");
                h.max = extractInt(block, "max");
                ed.health = h;
            }
            if (block.find("\"player\"") != std::string::npos) {
                PlayerData p;
                p.jumpSpeed = extractFloat(block, "jumpSpeed");
                p.moveSpeed = extractFloat(block, "moveSpeed");
                ed.player = p;
            }
            if (block.find("\"gravity\"") != std::string::npos) {
                GravityData g;
                g.strength = extractFloat(block, "strength");
                g.weight = extractFloat(block, "weight");
                ed.gravity = g;
            }
            if (block.find("\"collider\"") != std::string::npos) {
                ColliderData c;
                c.radius = extractFloat(block, "radius");
                c.offsetY = extractFloat(block, "offsetY");
                ed.collider = c;
            }

            world.entities.push_back(ed);
            pos = blockEnd + 1;
        }
    }

    return world;
}

// ── File I/O ──

bool WorldSerializer::save(const std::string& path, Engine& engine)
{
    WorldData world = exportWorld(engine);
    std::string json = worldToJson(world);

    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "WorldSerializer::save: cannot open " << path << "\n";
        return false;
    }
    file << json;
    file.close();

    std::cout << "World saved to " << path
              << " (" << world.entities.size() << " entities)\n";
    return true;
}

bool WorldSerializer::load(const std::string& path, Engine& engine)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "WorldSerializer::load: cannot open " << path << "\n";
        return false;
    }
    std::stringstream ss;
    ss << file.rdbuf();
    file.close();

    WorldData world = jsonToWorld(ss.str());
    importWorld(engine, world);

    std::cout << "World loaded from " << path
              << " (" << world.entities.size() << " entities)\n";
    return true;
}