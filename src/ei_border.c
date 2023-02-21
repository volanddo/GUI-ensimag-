#include "ei_border.h"
#include "ei_types.h"
#include "ei_utils.h"
#include "math.h"
#include "hw_interface.h"
#include "ei_draw.h"
#define TO_RAD 3.141592/180

/**
 * \brief 	Defines different part of a frame (for \ref draw_rounded_frame_border )
 */
typedef enum frame_part {
    	upper_frame,
    	lower_frame,
    	whole_frame
} ei_frame_part_t;


/**
 * @param	center		The center of the arc.
 * @param	radius		The radius of the arc.
 * @param	th1		The starting angle of the arc.
 * @param	th2		The last angle of the arc.
 * @return			A linked list of ei_point_t between th1 and th2
 */
static 		ei_linked_point_t * 	arc		(ei_point_t 	center,
							 int 		radius,
							 int 		th1,
							 int 		th2)
{

    	ei_linked_point_t *previous_point = NULL;
    	ei_linked_point_t *arc_points;

	if (radius == 0) {
		arc_points = malloc(sizeof (ei_linked_point_t));
		arc_points->point = center;
		arc_points->next = NULL;
		return arc_points;
	}

	float delta_th = (th2-th1)/(float)radius;

	for (int i = 0; i <= radius; i++) {
		float th = th1 + i * delta_th;
		arc_points = malloc(sizeof (ei_linked_point_t));
		arc_points->point = ei_point_add(center, ei_point(round(radius*cos(th*TO_RAD)), -round(radius*sin(th*TO_RAD))));
		arc_points->next = previous_point;
		previous_point = arc_points;
	}

	return arc_points;
}

/**
 * \brief	Frees a ei_linked_point_t instance
 */
static void 	free_linked_point	(ei_linked_point_t *linked_points)
{

	ei_linked_point_t *current_point = linked_points;

	while(current_point) {
		ei_linked_point_t *temp_linked_point = current_point;
		current_point = current_point->next;
		free(temp_linked_point);
	}

}

/**
 * \brief	Concatenates l2 to l1.
 * @param 	l1 	Must be not NULL.
 * @param 	l2
 */
static void 	cat_linked_point	(ei_linked_point_t *	l1,
					 ei_linked_point_t *	l2)
{
	ei_linked_point_t *current_linked_pt = l1;
	while (current_linked_pt->next) {
	    	current_linked_pt = current_linked_pt->next;
	}
	current_linked_pt->next = l2;
}

/**
 * TODO
 * @param 	rect		The screen_location of the frame.
 * @param 	radius		The radius of the corners.
 * @param 	frame_part	Which part of the frame to return.
 * @return	The linked list of points defining the outline of a rounded frame.
 */
static ei_linked_point_t *	rounded_frame		(ei_rect_t 		rect,
							 int 			radius,
							 ei_frame_part_t 	frame_part)
{
	ei_point_t top_l_center = rect.top_left;
	ei_point_t bot_l_center = ei_point_add(rect.top_left, ei_point(0, rect.size.height));
	ei_point_t bot_r_center = ei_point_add(rect.top_left, ei_point(rect.size.width, rect.size.height));
	ei_point_t top_r_center = ei_point_add(rect.top_left, ei_point(rect.size.width, 0));

	top_l_center = ei_point_add(top_l_center, ei_point(radius, radius));
	bot_l_center = ei_point_add(bot_l_center, ei_point(radius, -radius-1));
	bot_r_center = ei_point_add(bot_r_center, ei_point(-radius-1, -radius-1));
	top_r_center = ei_point_add(top_r_center, ei_point(-radius-1, radius));

	ei_linked_point_t *top_l_arc = arc(top_l_center, radius, 180, 90);
	ei_linked_point_t *top_r_arc_upper = arc(top_r_center, radius, 90, 45);
	ei_linked_point_t *top_r_arc_lower = arc(top_r_center, radius, 45, 0);
	ei_linked_point_t *bot_r_arc = arc(bot_r_center, radius, 0, -90);
	ei_linked_point_t *bot_l_arc_lower = arc(bot_l_center, radius, -90, -135);
	ei_linked_point_t *bot_l_arc_upper = arc(bot_l_center, radius, -135, -180);


	int h = rect.size.height / 2;
	if (rect.size.width < rect.size.height)
	    	h = rect.size.width / 2;

	ei_linked_point_t  *middle_left = malloc(sizeof (ei_linked_point_t));
	middle_left->point = ei_point_add(rect.top_left, ei_point(h, rect.size.height-h));
	middle_left->next = NULL;

	ei_linked_point_t  *middle_right = malloc(sizeof (ei_linked_point_t));
	middle_right->point = ei_point_add(rect.top_left, ei_point(rect.size.width-h, h));
	middle_right->next = NULL;

	ei_linked_point_t *final_frame = middle_left;

	switch (frame_part) {
		case upper_frame:
			cat_linked_point(top_l_arc, bot_l_arc_upper);
			cat_linked_point(top_r_arc_upper, top_l_arc);
			cat_linked_point(middle_right, top_r_arc_upper);
			cat_linked_point(middle_left, middle_right);
			free_linked_point(bot_r_arc);
			free_linked_point(bot_l_arc_lower);
			free_linked_point(top_r_arc_lower);
			break;
		case lower_frame:
			cat_linked_point(top_r_arc_lower, middle_right);
			cat_linked_point(bot_r_arc, top_r_arc_lower);
			cat_linked_point(bot_l_arc_lower,bot_r_arc);
			cat_linked_point(middle_left, bot_l_arc_lower);
			free_linked_point(top_l_arc);
			free_linked_point(bot_l_arc_upper);
			free_linked_point(top_r_arc_upper);
		    	break;
		case whole_frame:
			cat_linked_point(top_r_arc_lower, top_r_arc_upper);
			cat_linked_point(bot_r_arc, top_r_arc_lower);
			cat_linked_point(bot_l_arc_lower,bot_r_arc);
			cat_linked_point(bot_l_arc_upper, bot_l_arc_lower);
			cat_linked_point(top_l_arc, bot_l_arc_upper);
			final_frame = top_l_arc;
			free_linked_point(middle_right);
			free_linked_point(middle_left);
			break;
	}

	return final_frame;
}

/**
 * @param	color
 * @param 	factor
 * @return	Returns the clamped (between 0 and 255) result of the color multiplied by factor.
 */
static ei_color_t 	color_mult	(ei_color_t 	color,
					 float 		factor)
{
	ei_color_t new_col = {factor * color.red, factor * color.green, factor * color.blue, color.alpha};

	if (factor * color.red > 255)
	    	new_col.red     = 255;
	if (factor * color.green > 255)
	    	new_col.green   = 255;
	if (factor * color.blue > 255)
	    	new_col.blue    = 255;

	return new_col;
}

/**
 * \brief 	Draws the body of a frame (frame without it's borders).
 *
 * @param	surface		The surface on which to draw.
 * @param 	rect		The screen_location of the frame (borders included).
 * @param 	border_width	The width of the frame's borders.
 * @param 	corner_radius	The radius of the frame corners.
 * @param 	color		The frame body color.
 * @param 	clipper		If not NULL, the drawing is restricted within this rectangle.
 */
static void 	draw_rounded_frame_body		(ei_surface_t 		surface,
					 	 ei_rect_t 		rect,
					 	 int 			border_width,
					 	 int 			corner_radius,
					 	 ei_color_t 		color,
					 	 const ei_rect_t *	clipper)
{

	ei_rect_t screen_loc_without_border = ei_rect(ei_point_add(rect.top_left, ei_point(border_width, border_width)),
						      ei_size_sub(rect.size, ei_size(2*border_width, 2*border_width)));

	ei_linked_point_t *button_polygon = rounded_frame(screen_loc_without_border, corner_radius, whole_frame);
	ei_draw_polygon(surface, button_polygon, color, clipper);

	free_linked_point(button_polygon);
}

/**
 * \brief	Draws the border of a frame with colors according to the relief(frame without it's body).
 * @param	surface		The surface on which to draw.
 * @param 	rect		The screen_location of the frame (borders included).
 * @param 	border_width	The width of the frame's borders.dth
 * @param 	corner_radius	The radius of the frame corners.
 * @param 	color		The frame body color.
 * @param	relief		The relief of the frame.
 * @param 	clipper		If not NULL, the drawing is restricted within this rectangle.
 */
static void 	draw_rounded_frame_border	(ei_surface_t 		surface,
					         ei_rect_t 		rect,
					         int 			border_width,
					         int 			corner_radius,
					         ei_color_t 		color,
					         ei_relief_t 		relief,
					         const ei_rect_t *	clipper)
{
	ei_color_t upper_color = color;
	ei_color_t lower_color = color;

	if (relief == ei_relief_raised) {
		upper_color = color_mult(color, 1.1);
		lower_color = color_mult(color, 0.9);
	} else if(relief == ei_relief_sunken) {
		lower_color = color_mult(color, 1.1);
		upper_color = color_mult(color, 0.9);
	}

	ei_linked_point_t *upper_border = rounded_frame(rect, corner_radius, upper_frame);
	ei_linked_point_t *lower_border = rounded_frame(rect, corner_radius, lower_frame);

	ei_draw_polygon(surface, upper_border, upper_color, clipper);
	ei_draw_polygon(surface, lower_border, lower_color, clipper);

	free_linked_point(upper_border);
	free_linked_point(lower_border);
}



void 	draw_rounded_frame	(ei_surface_t 		surface,
                        	 ei_rect_t 		rect,
                        	 int 			border_width,
                        	 int 			corner_radius,
                        	 ei_color_t 		color,
                        	 ei_relief_t 		relief,
                        	 const ei_rect_t *	clipper)
{
	if (color.alpha == 255) {
		if (border_width != 0)
			draw_rounded_frame_border(surface, rect, border_width, corner_radius, color, relief, clipper);
		draw_rounded_frame_body(surface, rect, border_width, corner_radius, color, clipper);
	} else {
		draw_rounded_frame_body(surface, rect, 0, corner_radius, color, clipper);
	}
}



void 	draw_half_rounded_frame		(ei_surface_t 		surface,
					 ei_rect_t 		rect,
					 int 			corner_radius,
					 ei_color_t 		color,
					 const ei_rect_t *	clipper)
{
	ei_point_t top_l_center = ei_point_add(rect.top_left, ei_point(corner_radius, corner_radius));
	ei_point_t bot_l_center = ei_point_add(rect.top_left, ei_point(0, rect.size.height-1));
	ei_point_t bot_r_center = ei_point_add(rect.top_left, ei_point(rect.size.width-1, rect.size.height-1));
	ei_point_t top_r_center = ei_point_add(rect.top_left, ei_point(rect.size.width-corner_radius-1, +corner_radius));

	ei_linked_point_t *top_l_arc = arc(top_l_center, corner_radius, 180, 90);
	ei_linked_point_t *top_r_arc = arc(top_r_center, corner_radius, 90, 0);
	ei_linked_point_t *bot_l_arc = malloc(sizeof (ei_linked_point_t));
	bot_l_arc->point = bot_l_center;
	bot_l_arc->next = NULL;
	ei_linked_point_t *bot_r_arc = malloc(sizeof (ei_linked_point_t));
	bot_r_arc->point = bot_r_center;
	bot_r_arc->next = NULL;


	cat_linked_point(top_r_arc, top_l_arc);
	cat_linked_point(bot_r_arc, top_r_arc);
	cat_linked_point(bot_l_arc, bot_r_arc);

	ei_draw_polygon(surface, bot_l_arc, color, clipper);

	free_linked_point(bot_l_arc);
}
