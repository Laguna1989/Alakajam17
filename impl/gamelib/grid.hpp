#ifndef ALAKAJAM17_GRID_HPP
#define ALAKAJAM17_GRID_HPP

#include "audio/sound_interface.hpp"
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

    int getPathsCompleted() const;

    bool m_endGame { false };
    void setSpawnParticlesCallback(
        std::function<void(jt::Vector2f const& pos, jt::Color const& col)> func);

private:
    int const m_mapSizeX = 30;
    int const m_mapSizeY = 20;
    std::vector<std::shared_ptr<jt::Shape>> m_shapes {};
    std::shared_ptr<jt::Shape> m_currentShape { nullptr };
    std::shared_ptr<jt::tilemap::TileNode> m_startNode { nullptr };
    std::shared_ptr<jt::tilemap::TileNode> m_endNode { nullptr };

    std::function<void(jt::Vector2f const& pos, jt::Color const& col)> m_particleFunc;

    std::vector<std::shared_ptr<jt::tilemap::TileNode>> m_nodeList {};

    std::size_t m_currentColorIndex { 0u };
    // TODO fill this with palette colors
    std::vector<jt::Color> m_allColors { GP::getPalette().getColor(26),
        GP::getPalette().getColor(30), GP::getPalette().getColor(23), GP::getPalette().getColor(5),
        GP::getPalette().getColor(13) };

    std::vector<std::shared_ptr<jt::tilemap::TileNode>> m_primaryHubs {};
    std::vector<std::shared_ptr<jt::tilemap::TileNode>> m_secondaryHubs {};

    int m_pathsCompleted { 0 };

    int m_allowedMaxDistanceToPrimaryHub { 2 };
    jt::Color m_currentDrawColor;

    float m_maxOverflowTimer { 35.0f };

    float m_spawnTimer { 0.0f };
    float m_defaultSpawnTimer { 8.0f };
    int m_expectedUnconnectedSecondaryHubs { 1 };

    std::shared_ptr<jt::SoundInterface> m_click;
    std::shared_ptr<jt::SoundInterface> m_click2;
    std::shared_ptr<jt::SoundInterface> m_bell;

    float GetSpawnTime();

    void doCreate() override;
    void doUpdate(float const elapsed) override;
    void doDraw() const override;

    void createTiles();
    void highlightTileUnderCursor();
    void handleSpawnConnectionInput();
    void updateCurrentShapeIfSet(float const elapsed);
    bool spawnConnection();
    void createPrimaryHub();
    void createSecondaryHub();
    bool checkForCompletedPath();
    void pathCompleted();

    std::shared_ptr<jt::tilemap::TileNode> getRandomPrimaryHub();
    std::shared_ptr<jt::tilemap::TileNode> getPrimaryHubWithLeastSecondaries();
    int getUnconnectedSecondariesForPrimaryHubColor(jt::Color const& c);
    std::shared_ptr<jt::tilemap::TileNode> getPrimaryHubForColor(jt::Color const& c);

    void fadeNodesBasedOnMouseDistance();
};

#endif // ALAKAJAM17_GRID_HPP
