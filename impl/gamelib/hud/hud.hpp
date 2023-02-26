#ifndef GAME_HUD_HPP
#define GAME_HUD_HPP

#include <game_object.hpp>
#include <hud/observer_interface.hpp>
#include <sprite.hpp>
#include <text.hpp>

class ScoreDisplay;

class Hud : public jt::GameObject {
public:
    std::shared_ptr<ObserverInterface<int>> getObserverScoreP1() const;
    std::shared_ptr<ObserverInterface<int>> getObserverTime() const;

private:
    std::shared_ptr<ScoreDisplay> m_scoreP1Display;
    std::shared_ptr<ScoreDisplay> m_timerDisplay;

    jt::Text::Sptr m_scoreP1Text;
    jt::Text::Sptr m_timerText;

    void doCreate() override;

    void doUpdate(float const elapsed) override;

    void doDraw() const override;
};

#endif
