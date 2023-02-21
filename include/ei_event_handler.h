#ifndef PROJETC_IG_EI_EVENT_HANDLER_H
#define PROJETC_IG_EI_EVENT_HANDLER_H

#include "ei_widget.h"
#include "ei_event.h"
#include "ei_types.h"


/**
 * \brief Defines the structure of an event handler.
 * Each event handler contains a unique key {widget, tag, event type, callback} that differs from the other event handlers.
 */
typedef struct ei_event_handler_t {
	ei_widget_t *			widget;     ///< pointer to the widget concerned by the event and the callback (can be NULL).
	ei_tag_t 			tag;        ///< tag that defines a class or all the widgets concerned by the event and callback.
	ei_eventtype_t 			eventtype;  ///< the type of the event.
	ei_callback_t 			callback;   ///< the callback called when the event and concerned widget are encountered.
	void*				user_param; ///< the user parameters (can be set by the user).
	struct ei_event_handler_t *	next;       ///< pointer to the next event handler.
} ei_event_handler_t;

/**
 * \brief The (global) table of event handlers. This table is indexed by the type of each events (8 different
 * types of events in total). Each index corresponds to each event type and contains a pointer to a linked list of
 * event handlers that have the same event type.
 */
extern ei_event_handler_t **event_handlers_tab;

/**
 * \brief Function that manages to call all the callbacks related to the given @param current_event on the widget
 * that are concerned by the event.
 * Goes through the linked list of event handlers that corresponds to the current event in the table of events.
 */
extern void 	handle_event	(ei_event_t *current_event);



#endif //PROJETC_IG_EI_EVENT_HANDLER_H
