#include "api/RendererAPI.h"
#include <iostream>

uint32_t RendererAPI::uploadMesh(const MeshData& data)
{
    if (!m_renderer) {
        std::cerr << "RendererAPI::uploadMesh: no renderer bound\n";
        return 0;
    }
    return m_renderer->uploadMesh(data);
}

void RendererAPI::updateMesh(uint32_t handle, const MeshData& data)
{
    if (!m_renderer) {
        std::cerr << "RendererAPI::updateMesh: no renderer bound\n";
        return;
    }
    m_renderer->updateMesh(handle, data);
}

uint32_t RendererAPI::loadMesh(const char* filepath)
{
    if (!m_renderer) {
        std::cerr << "RendererAPI::loadMesh: no renderer bound\n";
        return 0;
    }
    return m_renderer->loadMesh(filepath);
}

int RendererAPI::loadTexture(const char* filepath)
{
    if (!m_renderer) {
        std::cerr << "RendererAPI::loadTexture: no renderer bound\n";
        return -1;
    }
    return m_renderer->loadTexture(filepath);
}

void RendererAPI::setCamera(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up)
{
    if (!m_renderer) {
        std::cerr << "RendererAPI::setCamera: no renderer bound\n";
        return;
    }
    m_renderer->setCamera(pos, target, up);
}

void RendererAPI::bind(IRenderer* renderer)
{
    m_renderer = renderer;
}