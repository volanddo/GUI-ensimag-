#include "ei_types.h"
#include "ei_utils.h"
#include "ei_widget.h"
#include "ei_placer.h"
#include "ei_application.h"

/**
 * \brief Computes and returns the new anchor position given the @param size of a widget
 * and its @param anchor parameter.
 */
static ei_point_t get_anchor_pos (const ei_size_t 	size,
				  const ei_anchor_t 	anchor)
{

        ei_point_t anchor_point = ei_point_zero();

        switch (anchor)
        {
        case ei_anc_north:
                anchor_point = ei_point(size.width / 2, 0);
                break;
        case ei_anc_northwest:
                anchor_point = ei_point(0, 0);
                break;
        case ei_anc_northeast:
                anchor_point = ei_point(size.width, 0);
                break;
        case ei_anc_south:
                anchor_point = ei_point(size.width / 2, size.height);
                break;
        case ei_anc_southwest:
                anchor_point = ei_point(0, size.height);
                break;
        case ei_anc_southeast:
                anchor_point = ei_point(size.width, size.height);
                break;
        case ei_anc_center:
                anchor_point = ei_point(size.width / 2, size.height / 2);
                break;
        case ei_anc_east:
                anchor_point = ei_point(size.width, size.height / 2);
                break;
        case ei_anc_west:
                anchor_point = ei_point(0, size.height / 2);
                break;
        default:
                break;
        }
        return anchor_point;
}

void placer_runfunc (struct ei_widget_t *widget)
{

	ei_placer_param_t *placer_param = (ei_placer_param_t *) widget->geom_params;

	ei_size_t parent_screen_size = ei_size_zero();
	if (widget->parent)
	    	parent_screen_size = widget->parent->content_rect->size;

	/* we compute the new size given the relatives positions of the geometry parameters */
	ei_size_t w_size        = ei_size(parent_screen_size.width * placer_param->rel_width, parent_screen_size.height * placer_param->rel_height);
	w_size                  = ei_size_add(w_size, ei_size(placer_param->width, placer_param->height));

        /* we force the size to the requested size if the last call to ei_place says so
         * (this is used for example if we define the requested size after the call to ei_place) */
	if (placer_param->use_requested_size_width)
		w_size.width = widget->requested_size.width;
	if (placer_param->use_requested_size_height)
		w_size.height = widget->requested_size.height;

	ei_point_t anchor_widget_pos    = get_anchor_pos(w_size, placer_param->anchor);

	ei_point_t w_top_left_pos       = ei_point(parent_screen_size.width * placer_param->rel_x, parent_screen_size.height * placer_param->rel_y);
	w_top_left_pos                  = ei_point_add(w_top_left_pos, ei_point(placer_param->x, placer_param->y));
	w_top_left_pos                  = ei_point_sub(w_top_left_pos, anchor_widget_pos);

	if (widget->parent)
	    	w_top_left_pos = ei_point_add(w_top_left_pos, widget->parent->content_rect->top_left);

	widget->screen_location = ei_rect(w_top_left_pos, w_size);
	widget->wclass->geomnotifyfunc(widget);
}

