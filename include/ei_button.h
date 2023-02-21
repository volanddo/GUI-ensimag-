#ifndef EI_BUTTON_H
#define EI_BUTTON_H

#include "ei_widget.h"
#include "ei_frame.h"
#include "ei_button.h"
#include "ei_event.h"

/**
 * \brief Defines the structure of every button.
 */
typedef struct ei_button_t {
    ei_frame_t 	        frame;         ///< Common attributes with the class of widget 'frame' : polymorphism

    /* parameters specific to the button class */
    int		        corner_radius; ///< Radius of the button used for rounded shapes.
    ei_callback_t	callback;      ///< A callback specific to each button, called when it is pressed. Is defined by the user.
    void * 		user_param;    ///< Parameters passed to the callback of the button (see above).

} ei_button_t;

/**
 * \brief \ref ei_widgetclass_drawfunc_t specific to the button class.
 */
extern void 			buttonclass_draw		(struct ei_widget_t *	widget,
							        ei_surface_t 		surface,
							        ei_surface_t 		pick_surface,
							        ei_rect_t *		clipper);

/**
 * \brief \ref ei_widgetclass_allocfunc_t specific to the button class.
 */
extern struct ei_widget_t *	buttonclass_alloc		(void);

/**
 * \brief \ref ei_widgetclass_setdefaultsfunc_t specific to the button class.
 *  <ul>
 *			<li> Some of the common attributes of the widgets button and frame are set to their default value
 *			by calling \ref ei_frame_configure.
 *			Each default value is specified in the definition of \ref ei_button_configure. </li>
 *			<li> Each button isn't related to any callback by default. </li>
 *  </ul>
 */
extern void 			buttonclass_setdefaults		(struct ei_widget_t *widget);

/**
 * \brief General callback called when a event is related to a button.
 * Other callbacks are used inside this function to handle the different behaviors
 * of the button, such as : pressing the button, releasing the button.
 */
extern ei_bool_t 	        button_default_callback		(ei_widget_t *	widget,
							        ei_event_t *	event,
							        void *		user_param);

#endif