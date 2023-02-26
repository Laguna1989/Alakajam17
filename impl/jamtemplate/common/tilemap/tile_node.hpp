
#ifndef JAMTEMPLATE_TILENODE_HPP
#define JAMTEMPLATE_TILENODE_HPP

#include <bar.hpp>
#include <color/color.hpp>
#include <graphics/drawable_interface.hpp>
#include <pathfinder/node_interface.hpp>
#include <vector.hpp>
#include <memory>

namespace jt {
namespace tilemap {

class TileNode {
public:
    TileNode(std::shared_ptr<jt::DrawableInterface> drawable,
        std::shared_ptr<jt::pathfinder::NodeInterface> node, std::shared_ptr<jt::Bar> bar);

    std::shared_ptr<jt::pathfinder::NodeInterface>& getNode();
    std::shared_ptr<jt::DrawableInterface>& getDrawable();
    std::shared_ptr<jt::Bar> m_overflowBar;

    void setBlocked(bool blocked);
    bool getBlocked() const;
    void reset();

    float getHeight() const;
    void setHeight(float mHeight);

    jt::Color m_riverColor { jt::colors::White };
    bool m_connected { false };

    float m_overflowTimer = 0.0f;

private:
    std::shared_ptr<jt::DrawableInterface> m_drawable;
    std::shared_ptr<jt::pathfinder::NodeInterface> m_node;

    float m_height { 0.0f };

public:
};

} // namespace tilemap
} // namespace jt

#endif // JAMTEMPLATE_TILENODE_HPP
