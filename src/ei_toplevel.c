#include "ei_toplevel.h"
#include "ei_border.h"
#include "ei_utils.h"
#include "ei_event.h"
#include "ei_draw_extension.h"
#include "ei_geometrymanager.h"
#include "ei_widget_extension.h"
#include "ei_application.h"
#include "ei_application_extension.h"
#include "ei_placer.h"

/* global constants related to the toplevel's events and appearance.*/
int 			top_border_height 	= 32;
static int 		top_corner_radius 	= 12;
static int 		resize_corner_side 	= 15;
static int 		threshold_resize 	= 10;

static ei_color_t 	resize_area_color 	= {0x0, 0x0, 0x0, 0xaa};
static ei_color_t 	back_color 		= {0x19, 0x1D, 0x27, 0xff};
static ei_color_t 	title_color 		= {255, 255, 255, 0xff};

static double 		maximize_delay 		= .2;
static double 		animation_duration 	= .09;

static int 		top_button_radius 	= 8;
static ei_color_t 	top_button_color 	= {0xcc, 0x33, 0x33, 0xff};

static ei_point_t 	previous_mouse_pos;

/**
 * \brief Types of resize zones, used for resizing the toplevel widgets.
 */
typedef enum zone_resize {
	left_border,
	right_border,
	bottom_border,
	top_border,
	corner,
	none
} zone_resize_t;

static zone_resize_t zone_concerned;

/**
 * \brief 	Different zones of the screen defined for the display of toplevel widgets.
 */
typedef enum screen_part {
	right_screen,    ///< represents the first (vertical) half of the screen
	left_screen,     ///< represents the second (vertical) half of the screen
	whole_screen
} screen_part_t;

/**
 * \brief 	Structure holding animation parameters
 */
typedef struct animation
{
	ei_widget_t *           widget; 	///< the widget concerned by the animation
	double                  start_time;     ///< starting time of the animation
	double                  duration;       ///< duration of the animation
	ei_placer_param_t       start;    	///< geometry parameters of the widget at the start of the animation
	ei_placer_param_t       goal;     	///< geometry parameters the widget has at the end of the animation
} animation_t;

/**	TODO BRICE
 * \brief	Event callback. A widget is bound to this callback when it's screen_location needs to be updated before drawing.
 */
static ei_bool_t 	require_redraw		(ei_widget_t *	widget,
						 ei_event_t *	event,
						 void *		user_param)
{
	if (widget != user_param)
	    	return EI_FALSE;

	ei_toplevel_t *toplevel = (ei_toplevel_t *)widget;
	ei_rect_t bounding_rect = rect_union(toplevel->displayed_rect, widget->screen_location);
	ei_app_invalidate_rect(&bounding_rect);

	return EI_FALSE;
}

/**
 * \brief	TODO BRICE
 * @param 	widget
 * @param 	event
 * @param 	user_param
 * @return
 */
static ei_bool_t 	last_redraw	(ei_widget_t *	widget,
					 ei_event_t *	event,
					 void *		user_param)
{
	if (widget == user_param)
	    	return EI_FALSE;

	require_redraw(user_param, event, user_param);
	ei_unbind(ei_ev_app, widget, "all", &last_redraw, user_param);
	return EI_FALSE;
}

/**
 * \brief 	When an animation is in progress this event callback handles the changes to make regarding the
 * geometry parameters of the widget.
 */
static ei_bool_t 	animation_manager	(ei_widget_t *	widget,
					 ei_event_t *	event,
					 void *		user_param)
{
	animation_t *animation_param = (animation_t *)user_param;

	if (animation_param->widget != widget)
	    	return EI_FALSE;

	double state = (hw_now()-animation_param->start_time)/animation_param->duration;

	if (state >= 1.) {

		ei_unbind(ei_ev_app, NULL, "all", &animation_manager, user_param);
		state = 1.;
	}

	ei_placer_param_t placer_param = animation_param->goal;
	ei_placer_param_t start_placer_param = animation_param->start;
	ei_placer_param_t *current_placer_param = (ei_placer_param_t *)animation_param->widget->geom_params;

	current_placer_param->x         = (1.-state)*start_placer_param.x + state*placer_param.x;
	current_placer_param->y         = (1.-state)*start_placer_param.y + state*placer_param.y;
	current_placer_param->width     = (1.-state)*start_placer_param.width + state*placer_param.width;
	current_placer_param->height    = (1.-state)*start_placer_param.height + state*placer_param.height;

	animation_param->widget->geom_params->manager->runfunc(animation_param->widget);

	if (state == 1.) {

		ei_bind(ei_ev_app, widget, "all", &last_redraw, widget);
		free(animation_param);
		schedule_app_callback(widget, event, NULL); // PAS tres elegant
		return EI_TRUE;
	}
	return EI_FALSE;
}

/**
 * \brief Puts the size a @param toplevel widget either to the whole screen, right screen or left screen depending
 * on the given parameter @param screen_part. Replace it's previous geometry manager.
 */
static void 	maximize	(ei_toplevel_t *	toplevel,
				 screen_part_t 		screen_part)
{

	if (toplevel->resizable == ei_axis_none)
	    	return;
	toplevel->previous_geom_params = malloc(sizeof (ei_placer_param_t));


	// Replacing with the placer manager with x, y, width and height parameters set
	ei_rect_t current_screen_loc    = toplevel->widget.screen_location;
	ei_point_t new_topleft          = ei_point_sub(current_screen_loc.top_left, toplevel->widget.parent->screen_location.top_left);
	ei_place((ei_widget_t *) toplevel, NULL, &new_topleft.x, &new_topleft.y,
		 &current_screen_loc.size.width, &current_screen_loc.size.height, NULL, NULL, NULL, NULL);

	*toplevel->previous_geom_params = *((ei_placer_param_t *)toplevel->widget.geom_params); // S'assurer que le manager est placer
	toplevel->is_maximized = EI_TRUE;

	int height = toplevel->widget.parent->screen_location.size.height, width = toplevel->widget.parent->screen_location.size.width;
	int new_x = width;

	switch (screen_part) {
		case right_screen:
			new_x *= .5;
			width *= .5;
			break;
		case left_screen:
			new_x *= 0.;
			width *= .5;
			break;
		case whole_screen:
			new_x *= 0.;
			width *= 1.;
			break;
		default:
			break;
	}

	switch (toplevel->resizable) {
		case ei_axis_x:
			height = toplevel->widget.screen_location.size.height;
			break;
		case ei_axis_y:
			width = toplevel->widget.screen_location.size.width;
			break;
		case ei_axis_none:
			width = toplevel->widget.screen_location.size.width;
			height = toplevel->widget.screen_location.size.height;
		default:
			break;
	}
	ei_place((ei_widget_t *)toplevel, NULL, &new_x, NULL, &width, &height, NULL, NULL, NULL, NULL);

        /* we set the animation parameters of the toplevel */

	animation_t *animation_param    = malloc(sizeof(animation_t));
	animation_param->widget         = (ei_widget_t *)toplevel;

	animation_param->goal   = *((ei_placer_param_t *)toplevel->widget.geom_params);

	animation_param->start  = *toplevel->previous_geom_params;

	animation_param->start_time     = hw_now();
	animation_param->duration       = animation_duration;
	*((ei_placer_param_t *)toplevel->widget.geom_params) = *toplevel->previous_geom_params;
	toplevel->widget.geom_params->manager->runfunc( (ei_widget_t *) toplevel);

	ei_bind(ei_ev_app, NULL, "toplevel", &require_redraw, &toplevel->widget);
	ei_bind(ei_ev_app, NULL, "all", &animation_manager, animation_param);

}

/**
 * \brief 	Called when a @param toplevel has been put on the whole screen, or on the half screen (ie, called
 *              called in \ref maximize). Puts the toplevel size to the size it had before being called by \ref maximize.
 */
static void 	unmaximize	(ei_toplevel_t *toplevel, ei_point_t *where)
{

	toplevel->is_maximized = EI_FALSE;

        /* we set the animation parameters of the toplevel */

	animation_t *animation_param    = malloc(sizeof(animation_t));
	animation_param->widget         = (ei_widget_t *)toplevel;

	animation_param->start = *((ei_placer_param_t *)toplevel->widget.geom_params);

	if (where) {
		float rapport = (where->x - toplevel->widget.screen_location.top_left.x) / (float)toplevel->widget.screen_location.size.width; // Bien set le geom param a l'initialisation
		int new_x = where->x-rapport*toplevel->previous_geom_params->width-toplevel->widget.parent->screen_location.top_left.x;
		int new_y = toplevel->widget.screen_location.top_left.y + (where->y - previous_mouse_pos.y)-toplevel->widget.parent->screen_location.top_left.y;
		ei_place((ei_widget_t *) toplevel, NULL, &new_x, &new_y, &toplevel->previous_geom_params->width, &toplevel->previous_geom_params->height, NULL, NULL, NULL, NULL);
		animation_param->goal = *((ei_placer_param_t *)toplevel->widget.geom_params);
		*((ei_placer_param_t *)toplevel->widget.geom_params) = *toplevel->previous_geom_params;
	} else {
		animation_param->goal = *toplevel->previous_geom_params;
	}

        animation_param->start_time     = hw_now();
        animation_param->duration       = animation_duration;

	ei_bind(ei_ev_app, NULL, "toplevel", &require_redraw, &toplevel->widget);
	ei_bind(ei_ev_app, NULL, "all", &animation_manager, animation_param);
}

/**
 * \brief 	Returns a boolean which indicates if the mouse position is on the left border zone
 * 		related to the resizing of the toplevel.
 * @param 	widget	the toplevel
 * @param 	event	the event (ei_mouse_buttondown) used to access the current position of the mouse.
 */
static ei_bool_t 	is_in_left_border	(ei_widget_t *	widget,
						 ei_event_t *	event	)
{

        ei_point_t current_position             = event->param.mouse.where;
        ei_widget_t *current_widget_picked      = ei_widget_pick(&current_position);
        ei_point_t rel_position_in_toplevel     = ei_point_add(current_position,
                                                           ei_point(threshold_resize, 0));
        if ((ei_widget_pick(&rel_position_in_toplevel)==widget)
            && (!is_point_in_rect(current_position, current_widget_picked->screen_location)))
                return EI_TRUE;
        return EI_FALSE;
}

/**
 * \brief 	Returns a boolean which indicates if the mouse position is on the right border zone
 * 		related to the resizing of the toplevel.
 * @param	widget	the toplevel
 * @param	event	the event (ei_mouse_buttondown) used to access the current position of the mouse.
 */
static ei_bool_t 	is_in_right_border	(ei_widget_t *	widget,
						 ei_event_t *	event)
{
        ei_point_t current_position             = event->param.mouse.where;
        ei_widget_t *current_widget_picked      = ei_widget_pick(&current_position);
        ei_point_t rel_position_in_toplevel     = ei_point_add(current_position,
                                                           ei_point(-threshold_resize, 0));
        if ((ei_widget_pick(&rel_position_in_toplevel)==widget)
            && (!is_point_in_rect(current_position, current_widget_picked->screen_location)))
                return EI_TRUE;
        return EI_FALSE;
}

/**
 * \brief 	Returns a boolean which indicates if the mouse position is on the bottom border zone
 * 		related to the resizing of the toplevel.
 * @param 	widget 	the toplevel
 * @param 	event 	the event (ei_mouse_buttondown) used to access the current position of the mouse.
 */
static ei_bool_t 	is_in_bottom_border	(ei_widget_t *	widget,
						 ei_event_t *	event)
{
        ei_point_t current_position             = event->param.mouse.where;
        ei_widget_t *current_widget_picked      = ei_widget_pick(&current_position);
        ei_point_t rel_position_in_toplevel     = ei_point_add(current_position,
                                                           ei_point(0, -threshold_resize));
        if ((ei_widget_pick(&rel_position_in_toplevel)==widget)
            && (!is_point_in_rect(current_position, current_widget_picked->screen_location)))
                return EI_TRUE;
        return EI_FALSE;
}

/**
 * \brief 	Returns a boolean which indicates if the mouse position is on the top border zone
 * 		related to the resizing of the toplevel.
 * @param 	widget	the toplevel
 * @param 	event 	the event (ei_mouse_buttondown) used to access the current position of the mouse.
 */
static ei_bool_t 	is_in_top_border	(ei_widget_t *	widget,
						 ei_event_t *	event)
{
        ei_point_t current_position             = event->param.mouse.where;
        ei_widget_t *current_widget_picked      = ei_widget_pick(&current_position);
        ei_point_t rel_position_in_toplevel     = ei_point_add(current_position,
                                                           ei_point(0, threshold_resize));
	if ((ei_widget_pick(&rel_position_in_toplevel)==widget)
        && (!is_point_in_rect(current_position, current_widget_picked->screen_location)))
                return EI_TRUE;
    	return EI_FALSE;
}

/**
 * \brief 	Returns a boolean which indicates if the mouse position is on the bottom right corner zone
 * 		related to the resizing of the toplevel.
 * @param 	widget 	the toplevel
 * @param 	event	the event (ei_mouse_buttondown) used to access the position of the mouse.
 */
static ei_bool_t 	is_in_corner_zone	(ei_widget_t *	widget,
						 ei_event_t *	event)
{

        ei_point_t bottom_right                 = ei_point_add(widget->screen_location.top_left,
                                                               ei_size_as_point(widget->screen_location.size));
        ei_rect_t bottom_right_corner_zone      = ei_rect(ei_point_sub(bottom_right,ei_point(resize_corner_side, resize_corner_side)),
                                                          ei_size(resize_corner_side, resize_corner_side));


	return is_point_in_rect(event->param.mouse.where, bottom_right_corner_zone);
}

/**
 * \brief 	Returns the resize zone concerned by the event ei_mouse_buttondown.
 * @param 	widget 	the toplevel
 * @param 	event 	the event (ei_mouse_buttondown) used to access the position of the mouse.
 */
static zone_resize_t 	resize_zone_concerned	(ei_widget_t *	widget,
						ei_event_t *	event)
{
	zone_concerned = none;
	if (is_in_left_border(widget, event))
	    	zone_concerned = left_border;
	if (is_in_right_border(widget, event))
	    	zone_concerned = right_border;
	if (is_in_bottom_border(widget, event))
	    	zone_concerned = bottom_border;
	if (is_in_top_border(widget, event))
	    	zone_concerned = top_border;
	if (is_in_corner_zone(widget, event))
	    	zone_concerned = corner;
	return zone_concerned;
}

/**
 * The callback used to handle the resizing of the toplevel.
 * @param 	widget 		always at NULL.
 * @param 	event 		event corresponding to ei_mouse_buttondown.
 * @param	user_param 	corresponds to the toplevel's widget
 */
static ei_bool_t 	resize_toplevel	(ei_widget_t *	widget,
					 ei_event_t *	event,
					 void *		user_param)
{
	widget = user_param;
	ei_toplevel_t *toplevel = (ei_toplevel_t *)widget;

	ei_point_t restrained_mouse_pos = event->param.mouse.where;
	if (event->param.mouse.where.x < widget->parent->content_rect->top_left.x)
                restrained_mouse_pos.x = widget->parent->content_rect->top_left.x;

	if (event->param.mouse.where.x > widget->parent->content_rect->top_left.x + widget->parent->content_rect->size.width)
                restrained_mouse_pos.x = widget->parent->content_rect->top_left.x + widget->parent->content_rect->size.width;
	if (event->param.mouse.where.y < widget->parent->content_rect->top_left.y)
	    	restrained_mouse_pos.y = widget->parent->content_rect->top_left.y;

	if (event->param.mouse.where.y > widget->parent->content_rect->top_left.y + widget->parent->content_rect->size.height)
	    	restrained_mouse_pos.y = widget->parent->content_rect->top_left.y + widget->parent->content_rect->size.height;

        ei_point_t previous_top_left    = widget->screen_location.top_left;
	ei_size_t size_delta            = ei_size(0, 0);
	ei_size_t mouse_delta           = ei_point_as_size(ei_point_sub(restrained_mouse_pos, previous_mouse_pos));
	ei_axis_set_t axis_resized      = toplevel->resizable;

        /*computes the new location of the widget if the left or top border are concerned and the new size of the widget*/
	switch(zone_concerned) {

		case left_border:
			if ((axis_resized==ei_axis_x) || (axis_resized == ei_axis_both)) {

				size_delta                              = ei_size(-mouse_delta.width, 0);
				widget->screen_location.top_left.x      += mouse_delta.width;
			}
			break;

		case right_border:
			if ((axis_resized==ei_axis_x) || (axis_resized == ei_axis_both))
			    	size_delta = ei_size(mouse_delta.width, 0);
			break;

		case bottom_border:
			if ((axis_resized == ei_axis_y) || (axis_resized == ei_axis_both))
			    	size_delta = ei_size(0, mouse_delta.height);
			break;

		case top_border:
			if ((axis_resized == ei_axis_y) || (axis_resized == ei_axis_both))  {
                                size_delta = ei_size(0, -mouse_delta.height);
                                widget->screen_location.top_left.y += mouse_delta.height;
			}
			break;
		case corner:
			if (axis_resized == ei_axis_both)
			    	size_delta = mouse_delta;
			else if (axis_resized == ei_axis_x)
			    	size_delta = ei_size(mouse_delta.width, 0);
			else if (axis_resized == ei_axis_y)
			    	size_delta = ei_size(0, mouse_delta.height);
			break;

		default:
                        break;
	}

	ei_size_t previous_size         = widget->screen_location.size;
	ei_size_t new_size              = ei_size_add(previous_size, size_delta);

        /* modifies the newfound values if they do not respect the minimum size values */
        if (new_size.width < toplevel->min_size.width) {
                new_size.width = toplevel->min_size.width;
                if (size_delta.width < 0)
                    widget->screen_location.top_left.x = previous_top_left.x;

        } else {
                previous_mouse_pos.x = restrained_mouse_pos.x;
        }
        if (new_size.height < toplevel->min_size.height) {
                new_size.height = toplevel->min_size.height;
                if (size_delta.height < 0)
                        widget->screen_location.top_left.y = previous_top_left.y;
        } else {
                previous_mouse_pos.y = restrained_mouse_pos.y;
        }
        ei_point_t rel_top_left = ei_point_sub(widget->screen_location.top_left, widget->parent->content_rect->top_left);

        /* update of the toplevel's geometry parameters */
        ei_place(widget, NULL, &rel_top_left.x, &rel_top_left.y, &new_size.width, &new_size.height, NULL, NULL, NULL, NULL);

        return EI_TRUE;
}

/**
 * \brief	The callback used to handle the moving of the toplevel, from simple moving to more complex animation
 * 		such as changing the size of the toplevel to the whole screen, right half screen or left half screen.
 * @param 	widget		Always NULL.
 * @param 	event 		Event corresponding to ei_mouse_buttondown.
 * @param 	user_param 	Corresponds to the toplevel's widget
 */
static ei_bool_t 	move_toplevel	(ei_widget_t*	widget,
					 ei_event_t*	event,
					 void *		user_param)
{

	widget = user_param;
	ei_toplevel_t *toplevel = (ei_toplevel_t *) widget;

	ei_point_t restrained_mouse_pos = event->param.mouse.where;
	if (event->param.mouse.where.x < widget->parent->content_rect->top_left.x)
	    	restrained_mouse_pos.x  = widget->parent->content_rect->top_left.x;

	if (event->param.mouse.where.x >
		widget->parent->content_rect->top_left.x + widget->parent->content_rect->size.width)
		restrained_mouse_pos.x = widget->parent->content_rect->top_left.x + widget->parent->content_rect->size.width;

	if (event->param.mouse.where.y < widget->parent->content_rect->top_left.y)
	    	restrained_mouse_pos.y = widget->parent->content_rect->top_left.y;

	if (event->param.mouse.where.y >
		widget->parent->content_rect->top_left.y + widget->parent->content_rect->size.height)
		restrained_mouse_pos.y = widget->parent->content_rect->top_left.y + widget->parent->content_rect->size.height;

	ei_point_t mouse_delta = ei_point_sub(restrained_mouse_pos, previous_mouse_pos);

	/* we update the localisation */
	ei_point_t new_top_left = ei_point_add(widget->screen_location.top_left, mouse_delta);

	if (new_top_left.y + top_border_height >
	    widget->parent->content_rect->top_left.y + widget->parent->content_rect->size.height)
	    	new_top_left.y = widget->parent->content_rect->top_left.y + widget->parent->content_rect->size.height -
		    top_border_height;

	new_top_left = ei_point_sub(new_top_left, widget->parent->content_rect->top_left);

	if (new_top_left.y <= 0)
	    	new_top_left.y = 0;
	else if (toplevel->is_maximized)
	    	unmaximize(toplevel, &event->param.mouse.where);


	ei_place(widget, NULL, &new_top_left.x, &new_top_left.y, &widget->screen_location.size.width,
		 &widget->screen_location.size.height, NULL, NULL, NULL, NULL);
	previous_mouse_pos = restrained_mouse_pos;

	return EI_TRUE;
}

/**
 * \brief 	The callback used to release all the callbacks related to the moving of the toplevel.
 * @param 	widget  	Always NULL.
 * @param 	event 	 	Event corresponding to ei_mouse_buttonup.
 * @param 	user_param  	Corresponds to the toplevel's widget
 */
static ei_bool_t 	toplevel_move_release_callback	(ei_widget_t *	widget,
							 ei_event_t *	event,
							 void *		user_param)
{
	widget = user_param;
	ei_toplevel_t *toplevel = (ei_toplevel_t *)widget;

	if (widget->screen_location.top_left.y == widget->parent->content_rect->top_left.y && !toplevel->is_maximized)
	    	maximize(toplevel, whole_screen);

	if (previous_mouse_pos.x <= widget->parent->content_rect->top_left.x)
	    	maximize(toplevel, left_screen);
	else if (previous_mouse_pos.x >= widget->parent->content_rect->top_left.x + widget->parent->content_rect->size.width-1)
	    	maximize(toplevel, right_screen);

	require_redraw(widget, event, user_param);

	ei_unbind(ei_ev_app, NULL, "toplevel", &require_redraw, user_param);
	ei_unbind(ei_ev_mouse_move, NULL, "all", &move_toplevel, user_param);
	ei_unbind(ei_ev_mouse_buttonup, NULL, "all", &toplevel_move_release_callback, user_param);

	return EI_TRUE;
}

/**
 * \brief 	The callback used to release all the callbacks related to the resizing of the toplevel.
 * @param 	widget 		Always NULL.
 * @param 	event 		Event corresponding to ei_mouse_buttonup.
 * @param 	user_param 	Corresponds to the toplevel's widget.
 */
static ei_bool_t 	toplevel_resize_release_callback	(ei_widget_t *	widget,
								  ei_event_t *	event,
								  void *	user_param)
{
	require_redraw(widget, event, user_param);

	ei_unbind(ei_ev_app, NULL, "toplevel", &require_redraw, user_param);
	ei_unbind(ei_ev_mouse_move, NULL, "all", &resize_toplevel, user_param);
	ei_unbind(ei_ev_mouse_buttonup, NULL, "all", &toplevel_resize_release_callback, user_param);

	return EI_TRUE;
}

/**
 * \brief 	Puts the toplevel on the foreground of the application by changing its place in the hierarchy of the widgets.
 * @param 	widget	the toplevel concerned
 */
static void 	toplevel_foreground	(ei_widget_t *widget)
{
	ei_widget_t *current_widget             = widget->parent->children_head;
	if ((widget->parent->children_head!= widget->parent->children_tail)
        &&(widget != widget->parent->children_tail)) {

                if (widget == widget->parent->children_head)
                        widget->parent->children_head = widget->next_sibling;

                if (current_widget != widget) {
                        while (current_widget->next_sibling != widget) {
                                current_widget = current_widget->next_sibling;
                        }
                        current_widget->next_sibling = current_widget->next_sibling->next_sibling;
                }
                widget->parent->children_tail->next_sibling     = widget;
                widget->parent->children_tail                   = widget;
                widget->next_sibling                            = NULL;
	}
	ei_app_invalidate_rect(&widget->screen_location);
}

/**
 * \brief 	Callback that handles the change of focus of a toplevel when
 * 		a user clicks on a toplevel.
 */
static void 	require_change_focus	(ei_widget_t *	widget)
{
	if (widget->parent) {

		ei_widget_t  *current_widget            = widget;
		ei_widget_t  *toplevel_last_parent      = NULL;
		while (current_widget != ei_app_root_widget()) {
			current_widget = current_widget->parent;
			if (strcmp(current_widget->wclass->name, "toplevel") == 0)
				toplevel_last_parent = current_widget;
		}
		if (toplevel_last_parent)
                        toplevel_foreground(toplevel_last_parent);
	}

	if (strcmp(widget->wclass->name, "toplevel")==0)
                toplevel_foreground(widget);
}


ei_bool_t 	toplevel_default_callback	(ei_widget_t *	widget,
						 ei_event_t *	event,
						 void *		user_param)
{

        ei_toplevel_t *toplevel         = (ei_toplevel_t *)widget;
        ei_widget_t *widget_picked        = ei_widget_pick(&event->param.mouse.where);

        zone_resize_t zone_clicked      = resize_zone_concerned(widget, event);

        /* if we didn't click on the toplevel or on one of its resize zone, we still have to check
        * if we didn't click on a widget in the toplevel and in this case, we'd have to put it on
        * the foreground*/
        if ((widget != widget_picked)&&(zone_clicked == none)) {
                require_change_focus(widget_picked);
                return EI_FALSE;
        }

        require_redraw(widget, event, user_param);

        /* we keep the position of the mouse in memory for future use*/
        previous_mouse_pos = event->param.mouse.where;

        if ((toplevel->resizable != ei_axis_none)&&(zone_clicked != none)) {
                fprintf(stderr, "zone : %i \n", zone_clicked);
                ei_widget_t *widget_involved = widget;

                if (zone_clicked == corner) {
                        fprintf(stderr, "ok\n");
                        widget_involved = widget_picked;
                }
                /*if we clicked on  its resize zone, we're not ON the toplevel but
                 * we still have to put it on the foreground*/

                require_change_focus(widget_involved);
                ei_bind(ei_ev_app, NULL, "toplevel", &require_redraw, widget_involved);
                ei_bind(ei_ev_mouse_move, NULL, "all", &resize_toplevel, widget_involved);
                ei_bind(ei_ev_mouse_buttonup, NULL, "all", &toplevel_resize_release_callback, widget_involved);
                return EI_TRUE;

        } else if (widget == widget_picked) {
                /* we clicked ON the toplevel OR on widgets in its subtree and therefore have to put it on the foreground*/
                require_change_focus(widget);

                if (event->param.mouse.where.y < widget->screen_location.top_left.y + top_border_height) {

                            ei_bind(ei_ev_app, NULL, "toplevel", &require_redraw, widget);
                            ei_bind(ei_ev_mouse_move, NULL, "all", &move_toplevel, widget);
                            ei_bind(ei_ev_mouse_buttonup, NULL, "all", &toplevel_move_release_callback, widget);

                            if (hw_now() - toplevel->previous_click_time < maximize_delay) {

                                        ei_rect_t inv_rect = hw_surface_get_rect(ei_app_root_surface());
                                        ei_app_invalidate_rect(&inv_rect);

                                        if (!toplevel->is_maximized)
                                                maximize(toplevel, whole_screen);
                                        else
                                                unmaximize(toplevel, NULL);
                            }

            toplevel->previous_click_time = hw_now();
            return EI_TRUE;
        }
    }
	return EI_FALSE;
}

void 	toplevelclass_draw	(struct ei_widget_t *	widget,
				 ei_surface_t 		surface,
				 ei_surface_t 		pick_surface,
				 ei_rect_t *		clipper)
{

	ei_toplevel_t *toplevel         = (ei_toplevel_t *) widget;

	toplevel->displayed_rect        = widget->screen_location;

	ei_rect_t content_clipper       = *widget->parent->content_rect;

	if (clipper)
                content_clipper         = rect_intersection(*clipper, content_clipper);

	// Background of the toplevel
        back_color.alpha        = toplevel->color.alpha;
	int corner_radius       = toplevel->is_maximized ? 0 : top_corner_radius;
	draw_half_rounded_frame(surface, widget->screen_location, corner_radius, back_color, &content_clipper);

	// Picking Offscreen
	draw_half_rounded_frame(pick_surface, widget->screen_location, top_corner_radius, *widget->pick_color, &content_clipper);

        // Title of the toplevel
        if(toplevel->title)
                draw_text_toplevel(surface, toplevel, title_color, content_clipper);

        // Content area
        draw_rounded_frame(surface, *toplevel->widget.content_rect, 0, 0, toplevel->color, ei_relief_none, &content_clipper);

        //Quit button to draw
        if(toplevel->closable)
                toplevel->button->wclass->drawfunc(toplevel->button, surface, pick_surface, &content_clipper);

        // Resize corner area to draw
        if (toplevel->resizable && !toplevel->is_maximized) {
                ei_point_t bot_right = ei_point_add(widget->screen_location.top_left,
                                                    ei_size_as_point(widget->screen_location.size));
                ei_rect_t resize_area = ei_rect(ei_point_sub(bot_right, ei_point(resize_corner_side, resize_corner_side)),
                                                ei_size(resize_corner_side, resize_corner_side));

                draw_rounded_frame(surface, resize_area, 0, 0, resize_area_color, ei_relief_none, &content_clipper);
        }

}

struct ei_widget_t *toplevelclass_alloc(void) {
	ei_widget_t *new_toplevel = calloc(1, sizeof(ei_toplevel_t));
	return new_toplevel;
}


void toplevelclass_setdefaults(struct ei_widget_t *widget) {
	ei_toplevel_t *toplevel = (ei_toplevel_t *) widget;

	toplevel->widget.requested_size.width   = 320;
	toplevel->widget.requested_size.height  = 240;

	toplevel->color                         =  ei_default_background_color;

	toplevel->border_width                  = 4;

	toplevel->title                         = malloc(sizeof (char) * 9);
	strcpy(toplevel->title, "Toplevel") ;

	toplevel->closable                      = EI_TRUE;

	toplevel->resizable                     = ei_axis_both;

	toplevel->is_maximized                  = EI_FALSE;

	toplevel->min_size                      = ei_size(160, 120);


	int border_width        = 0;
	int margin              = top_border_height/2;
	int button_x            = margin + widget->screen_location.top_left.x;
	int button_y            = margin + widget->screen_location.top_left.y;

	ei_size_t button_quit_size      = ei_size(2*top_button_radius, 2*top_button_radius);
	ei_anchor_t button_anchor       = ei_anc_center;

	ei_callback_t button_callback   = button_quit_callback;

	toplevel->button        = ei_widget_create("button", NULL, NULL, NULL);
	ei_button_configure(toplevel->button, &button_quit_size, &top_button_color,
			    &border_width, &top_button_radius, NULL, NULL, NULL, NULL,
			    NULL, NULL, NULL, NULL, &button_callback, (void **) &widget);
	ei_place(toplevel->button, &button_anchor, &button_x, &button_y, NULL, NULL, NULL, NULL, NULL, NULL);
	ei_bind(ei_ev_mouse_buttondown, toplevel->button, NULL, button_quit_callback, toplevel);
}


void toplevelclass_release(struct ei_widget_t *widget) {

	ei_toplevel_t *toplevel = (ei_toplevel_t *) widget;

	if (toplevel->closable) {
                ei_widget_destroy(toplevel->button);
                toplevel->button = NULL;
	}

	if (toplevel->title)
	    	free(toplevel->title);

	if (toplevel->previous_geom_params)
		free(toplevel->previous_geom_params);
}


void toplevelclass_geomnotify(struct ei_widget_t *widget) {
	ei_toplevel_t *toplevel = (ei_toplevel_t *)widget;
	int border_width = toplevel->border_width;

        if (toplevel->closable) {
                /* updates the toplevel's quit button geometry*/
                int margin      = top_border_height / 2;
                int button_x    = margin + widget->screen_location.top_left.x;
                int button_y    = margin + widget->screen_location.top_left.y;
                ei_anchor_t button_anchor = ei_anc_center;
                ei_place(toplevel->button, &button_anchor, &button_x, &button_y, NULL, NULL, NULL, NULL, NULL, NULL);
        }
	/* updates the toplevel's content rect*/
	ei_point_t top_left_content     = ei_point_add(widget->screen_location.top_left, ei_point(border_width, top_border_height));
	ei_size_t content_size          = ei_size_sub(widget->screen_location.size, ei_size( 2 * border_width, border_width+top_border_height));
	*widget->content_rect           = ei_rect(top_left_content, content_size);

	/* adds the widget to the list of widget whose subtree needs to be updated */
	require_update(widget);
}

ei_bool_t button_quit_callback(ei_widget_t* widget, ei_event_t* event, void* user_param)
{
	ei_widget_destroy(user_param);
	return EI_TRUE;
}