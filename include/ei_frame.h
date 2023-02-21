#ifndef EI_FRAME_H
#define EI_FRAME_H

#include "ei_widget.h"

/**
 * \brief Defines the structure of every frame.
 */
typedef struct ei_frame_t {
        ei_widget_t		widget;

        /* parameters for appearance */
        ei_color_t	        color;           ///< color of the frame
        int	                border_width;    ///< border width of the frame
        ei_relief_t             relief;          ///< relief of the frame

        /* parameters for the text on the frame*/
        char*		        text;         ///< text displayed on the frame
        ei_font_t		text_font;    ///< font of the text
        ei_color_t		text_color;   ///< color of the text
        ei_anchor_t		text_anchor;  ///< anchor position of the text

        /* parameters for the image on the frame */
        ei_surface_t	        img;         ///< the surface of an image
        ei_rect_t*		img_rect;    ///< the rectangle containing the part of the image which needs to be displayed
        ei_anchor_t		img_anchor;  ///< anchor position of the image rectangle (see definition above)
} ei_frame_t;

/**
 * \brief \ref ei_widgetclass_drawfunc_t specific to the frame class.
 */
extern void 		frameclass_draw		        (struct ei_widget_t *	widget,
                                                        ei_surface_t 		surface,
                                                        ei_surface_t 		pick_surface,
                                                        ei_rect_t *		clipper);

/**
 * \brief \ref ei_widgetclass_allocfunc_t specific to the frame class.
 */
extern ei_widget_t *	frameclass_alloc	        (void);

/**
 * \brief \ref ei_widgetclass_setdefaultsfunc_t specific to the frame class.
 *  <ul>
 *			<li> Sets the parameters present in \ref ei_frame_configure to their default value (Cf
 *			description of \ref ei_frame_configure), </li>
 *  </ul>
 */
extern void 		frameclass_setdefaults	        (ei_widget_t *	widget);

/**
 *  \brief \ref ei_widgetclass_releasefunc_t specific to the frame class.
 *  This function is also used by the button class of widgets as the only
 *  attributes who need to be freed in the structure of a button are the ones in
 *  common with that of a frame.
 */
void 		        frameclass_release	        (ei_widget_t *	widget);

/**
 * \brief  \ref ei_widgetclass_geomnotifyfunc_t specific to the frame class.
 * This function is also used by the button class of widgets.
 *  <ul>
 *			<li> Updates the widget's content rect, </li>
 *			<li> Adds the widget to a list of widget to update by calling \ref require_update. </li>
 *  </ul>
 */
void 		        frameclass_geomnotify	        (ei_widget_t *widget);

#endif