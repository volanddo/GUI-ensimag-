#include "ei_draw_extension.h"
#include "ei_utils.h"
#include "ei_types.h"
#include "hw_interface.h"
#include "ei_draw.h"


ei_bool_t is_contained_in (ei_rect_t a,
                           ei_rect_t b)
{
	return (a.top_left.x >= b.top_left.x &&
		a.top_left.y >= b.top_left.y &&
		a.top_left.x+a.size.width <= b.top_left.x+b.size.width &&
		a.top_left.y+a.size.height <= b.top_left.y+b.size.height);
}



ei_color_t get_color (ei_surface_t 	surface,
		     ei_point_t *	where)
{
	uint32_t *address = (uint32_t *)hw_surface_get_buffer(surface);
	ei_size_t surface_size = hw_surface_get_size(surface);
	address += (surface_size.width * where->y + where->x);

	return uint32_to_color(surface, *address);
}


ei_bool_t is_point_in_rect (ei_point_t 	point,
			   ei_rect_t 	rect)
{
	return (rect.top_left.x < point.x && point.x < rect.top_left.x + rect.size.width &&
		rect.top_left.y < point.y && point.y < rect.top_left.y + rect.size.height);
}


ei_bool_t  collision (const ei_rect_t *a,
		      const ei_rect_t *b)
{
	if (!(a && b)) {
		return EI_TRUE;
	}
	return  a->top_left.x < b->top_left.x + b->size.width  &&
	    a->top_left.x + a->size.width  > b->top_left.x &&
	    a->top_left.y < b->top_left.y + b->size.height &&
	    a->top_left.y + a->size.height > b->top_left.y;
}


ei_rect_t rect_intersection (ei_rect_t 	a,
			     ei_rect_t 	b)
{
	if (!collision(&a, &b))
	    	return ei_rect_zero();

	int x = a.top_left.x > b.top_left.x ? a.top_left.x : b.top_left.x;
	int y = a.top_left.y > b.top_left.y ? a.top_left.y : b.top_left.y;

	int x_down = a.top_left.x + a.size.width < b.top_left.x + b.size.width ? a.top_left.x + a.size.width : b.top_left.x + b.size.width;
	int y_down= a.top_left.y + a.size.height < b.top_left.y + b.size.height ? a.top_left.y + a.size.height : b.top_left.y + b.size.height;
	return ei_rect(ei_point(x,y), ei_size(x_down - x, y_down - y));
}


ei_rect_t rect_union (ei_rect_t a,
		      ei_rect_t b)
{
	int x = a.top_left.x < b.top_left.x ? a.top_left.x : b.top_left.x;
	int y = a.top_left.y < b.top_left.y ? a.top_left.y : b.top_left.y;

	int x_down = a.top_left.x + a.size.width > b.top_left.x + b.size.width ? a.top_left.x + a.size.width : b.top_left.x + b.size.width;
	int y_down= a.top_left.y + a.size.height > b.top_left.y + b.size.height ? a.top_left.y + a.size.height : b.top_left.y + b.size.height;
	return ei_rect(ei_point(x,y), ei_size(x_down - x + 1, y_down - y + 1));
}



ei_rect_t get_bounding_rect (const ei_linked_rect_t *polygon)
{
	ei_rect_t bounding_rect;
	int min_x = polygon->rect.top_left.x, min_y = polygon->rect.top_left.y;
	int max_x = polygon->rect.top_left.x + polygon->rect.size.width, max_y = polygon->rect.top_left.y + polygon->rect.size.height;
	polygon = polygon->next;
	while (polygon) {
		min_x = polygon->rect.top_left.x < min_x ? polygon->rect.top_left.x : min_x;
		min_y = polygon->rect.top_left.y < min_y ? polygon->rect.top_left.y : min_y;
		max_x = polygon->rect.top_left.x + polygon->rect.size.width > max_x ? polygon->rect.top_left.x + polygon->rect.size.width : max_x;
		max_y = polygon->rect.top_left.y + polygon->rect.size.height > max_y ? polygon->rect.top_left.y + polygon->rect.size.height : max_y;
		polygon = polygon->next;
	}
	bounding_rect = ei_rect(ei_point(min_x, min_y), ei_size(max_x-min_x, max_y-min_y));
	return bounding_rect;
}



ei_color_t color_lerp(ei_color_t col_dst,
			ei_color_t col_src)
{

	ei_color_t col = {
		(col_src.alpha * col_src.red + (255 - col_src.alpha) * col_dst.red) / 255,
		(col_src.alpha * col_src.green + (255 - col_src.alpha) * col_dst.green) / 255,
		(col_src.alpha * col_src.blue + (255 - col_src.alpha) * col_dst.blue) / 255,
		col_src.alpha
	};
	return col;
}

ei_color_t uint32_to_color (ei_surface_t 	source,
			    uint32_t		src_color)
{
	int r_index, g_index, b_index, a_index;

	hw_surface_get_channel_indices(source, &r_index, &g_index, &b_index, &a_index);

	/* we adapt the 32 bits to 8 bits */
	uint8_t r_val_src = (uint8_t) (src_color >> r_index*8) & 255;
	uint8_t g_val_src = (uint8_t) (src_color >> g_index*8) & 255;
	uint8_t b_val_src = (uint8_t) (src_color >> b_index*8) & 255;
	uint8_t a_val_src = (uint8_t) (src_color >> a_index*8) & 255;
	ei_color_t col = {r_val_src, g_val_src, b_val_src, a_val_src};
	return col;
}


/**
 * \brief	        Gets the top left point of an object which can be drawn in the given widget @param frame.
 *
 * @param	frame		    \ref ei_frame_t where the object will be draw.
 * @param	position_rel	    \ref ei_anchor_t which gives the relative position of the object.
 *                                  In default case, it's center.
 * @param	object_size	    \ref ei_size_t of the object to write.
 *
 * @return 			Return the \ref ei_frame_t top_left_point of the rect to draw our object.
 */
static ei_point_t get_start_point (
			    ei_frame_t *	frame,
			    ei_anchor_t 	position_rel,
			    ei_size_t 		object_size)
{
	int widget_with = frame->widget.content_rect->size.width;
	int widget_height = frame->widget.content_rect->size.height;
	ei_point_t widget_top_left = frame->widget.content_rect->top_left;
	ei_point_t  start;

	switch (position_rel) {
		case ei_anc_north:
			start.x =  widget_top_left.x + (widget_with - object_size.width)/2;
			start.y =  widget_top_left.y;
			break;
		case ei_anc_northeast:
			start.x =  widget_top_left.x + widget_with - object_size.width;
			start.y =  widget_top_left.y;
			break;
		case ei_anc_east:
			start.x =  widget_top_left.x + widget_with - object_size.width;
			start.y =  widget_top_left.y + (widget_height - object_size.height)/2;
			break;
		case ei_anc_southeast:
			start.x =  widget_top_left.x + widget_with - object_size.width;
			start.y =  widget_top_left.y + widget_height - object_size.height;
			break;
		case ei_anc_south:
			start.x =  widget_top_left.x + (widget_with - object_size.width)/2;
			start.y =  widget_top_left.y + widget_height - object_size.height;
			break;
		case ei_anc_southwest:
			start.x =  widget_top_left.x;
			start.y =  widget_top_left.y + widget_height - object_size.height;
			break;
		case ei_anc_west:
			start.x =  widget_top_left.x;
			start.y =  widget_top_left.y + (widget_height - object_size.height)/2;
			break;
		case ei_anc_northwest:
			start.x =  widget_top_left.x;
			start.y =  widget_top_left.y;
			break;
		default:
			start.x =  widget_top_left.x + (widget_with - object_size.width)/2 ;
			start.y =  widget_top_left.y + (widget_height - object_size.height)/2 ;
			break;
	}

    	return ei_point(start.x + (frame->relief == ei_relief_sunken)*frame->border_width,
                    start.y + (frame->relief == ei_relief_sunken)*frame->border_width);
}


void draw_frame_text (ei_surface_t 	surface,
		       ei_frame_t *	frame,
		       ei_rect_t *	content_clipper)
{
	// computation of the start point coordinates
	ei_size_t object_size;
	hw_text_compute_size(frame->text, frame->text_font,&(object_size.width), &(object_size.height));
	ei_point_t start_point = get_start_point(frame, frame->text_anchor,object_size);

	ei_rect_t clipper_text = *frame->widget.content_rect;

	// computation of the content clipper
	if (content_clipper)
	    	clipper_text = rect_intersection(*content_clipper, *frame->widget.content_rect);

        if (frame->widget.parent)
                clipper_text = rect_intersection(clipper_text, *frame->widget.parent->content_rect);

        // writing of the text.
        ei_draw_text(surface, &start_point, frame->text, frame->text_font, frame->text_color, &clipper_text);

}

void draw_frame_image (ei_surface_t 	surface,
		       ei_frame_t *	frame)
{
    	// initialisation of start point
	ei_size_t object_size;

	if (frame->img_rect)
	    	object_size = frame->img_rect->size;
	else
	    	object_size = hw_surface_get_rect(frame->img).size;

	// Compute of start coord
	ei_point_t dst_rect_top_left            = get_start_point(frame, frame->img_anchor, object_size);
	ei_rect_t mod_dst_rect, dst_rect        = ei_rect(dst_rect_top_left, object_size);
	mod_dst_rect = rect_intersection(dst_rect, *frame->widget.content_rect);

        if (frame->widget.parent)
                mod_dst_rect = rect_intersection(mod_dst_rect, *frame->widget.parent->content_rect);

        ei_copy_surface(surface, &mod_dst_rect, frame->img, frame->img_rect, EI_FALSE);
}


void 	draw_text_toplevel	(ei_surface_t 		surface,
				 ei_toplevel_t *	toplevel,
				 ei_color_t 		title_color,
				 ei_rect_t 		    content_clipper)
{
	ei_point_t start;
	ei_rect_t clipper_text;
	ei_size_t text_size;
	int init_len    = (int)strlen(toplevel->title);
	int len         = init_len, x, y;
	char *copy      = malloc(sizeof (char)*(len+1));
	strcpy(copy, toplevel->title);

	do {
		x = toplevel->widget.screen_location.top_left.x + top_border_height*(toplevel->closable == EI_TRUE);
		y =toplevel->widget.screen_location.top_left.y;

		int width       = toplevel->widget.screen_location.size.width
                                - top_border_height*(toplevel->closable == EI_TRUE)
                                - toplevel->border_width;

		int height      = top_border_height;

		clipper_text = ei_rect( ei_point( x, y),ei_size( width, height));
		clipper_text = rect_intersection(clipper_text, content_clipper);
		hw_text_compute_size(toplevel->title, ei_default_font,&(text_size.width), &(text_size.height));

		start.x =  x + (width - text_size.width)/2;
		start.y =  y + (height - text_size.height)/2;

        if (len > 1 && start.x < x)
        {
            toplevel->title[len-1] = '\0';
            len--;
        }
    }while(len > 1 &&  start.x < x);
    if(len!=init_len) {
        start.x = toplevel->widget.screen_location.top_left.x + top_border_height;

        for (int i = len - 2; i < len+1; i++) {
            if (i > 0 && i < init_len)
                toplevel->title[i] = '.';
        }

        for (int i = len +1; i < len + 3; i++) {
            if (i > 0 && i < init_len)
                toplevel->title[i] = ' ';
        }
    }
    ei_draw_text(surface, &start, toplevel->title, ei_default_font, title_color, &clipper_text);

        strcpy(toplevel->title, copy);
        free(copy);
}

