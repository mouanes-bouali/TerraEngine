#include "RenderSystem.h"
#include "entities/Components.h"
#include <glm/gtc/matrix_transform.hpp>

void RenderSystem::collect(EntityManager& em, RenderContext& ctx)
{
    // Precompute signature on first call
    static bool sigInit = false;
    static Signature REQUIRED_SIGNATURE = 0;
    if (!sigInit) {
        REQUIRED_SIGNATURE = (1ULL << EntityManager::template getComponentTypeID<CTransform>())
                           | (1ULL << EntityManager::template getComponentTypeID<CRenderable>());
        sigInit = true;
    }

    // Get dense arrays for cache-friendly iteration
    auto& transformPool = *em.getPool<CTransform>();
    auto& transforms = transformPool.data();
    auto& owners = transformPool.ownerList();

    auto& renderablePool = *em.getPool<CRenderable>();
    auto& renderables = renderablePool.data();

    // Iterate the CTransform dense array (contiguous memory)
    for (size_t i = 0; i < owners.size(); ++i)
    {
        EntityID e = owners[i];

        // Single bitwise AND instead of two separate hasTag calls
        if (!em.matches(e, REQUIRED_SIGNATURE))
            continue;

        // Direct dense array access — no sparse lookup for CTransform
        auto& t = transforms[i];

        // One sparse lookup for CRenderable (still needed)
        int32_t ri = renderablePool.getSparseIndex(e);
        if (ri < 0 || (size_t)ri >= renderables.size()) continue;
        auto& r = renderables[ri];

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(t.x, t.y, t.z));
        model = glm::rotate(model, t.rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(t.scaleX, t.scaleY, t.scaleZ));

        ctx.opaque.push_back({
            .modelMatrix = model,
            .color       = r.color,
            .textureId   = r.textureId,
            .mesh        = r.meshHandle
        });
    }
}
