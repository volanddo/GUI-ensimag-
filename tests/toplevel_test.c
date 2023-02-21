//
// Created by helen on 12/05/22.
//
#include <stdlib.h>
#include "ei_application.h"
#include "ei_widget.h"
#include "ei_utils.h"
#include "ei_event.h"
#include "ei_geometrymanager.h"

ei_bool_t quit_key(ei_widget_t* widget, ei_event_t* event, void* user_param)
{
    if (event->param.key.key_code == SDLK_ESCAPE) {
        ei_app_quit_request();
        return EI_TRUE;
    }
    return EI_FALSE;
}
ei_bool_t process_key(ei_widget_t* widget, ei_event_t* event, void* user_param)
{
        if (event->param.key.key_code ==SDLK_a) {
                ei_widget_destroy(widget);
                return EI_TRUE;
        }

        return EI_FALSE;
}
void create_window(void) {
    /*big toplevel widgets */
    ei_widget_t *top_level1;
    ei_widget_t  *top_level2;
    /* little toplevel widgets */
    ei_widget_t *little_top_level;
    ei_widget_t  *little_top_level2;
    ei_size_t little_top_level_size = {50,150};
    ei_size_t *min_size_little_toplevel = &little_top_level_size;
    ei_color_t	top_level_color		= {0xdd, 0xdd, 0xdd, 0xff};
    ei_anchor_t 	top_level_anchor	= ei_anc_northwest;
    float           top_level_rel_width = .8;
    float           top_level_rel_height = .8;
    int toplevel_border_width = 6;
    float           top_level_rel_x = .1;
    float           top_level_rel_y = .1;
    int             top_level2_x = 20;
    int             top_level2_y = 800;
    ei_size_t	    top_level2_size = {50,200};
    ei_size_t *min_size_toplevel = &top_level2_size;
    ei_bool_t       top_level_closable = EI_TRUE;
    ei_axis_set_t   top_level_axis = ei_axis_both;
    char *title_toplevel = "Jeu";
    ei_bool_t       top_level_closable2 = EI_FALSE;
    ei_axis_set_t   top_level_resizable2 = ei_axis_y;

    /* Create, configure and place the frame on screen. */

    top_level1 = ei_widget_create("toplevel", ei_app_root_widget(), NULL, NULL);
    /*petites toplevels*/
    little_top_level = ei_widget_create("toplevel", top_level1, NULL, NULL);
    little_top_level2 = ei_widget_create("toplevel", little_top_level, NULL, NULL);
    ei_toplevel_configure(top_level1, min_size_toplevel, &top_level_color,
                          &toplevel_border_width, &title_toplevel, &top_level_closable, &top_level_resizable2, &min_size_toplevel);
    ei_toplevel_configure(little_top_level, &little_top_level_size, NULL,
                          &toplevel_border_width, &title_toplevel, &top_level_closable, &top_level_axis, &min_size_little_toplevel);
    ei_toplevel_configure(little_top_level2, &little_top_level_size, &top_level_color,
                          &toplevel_border_width, &title_toplevel, &top_level_closable2, &top_level_axis, &min_size_little_toplevel);
    ei_place(top_level1, &top_level_anchor, NULL, NULL, NULL, NULL, &top_level_rel_x, &top_level_rel_y, &top_level_rel_width, &top_level_rel_height);
    ei_place(little_top_level, &top_level_anchor, NULL, NULL, NULL, NULL, &top_level_rel_x, &top_level_rel_y, &top_level_rel_width, &top_level_rel_height);
    ei_place(little_top_level2, &top_level_anchor, NULL, NULL, NULL, NULL, &top_level_rel_x, &top_level_rel_y, &top_level_rel_width, &top_level_rel_height);

    top_level2 = ei_widget_create("toplevel", ei_app_root_widget(), NULL, NULL);
    ei_toplevel_configure(top_level2, min_size_toplevel, &top_level_color,
                          &toplevel_border_width, &title_toplevel, &top_level_closable, &top_level_axis, &min_size_toplevel);
    ei_place(top_level2,  &top_level_anchor, &top_level2_x, &top_level2_y, NULL, NULL, NULL, NULL, &top_level_rel_width, &top_level_rel_height);

    ei_bind(ei_ev_keydown, little_top_level, NULL, process_key, NULL);

}


int main(void) {
    ei_bool_t fullscreen = EI_FALSE;
    ei_size_t screen_size = ei_size(1200, 1000);
    ei_color_t	screen_bg_color		= {0xaa, 0x11, 0x11, 0xff};

    ei_app_create(screen_size, fullscreen);
    ei_frame_configure(ei_app_root_widget(), NULL, &screen_bg_color, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    create_window();

    ei_bind(ei_ev_keydown, NULL, "all", quit_key, NULL);

    ei_app_run();

    ei_unbind(ei_ev_keydown, NULL, "all", quit_key, NULL);
    ei_app_free();

    return EXIT_SUCCESS;
}