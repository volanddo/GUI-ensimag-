#include "ei_frame.h"
#include "ei_widget.h"
#include "ei_widget_extension.h"
#include "ei_types.h"
#include "ei_utils.h"
#include "ei_draw.h"
#include "ei_geometrymanager.h"
#include "ei_border.h"
#include "ei_draw_extension.h"


/*Code les fonctions nécessaires à la classe de widgets 'FRAME'*/


void frameclass_draw (struct ei_widget_t *widget,
		      ei_surface_t surface,
		      ei_surface_t pick_surface,
		      ei_rect_t *clipper)
{
	ei_frame_t *frame               = (ei_frame_t *)widget;
	ei_rect_t *content_clipper      = malloc(sizeof (ei_rect_t));

	if (widget->parent) {

		if (clipper)
                        *content_clipper = rect_intersection(*clipper, *widget->parent->content_rect);
                else
                        *content_clipper = *widget->parent->content_rect;

	} else {

		if (clipper)
                        *content_clipper        = *clipper;
                else
                        content_clipper         = NULL;
	}

	// Visible screen
	draw_rounded_frame(surface,
			   widget->screen_location,
			   frame->border_width,
			   0,
			   frame->color,
			   frame->relief,
			   content_clipper);

	if (frame->img)
	   	draw_frame_image(surface, frame);


        if (frame->text)
                draw_frame_text(surface, frame, content_clipper);

        // Picking Offscreen
        draw_rounded_frame(pick_surface,
                       widget->screen_location,
                       0,
                       0,
                       *widget->pick_color,
                       ei_relief_none,
                       content_clipper);

        free(content_clipper);
}

ei_widget_t *frameclass_alloc (void)
{
	ei_widget_t *new_frame = calloc(1, sizeof(ei_frame_t));
	return new_frame;
}

void frameclass_setdefaults (ei_widget_t *widget)
{
	ei_frame_t *frame               = (ei_frame_t *)widget;

	frame->widget.requested_size    = ei_size_zero();

	frame->color                    = ei_default_background_color;

	frame->border_width             = 0;

	frame->relief                   = ei_relief_none;

	frame->text                     = NULL;

	frame->text_font                = ei_default_font;

	frame->text_color               = ei_font_default_color;
	frame-> text_anchor             = ei_anc_center;

	frame-> img                     = NULL;
	frame->img_rect                 = NULL;

	frame->img_anchor               = ei_anc_center;
}

void frameclass_release (ei_widget_t *widget) {

	ei_frame_t *frame = (ei_frame_t *)widget;

	if (frame->text)
                free(frame->text);

	if (frame->img_rect)
                free(frame->img_rect);

	if (frame->img)
                hw_surface_free(frame->img);
}

void frameclass_geomnotify (ei_widget_t *widget)
{
    	int border_width = ((ei_frame_t *)widget)->border_width;
	ei_point_t top_left_content = ei_point_add(widget->screen_location.top_left, ei_point(border_width, border_width));

	if (widget->screen_location.size.width > border_width*2 && widget->screen_location.size.height > border_width*2) {
		ei_size_t content_size  = ei_size_sub(widget->screen_location.size, ei_size(2 * border_width, 2 * border_width));
		*widget->content_rect   = ei_rect(top_left_content, content_size);
	} else {
	    	*widget->content_rect   = ei_rect(top_left_content, ei_size(0,0));
	}

	require_update(widget);
}
