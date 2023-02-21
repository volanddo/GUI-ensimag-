#include "ei_types.h"
#include "ei_utils.h"
#include "ei_widget.h"
#include "ei_geometrymanager.h"
#include "ei_placer.h"
#include "ei_application.h"
#include <string.h>

ei_geometrymanager_t *geometrymanagers_chain = NULL;

void ei_geometrymanager_register (ei_geometrymanager_t *geometrymanager)
{
	geometrymanager->next   = geometrymanagers_chain;
	geometrymanagers_chain  = geometrymanager;
}

ei_geometrymanager_t *ei_geometrymanager_from_name (ei_geometrymanager_name_t name)
{
	ei_geometrymanager_t *current_manager = geometrymanagers_chain;
	while (current_manager && strcmp(current_manager->name, name) != 0) {
	    	current_manager = current_manager->next;
	}
	return current_manager;
}

void ei_geometrymanager_unmap(ei_widget_t *widget)
{
	ei_app_invalidate_rect(&(widget->screen_location));
	if (widget->geom_params->manager->releasefunc)
	    	widget->geom_params->manager->releasefunc(widget);
	free(widget->geom_params);
	widget->geom_params                     = NULL;
	widget->screen_location.size            = ei_size_zero();
	widget->screen_location.top_left        = ei_point_zero();
}

void ei_register_placer_manager(void)
{
	ei_geometrymanager_t *placer    = malloc(sizeof(ei_geometrymanager_t));
	strcpy(placer->name, "placer");
	placer->runfunc                 = &placer_runfunc;
	placer->releasefunc             = NULL;
	ei_geometrymanager_register(placer);
}

void ei_place(ei_widget_t *widget,
			  ei_anchor_t *anchor,
			  int *x,
			  int *y,
			  int *width,
			  int *height,
			  float *rel_x,
			  float *rel_y,
			  float *rel_width,
			  float *rel_height)
{
	ei_placer_param_t *param_widget                 = NULL;
	ei_geometrymanager_t *current_widget_manager    = NULL;

	if (widget->geom_params)
	    	current_widget_manager  = widget->geom_params->manager;
	ei_geometrymanager_t *placer    = ei_geometrymanager_from_name("placer");


	if (!current_widget_manager) {
		/* if the widget isn't handled by any geometry manager */
	    	param_widget = malloc(sizeof(ei_placer_param_t));

	} else if (current_widget_manager != placer) {
		/* if the widget is handled by another geometry manager */
		widget->geom_params->manager->releasefunc(widget);
                free(widget->geom_params);
		param_widget = malloc(sizeof(ei_placer_param_t));

	} else { /* if the widget is handled by the geometry manager 'placer' */
	    	param_widget = (ei_placer_param_t *) widget->geom_params;
	}

	param_widget->geomparam.manager = placer;
	param_widget->anchor = (anchor) ? *anchor : ei_anc_northwest;

	param_widget->x = x ? *x : 0;
	param_widget->y = y ? *y : 0;

	param_widget->width     = width ? *width : (rel_width ? 0 : widget->requested_size.width);
	param_widget->height    = height ? *height : (rel_height ? 0 : widget->requested_size.height);
	param_widget->rel_x     = rel_x ? *rel_x : 0.0;
	param_widget->rel_y     = rel_y ? *rel_y : 0.0;

	param_widget->rel_width  = rel_width ? *rel_width : 0.0;
	param_widget->rel_height = rel_height ? *rel_height : 0.0;

	param_widget->use_requested_size_width  = !width ? (!rel_width ? EI_TRUE : EI_FALSE) : EI_FALSE;
        param_widget->use_requested_size_height = !height ? (!rel_height ? EI_TRUE : EI_FALSE) : EI_FALSE;

	widget->geom_params = (ei_geometry_param_t *) param_widget;

	ei_app_invalidate_rect(&widget->screen_location);
	widget->geom_params->manager->runfunc(widget);
	ei_app_invalidate_rect(&widget->screen_location);
}