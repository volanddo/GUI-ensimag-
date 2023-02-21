#include "ei_application.h"
#include "ei_application_extension.h"
#include "ei_types.h"
#include "ei_utils.h"
#include "ei_widget.h"
#include "ei_geometrymanager.h"
#include "ei_gridder.h"
#include "ei_event_handler.h"
#include "ei_draw_extension.h"
#include "ei_widget_extension.h"
#include <stdio.h>


static ei_widget_t *	root_widget 		= NULL;
static ei_surface_t 	root_window;
static ei_bool_t 	app_quitting 		= EI_FALSE;
int 		        time_between_frame 	= 16;
ei_surface_t 		pick_surface;
ei_event_handler_t **	event_handlers_tab 	= NULL;
ei_widget_t **          widget_array            = NULL;
uint32_t                widget_array_length     = 2;
uint32_t                widget_count            = 0;
ei_linked_rect_t *	rects_update 		= NULL;

void 	ei_app_create	(ei_size_t 	main_window_size,
			 ei_bool_t 	fullscreen)
{
	/* initialization of the hardware */
	hw_init();

	/* initialization of the table of event handlers */
	event_handlers_tab = calloc(ei_ev_last, sizeof(ei_event_handler_t *));

	/* registration of all the classes of widget and the only geometry manager, the 'placer' */
	ei_frame_register_class();
	ei_button_register_class();
	ei_toplevel_register_class();
	ei_register_placer_manager();
        ei_register_gridder_manager();

	/* creation of the root window */
	root_window = hw_create_window(main_window_size, fullscreen);

	/* offscreen for picking */
	pick_surface = hw_surface_create(root_window, hw_surface_get_size(root_window), EI_TRUE);

	/* creation of the root widget (frame) */
	widget_array = calloc(widget_array_length, sizeof (ei_widget_t *));
	root_widget = ei_widget_create("frame", NULL, NULL, NULL);
	root_widget->screen_location = ei_rect(ei_point_zero(), hw_surface_get_size(root_window));
	*root_widget->content_rect = root_widget->screen_location;

	/* event handler used for the display in 60 fps */
	ei_bind(ei_ev_app, NULL, "all", schedule_app_callback, NULL);
}

/**
 * \brief Frees all the linked list of widget classes.
 */
static void 	free_widgetclasses	(void)
{
	ei_widgetclass_t  *current_class = widgetclasses_chain;
	ei_widgetclass_t *widgetclass_to_free;

	while (current_class) {
		widgetclass_to_free     = current_class;
		current_class           = current_class->next;
		free(widgetclass_to_free);
	}
        widgetclasses_chain = NULL;
}

/**
 * \brief Frees all the linked list of geometry managers.
 */
static void 	free_geometry_managers	(void)
{
	ei_geometrymanager_t *current_manager = geometrymanagers_chain;
	ei_geometrymanager_t *manager_to_free;

	while (current_manager) {
		manager_to_free = current_manager;
		current_manager = current_manager->next;
		free(manager_to_free);
	}
        geometrymanagers_chain = NULL;
}

/**
 * Frees all the remaining event handlers in the table of handlers.
 */
static void 	free_tab_events	(void)
{

	for (uint8_t i=0; i<ei_ev_last; i++) {
		ei_event_handler_t *current_handler = event_handlers_tab[i];
		ei_event_handler_t *handler_to_free;
		while (current_handler) {
			handler_to_free = current_handler;
			current_handler = current_handler->next;
			free(handler_to_free);
		}
	}
	free(event_handlers_tab);
        event_handlers_tab = NULL;
}

void 	ei_app_free	(void)
{
	ei_widget_destroy(ei_app_root_widget());

	free_widgetclasses();
	free_geometry_managers();
	free_tab_events();
        free(widget_array);
	hw_surface_free(root_window);
	hw_surface_free(pick_surface);
	hw_quit();
}

/**
 * \brief Frees the linked list of rectangles used for the update.
 */
static void free_update_rects(void)
{

	ei_linked_rect_t *current_rect = rects_update, *rect_to_free;

	while (current_rect) {
		rect_to_free = current_rect;
		current_rect = current_rect->next;
		free(rect_to_free);
	}
	rects_update = NULL;
}

/**
 * TODO (Brice) + commenter un peu dans la fonction stp
 */
static void 	optimize_rects_update	(void)
{
	if (!rects_update || !rects_update->next)
                return;

	ei_rect_t bounding_rect = get_bounding_rect(rects_update);
	int bounding_area       = bounding_rect.size.width * bounding_rect.size.height, inter_area = 0;
	ei_linked_rect_t *current_rect = rects_update;

	int rect_count = 0;
	while (current_rect) {
		inter_area      += current_rect->rect.size.width * current_rect->rect.size.height;
		current_rect    = current_rect->next;
		rect_count++;
	}

	if (inter_area < bounding_area * 10. / rect_count) // A faire en fct du nombre de inv Rects
	    	return;

	current_rect = rects_update;

	while (current_rect) {
		ei_linked_rect_t *temp_rect     = current_rect;
		current_rect                    = current_rect->next;
		free(temp_rect);
	}

	rects_update            = malloc(sizeof(ei_linked_rect_t));
	rects_update->rect      = bounding_rect;
	rects_update->next      = NULL;
}

/**
 * \brief Goes through the whole hierarchy of widgets starting from de root widget and draw those which intercept the clipper.
 * @param clipper : a pointer to a rectangle which defines the widgets to be redrawn
 */
static void 	draw_tree	(ei_rect_t *	clipper)
{
        //TODO (Brice) -> commente les différentes étapes de la fct stp ?
	ei_widget_t *current_widget             = root_widget;
	update_widget_list();
	ei_linked_rect_t *parent_clipper        = malloc(sizeof (ei_linked_rect_t));
	parent_clipper->rect                    = hw_surface_get_rect(ei_app_root_surface());

	if (clipper) {
                parent_clipper->rect = *clipper;
        }
	parent_clipper->next                    = NULL;

	ei_bool_t already_handled               = EI_FALSE;
	do {
		ei_bool_t is_colliding = collision(&current_widget->screen_location, &parent_clipper->rect);
		if ( (!already_handled) && is_colliding ) {
			/* if the widget hasn't been already drawn and intercepts the clipper, it needs to be drawn*/
			current_widget->wclass->drawfunc(current_widget, ei_app_root_surface(), pick_surface, &parent_clipper->rect);
		}

		if ((!already_handled) && is_colliding && current_widget->children_head) {

			ei_linked_rect_t *new_clipper   = malloc(sizeof (ei_linked_rect_t));
			new_clipper->rect               = rect_intersection(*current_widget->content_rect, parent_clipper->rect);
			if (clipper)
                                new_clipper->rect       = rect_intersection(new_clipper->rect, *clipper);
			new_clipper->next               = parent_clipper;
			parent_clipper                  = new_clipper;

			already_handled                 = EI_FALSE;
			current_widget                  = current_widget->children_head;

		} else if (current_widget->next_sibling) {

			already_handled                 = EI_FALSE;
			current_widget                  = current_widget->next_sibling;

		} else if (current_widget->parent) {

			ei_linked_rect_t *temp_rect     = parent_clipper;
			parent_clipper                  = parent_clipper->next;
			free(temp_rect);

			current_widget                  = current_widget->parent;
			already_handled                 = EI_TRUE;
		}

	} while (current_widget != root_widget);

	free(parent_clipper);
}

ei_bool_t schedule_app_callback(ei_widget_t *	widget,
				ei_event_t *	event,
				void *		user_param)
{

	static double draw_tree_time            = 0;
	static int count                        = 0;
	hw_event_schedule_app(time_between_frame, NULL);

	optimize_rects_update();

	hw_surface_lock(ei_app_root_surface());

	int rect_to_mod_number                  = 0;
        ei_linked_rect_t *rect_to_modify        = rects_update;

	while (rect_to_modify) {
		draw_tree(&rect_to_modify->rect);
		rect_to_modify                  = rect_to_modify->next;
		rect_to_mod_number++;
	}

	hw_surface_unlock(ei_app_root_surface());

	if (rects_update)
                hw_surface_update_rects(ei_app_root_surface(), rects_update);
	free_update_rects();
	//fprintf(stderr, "DrawTree: %lfms for %i rects\n", (hw_now()-draw_tree_time)*1000., rect_to_mod_number);
	count++;

	if (count > 10) {
		count = 0;
		//fprintf(stderr, "Frequence : %lfHz\n", 10/(hw_now()-draw_tree_time));
		//fflush(stderr);
		draw_tree_time          = hw_now();
	}
    	return EI_TRUE;
}

void ei_app_run(void)
{
	/* we firstly draw the whole hierarchy of widgets */
	hw_surface_lock(ei_app_root_surface());
	draw_tree(NULL);
	hw_surface_unlock(ei_app_root_surface());
	hw_surface_update_rects(ei_app_root_surface(), NULL);

	hw_event_schedule_app(time_between_frame, NULL);

	while (!app_quitting) {

                /* gets the current event detected*/
		ei_event_t *current_event = malloc(sizeof (ei_event_t));
		hw_event_wait_next(current_event);

                /* manages the callbacks given the current event*/
		handle_event(current_event);

		free(current_event);
	}
}

void 	ei_app_invalidate_rect		(ei_rect_t* input_rect)
{

	ei_rect_t rect = *input_rect;
	rect = rect_intersection(hw_surface_get_rect(ei_app_root_surface()), rect);

	ei_linked_rect_t *current_rect = rects_update;
	if (!rects_update) {
		rects_update                    = malloc(sizeof(ei_linked_rect_t));
		rects_update->rect              = rect;
		rects_update->next              = NULL;
		return;
	}
	while(current_rect->next) {

		if (is_contained_in(current_rect->rect, rect)) {
			current_rect->rect      = rect;
			return;
		} else if (is_contained_in(rect, current_rect->rect)) {
		    	return;
		}
		current_rect                    = current_rect->next;
	}

	if (is_contained_in(current_rect->rect, rect)) {
		current_rect->rect              = rect;
		return;
	} else if (is_contained_in(rect, current_rect->rect)) {
	    	return;
	}

	current_rect->next                      = malloc(sizeof(ei_linked_rect_t));
	current_rect->next->rect                = rect;
	current_rect->next->next                = NULL;

}

void 	ei_app_quit_request	(void)
{
    	app_quitting = EI_TRUE;
}

ei_widget_t* 	ei_app_root_widget	(void) {
    	return root_widget;
}

ei_surface_t 	ei_app_root_surface	(void) {
    	return root_window;
}
