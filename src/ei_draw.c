#include "ei_draw.h"
#include "ei_types.h"
#include "hw_interface.h"
#include "ei_draw_extension.h"
#include "ei_utils.h"


uint32_t ei_map_rgba (ei_surface_t 	surface,
		      ei_color_t 	color)
{
	int r_index;
	int g_index;
	int b_index;
	int a_index;
	hw_surface_get_channel_indices(surface, &r_index, &g_index, &b_index, &a_index);
	return (((uint32_t)color.red) << r_index*8)    |
		   (((uint32_t)color.green) << g_index*8) |
		   (((uint32_t)color.blue) << b_index*8)  |
		   (((uint32_t)color.alpha) << a_index*8);
}



void ei_fill (ei_surface_t 	surface,
	      const ei_color_t*	color,
	      const ei_rect_t*	clipper)
{
	ei_rect_t clipper_to_use;
	if(!color)
	    	color = &ei_font_default_color;

	if(!clipper)
	    	clipper_to_use = hw_surface_get_rect(surface);
	else
	    	clipper_to_use = rect_intersection(*clipper, hw_surface_get_rect(surface));
	/* we get the address of the first pixel */
	uint32_t *address = (uint32_t *) hw_surface_get_buffer(surface);
	ei_size_t surface_size = hw_surface_get_size(surface);

	for (int x = clipper_to_use.top_left.x; x < clipper_to_use.top_left.x + clipper_to_use.size.width; x +=1) {

		for (int y = clipper_to_use.top_left.y; y < clipper_to_use.top_left.y + clipper_to_use.size.height; y +=1) {
			uint32_t surface_color = *(address + x + y*surface_size.width);
			ei_color_t col_dest = uint32_to_color(surface, surface_color);
			col_dest = color_lerp(col_dest, *color);
			*(address + x + y*surface_size.width) = ei_map_rgba(surface, col_dest);
		}
	}

}

/**
 * \brief Returns the minimum of the integers @param a and @param b
 */
static int min(int a, int b)
{
    	return a<b ? a : b;
}

int ei_copy_surface (ei_surface_t  		destination,
                     const ei_rect_t *  	dst_rect,
                     ei_surface_t  	    	source,
                     const ei_rect_t *  	src_rect,
                     ei_bool_t  	        alpha)
{
	/* we get the size and fix the origin of each surface */
	ei_size_t src_screen_size = hw_surface_get_size(source);
	ei_size_t dst_screen_size = hw_surface_get_size(destination);

	uint32_t * dst_adr = (uint32_t *) hw_surface_get_buffer(destination);
	uint32_t * src_adr = (uint32_t *) hw_surface_get_buffer(source);

	/* before anything else we check the NULL cases and verify if we
	 * are writing in the correct zone */
	ei_rect_t *dst_rect_use = (ei_rect_t *) dst_rect;

	if (!dst_rect) {
		dst_rect_use = malloc(sizeof (ei_rect_t));
		*dst_rect_use = hw_surface_get_rect(destination);
	}

	ei_rect_t src_rect_use;
	if (!src_rect)
	    	src_rect_use = hw_surface_get_rect(source);
	else
	    	src_rect_use = *src_rect;
	/* there's no need to cop the pixels that go over the rectangle */
	src_rect_use.size.width = min(src_rect_use.size.width, dst_rect_use->size.width );
	src_rect_use.size.height = min(src_rect_use.size.height, dst_rect_use->size.height);

	/* for each pixel in the rectangle we have to copy */
	for (uint32_t i = 0; i< src_rect_use.size.height;  i++) {

		for (uint32_t j = 0; j< src_rect_use.size.width; j ++) {
			/* we get the pixel inside the destination and source rectangles */
			int y_src = (int)i + src_rect_use.top_left.y;
			int x_src = (int)j + src_rect_use.top_left.x;
			int y_dst = (int)i + dst_rect_use->top_left.y;
			int x_dst = (int)j + dst_rect_use->top_left.x;

			/* if the coordinates are effectively in the screen, we copy them */
			if (    y_src >= 0 && y_src < src_screen_size.height
				&&  x_src >= 0 && x_src < src_screen_size.width
				&&  y_dst >= 0 && y_dst < dst_screen_size.height
				&&  x_dst >= 0 && x_dst < dst_screen_size.width)
			{
			    /* we use the color to copy by default */
			    uint32_t src_color = *(src_adr + (x_src + y_src*src_screen_size.width));
			    if (alpha) {
				    uint32_t surface_color = *(dst_adr + x_dst + y_dst*dst_screen_size.width);
				    ei_color_t col_dest = uint32_to_color(destination, surface_color);
				    col_dest = color_lerp(col_dest, uint32_to_color(source,src_color));
				    *(dst_adr + x_dst + y_dst*dst_screen_size.width) = ei_map_rgba(destination, col_dest);
			    } else {
					*(dst_adr + x_dst + y_dst*dst_screen_size.width) = src_color;
			    }
			}

		}
	}

	if (!dst_rect)
	    	free(dst_rect_use);

}

void ei_draw_text (ei_surface_t	    	surface,
                   const ei_point_t*	where,
                   const char*		text,
                   ei_font_t		font,
                   ei_color_t		color,
                   const ei_rect_t*	clipper)
{
	ei_size_t char_size;
	if (!font)
	    	font = ei_default_font;

	hw_text_compute_size(text, font, &char_size.width, &char_size.height);

	ei_surface_t text_surface = hw_text_create_surface(text, font, color);

	ei_rect_t dst_rect = ei_rect(*where, hw_surface_get_rect(text_surface).size), mod_dst_rect = dst_rect;
	if (clipper)
	    	mod_dst_rect = rect_intersection(mod_dst_rect, *clipper);

	ei_rect_t src_rect = ei_rect(hw_surface_get_rect(text_surface).top_left, dst_rect.size);
	src_rect.top_left = ei_point_add(src_rect.top_left, ei_point_sub(mod_dst_rect.top_left, dst_rect.top_left));

	ei_copy_surface(surface, &mod_dst_rect, text_surface, &src_rect, EI_TRUE);
	hw_surface_free(text_surface);
}