#include "ei_event.h"
#include "ei_types.h"
#include "ei_widget.h"
#include "ei_event_handler.h"

void 	ei_bind (ei_eventtype_t 	eventtype,
		 ei_widget_t *	widget,
		 ei_tag_t 		tag,
		 ei_callback_t 	callback,
		 void *		user_param)
{


	ei_event_handler_t *new_handler = malloc(sizeof(ei_event_handler_t));
	new_handler->callback = callback;
	new_handler->eventtype = eventtype;
	new_handler->user_param = user_param;

	if (widget) {
		new_handler->widget = widget;
		new_handler->tag = NULL;
	}
	else if (tag) {
		new_handler->widget = NULL;
		new_handler->tag = tag;
	}
	new_handler->next = event_handlers_tab[eventtype];
	event_handlers_tab[eventtype] = new_handler;
}

static ei_bool_t same_widget_and_tag(ei_event_handler_t *	handler,
				     ei_widget_t *		widget,
				     ei_tag_t 			tag)
{
	if (handler->tag && tag && strcmp(tag, handler->tag) == 0) {
	    	return EI_TRUE;
	} else if (handler->widget == widget) {
	    	return EI_TRUE;
	}
	return EI_FALSE;
}

void 	ei_unbind (ei_eventtype_t 	eventtype,
		   ei_widget_t *	widget,
		   ei_tag_t 		tag,
		   ei_callback_t 	callback,
		   void *		user_param) {

	ei_event_handler_t sentinel_handler = {NULL, NULL, 0, NULL, NULL, event_handlers_tab[eventtype]};
	ei_event_handler_t *current_handler = &sentinel_handler;

	while ((current_handler->next) &&
	!(  (current_handler->next->eventtype == eventtype &&
		same_widget_and_tag(current_handler->next, widget, tag) &&
		current_handler->next->callback == callback &&
		current_handler->next->user_param == user_param)
		)) {
		current_handler = current_handler->next;
	}
	if (current_handler->next)
        {
		ei_event_handler_t *event_handler_to_free = current_handler->next;
		current_handler->next = event_handler_to_free->next;

                /* free properly*/
                ei_event_handler_t **ptr_handler_to_free = &event_handler_to_free;
		free(*ptr_handler_to_free);
                *ptr_handler_to_free = NULL;
	}
	event_handlers_tab[eventtype] = sentinel_handler.next;
}