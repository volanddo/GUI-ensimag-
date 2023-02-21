//
// Created by peres on 05/05/22.
//

#ifndef PROJETC_IG_EI_BORDER_H
#define PROJETC_IG_EI_BORDER_H

#include "ei_types.h"
#include "hw_interface.h"

/**
 * \brief	Draw the border of a frame with colors according to the relief(frame without it's body).
 * @param	surface		The surface on which to draw.
 * @param 	rect		The screen_location of the frame (borders included).
 * @param 	border_width	The width of the frame's borders.dth
 * @param 	corner_radius	The radius of the frame corners.
 * @param 	color		The frame's body color.
 * @param	relief		The relief of the frame.
 * @param 	clipper		If not NULL, the drawing is restricted within this rectangle.
 */
extern void 	draw_rounded_frame		(ei_surface_t 		surface,
						 ei_rect_t 		rect,
						 int 			border_width,
						 int 			corner_radius,
						 ei_color_t 		color,
						 ei_relief_t 		relief,
						 const ei_rect_t *	clipper);

/**
 * \brief	Draw the border of a frame with colors according to the relief(frame without it's body).
 * @param	surface		The surface on which to draw.
 * @param 	rect		The screen_location of the frame (borders included).
 * @param 	corner_radius	The radius of the frame corners.
 * @param 	color		The frame's body color.
 * @param 	clipper		If not NULL, the drawing is restricted within this rectangle.
 */
extern void 	draw_half_rounded_frame		(ei_surface_t 		surface,
						 ei_rect_t 		rect,
						 int 			corner_radius,
						 ei_color_t 		color,
						 const ei_rect_t *	clipper);

#endif //PROJETC_IG_EI_BORDER_H
