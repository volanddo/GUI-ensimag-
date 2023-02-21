#include <stdio.h>
#include <stdlib.h>

#include "ei_application.h"
#include "ei_event.h"
#include "hw_interface.h"
#include "ei_widget.h"
#include "ei_geometrymanager.h"
#include "ei_gridder.h"
#include "ei_types.h"

static ei_bool_t print_mouse_pos( ei_widget_t *widget, ei_event_t *event, void *user_param)
{
        printf("La souris est en ( %i %i )\n", event->param.mouse.where.x, event->param.mouse.where.y);
}

ei_bool_t process_key(ei_widget_t* widget, ei_event_t* event, void* user_param)
{
        if (event->param.key.key_code == SDLK_ESCAPE) {
                ei_app_quit_request();
                return EI_TRUE;
        }

        return EI_FALSE;
}

ei_widget_t *create_top_level(ei_widget_t * parent, ei_size_t *requested_size, ei_size_t *min_size)
{
        ei_size_t	top_level_size		= {400,400};
        ei_color_t	top_level_color		= {0xdd, 0xdd, 0xdd, 0xff};

        ei_bool_t       top_level_closable = EI_TRUE;
        ei_axis_set_t   top_level_axis = ei_axis_both;
        int top_level_border = 0;
        char *brice = "Briceeeeeeeeeeeeeeee";
        ei_anchor_t text_anchor = ei_anc_southeast;

        ei_widget_t * top_level = ei_widget_create("toplevel", parent, NULL, NULL);
        ei_toplevel_configure(top_level, requested_size, &top_level_color,
                          &top_level_border, &brice, &top_level_closable, &top_level_axis, &min_size);
        return top_level;
}

ei_widget_t *create_button(ei_widget_t *parent, ei_size_t *button_size, ei_color_t *col, char * txt)
{
        ei_relief_t	button_relief		= ei_relief_raised;
        int		button_corner_radius	= 0;
        ei_callback_t   button_callback = &print_mouse_pos;

        ei_widget_t *button = ei_widget_create("button", parent, NULL, NULL);


        ei_button_configure(button, button_size, col,
                        NULL, &button_corner_radius, &button_relief, &txt, NULL, NULL, NULL,
                        NULL, NULL, NULL, &button_callback, NULL);
    return button;
}

ei_widget_t *create_frame(ei_widget_t *parent, ei_size_t	*frame_size)
{
    ei_color_t	frame_color		= {0x08, 0x68, 0x88, 0xff};
    ei_relief_t	frame_relief		= ei_relief_raised;
    int		frame_border_width	= 0;
    char*           frame_text              = "test frame";
    ei_widget_t * frame = ei_widget_create("frame", parent, NULL, NULL);

    ei_frame_configure(frame, frame_size, &frame_color,
                       &frame_border_width, &frame_relief, &frame_text, NULL, NULL, NULL,
                       NULL, NULL, NULL);
    return frame;
}

int main(int argc, char** argv)
{
        ei_size_t	screen_size		= {1000, 1000};
        ei_color_t	root_bgcol		= {0x52, 0x7f, 0xb4, 0xff};

        ei_widget_t*	frame;
        ei_widget_t* button;
        ei_widget_t* top_level;

        /* Create the application and change the color of the background*/
        ei_app_create(screen_size, EI_FALSE);
        ei_frame_configure(ei_app_root_widget(), NULL, &root_bgcol, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

        /* Create, configure and place the frame on screen. */
        int n_row = 2;
        int n_line = 2;
        create_grid(ei_app_root_widget(), &n_row,&n_line);

        ei_color_t col = {100,100,100, 255};

        top_level = create_top_level(ei_app_root_widget(), NULL, NULL);

        int i =0, j = 0;
        int i_end =1, j_end = 0;
        float x_rel = 1.0, y_rel = 1.0;
        ei_grid(top_level, NULL, NULL,NULL,&i_end, NULL, NULL, NULL, NULL);
        i = 1, j = 0;

        n_row = 9;
        n_line= 9;
        create_grid(top_level, &n_row,&n_line);
        // test toute les params a null
        for(i = 0; i < 9; i++)
        {
                for(j = 0; j < 9; j++)
                {
                        ei_color_t color = {i*255/9,j*255/9,128,255};
                        char tst[10];
                        sprintf(tst, "(%d,%d)", i,j);

                        button = create_button(top_level, NULL, &color,tst);

                        ei_grid(button, NULL, NULL,NULL,NULL, NULL, NULL, NULL, NULL);
                }

        }

        ei_size_t size = {100,100};
        frame = create_frame(ei_app_root_widget(), &size);
        n_line = 2;
        n_row = 2;
        create_grid(frame, &n_line, &n_row);
        ei_grid(frame, NULL, NULL,NULL,NULL, NULL, NULL, NULL, NULL);
        for(i = 0; i < n_row; i++)
        {
                for(j = 0; j < n_line; j++) {
                        ei_widget_t *toplevel_2 = create_top_level(frame, NULL, NULL);
                        ei_grid(toplevel_2, &i, &j,NULL,NULL, NULL, NULL, NULL, NULL);
                        create_grid(toplevel_2, NULL, NULL);

                        button = create_button(toplevel_2, NULL, NULL, NULL);
                        int x_end = 3;
                        int y_end = 3;
                        int x = 0, y = 0;
                        ei_grid(button, &x, &y,&x_end,&y_end, NULL, NULL, NULL, NULL);

                }
        }




        ei_bind(ei_ev_keydown,		NULL, "all", process_key, NULL);
        /* Run the application's main loop. */
        ei_app_run();
        ei_unbind(ei_ev_keydown,		NULL, "all", process_key, NULL);
        /* We just exited from the main loop. Terminate the application (cleanup). */
        ei_app_free();

        return (EXIT_SUCCESS);
}