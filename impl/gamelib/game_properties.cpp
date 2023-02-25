﻿#include "game_properties.hpp"
#include <color/palette_builder.hpp>

namespace {

jt::Palette createPalette()
{
    jt::PaletteBuilder builder;
    return builder
        .addColorsFromGPL(R"(GIMP Palette
#Palette Name: 31
#Description:
#Colors: 31
99	102	99	636663
135	133	124	87857c
188	173	159	bcad9f
242	184	136	f2b888
235	150	97	eb9661
181	89	69	b55945
115	76	68	734c44
61	51	51	3d3333
89	62	71	593e47
122	88	89	7a5859
165	120	85	a57855
222	159	71	de9f47
253	209	121	fdd179
254	225	184	fee1b8
212	198	146	d4c692
166	176	79	a6b04f
129	148	71	819447
68	112	45	44702d
47	77	47	2f4d2f
84	103	86	546756
137	164	119	89a477
164	197	175	a4c5af
202	230	217	cae6d9
241	246	240	f1f6f0
213	214	219	d5d6db
187	195	208	bbc3d0
150	169	193	96a9c1
108	129	161	6c81a1
64	82	115	405273
48	56	67	303843
20	35	58	14233a
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
