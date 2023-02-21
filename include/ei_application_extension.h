#ifndef PROJETC_IG_EI_APPLICATION_EXTENSION_H
#define PROJETC_IG_EI_APPLICATION_EXTENSION_H

#include "ei_widget.h"
#include "ei_event.h"

/**
 * \brief (global) Defines the time between each drwing of the whole hierarchy of widgets.
 * Can be modified by the user. Default value set to 16.
 */
extern int 		        time_between_frame;

/**
 * \brief Callback used to manage the frequency of display (number of redrawing per second).
 * The time between each drawing of the whole hierarchy is handled by \ref time_between_frame.
 */
extern  ei_bool_t 		schedule_app_callback	(ei_widget_t *	widget,
							 ei_event_t *	event,
							 void *		user_param);

/**
 * \brief (global) linked list of rectangles whose widgets
 * located inside need to be redrawn.
 */
extern  ei_linked_rect_t *	rects_update;

/**
 *  \brief Offscreen of picking
 */
extern  ei_surface_t 		pick_surface;

#endif //PROJETC_IG_EI_APPLICATION_EXTENSION_H
