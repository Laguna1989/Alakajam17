﻿#include "state_game.hpp"
#include <box2dwrapper/box2d_world_impl.hpp>
#include <color/color.hpp>
#include <game_interface.hpp>
#include <game_properties.hpp>
#include <hud/hud.hpp>
#include <screeneffects/vignette.hpp>
#include <shape.hpp>
#include <state_menu.hpp>
#include <tweens/tween_alpha.hpp>

void StateGame::doInternalCreate()
{
    float const w = static_cast<float>(GP::GetWindowSize().x);
    float const h = static_cast<float>(GP::GetWindowSize().y);

    using jt::Shape;
    using jt::TweenAlpha;

    m_background = std::make_shared<Shape>();
    m_background->makeRect({ w, h }, textureManager());
    auto colrgb = GP::getPalette().getColor(15);
    m_background->setColor(colrgb);
    m_background->setIgnoreCamMovement(true);
    m_background->update(0.0f);

    createGrid();

    m_vignette = std::make_shared<jt::Vignette>(GP::GetScreenSize());
    add(m_vignette);
    m_hud = std::make_shared<Hud>();
    add(m_hud);

    // StateGame will call drawObjects itself.
    setAutoDraw(false);

    getGame()->gfx().camera().move(jt::Vector2f { -15.0f, -15.0f });
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
    }

    //    m_grid->update(elapsed);

    m_background->update(elapsed);
    m_vignette->update(elapsed);
}

void StateGame::doInternalDraw() const
{
    m_background->draw(renderTarget());
    drawObjects();
    m_grid->draw();

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
