#include "state_game.hpp"
#include <box2dwrapper/box2d_world_impl.hpp>
#include <color/color.hpp>
#include <color/color_factory.hpp>
#include <drawable_helpers.hpp>
#include <game_interface.hpp>
#include <game_properties.hpp>
#include <hud/hud.hpp>
#include <random/random.hpp>
#include <screeneffects/vignette.hpp>
#include <shape.hpp>
#include <state_end.hpp>
#include <tweens/tween_alpha.hpp>
#include <tweens/tween_base.hpp>
#include <tweens/tween_color.hpp>
#include <tweens/tween_position.hpp>
#include <tweens/tween_scale.hpp>

void StateGame::doInternalCreate()
{
    float const w = static_cast<float>(GP::GetWindowSize().x);
    float const h = static_cast<float>(GP::GetWindowSize().y);

    using jt::Shape;
    using jt::TweenAlpha;

    m_background = std::make_shared<Shape>();
    m_background->makeRect({ w, h }, textureManager());
    auto colrgb = GP::getPalette().getColor(12);
    m_background->setColor(colrgb);
    m_background->setIgnoreCamMovement(true);
    m_background->update(0.0f);

    createGrid();

    createParticleSystem();
    m_wind = std::make_shared<jt::WindParticles>(GP::GetWindowSize(),
        std::vector<jt::Color> { jt::ColorFactory::fromHexString("#f1f6f0"),
            jt::ColorFactory::fromHexString("#de9f47"), jt::Color { 255, 255, 255, 100 } });
    add(m_wind);
    m_vignette = std::make_shared<jt::Vignette>(GP::GetScreenSize());
    add(m_vignette);
    m_hud = std::make_shared<Hud>();
    add(m_hud);

    // StateGame will call drawObjects itself.
    setAutoDraw(false);

    getGame()->gfx().camera().move(jt::Vector2f { -15.0f, -15.0f });
}

void StateGame::createParticleSystem()
{
    m_spawnParticles = jt::ParticleSystem<jt::Shape, 100>::createPS(
        [this]() {
            auto s = jt::dh::createShapeCircle(5, jt::colors::White, textureManager());
            s->setOrigin(jt::Vector2f { 5, 5 });
            s->setPosition(jt::Vector2f { -2000.0f, -2000.0f });
            s->setScale(jt::Vector2f { 0.5f, 0.5f });
            return s;
        },
        [this](auto s, auto pos) {
            auto const endPosition = pos;

            auto const angle = jt::Random::getFloat(0.0f, 2 * 3.141592f);
            auto const startPosition
                = pos + jt::Random::getFloat(5.0f, 20.0f) * jt::Vector2f { cos(angle), sin(angle) };
            s->setPosition(startPosition);
            s->setColor(jt::Color { 255, 255, 255, 0 });
            s->setScale(jt::Vector2f { 0.1f, 0.1f });

            auto twpos = jt::TweenPosition::create(s, 0.55f, startPosition, endPosition);
            add(twpos);

            jt::TweenAlpha::Sptr twaIn = jt::TweenAlpha::create(s, 1.0f, 0u, 255u);
            twaIn->setSkipFrames(1);
            twaIn->addCompleteCallback([this, s]() {
                jt::TweenAlpha::Sptr twaOut = jt::TweenAlpha::create(s, 0.9f - 0.25f, 255, 0);
                twaOut->setSkipFrames(1);
                twaOut->setStartDelay(0.25f);
                add(twaOut);
            });
            add(twaIn);

            jt::TweenColor::Sptr twcl
                = jt::TweenColor::create(s, 0.3f, jt::colors::White, m_particleColor);
            twcl->setStartDelay(0.5f);
            twcl->setSkipFrames(1);
            add(twcl);

            auto tws = jt::TweenScale::create(
                s, 0.9f - 0.1f, jt::Vector2f { 1.4f, 1.4f }, jt::Vector2f { 0.2f, 0.2f });
            tws->setSkipFrames(1);
            tws->setStartDelay(0.1f);
            add(tws);
        });
    add(m_spawnParticles);

    m_grid->setSpawnParticlesCallback([this](jt::Vector2f const& pos, jt::Color const& c) {
        m_particleColor = c;
        m_spawnParticles->fire(15, pos);
    });
}

void StateGame::createGrid()
{
    m_grid = std::make_shared<Grid>();
    add(m_grid);
}

void StateGame::doInternalUpdate(float const elapsed)
{
    if (m_running) {
        m_wind->m_windSpeed = 0.75f * (0.25f + 0.2f * sin(0.1f * getAge()));
        // update game logic here
        m_hud->getObserverScoreP1()->notify(m_grid->getPathsCompleted());
        m_hud->getObserverTime()->notify(static_cast<int>(getAge()));
        if (getGame()->input().keyboard()->justPressed(jt::KeyCode::M)) {
            getGame()->audio().groups().setGroupVolume("master", 0.0f);
        }
        if (getGame()->input().keyboard()->justPressed(jt::KeyCode::U)) {
            getGame()->audio().groups().setGroupVolume("master", 1.0f);
        }
    }

    if (m_grid->m_endGame) {
        m_endText = "River overflow";
        endGame();
    }
    if (getGame()->input().keyboard()->pressed(jt::KeyCode::LShift)
        && getGame()->input().keyboard()->justPressed(jt::KeyCode::Escape)) {
        m_endText = "Manual quit";
        endGame();
    }

    m_background->update(elapsed);
    m_vignette->update(elapsed);
}

void StateGame::doInternalDraw() const
{
    m_background->draw(renderTarget());
    drawObjects();
    m_grid->draw();
    m_spawnParticles->draw();
    m_wind->draw();
    m_vignette->draw();
    m_hud->draw();
}

void StateGame::endGame()
{
    if (m_hasEnded) {
        // trigger this function only once
        return;
    }
    m_hasEnded = true;
    m_running = false;

    auto endState = std::make_shared<StateEnd>();
    endState->setConnections(m_grid->getPathsCompleted());
    endState->setTime(static_cast<int>(getAge()));
    endState->setEndText(m_endText);
    getGame()->stateManager().switchState(endState);
}

std::string StateGame::getName() const { return "State Game"; }
