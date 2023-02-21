//
// Created by helen on 12/05/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h> // for random seed
#include <string.h> // memset function

#include "ei_application.h"
#include "ei_widget.h"
#include "ei_utils.h"
#include "ei_event.h"
#include "ei_geometrymanager.h"

typedef struct game_map_t {
    int window_game_width;
    int window_game_height;

} game_map_t ;

ei_bool_t process_key(ei_widget_t* widget, ei_event_t* event, void* user_param)
{
    if (event->param.key.key_code == SDLK_ESCAPE) {
        ei_app_quit_request();
        return EI_TRUE;
    }

    return EI_FALSE;
}

void create_game_window(game_map_t* map) {
    /*grande toplevel*/
    ei_widget_t* top_level;
    ei_widget_t* little_top_level;
    ei_size_t	top_level_size		= {400,400};
    ei_size_t little_top_level_size = {200,200};
    ei_color_t	top_level_color		= {0xdd, 0xdd, 0xdd, 0xff};
    ei_anchor_t 	top_level_anchor	= ei_anc_northwest;
    float           top_level_rel_width = .8;
    float           top_level_rel_height = .8;
    int toplevel_border_width = 6;
    float           top_level_rel_x = .1;
    float           top_level_rel_y = .1;
    ei_bool_t       top_level_closable = EI_TRUE;
    ei_axis_set_t   top_level_axis = ei_axis_both;
    char *title_toplevel = "Jeu";

    /* Create, configure and place the frame on screen. */

    top_level = ei_widget_create("toplevel", ei_app_root_widget(), NULL, NULL);
    /*petite toplevel*/
    little_top_level = ei_widget_create("toplevel", top_level, NULL, NULL);

    ei_toplevel_configure(top_level, &top_level_size, &top_level_color,
                          &toplevel_border_width, &title_toplevel, &top_level_closable, &top_level_axis, &top_level_size);
    ei_toplevel_configure(top_level, &little_top_level_size, &top_level_color,
                          &toplevel_border_width, &title_toplevel, &top_level_closable, &top_level_axis, &little_top_level_size);
    ei_place(top_level, &top_level_anchor, NULL, NULL, NULL, NULL, &top_level_rel_x, &top_level_rel_y, &top_level_rel_width, &top_level_rel_height);
    ei_place(little_top_level, &top_level_anchor, NULL, NULL, NULL, NULL, &top_level_rel_x, &top_level_rel_y, &top_level_rel_width, &top_level_rel_height);
}


int main(void) {
    ei_bool_t fullscreen = EI_TRUE;
    ei_size_t screen_size = ei_size(1200, 1200);
    ei_color_t	screen_bg_color		= {0xff, 0x11, 0x11, 0xaa};
    game_map_t game_map = {800, 800};

    ei_app_create(screen_size, fullscreen);
    ei_frame_configure(ei_app_root_widget(), NULL, &screen_bg_color, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    create_game_window(&game_map);
    ei_bind(ei_ev_keydown, NULL, "all", process_key, NULL);

    ei_app_run();

    ei_unbind(ei_ev_keydown, NULL, "all", process_key, NULL);
    ei_app_free();

    return EXIT_SUCCESS;
}