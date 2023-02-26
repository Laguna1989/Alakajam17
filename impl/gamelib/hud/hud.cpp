#include "hud.hpp"
#include <color/color.hpp>
#include <drawable_helpers.hpp>
#include <game_interface.hpp>
#include <hud/score_display.hpp>

std::shared_ptr<ObserverInterface<int>> Hud::getObserverScoreP1() const { return m_scoreP1Display; }
std::shared_ptr<ObserverInterface<int>> Hud::getObserverTime() const { return m_timerDisplay; }

void Hud::doCreate()
{

    m_scoreP1Text = std::make_shared<jt::Text>();
    m_scoreP1Text = jt::dh::createText(renderTarget(), "", 16, jt::Color { 248, 249, 254 });
    m_scoreP1Text->setTextAlign(jt::Text::TextAlign::LEFT);
    m_scoreP1Text->setPosition({ 10, 4 });

    m_scoreP1Display = std::make_shared<ScoreDisplay>(m_scoreP1Text, "Connected: ");

    m_timerText = jt::dh::createText(renderTarget(), "", 16, jt::Color { 248, 249, 254 });
    m_timerText->setTextAlign(jt::Text::TextAlign::LEFT);
    m_timerText->setPosition({ 600 / 2 - 10, 4 });

    m_timerDisplay = std::make_shared<ScoreDisplay>(m_timerText, "Time: ");
}

void Hud::doUpdate(float const elapsed)
{
    m_scoreP1Text->update(elapsed);
    m_timerText->update(elapsed);
}

void Hud::doDraw() const
{
    m_scoreP1Text->draw(renderTarget());
    m_timerText->draw(renderTarget());
}
