#ifndef EI_GEOMETRYMANAGER_EXTENSION
#define EI_GEOMETRYMANAGER_EXTENSION

#include "ei_widget.h"
#include "ei_types.h"
#include "ei_geometrymanager.h"
#include "ei_application.h"
#include "ei_utils.h"
#include "ei_placer.h"

extern ei_geometrymanager_t *geometrymanagers_chain;

typedef struct ei_grid_t
{
    ei_point_t top_left;
    int width;
    int height;
    int n_row;
    int n_line;
    ei_widget_t * widget;
    struct ei_grid_t * next;
    ei_bool_t *occuped;
} ei_grid_t;

typedef struct ei_grid_param_t{
    ei_geometry_param_t geomparam;
    int			x;
    int			y;
    int			*x_end;
    int			*y_end;
    int			width;
    int			height;
    double		rel_width;
    double		rel_height;
} ei_grid_param_t;

/**
 * \brief	Registers the "gridder" geometry manager in the program. This must be called only
 *		once before the \ref ei_grid function can be called.
 */
void 			ei_register_gridder_manager 	(void);

/**
 * \brief	Configures the geometry of a widget using the "GRIDDER" geometry manager.
 *      If the parent widget already have a gridder, it use it. If he havn't,use the parent specified grid.
 *      If no grid are specified, create 3*3 grid.
 * 		If the widget was already managed by another geometry manager, then it is first
 *		removed from the previous geometry manager.
 * 		If the widget was already managed by the "GRIDDER", then this calls simply updates
 *		the placer parameters: arguments that are not NULL replace previous values.
 * 		When the arguments are passed as NULL, the GRIDDER uses default values (detailed in
 *		the argument descriptions below). If no size is provided (either absolute or
 *		relative), then the requested size of the widget is used, i.e. the minimal size
 *		required to display its content.
 */
ei_grid_t *create_grid(ei_widget_t* widget, const int *n_row, const int *n_line);

ei_grid_t * update_grid(ei_widget_t* widget);

extern void			ei_grid			(ei_widget_t*		widget,
                                    int*			    x,
                                    int*			    y,
                                    int*			    x_end,
                                    int*			    y_end,
                                    const int*			width,
                                    const int*			height,
                                    const float*		rel_width,
                                    const float*		rel_height);

extern void 	gridder_runfunc	(struct ei_widget_t *widget);

extern void 	gridder_realasefunc	(struct ei_widget_t *widget);

extern void     free_grid();

#endif