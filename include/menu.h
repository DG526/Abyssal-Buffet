#pragma once

#include "gf3d_vgraphics.h"
#include "gf2d_sprite.h"
#include "gf2d_font.h"

#include "entity.h"
#include "agumon.h"
#include "serpent.h"

void draw_mainMenu(float mouseFrame, Vector2D mousePos);
void draw_creatorGUI(float mouseFrame, Vector2D mousePos);
void draw_shop(SerpentPersStats* sps, PersCurrencies* wallet, float mouseFrame, Vector2D mousePos);
void upgrade_purchase(SerpentPersStats* sps, PersCurrencies* wallet, Vector2D mousePos);
void draw_levelGUI(Entity* serpent, PersCurrencies* wallet);