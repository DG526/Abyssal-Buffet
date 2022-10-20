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
#include "gf3d_sprite.h"

#include "simple_json.h"
#include "simple_json_string.h"
#include "simple_json_parse.h"
#include "simple_json_error.h"

#include "entity.h"
#include "agumon.h"
#include "serpent.h"
#include "player.h"
#include "world.h"

extern int __DEBUG;

int main(int argc,char *argv[])
{
    int done = 0;
    int a;
    
    Sprite *mouse = NULL;
    int mousex,mousey;
    float mouseFrame = 0;
    World *w;
    
    for (a = 1; a < argc;a++)
    {
        if (strcmp(argv[a],"--debug") == 0)
        {
            __DEBUG = 1;
        }
    }
    
    init_logger("gf3d.log");    
    gfc_input_init("config/input.cfg");
    slog("gf3d begin");
    gf3d_vgraphics_init("config/setup.cfg");
    slog_sync();
    
    entity_system_init(1024);
    
    mouse = gf3d_sprite_load("images/pointer.png",32,32, 16);
    
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
    SJson* metabolism;
    SJson* lureStrength;
    SJson* stealth;
    SJson* speed;
    SJson* headStart;
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
        speed = sj_object_get_value(speed, "metabolism");
        headStart = sj_object_get_value(headStart, "metabolism");
    }
    sj_get_integer_value(metabolism, sps->metobolism);
    sj_get_integer_value(lureStrength, sps->lureStrength);
    sj_get_integer_value(stealth, sps->stealth);
    sj_get_integer_value(speed, sps->speed);
    sj_get_integer_value(headStart, sps->headStart);

    sj_free(saveFile);
    sj_object_free(metabolism);
    sj_object_free(lureStrength);
    sj_object_free(stealth);
    sj_object_free(speed);
    sj_object_free(headStart);

    serpent_new(vector3d(0, 0, 25), sps);
    
    slog_sync();
    gf3d_camera_set_scale(vector3d(1,1,1));
    player_new(vector3d(0,0,20));
    
    // main game loop
    slog("gf3d main loop begin");
    while(!done)
    {
        gfc_input_update();
        SDL_GetMouseState(&mousex,&mousey);
        
        mouseFrame += 0.01;
        if (mouseFrame >= 16)mouseFrame = 0;
        entity_think_all();
        entity_update_all();
        gf3d_camera_update_view();
        gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());

        // configure render command for graphics command pool
        // for each mesh, get a command and configure it from the pool
        gf3d_vgraphics_render_start();

            //3D draws
                world_draw(w);
                entity_draw_all();
            //2D draws
                gf3d_sprite_draw(mouse,vector2d(mousex,mousey),vector2d(2,2),(Uint32)mouseFrame);
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
