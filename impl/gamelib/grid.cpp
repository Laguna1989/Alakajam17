#include "grid.hpp"
#include <drawable_helpers.hpp>
#include <game_interface.hpp>
#include <game_properties.hpp>
#include <math_helper.hpp>
#include <pathfinder/node.hpp>
#include <pathfinder/pathfinder.hpp>
#include <random/open_simplex_noise2d.hpp>
#include <system_helper.hpp>
#include <stdexcept>

void Grid::doCreate()
{
    createTiles();
    std::cout << m_allColors.size() << std::endl;
    createPrimaryHub();

    createSecondaryHub();
}

void Grid::createPrimaryHub()
{
    std::shared_ptr<jt::tilemap::TileNode> hub { nullptr };

    int counter = 0;
    while (true) {
        counter++;
        if (counter >= 100) {
            getGame()->logger().info("no place for primary hub found", { "grid" });
            // TODO
            break;
        }
        hub = *jt::SystemHelper::select_randomly(m_nodeList.begin(), m_nodeList.end());
        if (hub->m_riverColor != jt::colors::White) {
            hub = nullptr;
            continue;
        }
        auto const tileposX = hub->getNode()->getTilePosition().x;
        if (tileposX == 0 || tileposX == m_mapSizeX || tileposX == m_mapSizeX - 1) {
            hub = nullptr;
            continue;
        }
        auto const tileposY = hub->getNode()->getTilePosition().y;
        if (tileposY == 0 || tileposY == m_mapSizeY || tileposY == m_mapSizeY - 1) {
            hub = nullptr;
            continue;
        }

        // do not spawn close to other primary hub
        if (counter <= 90) {
            bool tooClose { false };
            for (auto otherPrimaryHub : m_primaryHubs) {
                auto const phPos = otherPrimaryHub->getNode()->getTilePosition();
                auto const distX = abs(static_cast<int>(phPos.x) - static_cast<int>(tileposX));
                auto const distY = abs(static_cast<int>(phPos.y) - static_cast<int>(tileposY));
                if (distX < 3 || distY < 3) {
                    tooClose = true;
                    break;
                }
            }
            if (tooClose) {
                hub = nullptr;
                continue;
            }
        }

        break;
    }
    hub->m_riverColor = getCurrentColor();
    hub->setHeight(9999999);
    m_primaryHubs.push_back(hub);
}

void Grid::createSecondaryHub()
{
    std::cout << "createSecondaryHub: " << m_allowedMaxDistanceToPrimaryHub << std::endl;
    std::shared_ptr<jt::tilemap::TileNode> hub { nullptr };
    int counter = 0;
    while (true) {
        counter++;
        if (counter >= 100) {
            getGame()->logger().info("no place for secondary hub found", { "grid" });
            // TODO
            break;
        }
        hub = *jt::SystemHelper::select_randomly(m_nodeList.begin(), m_nodeList.end());
        if (hub->m_riverColor != jt::colors::White) {
            hub = nullptr;
            continue;
        }

        // do not spawn on map border
        auto const tileposX = hub->getNode()->getTilePosition().x;
        if (tileposX == 0 || tileposX == m_mapSizeX || tileposX == m_mapSizeX - 1) {
            hub = nullptr;
            continue;
        }
        auto const tileposY = hub->getNode()->getTilePosition().y;
        if (tileposY == 0 || tileposY == m_mapSizeY || tileposY == m_mapSizeY - 1) {
            hub = nullptr;
            continue;
        }

        // should be in close distance to primary hub
        auto const phPos = getCurrentPrimaryHub()->getNode()->getTilePosition();
        auto const distX = abs(static_cast<int>(phPos.x) - static_cast<int>(tileposX));
        auto const distY = abs(static_cast<int>(phPos.y) - static_cast<int>(tileposY));
        if (distX > m_allowedMaxDistanceToPrimaryHub || distY > m_allowedMaxDistanceToPrimaryHub) {
            hub = nullptr;
            continue;
        }

        // TODO check all surrounding nodes should be lower than surroundings of primary node

        break;
    }
    hub->m_riverColor = getCurrentColor();
    hub->setHeight(0);
    m_secondaryHubs.push_back(hub);
}

void Grid::doUpdate(float const elapsed)
{
    handleSpawnConnectionInput();

    updateCurrentShapeIfSet(elapsed);

    for (auto& shp : m_shapes) {
        shp->update(elapsed);
    }
    for (auto& tile : m_nodeList) {
        tile->getDrawable()->setScale(jt::Vector2f { 0.5f, 0.5f });
        tile->getDrawable()->update(elapsed);
    }

    for (auto& ph : m_primaryHubs) {
        ph->getDrawable()->setScale(jt::Vector2f { 4.0f, 4.0f });
        ph->getDrawable()->setColor(ph->m_riverColor);
    }

    for (auto& ph : m_secondaryHubs) {
        ph->getDrawable()->setScale(jt::Vector2f { 2.0f, 2.0f });
        ph->getDrawable()->setColor(ph->m_riverColor);
    }

    highlightTileUnderCursor();

    if (getGame()->input().keyboard()->pressed(jt::KeyCode::LControl)
        && getGame()->input().keyboard()->justPressed(jt::KeyCode::C)) {
        switchToNextColor();
    }
}

void Grid::switchToNextColor()
{
    m_currentColorIndex++;
    if (m_currentColorIndex >= m_allColors.size()) {
        m_currentColorIndex = 0u;
    }
}

void Grid::handleSpawnConnectionInput()
{
    if (getGame()->input().mouse()->justPressed(jt::MouseButtonCode::MBRight)) {
        getGame()->logger().info("reset connection", { "grid" });
        m_currentShape = nullptr;
        m_startNode = nullptr;
        m_endNode = nullptr;
        return;
    }

    if (getGame()->input().mouse()->justPressed(jt::MouseButtonCode::MBLeft)) {
        if (m_currentShape == nullptr) {

            auto const possibleTile
                = getClosestTileTo(getGame()->input().mouse()->getMousePositionWorld());
            if (!possibleTile) {
                return;
            }

            m_currentShape = std::make_shared<jt::Shape>();
            m_currentShape->makeRect(jt::Vector2f { 100.0f, 10.0f }, textureManager());
            m_startNode = possibleTile;
            if (!m_startNode) {
                return;
            }
            m_currentShape->setPosition(m_startNode->getDrawable()->getPosition());
            m_currentShape->setOrigin(jt::Vector2f { 5.0f, 5.0f });
        } else {
            if (m_currentShape->getScale().x == 0.0f || m_currentShape->getScale().y == 0.0f) {
                return;
            }
            if (!m_startNode) {
                return;
            }
            if (m_startNode->m_riverColor != getCurrentColor()) {
                return;
            }
            if (m_startNode == m_endNode) {
                return;
            }

            std::string startPosString = "("
                + std::to_string(static_cast<int>(m_startNode->getNode()->getTilePosition().x))
                + ", "
                + std::to_string(static_cast<int>(m_startNode->getNode()->getTilePosition().y))
                + ", " + std::to_string(m_startNode->getHeight()) + ")";
            std::string endPosString = "("
                + std::to_string(static_cast<int>(m_endNode->getNode()->getTilePosition().x)) + ", "
                + std::to_string(static_cast<int>(m_endNode->getNode()->getTilePosition().y)) + ", "
                + std::to_string(m_endNode->getHeight()) + ")";
            getGame()->logger().info(
                "Try Spawn connection between " + startPosString + " - " + endPosString,
                { "grid" });

            auto const startColor = m_startNode->m_riverColor;
            if (startColor != jt::colors::White && startColor != getCurrentColor()) {
                return;
            }
            auto const endColor = m_endNode->m_riverColor;
            if (endColor != jt::colors::White && endColor != getCurrentColor()) {
                return;
            }
            if (m_startNode->getHeight() < m_endNode->getHeight()) {
                return;
            }

            spawnConnection();
        }
    }
}

void Grid::spawnConnection()
{
    std::string startPosString = "("
        + std::to_string(static_cast<int>(m_startNode->getNode()->getTilePosition().x)) + ", "
        + std::to_string(static_cast<int>(m_startNode->getNode()->getTilePosition().y)) + ", "
        + std::to_string(m_startNode->getHeight()) + ")";
    std::string endPosString = "("
        + std::to_string(static_cast<int>(m_endNode->getNode()->getTilePosition().x)) + ", "
        + std::to_string(static_cast<int>(m_endNode->getNode()->getTilePosition().y)) + ", "
        + std::to_string(m_endNode->getHeight()) + ")";
    getGame()->logger().info(
        "Spawn connection between " + startPosString + " - " + endPosString, { "grid" });

    // create pathfinding connection
    m_startNode->getNode()->addNeighbour(m_endNode->getNode());
    m_endNode->getNode()->addNeighbour(m_startNode->getNode());

    // color nodes
    m_startNode->m_riverColor = getCurrentColor();
    m_endNode->m_riverColor = getCurrentColor();
    m_currentShape->setColor(getCurrentColor());

    // check if a connection is closed
    if (m_startNode->m_riverColor == getCurrentColor()
        && m_endNode->m_riverColor == getCurrentColor()) {
        // check for completed path
        checkForCompletedPath();
    }

    m_shapes.push_back(m_currentShape);
    m_currentShape = nullptr;
    m_startNode = nullptr;
    m_endNode = nullptr;
}

void Grid::checkForCompletedPath()
{
    auto const primary = getCurrentPrimaryHub();
    auto const secondary = m_secondaryHubs.back();
    for (auto& t : m_nodeList) {
        t->reset();
    }
    auto path = jt::pathfinder::calculatePath(primary->getNode(), secondary->getNode());
    if (path.size() == 0) {
        getGame()->logger().verbose("no path found");
    } else {
        getGame()->logger().info("path completed");
        pathCompleted();
    }
}
std::shared_ptr<jt::tilemap::TileNode> Grid::getCurrentPrimaryHub()
{
    auto const primary_it = std::find_if(m_primaryHubs.begin(), m_primaryHubs.end(),
        [this](auto t) { return t->m_riverColor == getCurrentColor(); });
    if (primary_it == m_primaryHubs.end()) {
        throw std::invalid_argument { "no primary hub found" };
    }
    auto const primary = *primary_it;
    return primary;
}

void Grid::pathCompleted()
{
    m_pathsCompleted++;

    if (m_pathsCompleted % 3 == 0) {
        switchToNextColor();
        if (m_primaryHubs.size() < 3) {
            createPrimaryHub();
        }
    }
    if (m_pathsCompleted % 8 == 0) {
        m_allowedMaxDistanceToPrimaryHub++;
    }
    createSecondaryHub();
}

std::shared_ptr<jt::tilemap::TileNode> Grid::getPossibleEndTile(jt::Vector2f const& pos)
{
    if (!m_startNode) {
        return nullptr;
    }
    auto const possibleEndTile
        = getClosestTileTo(getGame()->input().mouse()->getMousePositionWorld());
    if (!possibleEndTile) {
        return nullptr;
    }
    auto const endTilepos = possibleEndTile->getNode()->getTilePosition();
    auto const startTilepos = m_startNode->getNode()->getTilePosition();

    auto const distX = static_cast<int>(endTilepos.x) - static_cast<int>(startTilepos.x);
    auto const distY = static_cast<int>(endTilepos.y) - static_cast<int>(startTilepos.y);
    //    std::cout << distX << " " << distY << std::endl;

    if (distX < -1 || distX > 1) {
        return nullptr;
    }
    if (distY < -1 || distY > 1) {
        return nullptr;
    }

    return possibleEndTile;
}

void Grid::updateCurrentShapeIfSet(float const elapsed)
{
    if (!m_currentShape) {
        return;
    }

    m_currentShape->update(elapsed);

    auto const possibleEndTile
        = getPossibleEndTile(getGame()->input().mouse()->getMousePositionWorld());

    if (!possibleEndTile) {
        if (!m_startNode) {
            return;
        }
        m_endNode = m_startNode;
        m_currentShape->setScale(jt::Vector2f { 0.0f, 0.0f });
        return;
    }

    m_endNode = possibleEndTile;
    if (!m_startNode) {
        return;
    }
    auto startNodePosition = m_startNode->getDrawable()->getPosition();
    if (m_endNode == m_startNode) {
        m_currentShape->setScale(jt::Vector2f { 0.0f, 0.0f });
        return;
    }

    auto const dif = m_endNode->getDrawable()->getPosition() - startNodePosition;
    auto const dist = jt::MathHelper::length(dif);
    auto scaleX = dist / 100;

    m_currentShape->setScale(jt::Vector2f { scaleX, 1.0f });
    float const angle = jt::MathHelper::angleOf(dif);
    m_currentShape->setRotation(angle);
}

void Grid::highlightTileUnderCursor()
{
    auto const mousePosition = getGame()->input().mouse()->getMousePositionWorld();
    auto const tileUnderCursor = getClosestTileTo(mousePosition);
    if (!tileUnderCursor) {
        return;
    }

    if (std::count(m_primaryHubs.begin(), m_primaryHubs.end(), tileUnderCursor) == 1) {
        tileUnderCursor->getDrawable()->setScale(jt::Vector2f { 4.5f, 4.5f });
    } else if (std::count(m_secondaryHubs.begin(), m_secondaryHubs.end(), tileUnderCursor) == 1) {
        tileUnderCursor->getDrawable()->setScale(jt::Vector2f { 2.5f, 2.5f });
    } else {
        tileUnderCursor->getDrawable()->setScale(jt::Vector2f { 1.5f, 1.5f });
    }
}

void Grid::doDraw() const
{
    for (auto& tile : m_nodeList) {
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
    if (static_cast<std::size_t>(index) >= m_nodeList.size()) {
        return nullptr;
    }
    return m_nodeList.at(index);
}

float getHeight(jt::OpenSimplexNoise2D& noise, int i, int j)
{
    return jt::MathHelper::clamp((noise.eval(i / 4.0f, j / 4.0f) + 1.0f) / 2.0f, 0.0f, 1.0f);
}

void Grid::createTiles()
{
    float const distX = GP::GetScreenSize().x / m_mapSizeX;
    float const distY = GP::GetScreenSize().y / m_mapSizeY;

    jt::OpenSimplexNoise2D noise2D { 1u };

    float minHeight { 99999 };
    float maxHeight { 0.0f };
    for (int i = 0; i != m_mapSizeX; ++i) {
        for (int j = 0; j != m_mapSizeY; ++j) {
            auto const height = getHeight(noise2D, i, j);
            if (height < minHeight) {
                minHeight = height;
            }
            if (height > maxHeight) {
                maxHeight = height;
            }
        }
    }

    for (int i = 0; i != m_mapSizeX; ++i) {
        for (int j = 0; j != m_mapSizeY; ++j) {
            auto height = getHeight(noise2D, i, j);
            height = (height - minHeight) / (maxHeight + minHeight);
            std::shared_ptr<jt::Shape> drawable
                = jt::dh::createShapeCircle(distX / 10, jt::colors::White, textureManager());
            drawable->setOrigin(jt::OriginMode::CENTER);
            drawable->setPosition(jt::Vector2f { i * distX, j * distY });
            std::cout << i << ", " << j << ", " << height << std::endl;
            std::uint8_t v = static_cast<std::uint8_t>(height * 255);
            jt::Color c { v, v, v, 255 };
            drawable->setColor(c);
            auto node = std::make_shared<jt::pathfinder::Node>();
            node->setPosition(
                jt::Vector2u { static_cast<unsigned int>(i), static_cast<unsigned int>(j) });

            auto tileNode = std::make_shared<jt::tilemap::TileNode>(drawable, node);
            tileNode->setHeight(height);
            m_nodeList.emplace_back(tileNode);
        }
    }
}

std::shared_ptr<jt::tilemap::TileNode> Grid::getClosestTileTo(jt::Vector2f const& pos)
{
    float const distX = GP::GetScreenSize().x / m_mapSizeX;
    float const distY = GP::GetScreenSize().y / m_mapSizeY;

    auto const posIntX = static_cast<int>((pos.x + distX / 2) / distX);
    auto const posIntY = static_cast<int>((pos.y + distY / 2) / distY);

    auto const tile = getTileAt(posIntX, posIntY);
    if (!tile) {
        return nullptr;
    }
    auto const dif
        = tile->getDrawable()->getPosition() - getGame()->input().mouse()->getMousePositionWorld();
    auto const dist = jt::MathHelper::lengthSquared(dif);
    if (dist > 18 * 18) {
        return nullptr;
    }
    return tile;
}

jt::Color Grid::getCurrentColor() const { return m_allColors.at(m_currentColorIndex); }
