//
// Created by peres on 06/05/22.
//

#ifndef PROJECT_IG_EI_DRAW_EXTENSION_H
#define PROJECT_IG_EI_DRAW_EXTENSION_H

#include "ei_types.h"
#include "hw_interface.h"
#include "ei_frame.h"
#include "ei_toplevel.h"

/**
 * \brief	        Gets the minimal ei_rect_t that contains the rectangles @param a and @param b
 */
extern  ei_rect_t                rect_union             (ei_rect_t a, ei_rect_t b);

/**
 * \brief	        Returns a boolean that indicates if the rectangle @param a is contained in the rectangle @param b.
 */
extern  ei_bool_t                is_contained_in        (ei_rect_t a, ei_rect_t b);


// TODO < ! > brice should comment this section.
//extern void draw_smooth_polygon(ei_surface_t surface, const ei_linked_point_t *polygon, ei_color_t color);

/**
 * \brief	                Returns the color of a pixel on a surface.
 *
 * @param	surface		The surface to get the pixel color
 * @param	where		A pointer to the point (corresponding a the pixel)
 */
extern  ei_color_t              get_color               (ei_surface_t surface, ei_point_t *where);


/**
 * \brief	 Returns a boolean that indicates if the point @param point is contained in the rectangle @param rect

 */
extern  ei_bool_t               is_point_in_rect        (ei_point_t point, ei_rect_t rect);

/**
 * TODO -> Brice
 * @param polygon
 * @return
 */
extern  ei_rect_t               get_bounding_rect       (const ei_linked_rect_t *polygon);


/**
 * \brief	     Returns a boolean that indicates if the rectangles @param rect1 and @param rect2 are in collision.
 */
extern  ei_bool_t               collision               (const ei_rect_t *a, const ei_rect_t *b);

/**
 * \brief	     Returns a new rectangle which is the intersection of the two rectangles @param rect1 and @param rect2.
 */
extern  ei_rect_t               rect_intersection       (ei_rect_t a, ei_rect_t b);


/**
 * \brief	            Returns a new color which is the result of the fusion of two given colors. The result color
 *                          is obtained by linear interpolation (hence the name of the function).
 *
 * @param	col_dst	    color of destination (the parameter of transparency 'alpha' isn't important in this parameter).
 * @param	col_src	    color of the source (the parameter of transparency 'alpha' is used for the fusion).
 *
 */
extern  ei_color_t 	        color_lerp		(ei_color_t col_dst, ei_color_t col_src);

/**
 * \brief	        Converts an 32 bits integer into a color writable on the given surface (with same channel).
 *                      This function is the reverse function of \ref ei_map_rgba.
 *
 * @param	source	    The surface will give the channel
 * @param	src_color   The color to convert in uint32_t
 *
 * @return 			Return a uint32_t who can write on a surface with the same channel of source.
 */
extern  ei_color_t 	        uint32_to_color		(ei_surface_t source, uint32_t src_color);

/**
 * \brief	           Draws the text of a widget @param frame given in a @param content_clipper.
 *
 * @param	surface		\ref ei_surface_t to draw.
 * @param	frame	        \ref the frame which has a text to draw.
 * @param	content_clipper	\ref the content clipper where the text needs to be draw.
 */

extern  void                    draw_frame_text         (ei_surface_t surface, ei_frame_t *frame, ei_rect_t *content_clipper);

/**
 * \brief	        Draws the image of a given widget @param frame in a content_clipper.
 *
 * @param	surface		\ref ei_surface_t to draw.
 * @param	frame	        \ref the frame which have an image to draw.
 * @param	content_clipper	\ref The content clipper where the text need to be draw.
 */
extern  void                    draw_frame_image        (ei_surface_t surface, ei_frame_t *frame);

/**
 * \brief	        Draws the title of a toplevel inside the given @param content_clipper.
 *                      The font used for all the toplevel widgets is the \ref ei_default_font .
 *
 * @param	surface		        \ref ei_surface_t to draw.
 * @param	toplevel	        \ref the toplevel which have a title to draw.
 * @param	top_button_radius	\ref The radius of the toplevel top button (use to compute the title position).
 * @param	title_color	        \ref The title color
 * @param	content_clipper	        \ref The content clipper where the title need to be draw.
 */
extern  void                    draw_text_toplevel      (ei_surface_t surface,
                                                        ei_toplevel_t *toplevel,
                                                        ei_color_t title_color,
                                                        ei_rect_t content_clipper);
#endif //PROJECT_IG_EI_DRAW_EXTENSION_H
