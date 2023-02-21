#include <stdio.h>
#include <stdlib.h>

#include "ei_application.h"
#include "ei_event.h"
#include "hw_interface.h"
#include "ei_widget.h"
#include "ei_geometrymanager.h"
#include "ei_types.h"

ei_bool_t process_key(ei_widget_t* widget, ei_event_t* event, void* user_param)
{
    if (event->param.key.key_code == SDLK_ESCAPE) {
        ei_app_quit_request();
        return EI_TRUE;
    }

    return EI_FALSE;
}

static ei_bool_t print_mouse_pos( ei_widget_t *widget, ei_event_t *event, void *user_param)
{
    printf("La souris est en ( %i %i )\n", event->param.mouse.where.x, event->param.mouse.where.y);
}

/*
 * main --
 *
 *	Main function of the application.
 */
int main(int argc, char** argv)
{
    ei_size_t	screen_size		= {600, 600};
    ei_color_t	root_bgcol		= {0x52, 0x7f, 0xb4, 0xff};

    ei_widget_t*	frame;
    ei_widget_t* button;
    ei_widget_t* top_level;

    ei_size_t	frame_size		= {300,200};
    float		frame_rel_x			= 0.8;
    float		frame_rel_y			= 0.2;
    ei_color_t	frame_color		= {0x08, 0x68, 0x88, 0xff};
    ei_relief_t	frame_relief		= ei_relief_raised;
    int		frame_border_width	= 6;
    int             frame_x                 = 10;
    int             frame_y                 = 10;
    char*           frame_text              = "test frame";


    ei_size_t	button_size		= {200,70};
    int		button_x			= -1;
    int		button_y			= -1;
    ei_color_t	button_color		= {0x90, 0xc0, 0xc0, 0xff};
    ei_relief_t	button_relief		= ei_relief_raised;
    int		button_corner_radius	= 7;
    ei_anchor_t 	button_anchor	= ei_anc_southeast;
    float           button_rel_x = 1.;
    float           button_rel_y = 1.;
    ei_callback_t   button_callback = &print_mouse_pos;

    ei_size_t	top_level_size		= {400,400};
    ei_color_t	top_level_color		= {0xff, 0x00, 0x00, 0xaa};
    ei_anchor_t 	top_level_anchor	= ei_anc_northwest;
    float           top_level_rel_width = .8;
    float           top_level_rel_height = .8;
    float           top_level_rel_x = .1;
    float           top_level_rel_y = .1;
    ei_bool_t       top_level_closable = EI_FALSE;
    ei_axis_set_t   top_level_axis = ei_axis_both;
    int top_level_border = 0;
    char *brice = "Briceeeeeeeeeeeeeeee";
    ei_anchor_t text_anchor = ei_anc_southeast;

    /* Create the application and change the color of the background*/
    ei_app_create(screen_size, EI_FALSE);
    ei_frame_configure(ei_app_root_widget(), NULL, &root_bgcol, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    /* Create, configure and place the frame on screen. */

    top_level = ei_widget_create("toplevel", ei_app_root_widget(), NULL, NULL);
    ei_toplevel_configure(top_level, &top_level_size, &top_level_color,
                          &top_level_border, &brice, &top_level_closable, &top_level_axis, NULL);
    button = ei_widget_create("button", top_level, NULL, NULL);
    char *start = "Start";

    ei_button_configure(button, &button_size, &button_color,
                        NULL, &button_corner_radius, &button_relief, &start, NULL, NULL, NULL,
                        NULL, NULL, NULL, &button_callback, NULL);
    frame = ei_widget_create("frame", top_level, NULL, NULL);

    ei_frame_configure(frame, &frame_size, &frame_color,
                       &frame_border_width, &frame_relief, &brice, NULL, NULL, NULL,
                       NULL, NULL, NULL);

    ei_place(top_level, &top_level_anchor, NULL, NULL, NULL, NULL, &top_level_rel_x, &top_level_rel_y, &top_level_rel_width, &top_level_rel_height);
    ei_place(frame, NULL, &frame_x, &frame_y, NULL, NULL, NULL, NULL, &frame_rel_x, &frame_rel_y );
    ei_place(button, &button_anchor, &button_x, &button_y, NULL, NULL,  &button_rel_x, &button_rel_y, NULL, NULL);




    ei_surface_t image = hw_image_load("misc/klimt.jpg", ei_app_root_surface());
    ei_rect_t img_rect = {0,0,20,20};
    ei_rect_t *adr_rect_img = &img_rect;
    ei_size_t size = {100,200};
    ei_color_t red = {100,200,0,255};
    int imx= 300,imy = 100;
    int img_border_width = 10;

    ei_widget_t *img_wdg = ei_widget_create("button", top_level, NULL, NULL);
    ei_button_configure(img_wdg, &size, &red,
                        &img_border_width, NULL, &button_relief, NULL, NULL, NULL, NULL,
                        &image, &adr_rect_img, NULL, &button_callback, NULL);
    ei_place(img_wdg, NULL, &imx, &imy, NULL, NULL, NULL, NULL, NULL, NULL );
    ei_bind(ei_ev_keydown,		NULL, "all", process_key, NULL);
    /* Run the application's main loop. */
    ei_app_run();
    ei_unbind(ei_ev_keydown,		NULL, "all", process_key, NULL);
    /* We just exited from the main loop. Terminate the application (cleanup). */
    ei_app_free();

    return (EXIT_SUCCESS);
}
