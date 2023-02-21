#include "ei_widget.h"
#include "ei_event.h"
#include "ei_event_handler.h"
#include "ei_application.h"
#include "ei_toplevel.c"


/**
 * \brief This function is in charge of calling every widget concerned by the currents event and callback.
 * @param handler : a pointer to the current event handler containing an event type, a widget or a tag, a callback and
 * an eventual user parameter.
 * @param current_event : the current event detected in \ref ei_app_run.
 *
 *  <ul>
 *			<li> Test if the event handler is concerned by a specific widget. If the event type is
 *			a mouse event (buttondown, buttonup, mouse_move) we call the callback on the widget if it's the
 *			one the mouse on. If it's another event type, we call directly the callback on the concerned widget.
 *			</li>
 *
 *			<li> Creates the toplevel's quit button and sets his parameters to their default value
 *			and links the quit button with its specific callback \ref button_quit_callback.
 *			</li>
 *  </ul>
 */
static ei_bool_t  call_eventual_handler (ei_event_handler_t *	handler,
					 ei_event_t *		current_event)
{

	if (handler->widget) {
                if      ((current_event->type   == ei_ev_mouse_buttonup ||
                        current_event->type     == ei_ev_mouse_buttondown ||
                        current_event->type     == ei_ev_mouse_move)) {

                        if (handler->widget == ei_widget_pick(&current_event->param.mouse.where)) {
                                ei_app_invalidate_rect(&handler->widget->screen_location);
                                return handler->callback(handler->widget, current_event, handler->user_param);
		        }
                        return EI_FALSE;
                }
                ei_app_invalidate_rect(&handler->widget->screen_location);
		return handler->callback(handler->widget, current_event, handler->user_param);
	}

	ei_bool_t handle_all    = strcmp(handler->tag, "all") == 0 ? EI_TRUE : EI_FALSE;

	for (ei_widget_t **current_widget = widget_array; current_widget < widget_array+widget_array_length; current_widget++) {
		if (!*current_widget)
			continue;
		if ((handle_all || strcmp((*current_widget)->wclass->name, handler->tag) == 0)&&
		handler->callback(*current_widget, current_event, handler->user_param))
			return EI_TRUE;
	}
	return EI_FALSE;
}

void handle_event (ei_event_t *	current_event)
{

	ei_bool_t event_handler_quitting                = EI_FALSE;
	ei_event_handler_t *event_handlers_chain        = event_handlers_tab[current_event->type];
	ei_event_handler_t *current_handler             = event_handlers_chain;

	while (current_handler && (!event_handler_quitting)) {
		ei_event_handler_t *temp_handler        = current_handler;
		current_handler                         = current_handler->next;
		event_handler_quitting                  = call_eventual_handler(temp_handler, current_event);
	}
}