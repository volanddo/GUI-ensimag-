#include "ei_widgetclass.h"
#include "ei_widget.h"
#include "ei_frame.h"
#include "ei_button.h"
#include "ei_toplevel.h"
#include "ei_widget_extension.h"
#include "ei_event_handler.h"
#include "hw_interface.h"
#include "ei_draw_extension.h"
#include "ei_utils.h"
#include "ei_application.h"
#include "ei_application_extension.h"
#include "ei_gridder.h"



static uint32_t add_to_widget_array (ei_widget_t *widget)
{
	if (widget_array_length == widget_count)
	{
		//widget_array = realloc(widget_array, sizeof(ei_widget_t *) * 2 * widget_array_length);
                ei_widget_t **new_widget_array = calloc(2 * widget_array_length, sizeof(ei_widget_t *));
		for (uint32_t i = 0; i < widget_array_length; i++)
			*(new_widget_array + i) = *(widget_array + i);
		widget_array_length *= 2;
                free(widget_array);
                widget_array = new_widget_array;
	}
	ei_widget_t **current_widget = widget_array;
	while (*current_widget)
		current_widget++;
	*current_widget = widget;
	widget_count++;
	return current_widget - widget_array;
}

static void remove_from_widget_array (uint32_t id)
{
	//fprintf(stderr, "%i %i\n", widget_array_length, widget_count);
	*(widget_array + id) = NULL;
	widget_count--;
}

/**
 * \brief Returns the id (identity) of a given @param widget.
 */
static uint32_t 	request_id	(ei_widget_t *widget)
{
	return add_to_widget_array(widget);
}

/**
 * \brief returns
 * @param where
 * @return
 */
static uint32_t 	get_id	(ei_point_t *where)
{

        ei_color_t color_id = get_color(pick_surface, where);
        fprintf(stderr, "%i\n", color_id.red + (color_id.green << 8) + (color_id.blue << 16));
        return color_id.red + (color_id.green << 8) + (color_id.blue << 16);
}

static ei_color_t 	id_to_color	(uint32_t id)
{
        ei_color_t color;
        color.red   = (char)( id        & 0xff);
        color.green = (char)((id >> 8 ) & 0xff);
        color.blue  = (char)((id >> 16) & 0xff);
        color.alpha = 0xff;
        return color;
}


ei_widget_t*    ei_widget_create    (ei_widgetclass_name_t	class_name,
				     ei_widget_t*		parent,
				     void*			user_data,
				     ei_widget_destructor_t 	destructor)
{

	/* verification of the existence of the class of widgets */
	ei_widgetclass_t *widget_class = ei_widgetclass_from_name(class_name);

	if (!widget_class)
                return NULL;

        /* if the class of widgets is registered in the library */
	
        /* allocation of memory for the new widget */
        ei_widget_t *new_widget = widget_class->allocfunc();


        /* initialization of the attributes common to every widget*/
        new_widget->wclass              = widget_class;
        new_widget->pick_id             = request_id(new_widget);
        new_widget->pick_color          = malloc(sizeof (ei_color_t));
        *(new_widget->pick_color)       = id_to_color(new_widget->pick_id);
        new_widget->user_data           = user_data;
        new_widget->destructor          = destructor;

        /* Widget Hierachy Management */
        new_widget->parent              = parent;
        new_widget->children_head       = NULL;
        new_widget->children_tail       = NULL;
        new_widget->next_sibling        = NULL;

        new_widget->content_rect        = malloc(sizeof (ei_rect_t));

        if (parent) {
                /* update of the hierarchy of widgets in case the widget has a parent*/
                if (parent->children_head) {
                        /*if the linked list is not empty : we add the widget at the end and
                         * update the attributes next sibling of the last sibling and children_head (parent)*/
                        ei_widget_t *left_sibling = parent->children_head;
                        while (left_sibling->next_sibling) {
                                left_sibling    = left_sibling->next_sibling;
                        }
                        left_sibling->next_sibling = new_widget;
                        parent->children_tail   = new_widget;
                } else {
                        /* if the linked list is empty : we just update the children_tail and children_head of the parent*/
                        parent->children_tail   = new_widget;
                        parent->children_head   = new_widget;
                }
        }
        /* initialization of the attributes that are specific to the widget class*/
        widget_class->setdefaultsfunc(new_widget);
        return new_widget;
}

/**
 * \brief Frees (by calling the \ref unbind function) all the event handlers related to the given @param widget.
 * This is particularly useful when the widget is involved in an event and needs to be destroyed immediatly at the same time.
 */
static void 	unbind_events_widget	(ei_widget_t *widget)
{
	for(uint8_t i=0; i<ei_ev_last; i++) {
		ei_event_handler_t *current_handler = event_handlers_tab[i];
		ei_event_handler_t *handler_to_unbind;
		while(current_handler) {
			handler_to_unbind       = current_handler;
			current_handler         = current_handler->next;
			if ( (handler_to_unbind->user_param==widget) || (handler_to_unbind->widget == widget))
				ei_unbind(handler_to_unbind->eventtype, handler_to_unbind->widget,
                                          handler_to_unbind->tag, handler_to_unbind->callback,
                                          handler_to_unbind->user_param);
		}
    	}
}

/**
 * \brief The function which destroys effectively each widget.
 *  <ul>
 *			<li> As required, this function calls the destructor of the widget
 *			(if it has one) before freeing it (= destroying it), </li>
 *			<li> Frees all the memory allocated for the utilisation of the given widget @param widget,
 *			such as : some of its parameters, its place in the linked list of widgets to update, its place
 *			in the global linked list. </li>
 *			<li> Also calls </li>
 *  </ul>
 */
static void 	free_widget	(ei_widget_t *widget)
{


        if (widget->destructor) {
                widget->destructor(widget);
        }

        /*we free all the event handlers related to this widget*/
        unbind_events_widget(widget);

        /* releases the attributes specific to the widget's class */
        widget->wclass->releasefunc(widget);

        if (widget->pick_color)
                free(widget->pick_color);
        if (widget->content_rect)
                free(widget->content_rect);
        if (widget->geom_params)
                /* if the widget has a geometry manager, we need to disconnect the widget from it */
                ei_geometrymanager_unmap(widget);

        /* we free the widget from the list of widgets whose subtree needs an update*/
        //free_widget_from_update(widget);

        /* we free the widget from the global list of widgets */
        remove_from_widget_array(widget->pick_id);


        /* then we free properly the widget */
        ei_widget_t **ptr_widget        = &widget;
	free(*ptr_widget);
        *ptr_widget                     = NULL;
}

void	ei_widget_destroy	(ei_widget_t* widget) {

	ei_widget_t *current_widget = widget;

	if (widget->children_head) {
	    	current_widget = widget->children_head;
	}
	while (current_widget != widget) {
		if (current_widget->children_head) {
		    	current_widget = current_widget->children_head;
		} else {
			ei_widget_t *widget_to_destroy = current_widget;
			if (current_widget->next_sibling) {
			    	current_widget = current_widget->next_sibling;
			} else if (current_widget->parent) {
			    	current_widget = current_widget->parent;
			}
			if ((current_widget != widget) && widget_to_destroy->parent) {
                                widget_to_destroy->parent->children_head = widget_to_destroy->next_sibling;
				if (widget_to_destroy == widget_to_destroy->parent->children_tail) {
                                        widget_to_destroy->parent->children_tail = NULL;
				}
			}
                        /* the widget is freed properly by setting the content of the memory zone to NULL */
                        ei_widget_t **ptr_widget_to_destroy     = &widget_to_destroy;
			free_widget(*ptr_widget_to_destroy);
                        *ptr_widget_to_destroy                  = NULL;
		}
	}

	if (widget->parent) {
		current_widget = widget->parent->children_head;
		/* if the widget is the last sibling, we can free it directly */
		if (current_widget == widget) {
			widget->parent->children_head = widget->next_sibling;
			free_widget(widget);
		}
		/*if the widget is surrounded by siblings, we need to redirect the sibling & parent links before freeing it*/
		else {
			while(current_widget->next_sibling != widget) {
			    	current_widget = current_widget->next_sibling;
			}
			current_widget->next_sibling = widget->next_sibling;
			if (widget == widget->parent->children_tail) {
			    	widget->parent->children_tail = current_widget;
			}
			free_widget(widget);
		}
	}
	/*if the widget doesn't have a parent, it also doesn't have siblings so it can be simply freed */
	else {
	    	free_widget(widget);
	}
}

ei_widget_t*	ei_widget_pick	(ei_point_t* where)
{
	return *(widget_array + get_id(where));
}

static void 	redraw	(ei_widget_t *widget)
{
	if (!widget->geom_params) {
	    	return;
	}
	ei_rect_t displayed_rect        = widget->screen_location;
	widget->geom_params->manager->runfunc(widget);
	ei_rect_t bounding_rect         = rect_union(displayed_rect, widget->screen_location);
	ei_app_invalidate_rect(&bounding_rect);
}

void    ei_frame_configure	(ei_widget_t*		widget,
				 ei_size_t*		requested_size,
				 const ei_color_t*	color,
				 int*			border_width,
				 ei_relief_t*		relief,
				 char**			text,
				 ei_font_t*		text_font,
				 ei_color_t*		text_color,
				 ei_anchor_t*		text_anchor,
				 ei_surface_t*		img,
				 ei_rect_t**		img_rect,
				 ei_anchor_t*		img_anchor) {


	ei_frame_t *frame = (ei_frame_t *)widget;

	if (color)
	    	frame->color = (ei_color_t ) *color;
	if (text) {
		if (*text) {
			free(frame->text);
			frame->text = malloc(sizeof(char) * (strlen(*text) + 1));
			strcpy(frame->text, *text);
		}
		else if (frame->text) {
			free(frame->text);
			frame->text = *text;
		}

	}

    	if (border_width)
		frame->border_width = *border_width;

	if (requested_size) {
	    	widget->requested_size = *requested_size;
	} else if (widget->requested_size.width == 0 && widget->requested_size.height == 0) {
		if (frame->text) {
			hw_text_compute_size(frame->text, frame->text_font, &widget->requested_size.width,
					     &widget->requested_size.height);
		}
		if (img_rect && *img_rect) {
			ei_size_t img_size = (*img_rect)->size;
			widget->requested_size.width =
				widget->requested_size.width < img_size.width ? img_size.width
									      : widget->requested_size.width;
			widget->requested_size.height =
				widget->requested_size.height < img_size.height ? img_size.height
										: widget->requested_size.height;
		} else if (img) {
			ei_size_t img_size = hw_surface_get_size(img);
			widget->requested_size.width =
				widget->requested_size.width < img_size.width ? img_size.width
									      : widget->requested_size.width;
			widget->requested_size.height =
				widget->requested_size.height < img_size.height ? img_size.height
										: widget->requested_size.height;
		}
		widget->requested_size = ei_size_add(widget->requested_size,
						     ei_size(2 * frame->border_width, 2 * frame->border_width));

	}

	if (relief)
	    	frame->relief = *relief;

	if (text_font)
	    	frame->text_font = *text_font;

	if (text_color)
	    	frame->text_color = *text_color;

	if (text_anchor)
	    	frame->text_anchor = *text_anchor;

	if (img) {
		if (*img) {
		    ei_rect_t rect      = hw_surface_get_rect(*img);
		    frame->img          = hw_surface_create(ei_app_root_surface(), rect.size, EI_TRUE);
		    ei_copy_surface(frame->img, NULL, *img, NULL, EI_TRUE);
		}
		else
		{
		    frame->img = NULL;
		}
	}
	if (img_rect) {
		if(*img_rect)
		{
            		frame->img_rect                 = malloc(sizeof (ei_rect_t));
			frame->img_rect->top_left       = (*img_rect)->top_left;
			frame->img_rect->size           = (*img_rect)->size;
		}
		else
		{
		    frame->img_rect = NULL;
		}
	}
	if (img_anchor)
	    	frame->img_anchor = *img_anchor;


	redraw(widget);

}

void			ei_button_configure		(ei_widget_t*		widget,
							 ei_size_t*		requested_size,
							 const ei_color_t*	color,
							 int*			border_width,
                                                         int*			corner_radius,
                                                         ei_relief_t*		relief,
                                                         char**			text,
                                                         ei_font_t*		text_font,
                                                         ei_color_t*		text_color,
                                                         ei_anchor_t*		text_anchor,
                                                         ei_surface_t*		img,
                                                         ei_rect_t**		img_rect,
                                                         ei_anchor_t*		img_anchor,
                                                         ei_callback_t*		callback,
                                                         void**			user_param) {
    ei_frame_configure(widget,
		       requested_size,
		       color,
		       border_width,
		       relief,
		       text,
		       text_font,
		       text_color,
		       text_anchor,
		       img,
		       img_rect,
		       img_anchor);

    ei_button_t *button         = (ei_button_t *)widget;

    if (corner_radius)
        button->corner_radius   = *corner_radius;

    if (callback)
        button->callback        = *callback;

    if (user_param)
        button->user_param      = *user_param;

    redraw(widget);
}

void			ei_toplevel_configure		(ei_widget_t*		widget,
							 ei_size_t*		requested_size,
							 ei_color_t*		color,
							 int*			border_width,
							 char**			title,
							 ei_bool_t*		closable,
							 ei_axis_set_t*		resizable,
						 	 ei_size_t**		min_size)
{
	ei_toplevel_t *toplevel         = (ei_toplevel_t *)widget;

	if (requested_size)
	    	widget->requested_size  = ei_size_add(*requested_size, ei_size(0, top_border_height));

	if (color)
	    	toplevel->color         = *color;

	if (border_width)
	    	toplevel->border_width  = *border_width;

	if (title)
	{
		if (*title) {
			free(toplevel->title);
			toplevel->title = malloc(sizeof(char) * (strlen(*title)+1));
			strcpy(toplevel->title, *title);
		} else {
		    	toplevel->title = NULL;
		}
	}
	if (closable) {
		if (*closable == EI_FALSE) {
			ei_widget_destroy(toplevel->button);
			widget->children_head = NULL;
		}
		toplevel->closable      = *closable;
	}

	if (resizable)
	    	toplevel->resizable     = *resizable;

	if ((min_size)&&(*min_size))
		toplevel->min_size      = **min_size;

	redraw(widget);
}