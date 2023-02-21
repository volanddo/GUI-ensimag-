#ifndef EI_TOPLEVEL_H
#define EI_TOPLEVEL_H

#include "ei_widget.h"
#include "ei_types.h"
#include "ei_event.h"
#include "ei_geometrymanager.h"
#include "ei_placer.h"

/**
 * \brief Defines the structure of every toplevel.
 */
typedef struct ei_toplevel_t {
    ei_widget_t		widget;

    /*parameters related to the appearance */
    ei_color_t		color; 			///< color of the content rectangle of the toplevel.
    int			border_width; 		///< border width of the toplevel. Can be set to 0.
    char*		title; 			///< title of the toplevel.
    ei_rect_t           displayed_rect; 	///< the rectangle used to display the toplevel (can be different from the screen location size rect).

    /*parameters related to the option of the toplevel (closable, resizable...)*/
    ei_bool_t		closable; 		///< boolean indicating if the toplevel is closable (EI_TRUE if yes, else EI_FALSE)
    ei_axis_set_t	resizable; 		///< indicates if the toplevel is resizable and according to what axis.
    ei_size_t		min_size; 		///< minimum size of the toplevel. Defined when the toplevel is closable.
    ei_widget_t*        button; 		///< pointer to the toplevel's quit button. value to NULL if closable is EI_FALSE.

    /*parameters used for maximizing the toplevel's size when the users double clicks on the mouse*/
    double              previous_click_time;
    ei_bool_t           is_maximized; 		///< boolean indicating if the toplevel occupies all its parent's content rect.
    ei_placer_param_t *previous_geom_params; 	///< geometry parameters stocked in case we want to go back to the old size and screen location.
} ei_toplevel_t;

extern int top_border_height;

/**
 * \brief \ref ei_widgetclass_drawfunc_t specific to the toplevel class.
 */
extern  void 			toplevelclass_draw		(struct ei_widget_t *	widget,
                                                                 ei_surface_t 		surface,
                                                                 ei_surface_t 		pick_surface,
                                                                 ei_rect_t *		clipper);

/**
 * \brief \ref ei_widgetclass_allocfunc_t specific to the toplevel class.
 */
extern  struct 	ei_widget_t *	toplevelclass_alloc(void);

/**
 * \brief \ref ei_widgetclass_releasefunc_t specific to the toplevel class.
 *  <ul>
 *			<li> Sets the parameters present in \ref ei_toplevel_configure to their default value (Cf
 *			description of \ref ei_toplevel_configure), </li>
 *			<li> Creates the toplevel's quit button and sets his parameters to their default value
 *			and links the quit button with its specific callback \ref button_quit_callback. </li>
 *  </ul>
 */
extern  void 			toplevelclass_release		(struct ei_widget_t *widget);

/**
 * \brief \ref ei_widgetclass_setdefaultsfunc_t specific to the toplevel class.
 */
extern  void 			toplevelclass_setdefaults	(struct ei_widget_t *widget);

/**
 * \brief  \ref ei_widgetclass_geomnotifyfunc_t specific to the toplevel class.
 *  <ul>
 *			<li> Updates the position of the toplevel's quit button, </li>
 *			<li> Updates the toplevel's content rect, </li>
 *			<li> Adds the widget to a list of widget to update by calling \ref require_update. </li>
 *  </ul>
 */
extern  void 			toplevelclass_geomnotify	(struct ei_widget_t *widget);

/**
 * \brief 	General callback called when a event is related to a toplevel.
 * This callback is in charged of binding other callbacks that handle most of the behaviors of a toplevel,
 * such as : resizing, moving, switching the size to the whole screen size,
 * or half the screen size when sliding the toplevel.
 */
extern  ei_bool_t 	        toplevel_default_callback	(ei_widget_t *	widget,
                                                                 ei_event_t *	event,
                                                                 void *		user_param);

/**
 * \brief 	Callback specific to every toplevel's quit button. Destroys the toplevel when called.
 * @param 	user_param 	Points to the concerned toplevel.
 * @param 	widget 		Points to the toplevel's quit button.
 */
extern  ei_bool_t 	        button_quit_callback		(ei_widget_t* 	widget,
                                                                 ei_event_t* 	event,
                                                                 void* 		user_param);

#endif