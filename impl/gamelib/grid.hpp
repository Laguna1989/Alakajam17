#ifndef ALAKAJAM17_GRID_HPP
#define ALAKAJAM17_GRID_HPP

#include <game_object.hpp>
#include <shape.hpp>
#include <vector.hpp>
#include <memory>
#include <vector>

class Grid : public jt::GameObject {
public:
private:
    std::vector<std::shared_ptr<jt::Shape>> m_shapes;
    std::shared_ptr<jt::Shape> m_currentShape { nullptr };
    jt::Vector2f m_shapeStart { 0.0f, 0.0f };

    void doCreate() override;
    void doUpdate(float const elapsed) override;
    void doDraw() const override;
};

#endif // ALAKAJAM17_GRID_HPP
