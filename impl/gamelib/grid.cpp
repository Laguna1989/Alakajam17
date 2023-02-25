#include "grid.hpp"
#include <pathfinder/node.hpp>
#include <drawable_helpers.hpp>
#include <game_interface.hpp>
#include <game_properties.hpp>
#include <math_helper.hpp>

void Grid::doCreate() { createTiles();}
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
    for(auto& tile : m_tiles)
    {
        tile->getDrawable()->update(elapsed);
    }
}
void Grid::doDraw() const
{
    for(auto& tile : m_tiles)
    {
        tile->getDrawable()->draw(renderTarget());
    }

    for (auto& shp : m_shapes) {
        shp->draw(renderTarget());
    }
    if (m_currentShape) {
        m_currentShape->draw(renderTarget());
    }
}
std::shared_ptr<jt::tilemap::TileNode> Grid::getTileAt(int x, int y)
{
    if (x < 0) {
        return nullptr;
    }
    if (y < 0) {
        return nullptr;
    }
    if (x >= m_mapSizeX) {
        return nullptr;
    }
    if (y >= m_mapSizeY) {
        return nullptr;
    }
    int const index = y + x * m_mapSizeY;
    if (static_cast<std::size_t>(index) >= m_tiles.size()) {
        return nullptr;
    }
    return m_tiles.at(index);
}
void Grid::createTiles() {

    float const distX = GP::GetScreenSize().x / m_mapSizeX;
    float const distY = GP::GetScreenSize().y / m_mapSizeY;
    for (int i = 0; i != m_mapSizeX; ++i) {
        for (int j = 0; j != m_mapSizeY; ++j) {
            std::shared_ptr<jt::Shape> drawable = jt::dh::createShapeCircle(
                distX/10, jt::colors::White, textureManager());
            drawable->setPosition(jt::Vector2f { i * distX, j * distY });
            auto node = std::make_shared<jt::pathfinder::Node>();
            node->setPosition(
                jt::Vector2u { static_cast<unsigned int>(i), static_cast<unsigned int>(j) });

            m_tiles.emplace_back(std::make_shared<jt::tilemap::TileNode>(drawable, node));
        }
    }

    for (int i = 5; i != 10; ++i) {
        getTileAt(i, 6)->setBlocked(true);
    }
    getTileAt(5, 0)->setBlocked(true);
    getTileAt(5, 1)->setBlocked(true);
    getTileAt(5, 2)->setBlocked(true);
    getTileAt(5, 3)->setBlocked(true);
    getTileAt(5, 4)->setBlocked(true);
    getTileAt(5, 5)->setBlocked(true);

    getTileAt(9, 5)->setBlocked(true);
}
