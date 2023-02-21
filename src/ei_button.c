#include "ei_button.h"
#include "ei_widget.h"
#include "ei_types.h"
#include "ei_border.h"
#include "ei_frame.h"
#include "ei_event.h"
#include "ei_application.h"
#include "ei_draw_extension.h"



static ei_bool_t is_button_pushed(ei_widget_t *widget, ei_event_t *event, void *user_param)
{

        ei_widget_t *button_pressed     = (ei_widget_t *) user_param;
        ei_frame_t *frame               = (ei_frame_t *) button_pressed;

        if (frame->relief == ei_relief_none) {
                return EI_TRUE;
        } else {
                /* if the mouse lies ON the quit button, it needs to be sunken */
                if (ei_widget_pick(&event->param.mouse.where) == (ei_widget_t *) button_pressed)
                        frame->relief = ei_relief_sunken;

                else /* else it is raised */
                        frame->relief = ei_relief_raised;
                ei_app_invalidate_rect(&button_pressed->screen_location);
        }

        return EI_TRUE;
}

static ei_bool_t button_release_callback(ei_widget_t *widget, ei_event_t *event, void *user_param)
{

        ei_unbind(ei_ev_mouse_move, NULL, "all", is_button_pushed, user_param);
        ei_unbind(ei_ev_mouse_buttonup, NULL, "all", button_release_callback, user_param);

        ei_widget_t *button_pressed     = (ei_widget_t *) user_param;

        ei_frame_t *frame               = (ei_frame_t *) button_pressed;

        if (frame->relief != ei_relief_none) {
                frame->relief   = ei_relief_raised;
                ei_app_invalidate_rect(&button_pressed->screen_location);
        }

        if (ei_widget_pick(&event->param.mouse.where) != button_pressed)
                return EI_FALSE;

        ei_button_t *button     = (ei_button_t *) button_pressed;

        return button->callback(widget, event, button->user_param);
}

ei_bool_t button_default_callback(ei_widget_t *widget, ei_event_t *event, void *user_param)
{

        if (ei_widget_pick(&event->param.mouse.where) != widget)
                return EI_FALSE;


        ei_frame_t *frame       = (ei_frame_t *)widget;

        if (frame->relief != ei_relief_none) {
                frame->relief = ei_relief_sunken;
                ei_app_invalidate_rect(&widget->screen_location);
        }

        ei_bind(ei_ev_mouse_move, NULL, "all", is_button_pushed, widget);
        ei_bind(ei_ev_mouse_buttonup, NULL, "all", button_release_callback, widget);

        return EI_TRUE;
}

void buttonclass_draw(struct ei_widget_t *widget,
                         ei_surface_t surface,
                         ei_surface_t pick_surface,
                         ei_rect_t *clipper)
{

        ei_rect_t *content_clipper = malloc(sizeof (ei_rect_t));

        if (widget->parent) {

                if (clipper)
                        *content_clipper        = rect_intersection(*clipper, *widget->parent->content_rect);
                else
                        *content_clipper        = *widget->parent->content_rect;

        } else {

                if (clipper)
                        *content_clipper        = *clipper;
                else
                        content_clipper         = NULL;
        }

        ei_frame_t *button_frame        = (ei_frame_t *)widget;
        ei_button_t *button             = (ei_button_t *)widget;

        draw_rounded_frame(surface,
                     widget->screen_location,
                       button_frame->border_width,
                     button->corner_radius,
                       button_frame->color,
                       button_frame->relief,
                       content_clipper);


        if (button_frame->img)
                draw_frame_image(surface, button_frame);

        if (button->frame.text)
                draw_frame_text(surface, button_frame, content_clipper);

        draw_rounded_frame(pick_surface,
                       widget->screen_location,
                       0,
                       button->corner_radius,
                       *widget->pick_color,
                       ei_relief_none,
                       content_clipper);

        free(content_clipper);
}


ei_widget_t *buttonclass_alloc(void) {
        ei_widget_t *new_button = calloc(1, sizeof(ei_button_t));
        return new_button;
}

void buttonclass_setdefaults(ei_widget_t *widget) {

        frameclass_setdefaults(widget);

        ei_frame_t *button_frame        = (ei_frame_t *)widget;
        ei_button_t *button             = (ei_button_t *)widget;

        button_frame->border_width      = k_default_button_border_width;

        button_frame->relief            = ei_relief_raised;

        button->corner_radius           = k_default_button_corner_radius;

        button->callback                = NULL;

        button->user_param              = NULL;
}