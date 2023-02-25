#ifndef ALAKAJAM17_GRID_HPP
#define ALAKAJAM17_GRID_HPP

#include <game_object.hpp>
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

    jt::Color getCurrentColor() const;

    void switchToNextColor();

private:
    int const m_mapSizeX = 20;
    int const m_mapSizeY = 15;
    std::vector<std::shared_ptr<jt::Shape>> m_shapes {};
    std::shared_ptr<jt::Shape> m_currentShape { nullptr };
    std::shared_ptr<jt::tilemap::TileNode> m_startNode { nullptr };
    std::shared_ptr<jt::tilemap::TileNode> m_endNode { nullptr };

    std::vector<std::shared_ptr<jt::tilemap::TileNode>> m_tiles {};

    std::size_t m_currentColorIndex { 0u };
    // TODO fill this with palette colors
    std::vector<jt::Color> m_allColors { jt::colors::Blue, jt::colors::Red, jt::colors::Green };

    std::vector<std::shared_ptr<jt::tilemap::TileNode>> m_primaryHubs {};
    std::vector<std::shared_ptr<jt::tilemap::TileNode>> m_secondaryHubs {};

    int m_pathsCompleted { 0 };
    int m_allowedMaxDistanceToPrimaryHub { 2 };

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
};

#endif // ALAKAJAM17_GRID_HPP
