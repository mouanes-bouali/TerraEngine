#pragma once
#include "entities/EntityManager.h"
#include "renderer/IRenderer.h"
#include "entities/Components.h"
#include <vector>

struct RenderContext {
    std::vector<RenderInstance> opaque;
};

class RenderSystem {
public:
    // Precompute the signature for entities that have BOTH CTransform + CRenderable
    inline static Signature REQUIRED_SIGNATURE = 0;
    inline static bool initialized = false;

    void collect(EntityManager& em, RenderContext& ctx);
};