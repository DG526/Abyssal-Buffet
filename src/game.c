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

#include "gf2d_sprite.h"
#include "gf2d_font.h"

#include "simple_json.h"
#include "simple_json_string.h"
#include "simple_json_parse.h"
#include "simple_json_error.h"

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
    
    entity_system_init(1024);
    
    mouse = gf2d_sprite_load("images/pointer.png",32,32, 16);
    
<<<<<<< HEAD
    w = world_load("config/testworld.json");
    /*
    for (a = 0; a < 10;a++)
    {
        agumon_new(vector3d(a * 10 -50,0,0));
    }
    */
    SerpentPersStats* sps;
    sps = (SerpentPersStats*)gfc_allocate_array(sizeof(SerpentPersStats), 1);
    if (!sps) {
        slog("Well dang, no room for the serpent's stats.");
        return 0;
    }
    SJson* saveFile = sj_load("serpentSave.sav");
    SJson* metabolism = sj_object_new();
    SJson* lureStrength = sj_object_new();
    SJson* stealth = sj_object_new();
    SJson* speed = sj_object_new();
    SJson* headStart = sj_object_new();
    if (!saveFile) {
        slog("Creating a new save file...");
        saveFile = sj_object_new();
        metabolism = sj_new_int(4);
        lureStrength = sj_new_int(4);
        stealth = sj_new_int(1);
        speed = sj_new_int(1);
        headStart = sj_new_int(1);
        sj_object_insert(saveFile, "metabolism", metabolism);
        sj_object_insert(saveFile, "lureStrength", lureStrength);
        sj_object_insert(saveFile, "stealth", stealth);
        sj_object_insert(saveFile, "speed", speed);
        sj_object_insert(saveFile, "headStart", headStart);

        sj_save(saveFile, "serpentSave.sav");
    }
    else {
        slog("Loading save file...");
        metabolism = sj_object_get_value(saveFile, "metabolism");
        lureStrength = sj_object_get_value(saveFile, "lureStrength");
        stealth = sj_object_get_value(saveFile, "stealth");
        speed = sj_object_get_value(saveFile, "speed");
        headStart = sj_object_get_value(saveFile, "headStart");
    }
    int spVal = 0, success = 0;
    if (speed)
        success = 1;
    sj_get_integer_value(speed, &spVal);
    slog("%i, Speed is going to be set to %i.", success, spVal);

    sj_get_integer_value(metabolism, &sps->metobolism);
    sj_get_integer_value(lureStrength, &sps->lureStrength);
    sj_get_integer_value(stealth, &sps->stealth);
    sj_get_integer_value(speed, &sps->speed);
    sj_get_integer_value(headStart, &sps->headStart);

    sj_free(saveFile);
    sj_object_free(metabolism);
    sj_object_free(lureStrength);
    sj_object_free(stealth);
    sj_object_free(speed);
    sj_object_free(headStart);

    Entity* serpent = serpent_new(vector3d(0, 0, 25), sps);
    Sprite* HBar = gf3d_sprite_load("images/HPBar.png", 500, 10, 1);

    for (int i = 0; i < 15; i++) {
        prey_new(vector3d(i * 10, i * 10, 20), 0.5f, 1.5f);
    }
=======
    
    agu = agumon_new(vector3d(0 ,0,0));
    if (agu)agu->selected = 1;
    w = world_load("config/testworld.json");
>>>>>>> 44df97f3c129a8df28592b55e211cae4afea3812
    
    SDL_SetRelativeMouseMode(SDL_TRUE);
    slog_sync();
    gf3d_camera_set_scale(vector3d(1,1,1));
    player_new(vector3d(-50,0,0));
    
    for (a = 0; a < 100; a++)
    {
        particle[a].position = vector3d(gfc_crandom() * 100,gfc_crandom() * 100,gfc_crandom() * 100);
        particle[a].color = gfc_color(gfc_random(),gfc_random(),gfc_random(),1);
        particle[a].size = 100 * gfc_random();
    }
    a = 0;
    sky = gf3d_model_load("models/sky.model");
    gfc_matrix_identity(skyMat);
    gfc_matrix_scale(skyMat,vector3d(100,100,100));
    
    // main game loop
    slog("gf3d main loop begin");
    while(!done)
    {
        gfc_input_update();
        gf2d_font_update();
        SDL_GetMouseState(&mousex,&mousey);
        
        mouseFrame += 0.01;
        if (mouseFrame >= 16)mouseFrame = 0;
        world_run_updates(w);
        entity_think_all();
        entity_update_all();
        gf3d_camera_update_view();
        gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());

        gf3d_vgraphics_render_start();

            //3D draws
                gf3d_model_draw_sky(sky,skyMat,gfc_color(1,1,1,1));
                world_draw(w);
                entity_draw_all();
                
                for (a = 0; a < 100; a++)
                {
                    gf3d_particle_draw(&particle[a]);
                }
            //2D draws
<<<<<<< HEAD
                gf3d_sprite_draw(mouse,vector2d(mousex,mousey),vector2d(2,2),(Uint32)mouseFrame);
                if (((SerpentData*)(serpent->customData))->health > 0) {
                    gf3d_sprite_draw(HBar, vector2d((gf3d_vgraphics_get_view_extent().width - 500) / 2, gf3d_vgraphics_get_view_extent().height - 20),
                        vector2d((((SerpentData*)(serpent->customData))->health / ((SerpentData*)(serpent->customData))->healthMax) * 2, 3), 0);
                }
=======
                gf2d_font_draw_line_tag("Press ALT+F4 to exit",FT_H1,gfc_color(1,1,1,1), vector2d(10,10));
                gf2d_sprite_draw(mouse,vector2d(mousex,mousey),vector2d(2,2),vector3d(8,8,GFC_PI * mousex / 600.0),gfc_color(0.3,.9,1,0.9),(Uint32)mouseFrame);
>>>>>>> 44df97f3c129a8df28592b55e211cae4afea3812
        gf3d_vgraphics_render_end();

        if (gfc_input_command_down("exit"))done = 1; // exit condition
    }    
    
    world_delete(w);
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
