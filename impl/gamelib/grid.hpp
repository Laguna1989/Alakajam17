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

private:
    int const m_mapSizeX = 15;
    int const m_mapSizeY = 10;
    std::vector<std::shared_ptr<jt::Shape>> m_shapes {};
    std::shared_ptr<jt::Shape> m_currentShape { nullptr };
    jt::Vector2f m_shapeStart { 0.0f, 0.0f };

    std::vector<std::shared_ptr<jt::tilemap::TileNode>> m_tiles {};

    void doCreate() override;
    void doUpdate(float const elapsed) override;
    void doDraw() const override;

    void createTiles();
    void highlightTileUnderCursor();
    void handleSpawnConnectionInput();
    void updateCurrentShapeIfSet(float const elapsed);
};

#endif // ALAKAJAM17_GRID_HPP
