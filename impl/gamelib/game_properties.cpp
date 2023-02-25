#include "game_properties.hpp"
#include <color/palette_builder.hpp>

namespace {

jt::Palette createPalette()
{
    jt::PaletteBuilder builder;
    return builder
        .addColorsFromGPL(R"(GIMP Palette
#Palette Name: Lite34
#Description:
#Colors: 34
153	14	90	990e5a
217	35	56	d92338
230	116	46	e6742e
242	165	70	f2a546
255	202	87	ffca57
255	253	130	fffd82
198	245	44	c6f52c
83	235	28	53eb1c
18	196	77	12c44d
0	122	96	007a60
42	17	143	2a118f
30	40	176	1e28b0
33	77	209	214dd1
43	120	227	2b78e3
70	180	240	46b4f0
130	251	255	82fbff
196	255	247	c4fff7
166	214	222	a6d6de
141	183	199	8db7c7
125	157	181	7d9db5
104	128	156	68809c
90	104	138	5a688a
103	41	128	672980
142	57	153	8e3999
184	83	163	b853a3
217	126	182	d97eb6
255	184	212	ffb8d4
255	253	242	fffdf2
238	204	176	eeccb0
207	150	134	cf9686
179	99	99	b36363
116	49	57	743139
71	21	43	47152b
34	4	34	220422
)")
        .create();
}

} // namespace

jt::Palette GP::getPalette()
{
    static auto const p = createPalette();
    return p;
}

int GP::PhysicVelocityIterations() { return 20; }

int GP::PhysicPositionIterations() { return 20; }
jt::Vector2f GP::PlayerSize() { return jt::Vector2f { 16.0f, 16.0f }; }
