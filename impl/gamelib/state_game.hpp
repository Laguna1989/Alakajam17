#ifndef GAME_STATE_GAME_HPP
#define GAME_STATE_GAME_HPP

#include "particle_system.hpp"
#include <box2dwrapper/box2d_world_interface.hpp>
#include <game_state.hpp>
#include <grid.hpp>
#include <screeneffects/wind_particles.hpp>
#include <vector.hpp>
#include <memory>
#include <vector>

// fwd decls
namespace jt {
class Shape;
class Sprite;
class Vignette;
} // namespace jt

class Hud;

class StateGame : public jt::GameState {
public:
    std::string getName() const override;
    jt::Color m_particleColor { jt::colors::White };

private:
    std::shared_ptr<jt::Shape> m_background;
    std::shared_ptr<jt::Vignette> m_vignette;
    std::shared_ptr<Hud> m_hud;

    std::shared_ptr<Grid> m_grid { nullptr };
    std::shared_ptr<jt::ParticleSystem<jt::Shape, 100>> m_spawnParticles { nullptr };

    bool m_running { true };
    bool m_hasEnded { false };

    void doInternalCreate() override;
    void doInternalUpdate(float const elapsed) override;
    void doInternalDraw() const override;

    void endGame();
    void createGrid();
    void createParticleSystem();
    std::string m_endText;
    std::shared_ptr<jt::WindParticles> m_wind;
};

#endif
