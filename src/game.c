#include <SDL.h>            

#include "simple_logger.h"
#include "gfc_input.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"

#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_model.h"
#include "gf3d_camera.h"
#include "gf3d_texture.h"
#include "gf3d_particle.h"

#include "simple_json.h"
#include "simple_json_string.h"
#include "simple_json_parse.h"
#include "simple_json_error.h"

#include "gf2d_sprite.h"
#include "gf2d_font.h"

#include "entity.h"
#include "agumon.h"
#include "serpent.h"
#include "ai_fish.h"
#include "player.h"
#include "world.h"

extern int __DEBUG;

int main(int argc,char *argv[])
{
    int done = 0;
    int a;
    
    Sprite *mouse = NULL;
    int mousex,mousey;
    //Uint32 then;
    float mouseFrame = 0;
    World *w;
    Entity *agu;
    Particle particle[100];
    Matrix4 skyMat;
    Model *sky;

    for (a = 1; a < argc;a++)
    {
        if (strcmp(argv[a],"--debug") == 0)
        {
            __DEBUG = 1;
        }
    }
    
    init_logger("gf3d.log",0);    
    gfc_input_init("config/input.cfg");
    slog("gf3d begin");
    gf3d_vgraphics_init("config/setup.cfg");
    gf2d_font_init("config/font.cfg");

    slog_sync();
    slog("Synced slog");
    entity_system_init(300);
    
    mouse = gf2d_sprite_load("images/pointer.png",32,32, 16);
    Sprite* ExpBar = gf2d_sprite_load("images/ExpBar.png", 500, 10, 1);
    Sprite* HBar = gf2d_sprite_load("images/HPBar.png", 500, 10, 1);
    Sprite* HungerBar = gf2d_sprite_load("images/HungerBar.png", 500, 10, 1);
    Sprite* currencyIcons = gf2d_sprite_load("images/Currencies.png", 32, 32, 5);

    Sprite* LvUp = gf2d_sprite_load("images/LEVEL UP.png", 192, 32, 1);
    Sprite* NodePosNeg = gf2d_sprite_load("images/Upgrade Node -+.png", 64, 32, 1);
    Sprite* NodeNeg = gf2d_sprite_load("images/Upgrade Node -.png", 64, 32, 1);
    Sprite* NodePos = gf2d_sprite_load("images/Upgrade Node +.png", 64, 32, 1);
    Sprite* NodeNeu = gf2d_sprite_load("images/Upgrade Node 0.png", 64, 32, 1);
    Sprite* UG = gf2d_sprite_load("images/Upgrade.png", 48, 32, 1);
    Sprite* DG = gf2d_sprite_load("images/Downgrade.png", 48, 32, 1);
    
    int firstTime = 0;

    SerpentPersStats* sps;
    sps = (SerpentPersStats*)gfc_allocate_array(sizeof(SerpentPersStats), 1);
    if (!sps) {
        slog("Well dang, no room for the serpent's stats.");
        slog_sync();
        return 0;
    }
    PersCurrencies* wallet;
    wallet = (PersCurrencies*)gfc_allocate_array(sizeof(PersCurrencies), 1);
    if (!wallet) {
        slog("Well dang, no room for the serpent's wallet.");
        slog_sync();
        return 0;
    }
    slog("Created slot for the serpent's stats.");
    slog_sync();
    SJson* saveFile = sj_load("serpentSave.sav");
    SJson* metabolism = sj_object_new();
    SJson* lureStrength = sj_object_new();
    SJson* stealth = sj_object_new();
    SJson* speed = sj_object_new();
    SJson* headStart = sj_object_new();
    SJson* cur1 = sj_object_new();
    SJson* cur2 = sj_object_new();
    SJson* cur3 = sj_object_new();
    SJson* cur4 = sj_object_new();
    SJson* cur5 = sj_object_new();
    slog("Initialized JSON variables.");
    slog_sync();
    if (!saveFile) {
        slog("Creating a new save file...");
        firstTime = 1;
        saveFile = sj_object_new();
        metabolism = sj_new_int(4);
        lureStrength = sj_new_int(4);
        stealth = sj_new_int(1);
        speed = sj_new_int(1);
        headStart = sj_new_int(1);
        cur1 = sj_new_int(0);
        cur2 = sj_new_int(0);
        cur3 = sj_new_int(0);
        cur4 = sj_new_int(0);
        cur5 = sj_new_int(0);
        sj_object_insert(saveFile, "metabolism", metabolism);
        sj_object_insert(saveFile, "lureStrength", lureStrength);
        sj_object_insert(saveFile, "stealth", stealth);
        sj_object_insert(saveFile, "speed", speed);
        sj_object_insert(saveFile, "headStart", headStart);
        sj_object_insert(saveFile, "cur1", cur1);
        sj_object_insert(saveFile, "cur2", cur2);
        sj_object_insert(saveFile, "cur3", cur3);
        sj_object_insert(saveFile, "cur4", cur4);
        sj_object_insert(saveFile, "cur5", cur5);

        sj_save(saveFile, "serpentSave.sav");
    }
    else {
        slog("Loading save file...");
        metabolism = sj_object_get_value(saveFile, "metabolism");
        lureStrength = sj_object_get_value(saveFile, "lureStrength");
        stealth = sj_object_get_value(saveFile, "stealth");
        speed = sj_object_get_value(saveFile, "speed");
        headStart = sj_object_get_value(saveFile, "headStart");
        cur1 = sj_object_get_value(saveFile, "cur1");
        cur2 = sj_object_get_value(saveFile, "cur2");
        cur3 = sj_object_get_value(saveFile, "cur3");
        cur4 = sj_object_get_value(saveFile, "cur4");
        cur5 = sj_object_get_value(saveFile, "cur5");
    }
    slog("Did file stuff.");
    slog_sync();
    int spVal = 0, success = 0;
    if (speed)
        success = 1;
    slog("%i, Speed is going to be set to %i.", success, spVal);
    slog_sync();
    sj_get_integer_value(speed, &spVal);

    sj_get_integer_value(metabolism, &sps->metabolism);
    sj_get_integer_value(lureStrength, &sps->lureStrength);
    sj_get_integer_value(stealth, &sps->stealth);
    sj_get_integer_value(speed, &sps->speed);
    sj_get_integer_value(headStart, &sps->headStart);
    sj_get_integer_value(cur1, &wallet->mutagen);
    sj_get_integer_value(cur2, &wallet->goldite);
    sj_get_integer_value(cur3, &wallet->silverium);
    sj_get_integer_value(cur4, &wallet->darkite);
    sj_get_integer_value(cur5, &wallet->orbs);

    sj_free(saveFile);
    sj_object_free(metabolism);
    sj_object_free(lureStrength);
    sj_object_free(stealth);
    sj_object_free(speed);
    sj_object_free(headStart);
    sj_object_free(cur1);
    sj_object_free(cur2);
    sj_object_free(cur3);
    sj_object_free(cur4);
    sj_object_free(cur5);

    int playing = 1;

    while(playing){
        int quitting = 0;
        if (firstTime) {
            firstTime = 0;
        }
        else {
            slog("ENTERING SHOP LOOP");
            slog_sync();
            while (!done && !quitting) {
                gfc_input_update();
                //slog("Updated input");
                //slog_sync();
                gf2d_font_update();
                //slog("Updated fonts");
                //slog_sync();
                SDL_GetMouseState(&mousex, &mousey);
                //slog("Got mouse state");
                //slog_sync();

                mouseFrame += 0.01;
                if (mouseFrame >= 16)mouseFrame = 0;

                gf3d_vgraphics_render_start();
                //slog("Started render.");
                //slog_sync();

                //3D draws
                //2D draws
                    //gf2d_sprite_draw(mouse,vector2d(mousex,mousey),vector2d(2,2), vector3d(0, 0, 0), gfc_color(1, 1, 1, 1), (Uint32)mouseFrame);
                gf2d_sprite_draw(HBar, vector2d((gf3d_vgraphics_get_view_extent().width - 505) / 2, gf3d_vgraphics_get_view_extent().height - 60),
                    vector2d(1.02 * 2, 6), vector3d(0, 0, 0), gfc_color(0, 0, 0, 0.8), 0);
                gf2d_sprite_draw(currencyIcons, vector2d(10, 10 + 26), vector2d(2, 2), vector3d(0, 0, 0), gfc_color(1, 1, 1, 1), 0);
                gf2d_sprite_draw(currencyIcons, vector2d(10, 45 + 26), vector2d(2, 2), vector3d(0, 0, 0), gfc_color(1, 1, 1, 1), 1);
                gf2d_sprite_draw(currencyIcons, vector2d(10, 80 + 26), vector2d(2, 2), vector3d(0, 0, 0), gfc_color(1, 1, 1, 1), 2);
                gf2d_sprite_draw(currencyIcons, vector2d(10, 115 + 26), vector2d(2, 2), vector3d(0, 0, 0), gfc_color(1, 1, 1, 1), 3);
                gf2d_sprite_draw(currencyIcons, vector2d(10, 150 + 26), vector2d(2, 2), vector3d(0, 0, 0), gfc_color(1, 1, 1, 1), 4);
                char buf[64];
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


                int size = 8;
                int halfSize = size / 2;

                //METABOLISM MODIFY:
                if (sps->metabolism == 1) {
                    gf2d_sprite_draw(NodePos, vector2d(gf3d_vgraphics_get_view_extent().width / 2, 10), vector2d(size,size), vector3d(0, 0, 0), c_metabolism, 0);
                    gf2d_sprite_draw(UG, vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 58 * halfSize, 10), vector2d(size,size), vector3d(0, 0, 0), c_metabolism, 0);
                }
                else if (sps->metabolism == 7) {
                    gf2d_sprite_draw(NodeNeg, vector2d(gf3d_vgraphics_get_view_extent().width / 2, 10), vector2d(size,size), vector3d(0, 0, 0), c_metabolism, 0);
                    gf2d_sprite_draw(DG, vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 5 * halfSize - 48 * halfSize, 10), vector2d(size,size), vector3d(0, 0, 0), c_metabolism, 0);
                }
                else {
                    gf2d_sprite_draw(NodePosNeg, vector2d(gf3d_vgraphics_get_view_extent().width / 2, 10), vector2d(size,size), vector3d(0, 0, 0), c_metabolism, 0);
                    gf2d_sprite_draw(DG, vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 5 * halfSize - 48 * halfSize, 10), vector2d(size,size), vector3d(0, 0, 0), c_metabolism, 0);
                    gf2d_sprite_draw(UG, vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 58 * halfSize, 10), vector2d(size,size), vector3d(0, 0, 0), c_metabolism, 0);
                }
                snprintf(buf, sizeof(buf), "Metabolism\nLv. %i", sps->metabolism);
                gf2d_font_draw_line_tag(buf, FT_BC_Large, c_metabolism, vector2d(gf3d_vgraphics_get_view_extent().width / 2 + 14 * size - 80, 10 + 32 * halfSize - 22 - 60));
                int yOffRoot = 32 * halfSize + 10;
                int yOff = yOffRoot;
                //LURE MODIFY:
                if (sps->lureStrength == 1) {
                    gf2d_sprite_draw(NodePos, vector2d(gf3d_vgraphics_get_view_extent().width / 2, 10 + yOff), vector2d(size,size), vector3d(0, 0, 0), c_lure, 0);
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


                gf2d_sprite_draw(mouse, vector2d(mousex, mousey), vector2d(2, 2), vector3d(0, 0, 0), gfc_color(0.5, 0, 0.5, 0.9), (Uint32)mouseFrame);


                gf3d_vgraphics_render_end();

                slog_sync();
                if (gfc_input_command_down("exit")) quitting = 1;
                if (gfc_input_key_pressed(" ")) done = 1;
            }
            done = 0;
        }
        if (quitting) {
            Save(wallet, sps);
            break;
        }


        w = world_load("config/abyss.json");
        /*
        for (a = 0; a < 10;a++)
        {
            agumon_new(vector3d(a * 10 -50,0,0));
        }
        */
    

        Entity* serpent = serpent_new(vector3d(0, 0, 25), sps, wallet);
        slog("Created serpent & HP Bar.");
        slog_sync();

        for (int i = 0; i < 15; i++) {
            //prey_new(vector3d(i * 10, i * 10, 20), 0.5f, 1.5f, serpent);
            spawn_new_around_serpent(serpent, 25, 0);
        }

        slog("Created 15 fish.");
        slog_sync();
        if (!serpent) {
            slog("UH OH.");
            slog_sync();
            return 0;
        }
    
        slog_sync();
        gf3d_camera_set_scale(vector3d(1,1,1));
        //player_new(vector3d(0,0,20));
        Uint64 runStartTime = SDL_GetTicks64();
        // main game loop
        slog("gf3d level loop begin");

        while(!done)
        {
            if (get_fish_count() - get_predator_count() < 10) {
                spawn_new_around_serpent(serpent, 25 * ((SerpentData*)(serpent->customData))->size, SDL_GetTicks64() - runStartTime);
                slog("Spawned a new fish.");
            }

            gfc_input_update();
            gf2d_font_update();
            SDL_GetMouseState(&mousex,&mousey);
        
            mouseFrame += 0.01;
            if (mouseFrame >= 16)mouseFrame = 0;
            world_run_updates(w);
            entity_think_all();
            entity_update_all();
            entity_fearCheck_all();
            gf3d_camera_update_view();
            gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());

            gf3d_vgraphics_render_start();

                //3D draws
                    world_draw(w);
                    entity_draw_all();
                //2D draws
                    //gf2d_sprite_draw(mouse,vector2d(mousex,mousey),vector2d(2,2), vector3d(0, 0, 0), gfc_color(1, 1, 1, 1), (Uint32)mouseFrame);
                    gf2d_sprite_draw(HBar, vector2d((gf3d_vgraphics_get_view_extent().width - 505)/ 2 , gf3d_vgraphics_get_view_extent().height - 60),
                        vector2d(1.02 * 2, 6), vector3d(0, 0, 0), gfc_color(0, 0, 0, 0.8), 0);
                    if (serpent && ((SerpentData*)(serpent->customData))->exp > 0) {
                        gf2d_sprite_draw(ExpBar, vector2d((gf3d_vgraphics_get_view_extent().width - 500) / 2, gf3d_vgraphics_get_view_extent().height - 55),
                            vector2d((((SerpentData*)(serpent->customData))->exp / ((SerpentData*)(serpent->customData))->expThreshold) * 2, 2), vector3d(0,0,0), gfc_color(1,1,1,1), 0);
                    }
                    if (serpent && ((SerpentData*)(serpent->customData))->health > 0) {
                        gf2d_sprite_draw(HBar, vector2d((gf3d_vgraphics_get_view_extent().width - 500) / 2, gf3d_vgraphics_get_view_extent().height - 40),
                            vector2d((((SerpentData*)(serpent->customData))->health / ((SerpentData*)(serpent->customData))->healthMax) * 2, 3), vector3d(0,0,0), gfc_color(1,1,1,1), 0);
                    }
                    if (serpent && ((SerpentData*)(serpent->customData))->hunger > 0) {
                        gf2d_sprite_draw(HungerBar, vector2d((gf3d_vgraphics_get_view_extent().width - 500) / 2, gf3d_vgraphics_get_view_extent().height - 20),
                            vector2d(((SerpentData*)(serpent->customData))->hunger * 2, 1), vector3d(0,0,0), gfc_color(1,1,1,1), 0);
                    }
                    gf2d_sprite_draw(currencyIcons, vector2d(10, 10 + 26), vector2d(2, 2), vector3d(0, 0, 0), gfc_color(1, 1, 1, 1), 0);
                    gf2d_sprite_draw(currencyIcons, vector2d(10, 45 + 26), vector2d(2, 2), vector3d(0, 0, 0), gfc_color(1, 1, 1, 1), 1);
                    gf2d_sprite_draw(currencyIcons, vector2d(10, 80 + 26), vector2d(2, 2), vector3d(0, 0, 0), gfc_color(1, 1, 1, 1), 2);
                    gf2d_sprite_draw(currencyIcons, vector2d(10, 115 + 26), vector2d(2, 2), vector3d(0, 0, 0), gfc_color(1, 1, 1, 1), 3);
                    gf2d_sprite_draw(currencyIcons, vector2d(10, 150 + 26), vector2d(2, 2), vector3d(0, 0, 0), gfc_color(1, 1, 1, 1), 4);
                    char buf[64];
                    snprintf(buf, sizeof(buf), ": %i", wallet->mutagen);
                    gf2d_font_draw_line_tag(buf,FT_BC_Normal,gfc_color(0.8,1,0.8,1), vector2d(42,26 + 14));
                    snprintf(buf, sizeof(buf), ": %i", wallet->goldite);
                    gf2d_font_draw_line_tag(buf,FT_BC_Normal,gfc_color(1,0.95,0.3,1), vector2d(42,61 + 14));
                    snprintf(buf, sizeof(buf), ": %i", wallet->silverium);
                    gf2d_font_draw_line_tag(buf,FT_BC_Normal,gfc_color(0.9,0.9,0.9,1), vector2d(42,96 + 14));
                    snprintf(buf, sizeof(buf), ": %i", wallet->darkite);
                    gf2d_font_draw_line_tag(buf,FT_BC_Normal,gfc_color(0.8,0,1,1), vector2d(42,131 + 14));
                    snprintf(buf, sizeof(buf), ": %i", wallet->orbs);
                    gf2d_font_draw_line_tag(buf,FT_BC_Normal,gfc_color(0,1,1,1), vector2d(42,166 + 14));

                    snprintf(buf, sizeof(buf), "SCORE: %i", ((SerpentData*)(serpent->customData))->score);
                    gf2d_font_draw_line_tag(buf, FT_BC_Large, gfc_color(1, 1, 1, 1), vector2d(5, 5));

                    if (((SerpentData*)(serpent->customData))->levelUpDisplay > 0) {

                    }

                    gf2d_sprite_draw(mouse,vector2d(mousex,mousey),vector2d(2,2),vector3d(0,0,0),gfc_color(0.5,0,0.5,0.9),(Uint32)mouseFrame);
                

            gf3d_vgraphics_render_end();

            if (((SerpentData*)(serpent->customData))->health <= 0) done = 1;

            slog_sync();
            if (gfc_input_command_down("exit")) { done = 1; playing = 0; }// exit condition
        }    
        done = 0;
        Save(wallet, sps);
        world_delete(w);
        entity_free_all();
    }
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
