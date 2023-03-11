#include "state_end.hpp"
#include "color/color_factory.hpp"
#include <drawable_helpers.hpp>
#include <game_interface.hpp>
#include <game_properties.hpp>
#include <state_menu.hpp>

void StateEnd::doInternalCreate()
{
    m_background
        = jt::dh::createShapeRect(GP::GetScreenSize(), GP::PaletteBackground(), textureManager());

    m_text = jt::dh::createText(renderTarget(), GP::ExplanationText(), 16U, GP::PaletteFontFront());
    m_text->setText(m_endText + "Time: " + std::to_string(m_time)
        + "s\nConnected Cities: " + std::to_string(m_connections));
    m_text->setTextAlign(jt::Text::TextAlign::CENTER);
    m_text->setPosition(GP::GetScreenSize() * 0.5f + jt::Vector2f { 0.0f, -20.0f });
    m_vignette = std::make_shared<jt::Vignette>(GP::GetScreenSize());
    add(m_vignette);

    m_wind = std::make_shared<jt::WindParticles>(GP::GetWindowSize(),
        std::vector<jt::Color> { jt::ColorFactory::fromHexString("#f1f6f0"),
            jt::ColorFactory::fromHexString("#de9f47"), jt::Color { 255, 255, 255, 100 } });
    add(m_wind);
}

void StateEnd::doInternalUpdate(float const elapsed)
{
    m_background->update(elapsed);
    m_text->update(elapsed);
    m_wind->m_windSpeed = 0.75f * (0.25f + 0.2f * sin(0.1f * getAge()));
    if (getAge() > 4.0f) {
        if (getGame()->input().keyboard()->justPressed(jt::KeyCode::Space)
            || getGame()->input().keyboard()->justPressed(jt::KeyCode::Enter)
            || getGame()->input().keyboard()->justPressed(jt::KeyCode::Escape)) {
            getGame()->stateManager().switchState(std::make_shared<StateMenu>());
        }
    }
}

void StateEnd::doInternalDraw() const
{
    m_background->draw(renderTarget());
    m_wind->draw();
    m_text->draw(renderTarget());
    m_vignette->draw();
}

void StateEnd::setTime(int timeInSeconds) { m_time = timeInSeconds; }

void StateEnd::setConnections(int connections) { m_connections = connections; }
void StateEnd::setEndText(std::string const& text) { m_endText = text; }
