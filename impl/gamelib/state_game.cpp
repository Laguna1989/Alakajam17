#include "state_game.hpp"
#include <box2dwrapper/box2d_world_impl.hpp>
#include <color/color.hpp>
#include <drawable_helpers.hpp>
#include <game_interface.hpp>
#include <game_properties.hpp>
#include <hud/hud.hpp>
#include <random/random.hpp>
#include <screeneffects/vignette.hpp>
#include <shape.hpp>
#include <state_menu.hpp>
#include <tweens/tween_alpha.hpp>
#include <tweens/tween_base.hpp>
#include <tweens/tween_color.hpp>
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
            auto startPosition
                = jt::Random::getRandomPointIn(jt::Rectf { pos.x - 5, pos.y - 5, 10, 10 });
            s->setPosition(startPosition);
            s->setColor(jt::colors::White);
            s->setScale(jt::Vector2f { 0.1f, 0.1f });

            jt::TweenColor::Sptr twcl
                = jt::TweenColor::create(s, 0.3f, jt::colors::White, m_particleColor);
            twcl->setStartDelay(0.5f);
            twcl->setSkipFrames(1);
            add(twcl);

            jt::TweenAlpha::Sptr twaIn = jt::TweenAlpha::create(s, 0.1f, 0, 255);
            twaIn->setSkipFrames(1);
            add(twaIn);

            jt::TweenAlpha::Sptr twaOut = jt::TweenAlpha::create(s, 0.9f - 0.25f, 255, 0);
            twaOut->setSkipFrames(1);
            twaOut->setStartDelay(0.25f);
            add(twaOut);

            auto tws = jt::TweenScale::create(
                s, 0.9f - 0.1f, jt::Vector2f { 0.1f, 0.1f }, jt::Vector2f { 2.0f, 2.0f });
            tws->setSkipFrames(1);
            tws->setStartDelay(0.1f);
            add(tws);
        });
    add(m_spawnParticles);

    m_grid->setSpawnParticlesCallback([this](jt::Vector2f const& pos, jt::Color const& c) {
        m_particleColor = c;
        m_spawnParticles->fire(5, pos);
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

    getGame()->stateManager().switchState(std::make_shared<StateMenu>());
}

std::string StateGame::getName() const { return "State Game"; }
