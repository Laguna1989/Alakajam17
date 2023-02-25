#ifndef ALAKAJAM17_GRID_HPP
#define ALAKAJAM17_GRID_HPP

#include <game_object.hpp>
#include <game_properties.hpp>
#include <shape.hpp>
#include <tilemap/tile_node.hpp>
#include <vector.hpp>
#include <memory>
#include <vector>

class Grid : public jt::GameObject {
public:
    std::shared_ptr<jt::tilemap::TileNode> getTileAt(int x, int y);
    std::shared_ptr<jt::tilemap::TileNode> getClosestTileTo(jt::Vector2f const& pos);

    std::shared_ptr<jt::tilemap::TileNode> getPossibleEndTile(jt::Vector2f const& pos);

    jt::Color getCurrentSpawnColor() const;
    jt::Color getCurrentDrawColor() const;

    void switchToNextColor();

private:
    int const m_mapSizeX = 30;
    int const m_mapSizeY = 20;
    std::vector<std::shared_ptr<jt::Shape>> m_shapes {};
    std::shared_ptr<jt::Shape> m_currentShape { nullptr };
    std::shared_ptr<jt::tilemap::TileNode> m_startNode { nullptr };
    std::shared_ptr<jt::tilemap::TileNode> m_endNode { nullptr };

    std::vector<std::shared_ptr<jt::tilemap::TileNode>> m_nodeList {};

    std::size_t m_currentColorIndex { 0u };
    // TODO fill this with palette colors
    std::vector<jt::Color> m_allColors { GP::getPalette().getColor(26),
        GP::getPalette().getColor(30), GP::getPalette().getColor(23), GP::getPalette().getColor(5),
        GP::getPalette().getColor(13) };

    std::vector<std::shared_ptr<jt::tilemap::TileNode>> m_primaryHubs {};
    std::vector<std::shared_ptr<jt::tilemap::TileNode>> m_secondaryHubs {};

    int m_pathsCompleted { 0 };

public:
    int getPathsCompleted() const;

private:
    int m_allowedMaxDistanceToPrimaryHub { 2 };
    jt::Color m_currentDrawColor;

    void doCreate() override;
    void doUpdate(float const elapsed) override;
    void doDraw() const override;

    void createTiles();
    void highlightTileUnderCursor();
    void handleSpawnConnectionInput();
    void updateCurrentShapeIfSet(float const elapsed);
    void spawnConnection();
    void createPrimaryHub();
    void createSecondaryHub();
    void checkForCompletedPath();
    void pathCompleted();
    std::shared_ptr<jt::tilemap::TileNode> getCurrentPrimaryHub();
    std::shared_ptr<jt::tilemap::TileNode>& move_up_one_step(
        std::shared_ptr<jt::tilemap::TileNode>& hub);
    void fadeNodesBasedOnMouseDistance();
};

#endif // ALAKAJAM17_GRID_HPP
