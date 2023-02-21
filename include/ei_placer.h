#ifndef EI_PLACER_H
#define EI_PLACER_H

#include "ei_geometrymanager.h"
#include "ei_widget.h"

/**
 * \brief Defines the structure of the 'placer' parameters.
 * Every widget handled by the geometry manager 'placer' possesses these parameters that can
 * be modified by the placer in the function \ref ei_place (see description of the function for more details about
 * these parameters). These changes are updated in the \ref placer_runfunc.
 */
typedef struct ei_placer_param_t {
    	ei_geometry_param_t geomparam;  ///< a pointer to the geometry manager 'placer'

        /*parameters present in ei_place*/
    	ei_anchor_t 	anchor;         ///< anchor position of the widget
    	int 		x;              ///< absolute x position
    	int 		y;              ///< absolute y position
    	int 		width;          ///< absolute width
    	int 		height;         ///< absolute height
    	float 		rel_x;          ///< relative x position (relative to the parent)
    	float 		rel_y;          ///< same as above but with the y position
    	float 		rel_width;      ///< relative width (relative to the parent)
    	float 		rel_height;     ///< same as above but with the height

        /* booleans used when the runfunc is called without ei_place*/
    	ei_bool_t 	use_requested_size_width; ///< boolean indicating if the widget has to use the requested size width
    	ei_bool_t 	use_requested_size_height; ///< same as above but with the height

} ei_placer_param_t;


/**
 * \brief the \ref ei_geometrymanager_runfunc_t specific to the geometry manager 'placer'.
 * Calls the \ref ei_widgetclass_geomnotifyfunc_t of the @param widget as its geometry changes and its subtree
 * (children, grandchildren...) needs to be updated.
 */
extern void 	placer_runfunc	(struct ei_widget_t *widget);


#endif