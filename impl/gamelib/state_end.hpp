
#ifndef ALAKAJAM17_STATE_END_HPP
#define ALAKAJAM17_STATE_END_HPP

#include <game_state.hpp>
#include <screeneffects/vignette.hpp>
#include <screeneffects/wind_particles.hpp>
#include <shape.hpp>
#include <text.hpp>

class StateEnd : public jt::GameState {
public:
    void setTime(int timeInSeconds);
    void setConnections(int connections);
    void setEndText(std::string const& text);

private:
    void doInternalCreate() override;
    void doInternalUpdate(float const elapsed) override;
    void doInternalDraw() const override;

    std::shared_ptr<jt::Shape> m_background;

    std::shared_ptr<jt::Text> m_text;

    std::shared_ptr<jt::WindParticles> m_wind;
    std::shared_ptr<jt::Vignette> m_vignette;
    int m_time { 0 };
    int m_connections { 0 };
    std::string m_endText;
};

#endif // ALAKAJAM17_STATE_END_HPP
