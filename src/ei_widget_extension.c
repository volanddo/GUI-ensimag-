#include "ei_widget.h"
#include "ei_widget_extension.h"
#include "ei_geometrymanager.h"
#include "string.h"

static ei_bool_t 		updating 		= EI_FALSE;
static ei_linked_widget_t *	widgets_to_update 	= NULL;

void    free_widget_from_update(ei_widget_t *widget)
{
        ei_linked_widget_t sentinel_linked_widget       = {NULL, widgets_to_update};
        ei_linked_widget_t *current_linked_widget       = &sentinel_linked_widget;

        while ((current_linked_widget->next) &&
        current_linked_widget->next->widget != widget) {
                current_linked_widget                   = current_linked_widget->next;
        }
        if (current_linked_widget->next) {
                ei_linked_widget_t *linked_widget_to_free       = current_linked_widget->next;
                current_linked_widget->next                     = current_linked_widget->next->next;
                free(linked_widget_to_free);
        }
        widgets_to_update = sentinel_linked_widget.next;
}

void 	require_update	(ei_widget_t *widget)
{
	/* we shall not modify the list of widgets to update when it is being updated */
	if (updating)
	    	return;
	ei_linked_widget_t *current_widget = widgets_to_update;

	/* if the list of widgets to update is empty */
	if (!current_widget) {
		widgets_to_update               = malloc(sizeof (ei_linked_widget_t));
		widgets_to_update->widget       = widget;
		widgets_to_update->next         = NULL;
		return;
	}
	/* else, we add the widget at the end of the list of widgets to update */
	while (current_widget->next) {
		if (widget == current_widget->widget)
			/* if the widget already exists in the list, there's no need adding it*/
			return;
		current_widget = current_widget->next;
	}
        current_widget->next            = malloc(sizeof (ei_linked_widget_t));
        current_widget->next->widget    = widget;
        current_widget->next->next      = NULL;
}

/**
 * \brief Updates recursively the geometry of all the widgets in the subtree of the given
 * parameter @param widget.
 */
static void 	update_children_rec	(ei_widget_t *widget)
{

	if (!widget->children_head)
	    	return;

	ei_widget_t *current_child = widget->children_head;
	while (current_child) {
		if (current_child->geom_params) {
		    	current_child->geom_params->manager->runfunc(current_child);
		}
		update_children_rec(current_child);
		current_child = current_child->next_sibling;
	}
}

void 	update_widget_list	(void)
{
	updating = EI_TRUE;
	ei_linked_widget_t *current_widget = widgets_to_update;
	while (current_widget) {
		ei_linked_widget_t *widget_to_free      = current_widget;
		update_children_rec(current_widget->widget);
		current_widget                          = current_widget->next;

		free(widget_to_free);
	}
	widgets_to_update = NULL;
	updating = EI_FALSE;
}