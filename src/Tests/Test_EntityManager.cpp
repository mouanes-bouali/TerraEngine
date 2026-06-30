#include "entities/EntityManager.h"
#include "entities/EntityBuilder.h"
#include <cassert>
#include <iostream>

#define TEST(name) void name()
#define RUN_TEST(name) do { \
    std::cout << "  " << #name << "... "; \
    name(); \
    std::cout << "PASSED\n"; \
} while(0)

TEST(entity_creation) {
    EntityManager em;
    EntityBuilder builder(em);
    EntityID e = builder().with<CTransform>().build();
    assert(e == 0);
    EntityID e2 = builder().with<CHealth>().build();
    assert(e2 == 1);
}

TEST(component_add_and_get) {
    EntityManager em;
    EntityBuilder builder(em);
    EntityID e = builder().with<CTransform>(CTransform(100.0f, 200.0f)).build();
    auto& t = em.getComponent<CTransform>(e);
    assert(t.x == 100.0f && t.y == 200.0f);
}

TEST(component_overwrite) {
    EntityManager em;
    EntityBuilder builder(em);
    EntityID e = builder().with<CHealth>(CHealth(100, 100)).build();
    em.addComponent<CHealth>(e, CHealth(50, 50));
    auto& h = em.getComponent<CHealth>(e);
    assert(h.current == 50);
}

TEST(hasTag) {
    EntityManager em;
    EntityBuilder builder(em);
    EntityID e = builder().with<CTransform>(CTransform(0, 0)).build();
    assert(em.hasTag<CTransform>(e));
    assert(!em.hasTag<CHealth>(e));
}

TEST(signature_matching) {
    EntityManager em;
    EntityBuilder builder(em);
    EntityID e = builder().with<CTransform>(CTransform(0, 0)).with<CHealth>(CHealth(100, 100)).build();
    assert(em.hasTag<CTransform>(e) && em.hasTag<CHealth>(e));
}

TEST(entity_destruction) {
    EntityManager em;
    EntityBuilder builder(em);
    EntityID e = builder().with<CTransform>(CTransform(0, 0)).with<CHealth>(CHealth(100, 100)).build();
    em.destroyEntity(e);
    Signature sig = 0;
    sig |= (1ULL << 0);
    assert(!em.matches(e, sig));
}

TEST(free_list_reuse) {
    EntityManager em;
    EntityBuilder builder(em);
    EntityID e0 = builder().with<CTransform>().build();
    em.destroyEntity(e0);
    EntityID e1 = builder().with<CTransform>().build();
    assert(e1 == e0);
}

TEST(movement_simulation) {
    EntityManager em;
    EntityBuilder builder(em);
    for (int i = 0; i < 3; ++i)
        builder().with<CTransform>(CTransform(i * 100.0f, 0.0f)).with<CHealth>(CHealth(100, 100)).build();
    float dt = 1.0f / 60.0f;
    for (int frame = 0; frame < 60; ++frame)
        for (EntityID e = 0; e < 3; ++e)
            if (em.hasTag<CTransform>(e))
                em.getComponent<CTransform>(e).x += 10.0f * dt;
    auto& t0 = em.getComponent<CTransform>(0);
    assert(t0.x >= 9.9f && t0.x <= 10.1f);
}

int main() {
    std::cout << "EntityManager Tests\n===================\n";
    RUN_TEST(entity_creation);
    RUN_TEST(component_add_and_get);
    RUN_TEST(component_overwrite);
    RUN_TEST(hasTag);
    RUN_TEST(signature_matching);
    RUN_TEST(entity_destruction);
    RUN_TEST(free_list_reuse);
    RUN_TEST(movement_simulation);
    std::cout << "\nAll tests passed.\n";
    return 0;
}