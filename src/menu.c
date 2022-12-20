#include "menu.h"
#include "mouse.h"
#include "simple_logger.h"

//UTILITY FUNCTS
/**
 * @brief Sees if mouse is in within a given rectangular field
 * @param mousePos: The mouse's current position
 * @param topLeft: The top-left corner of the field
 * @param bottomRight: The bottom-right corner of the field
 * @param isFieldActive: Whether the field is active. A value of 0 ignores the overlap.
 * @return 1 if the mouse is in the range, 0 otherwise.
 */

int isMouseInBox(Vector2D mousePos, Vector2D topLeft, Vector2D bottomRight, int isFieldActive) {
    int vertCheck = 0, horiCheck = 0;
    if (mousePos.y > topLeft.y && mousePos.y < bottomRight.y) vertCheck = 1;
    if (mousePos.x > topLeft.x && mousePos.x < bottomRight.x) horiCheck = 1;

    if (vertCheck && horiCheck) return 1;
    return 0;
}

//MAIN MENU
void draw_mainMenu(float mouseFrame, Vector2D mousePos) {
    Sprite* mouse = gf2d_sprite_load("images/pointer.png", 32, 32, 16);
    Sprite* logo = gf2d_sprite_load("images/Logo_White.png", 512, 512, 1);
    Sprite* play = gf2d_sprite_load("images/Play.png", 128, 64, 16);
    Sprite* build = gf2d_sprite_load("images/Build.png", 128, 64, 16);

    float logoScale = 2.5;
    Color c_logo = gfc_color(0.65, 0.05, 0.15, 1);

    gf2d_sprite_draw(logo, vector2d(gf3d_vgraphics_get_view_extent().width - 15 - 256 * logoScale, 15), vector2d(logoScale, logoScale), vector3d(0, 0, 0), c_logo, 0);

    gf2d_sprite_draw(play, vector2d(150, 200), vector2d(4, 4), vector3d(0, 0, 0), gfc_color(0.75, 0.05, 0.15, 1), 0);
    gf2d_sprite_draw(build, vector2d(150, 450), vector2d(4, 4), vector3d(0, 0, 0), gfc_color(0.65, 0.05, 0.25, 1), 0);

    gf2d_sprite_draw(mouse, mousePos, vector2d(2, 2), vector3d(0, 0, 0), gfc_color(0.5, 0, 0.5, 0.9), (Uint32)mouseFrame);
}

//SHOP GUI
void draw_shop(SerpentPersStats* sps, PersCurrencies* wallet, float mouseFrame, Vector2D mousePos) {
    

    Sprite* mouse = gf2d_sprite_load("images/pointer.png", 32, 32, 16);
    Sprite* currencyIcons = gf2d_sprite_load("images/Currencies.png", 32, 32, 5);

    Sprite* NodePosNeg = gf2d_sprite_load("images/Upgrade Node -+.png", 64, 32, 1);
    Sprite* NodeNeg = gf2d_sprite_load("images/Upgrade Node -.png", 64, 32, 1);
    Sprite* NodePos = gf2d_sprite_load("images/Upgrade Node +.png", 64, 32, 1);
    Sprite* NodeNeu = gf2d_sprite_load("images/Upgrade Node 0.png", 64, 32, 1);
    Sprite* UG = gf2d_sprite_load("images/Upgrade.png", 48, 32, 1);
    Sprite* DG = gf2d_sprite_load("images/Downgrade.png", 48, 32, 1);

    Sprite* L = gf2d_sprite_load("images/Arrow L.png", 32, 32, 1);
    Sprite* R = gf2d_sprite_load("images/Arrow R.png", 32, 32, 1);

    //gf2d_sprite_draw(HBar, vector2d((gf3d_vgraphics_get_view_extent().width - 505) / 2, gf3d_vgraphics_get_view_extent().height - 60),
    //    vector2d(1.02 * 2, 6), vector3d(0, 0, 0), gfc_color(0, 0, 0, 0.8), 0);
    gf2d_sprite_draw(currencyIcons, vector2d(10, 10 + 26), vector2d(2, 2), vector3d(0, 0, 0), gfc_color(1, 1, 1, 1), 0);
    gf2d_sprite_draw(currencyIcons, vector2d(10, 45 + 26), vector2d(2, 2), vector3d(0, 0, 0), gfc_color(1, 1, 1, 1), 1);
    gf2d_sprite_draw(currencyIcons, vector2d(10, 80 + 26), vector2d(2, 2), vector3d(0, 0, 0), gfc_color(1, 1, 1, 1), 2);
    gf2d_sprite_draw(currencyIcons, vector2d(10, 115 + 26), vector2d(2, 2), vector3d(0, 0, 0), gfc_color(1, 1, 1, 1), 3);
    gf2d_sprite_draw(currencyIcons, vector2d(10, 150 + 26), vector2d(2, 2), vector3d(0, 0, 0), gfc_color(1, 1, 1, 1), 4);
    char buf[64];
    Color c_valid = gfc_color(0, 1, 0, 1);
    Color c_invalid = gfc_color(1, 0, 0, 1);
    Color c_coward = gfc_color(1, 1, 0, 1);
    Color c_white = gfc_color(1, 1, 1, 1);

    Color c_mutagen = gfc_color(0.8, 1, 0.8, 1);
    Color c_goldite = gfc_color(1, 0.95, 0.3, 1);
    Color c_silverium = gfc_color(0.9, 0.9, 0.9, 1);
    Color c_darkite = gfc_color(0.8, 0, 1, 1);
    Color c_orbs = gfc_color(0, 1, 1, 1);

    Color c_metabolism = gfc_color(0.8, 0.5, 0, 1);
    Color c_lure = gfc_color(0.95, 0.95, 1, 1);
    Color c_speed = gfc_color(0.85, 0.875, 0.85, 1);
    Color c_stealth = gfc_color(0.6, 0.4, 0.8, 1);
    Color c_headStart = gfc_color(0, 1, 1, 1);

    int size = 8;
    int halfSize = size / 2;


    int* costs;

    if (isMouseInBox(mousePos,
        vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 5 * halfSize - 48 * halfSize, 10),
        vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 5 * halfSize - 48 * halfSize + 48 * halfSize, 10 + 32 * halfSize),
        sps->metabolism > 1)) {
        costs = getUpgradeCosts(Metabolism, sps->metabolism, -1);
        if (mouseDownLeft()) {
            Upgrade(Metabolism, sps->metabolism, -1, wallet, sps);
        }
    }
    else if (isMouseInBox(mousePos,
        vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 58 * halfSize, 10),
        vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 58 * halfSize + 48 * halfSize, 10 + 32 * halfSize),
        sps->metabolism < 7)) {
        costs = getUpgradeCosts(Metabolism, sps->metabolism, 1);
        if (mouseDownLeft()) {
            Upgrade(Metabolism, sps->metabolism, 1, wallet, sps);
        }
    }
    else if (isMouseInBox(mousePos,
        vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 5 * halfSize - 48 * halfSize, 10 + 138),
        vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 5 * halfSize - 48 * halfSize + 48 * halfSize, 10 + 32 * halfSize + 138),
        sps->lureStrength > 1)) {
        costs = getUpgradeCosts(LureStrength, sps->lureStrength, -1);
        if (mouseDownLeft()) {
            Upgrade(LureStrength, sps->lureStrength, -1, wallet, sps);
        }
    }
    else if (isMouseInBox(mousePos,
        vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 58 * halfSize, 10 + 138),
        vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 58 * halfSize + 48 * halfSize, 10 + 32 * halfSize + 138),
        sps->lureStrength < 7)) {
        costs = getUpgradeCosts(LureStrength, sps->lureStrength, 1);
        if (mouseDownLeft()) {
            Upgrade(LureStrength, sps->lureStrength, 1, wallet, sps);
        }
    }
    else if (isMouseInBox(mousePos,
        vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 58 * halfSize, 10 + 138 * 2),
        vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 58 * halfSize + 48 * halfSize, 10 + 32 * halfSize + 138 * 2),
        sps->stealth < 5)) {
        costs = getUpgradeCosts(Stealth, sps->stealth, 1);
        if (mouseDownLeft()) {
            Upgrade(Stealth, sps->stealth, 1, wallet, sps);
        }
    }
    else if (isMouseInBox(mousePos,
        vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 58 * halfSize, 10 + 138 * 3),
        vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 58 * halfSize + 48 * halfSize, 10 + 32 * halfSize + 138 * 3),
        sps->speed < 5)) {
        costs = getUpgradeCosts(Speed, sps->speed, 1);
        if (mouseDownLeft()) {
            Upgrade(Speed, sps->speed, 1, wallet, sps);
        }
    }
    else if (isMouseInBox(mousePos,
        vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 58 * halfSize, 10 + 138 * 4),
        vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 58 * halfSize + 48 * halfSize, 10 + 32 * halfSize + 138 * 4),
        sps->headStart < 3)) {
        costs = getUpgradeCosts(HeadStart, sps->headStart, 1);
        if (mouseDownLeft()) {
            Upgrade(HeadStart, sps->headStart, 1, wallet, sps);
        }
    }
    else {
        int defCosts[5] = { -1,-1,-1,-1,-1 };
        costs = defCosts;
    }

    snprintf(buf, sizeof(buf), ": %i", wallet->mutagen);
    gf2d_font_draw_line_tag(buf, FT_BC_Normal, c_mutagen, vector2d(42, 26 + 14));
    snprintf(buf, sizeof(buf), ": %i", wallet->goldite);
    gf2d_font_draw_line_tag(buf, FT_BC_Normal, c_goldite, vector2d(42, 61 + 14));
    snprintf(buf, sizeof(buf), ": %i", wallet->silverium);
    gf2d_font_draw_line_tag(buf, FT_BC_Normal, c_silverium, vector2d(42, 96 + 14));
    snprintf(buf, sizeof(buf), ": %i", wallet->darkite);
    gf2d_font_draw_line_tag(buf, FT_BC_Normal, c_darkite, vector2d(42, 131 + 14));
    snprintf(buf, sizeof(buf), ": %i", wallet->orbs);
    gf2d_font_draw_line_tag(buf, FT_BC_Normal, c_orbs, vector2d(42, 166 + 14));

    for (int i = 0; i < 5; i++) {
        if (costs[i] <= 0) continue;
        Color v;
        int w = 0;
        switch (i) {
        case 0:
            w = wallet->mutagen;
            break;
        case 1:
            w = wallet->goldite;
            break;
        case 2:
            w = wallet->silverium;
            break;
        case 3:
            w = wallet->darkite;
            break;
        case 4:
            w = wallet->orbs;
            break;
        }
        if (w >= costs[i])
            v = c_valid;
        else
            v = c_invalid;
        snprintf(buf, sizeof(buf), "-%i", costs[i]);
        gf2d_font_draw_line_tag(buf, FT_BC_Normal, v, vector2d(42+70, 26 + 14 + 35 * i));
    }



    //METABOLISM MODIFY:
    if (sps->metabolism == 1) {
        gf2d_sprite_draw(NodePos, vector2d(gf3d_vgraphics_get_view_extent().width / 2, 10), vector2d(size, size), vector3d(0, 0, 0), c_metabolism, 0);
        gf2d_sprite_draw(UG, vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 58 * halfSize, 10), vector2d(size, size), vector3d(0, 0, 0), c_metabolism, 0);
    }
    else if (sps->metabolism == 7) {
        gf2d_sprite_draw(NodeNeg, vector2d(gf3d_vgraphics_get_view_extent().width / 2, 10), vector2d(size, size), vector3d(0, 0, 0), c_metabolism, 0);
        gf2d_sprite_draw(DG, vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 5 * halfSize - 48 * halfSize, 10), vector2d(size, size), vector3d(0, 0, 0), c_metabolism, 0);
    }
    else {
        gf2d_sprite_draw(NodePosNeg, vector2d(gf3d_vgraphics_get_view_extent().width / 2, 10), vector2d(size, size), vector3d(0, 0, 0), c_metabolism, 0);
        gf2d_sprite_draw(DG, vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 5 * halfSize - 48 * halfSize, 10), vector2d(size, size), vector3d(0, 0, 0), c_metabolism, 0);
        gf2d_sprite_draw(UG, vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 58 * halfSize, 10), vector2d(size, size), vector3d(0, 0, 0), c_metabolism, 0);
    }
    snprintf(buf, sizeof(buf), "Metabolism\nLv. %i", sps->metabolism);
    gf2d_font_draw_line_tag(buf, FT_BC_Large, c_metabolism, vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 14 * size - 80, 10 + 32 * halfSize - 22 - 60));
    int yOffRoot = 32 * halfSize + 10;
    int yOff = yOffRoot;
    //LURE MODIFY:
    if (sps->lureStrength == 1) {
        gf2d_sprite_draw(NodePos, vector2d(gf3d_vgraphics_get_view_extent().width / 2, 10 + yOff), vector2d(size, size), vector3d(0, 0, 0), c_lure, 0);
        gf2d_sprite_draw(UG, vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 58 * halfSize, 10 + yOff), vector2d(size, size), vector3d(0, 0, 0), c_lure, 0);
    }
    else if (sps->lureStrength == 7) {
        gf2d_sprite_draw(NodeNeg, vector2d(gf3d_vgraphics_get_view_extent().width / 2, 10 + yOff), vector2d(size, size), vector3d(0, 0, 0), c_lure, 0);
        gf2d_sprite_draw(DG, vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 5 * halfSize - 48 * halfSize, 10 + yOff), vector2d(size, size), vector3d(0, 0, 0), c_lure, 0);
    }
    else {
        gf2d_sprite_draw(NodePosNeg, vector2d(gf3d_vgraphics_get_view_extent().width / 2, 10 + yOff), vector2d(size, size), vector3d(0, 0, 0), c_lure, 0);
        gf2d_sprite_draw(DG, vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 5 * halfSize - 48 * halfSize, 10 + yOff), vector2d(size, size), vector3d(0, 0, 0), c_lure, 0);
        gf2d_sprite_draw(UG, vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 58 * halfSize, 10 + yOff), vector2d(size, size), vector3d(0, 0, 0), c_lure, 0);
    }
    snprintf(buf, sizeof(buf), "Lure Strength\nLv. %i", sps->lureStrength);
    gf2d_font_draw_line_tag(buf, FT_BC_Large, c_lure, vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 14 * size - 90, 10 + 32 * halfSize - 22 - 60 + yOff));
    yOff = yOffRoot * 2;
    //STEALTH MODIFY:
    if (sps->stealth == 5) {
        gf2d_sprite_draw(NodeNeu, vector2d(gf3d_vgraphics_get_view_extent().width / 2, 10 + yOff), vector2d(size, size), vector3d(0, 0, 0), c_stealth, 0);
    }
    else {
        gf2d_sprite_draw(NodePos, vector2d(gf3d_vgraphics_get_view_extent().width / 2, 10 + yOff), vector2d(size, size), vector3d(0, 0, 0), c_stealth, 0);
        gf2d_sprite_draw(UG, vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 58 * halfSize, 10 + yOff), vector2d(size, size), vector3d(0, 0, 0), c_stealth, 0);
    }
    snprintf(buf, sizeof(buf), "Stealth\nLv. %i", sps->stealth);
    gf2d_font_draw_line_tag(buf, FT_BC_Large, c_stealth, vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 14 * size - 80, 10 + 32 * halfSize - 22 - 60 + yOff));
    yOff = yOffRoot * 3;
    //SPEED MODIFY:
    if (sps->stealth == 5) {
        gf2d_sprite_draw(NodeNeu, vector2d(gf3d_vgraphics_get_view_extent().width / 2, 10 + yOff), vector2d(size, size), vector3d(0, 0, 0), c_speed, 0);
    }
    else {
        gf2d_sprite_draw(NodePos, vector2d(gf3d_vgraphics_get_view_extent().width / 2, 10 + yOff), vector2d(size, size), vector3d(0, 0, 0), c_speed, 0);
        gf2d_sprite_draw(UG, vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 58 * halfSize, 10 + yOff), vector2d(size, size), vector3d(0, 0, 0), c_speed, 0);
    }
    snprintf(buf, sizeof(buf), "Speed\nLv. %i", sps->speed);
    gf2d_font_draw_line_tag(buf, FT_BC_Large, c_speed, vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 14 * size - 80, 10 + 32 * halfSize - 22 - 60 + yOff));
    yOff = yOffRoot * 4;
    //HEAD START MODIFY:
    if (sps->headStart == 3) {
        gf2d_sprite_draw(NodeNeu, vector2d(gf3d_vgraphics_get_view_extent().width / 2, 10 + yOff), vector2d(size, size), vector3d(0, 0, 0), c_headStart, 0);
    }
    else {
        gf2d_sprite_draw(NodePos, vector2d(gf3d_vgraphics_get_view_extent().width / 2, 10 + yOff), vector2d(size, size), vector3d(0, 0, 0), c_headStart, 0);
        gf2d_sprite_draw(UG, vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 58 * halfSize, 10 + yOff), vector2d(size, size), vector3d(0, 0, 0), c_headStart, 0);
    }
    snprintf(buf, sizeof(buf), "Head Start\nLv. %i", sps->headStart);
    gf2d_font_draw_line_tag(buf, FT_BC_Large, c_headStart, vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 14 * size - 80, 10 + 32 * halfSize - 22 - 60 + yOff));


    switch (getGameMode()) {
    case GM_NORMAL:
        if (isMouseInBox(mousePos,
            vector2d(15, gf3d_vgraphics_get_view_extent().height - 16 * halfSize - 15),
            vector2d(15 + 16 * halfSize, gf3d_vgraphics_get_view_extent().height - 15),
            1)) {
            if (mouseDownLeft()) {
                setGameMode(GM_COWARD);
            }
        }
        if (isMouseInBox(mousePos,
            vector2d(300, gf3d_vgraphics_get_view_extent().height - 16 * halfSize - 15),
            vector2d(300 + 16 * halfSize, gf3d_vgraphics_get_view_extent().height - 15),
            1)) {
            if (mouseDownLeft()) {
                setGameMode(GM_SASHIMI);
            }
        }
        gf2d_sprite_draw(L, vector2d(15, gf3d_vgraphics_get_view_extent().height - 16 * halfSize - 15), vector2d(halfSize, halfSize), vector3d(0, 0, 0), c_coward, 0);
        gf2d_font_draw_line_tag("Normal", FT_BC_XLarge, c_white, vector2d(135, gf3d_vgraphics_get_view_extent().height - 16 * halfSize - 15));
        gf2d_font_draw_line_tag("Mode", FT_BC_XLarge, c_white, vector2d(155, gf3d_vgraphics_get_view_extent().height - 16 * halfSize / 2 - 15));
        gf2d_sprite_draw(R, vector2d(300, gf3d_vgraphics_get_view_extent().height - 16 * halfSize - 15), vector2d(halfSize, halfSize), vector3d(0, 0, 0), c_invalid, 0);
        break;
    case GM_COWARD:
        if (isMouseInBox(mousePos,
            vector2d(300, gf3d_vgraphics_get_view_extent().height - 16 * halfSize - 15),
            vector2d(300 + 16 * halfSize, gf3d_vgraphics_get_view_extent().height - 15),
            1)) {
            if (mouseDownLeft()) {
                setGameMode(GM_NORMAL);
            }
        }
        gf2d_font_draw_line_tag("Coward", FT_BC_XLarge, c_coward, vector2d(135, gf3d_vgraphics_get_view_extent().height - 16 * halfSize - 15));
        gf2d_font_draw_line_tag("Mode", FT_BC_XLarge, c_coward, vector2d(155, gf3d_vgraphics_get_view_extent().height - 16 * halfSize / 2 - 15));
        gf2d_sprite_draw(R, vector2d(300, gf3d_vgraphics_get_view_extent().height - 16 * halfSize - 15), vector2d(halfSize, halfSize), vector3d(0, 0, 0), c_white, 0);
        break;
    case GM_SASHIMI:
        if (isMouseInBox(mousePos,
            vector2d(15, gf3d_vgraphics_get_view_extent().height - 16 * halfSize - 15),
            vector2d(15 + 16 * halfSize, gf3d_vgraphics_get_view_extent().height - 15),
            1)) {
            if (mouseDownLeft()) {
                setGameMode(GM_NORMAL);
            }
        }
        gf2d_sprite_draw(L, vector2d(15, gf3d_vgraphics_get_view_extent().height - 16 * halfSize - 15), vector2d(halfSize, halfSize), vector3d(0, 0, 0), c_white, 0);
        gf2d_font_draw_line_tag("Serpent", FT_BC_XLarge, c_invalid, vector2d(135, gf3d_vgraphics_get_view_extent().height - 16 * halfSize - 15));
        gf2d_font_draw_line_tag("Sashimi", FT_BC_XLarge, c_invalid, vector2d(135, gf3d_vgraphics_get_view_extent().height - 16 * halfSize / 2 - 15));
        break;
    }

    gf2d_sprite_draw(mouse, mousePos, vector2d(2, 2), vector3d(0, 0, 0), gfc_color(0.5, 0, 0.5, 0.9), (Uint32)mouseFrame);

}


//LEVEL GUI
void draw_levelGUI(Entity* serpent) {
    Sprite* ExpBar = gf2d_sprite_load("images/ExpBar.png", 500, 10, 1);
    Sprite* HBar = gf2d_sprite_load("images/HPBar.png", 500, 10, 1);
    Sprite* HungerBar = gf2d_sprite_load("images/HungerBar.png", 500, 10, 1);
    Sprite* currencyIcons = gf2d_sprite_load("images/Currencies.png", 32, 32, 5);

    Sprite* LvUp = gf2d_sprite_load("images/LEVEL UP.png", 384, 64, 1);
    Sprite* NodePosNeg = gf2d_sprite_load("images/Upgrade Node -+.png", 64, 32, 1);
    Sprite* NodeNeg = gf2d_sprite_load("images/Upgrade Node -.png", 64, 32, 1);
    Sprite* NodePos = gf2d_sprite_load("images/Upgrade Node +.png", 64, 32, 1);
    Sprite* NodeNeu = gf2d_sprite_load("images/Upgrade Node 0.png", 64, 32, 1);
    Sprite* UG = gf2d_sprite_load("images/Upgrade.png", 48, 32, 1);
    Sprite* DG = gf2d_sprite_load("images/Downgrade.png", 48, 32, 1);
}

//CREATOR GUI
void draw_creatorGUI(float mouseFrame, Vector2D mousePos) {
    Color white = gfc_color(1, 1, 1, 1);
    Sprite* mouse = gf2d_sprite_load("images/pointer.png", 32, 32, 16);
    Sprite* L = gf2d_sprite_load("images/Arrow L.png", 32, 32, 1);
    Sprite* R = gf2d_sprite_load("images/Arrow R.png", 32, 32, 1);
    Sprite* U = gf2d_sprite_load("images/Arrow U.png", 32, 32, 1);
    Sprite* D = gf2d_sprite_load("images/Arrow D.png", 32, 32, 1);
    Sprite* sav = gf2d_sprite_load("images/save.png", 32, 32, 1);
    int size = 8;
    int halfSize = size / 2;

    //Exit & Save
    gf2d_sprite_draw(L, vector2d(15, gf3d_vgraphics_get_view_extent().height - 16 * halfSize - 15), vector2d(halfSize, halfSize), vector3d(0, 0, 0), white, 0);
    gf2d_sprite_draw(sav, vector2d(gf3d_vgraphics_get_view_extent().width - 16 * halfSize - 15, gf3d_vgraphics_get_view_extent().height - 16 * halfSize - 15), vector2d(halfSize, halfSize), vector3d(0, 0, 0), white, 0);

    //Fins
    gf2d_sprite_draw(L, vector2d(gf3d_vgraphics_get_view_extent().width / 2 - 200 - 16 * halfSize, 20), vector2d(halfSize, halfSize), vector3d(0, 0, 0), white, 0);
    gf2d_sprite_draw(R, vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 200, 20), vector2d(halfSize, halfSize), vector3d(0, 0, 0), white, 0);
    
    //Color
    gf2d_sprite_draw(L, vector2d(gf3d_vgraphics_get_view_extent().width / 2 - 200 - 16 * halfSize, gf3d_vgraphics_get_view_extent().height - 16 * halfSize - 20), vector2d(halfSize, halfSize), vector3d(0, 0, 0), white, 0);
    gf2d_sprite_draw(R, vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 200, gf3d_vgraphics_get_view_extent().height - 16 * halfSize - 20), vector2d(halfSize, halfSize), vector3d(0, 0, 0), white, 0);
    
    //Head
    gf2d_sprite_draw(U, vector2d(50, gf3d_vgraphics_get_view_extent().height / 2 - 100 - 16 * halfSize), vector2d(halfSize, halfSize), vector3d(0, 0, 0), white, 0);
    gf2d_sprite_draw(D, vector2d(50, gf3d_vgraphics_get_view_extent().height / 2 + 100), vector2d(halfSize, halfSize), vector3d(0, 0, 0), white, 0);
    

    //Tail
    gf2d_sprite_draw(U, vector2d(gf3d_vgraphics_get_view_extent().width - 16 * halfSize - 50, gf3d_vgraphics_get_view_extent().height / 2 - 100 - 16 * halfSize), vector2d(halfSize, halfSize), vector3d(0, 0, 0), white, 0);
    gf2d_sprite_draw(D, vector2d(gf3d_vgraphics_get_view_extent().width - 16 * halfSize - 50, gf3d_vgraphics_get_view_extent().height / 2 + 100), vector2d(halfSize, halfSize), vector3d(0, 0, 0), white, 0);

    gf2d_sprite_draw(mouse, mousePos, vector2d(2, 2), vector3d(0, 0, 0), gfc_color(0.5, 0, 0.5, 0.9), (Uint32)mouseFrame);
}