#include "grid.hpp"
#include <game_interface.hpp>
#include <math_helper.hpp>

void Grid::doCreate() { }
void Grid::doUpdate(float const elapsed)
{
    if (getGame()->input().mouse()->justPressed(jt::MouseButtonCode::MBLeft)) {
        if (m_currentShape == nullptr) {
            m_currentShape = std::make_shared<jt::Shape>();
            m_currentShape->makeRect(jt::Vector2f { 100.0f, 10.0f }, textureManager());
            m_shapeStart = getGame()->input().mouse()->getMousePositionWorld();
            m_currentShape->setPosition(m_shapeStart);
            m_currentShape->setOrigin(jt::Vector2f { 5.0f, 5.0f });
        } else {
            m_shapes.push_back(m_currentShape);
            m_currentShape->setColor(jt::colors::Blue);
            m_currentShape = nullptr;
        }
    }

    if (m_currentShape) {
        auto const dif = getGame()->input().mouse()->getMousePositionWorld() - m_shapeStart;
        auto const dist = jt::MathHelper::length(dif);
        auto scaleX = dist / 100;
        if (scaleX >= 1.0f) {
            scaleX = 1.0f;
        }

        m_currentShape->setScale(jt::Vector2f { scaleX, 1.0f });
        float const angle = jt::MathHelper::angleOf(dif);
        m_currentShape->setRotation(angle);
        m_currentShape->update(elapsed);
    }

    for (auto& shp : m_shapes) {
        shp->update(elapsed);
    }
}
void Grid::doDraw() const
{
    for (auto& shp : m_shapes) {
        shp->draw(renderTarget());
    }
    if (m_currentShape) {
        m_currentShape->draw(renderTarget());
    }
}
