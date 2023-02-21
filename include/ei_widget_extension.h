#ifndef PROJETC_IG_EI_WIDGET_EXTENSION_H
#define PROJETC_IG_EI_WIDGET_EXTENSION_H

#include "ei_widgetclass.h"
#include "ei_widget.h"

/**
 * Defines the structure of a linked widget : a pointer to a widget + a pointer to
 * a list of widgets.
 */
typedef struct ei_linked_widget {

	ei_widget_t *widget;
	struct ei_linked_widget *next;

} ei_linked_widget_t;

/**
 * \brief (global) linked list of all the widgets in the hierarchy.
 */
extern          ei_widget_t**           widget_array;
extern          uint32_t                widget_array_length;
extern          uint32_t                widget_count;
/**
 * \brief (global) linked list of all the widget classes registered in the library.
 */
extern          ei_widgetclass_t*       widgetclasses_chain;

/**
 * \brief Adds the given @param widget to the (local) linked list of widgets
 * whose subtree needs to be updated. This function is called when the @param widget
 * had its geometry changed, ie by a \ref ei_widgetclass_geomnotifyfunc_t function.
 */
extern  void 	require_update		(ei_widget_t *widget);

/**
 * \brief Given a linked list of widgets whose subtree needs to be updated, this function
 * will call every widget in that linked list and update the geometry of all its
 * subtree (children, grandchildren, etc...).
 */
extern  void 	update_widget_list	(void);

/**
 * \brief Frees a given @param widget from the list of widgets whose subtree
 * needs an update. Called by \ref free_widget (when a widget needs to be destroyed).
 */
extern  void    free_widget_from_update  (ei_widget_t *widget);

#endif //PROJETC_IG_EI_WIDGET_EXTENSION_H
