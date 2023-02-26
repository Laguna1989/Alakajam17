#ifndef GAME_GAMEPROPERTIES_HPP
#define GAME_GAMEPROPERTIES_HPP

#include <color/color.hpp>
#include <color/palette.hpp>
#include <vector.hpp>
#include <string>

class GP {
public:
    GP() = delete;

    static std::string GameName() { return "Nile River"; }
    static std::string AuthorName() { return "Laguna_999"; }
    static std::string JamName() { return "Alakajam 17"; }
    static std::string JamDate() { return "2022-02"; }
    static std::string ExplanationText()
    {
        return "Connect the new cities to the river sources before the source overflows\n[Left "
               "Click] to spawn/place "
               "connection\n[Right Click] to cancel building a connection\nYou can only connect "
               "neighbours";
    }

    static jt::Vector2f GetWindowSize() { return jt::Vector2f { 1200, 800 }; }
    static float GetZoom() { return 2.0f; }
    static jt::Vector2f GetScreenSize() { return GetWindowSize() * (1.0f / GetZoom()); }

    static jt::Color PaletteBackground() { return GP::getPalette().getColor(12); }
    static jt::Color PaletteFontFront() { return GP::getPalette().getColor(6); }
    static jt::Color PalleteFrontHighlight() { return GP::getPalette().getColor(1); }
    static jt::Color PaletteFontShadow() { return GP::getPalette().getColor(11); }
    static jt::Color PaletteFontCredits() { return GP::getPalette().getColor(9); }

    static jt::Palette getPalette();

    static int PhysicVelocityIterations();
    static int PhysicPositionIterations();
    static jt::Vector2f PlayerSize();
};

#endif
