#include "ei_widgetclass.h"
#include "ei_frame.h"
#include "ei_button.h"
#include "ei_toplevel.h"
#include "hw_interface.h"
#include <string.h>

ei_widgetclass_t *widgetclasses_chain = NULL;

void    ei_widgetclass_register (ei_widgetclass_t* widgetclass) {
	widgetclass->next       = widgetclasses_chain;
	widgetclasses_chain     = widgetclass;
}

ei_widgetclass_t*	ei_widgetclass_from_name	(ei_widgetclass_name_t name)
{
	ei_widgetclass_t *current_class         = widgetclasses_chain;
	while (current_class && strcmp(current_class->name, name))
	    current_class                       = current_class->next;

	return current_class;

}

void    ei_frame_register_class		(void)
{
	ei_widgetclass_t *frameclass    = malloc(sizeof(ei_widgetclass_t));
	strcpy(frameclass->name, "frame");
	frameclass->allocfunc           = &frameclass_alloc;
	frameclass->releasefunc         = &frameclass_release;
	frameclass->drawfunc            = &frameclass_draw;
	frameclass->setdefaultsfunc     = &frameclass_setdefaults;
	frameclass->geomnotifyfunc      = &frameclass_geomnotify;
	ei_widgetclass_register(frameclass);
}

void    ei_button_register_class	(void)
{
	/*registering of the button widgetclass*/
	ei_widgetclass_t *buttonclass   = malloc(sizeof(ei_widgetclass_t));
	strcpy(buttonclass->name, "button");
	buttonclass->allocfunc          = &buttonclass_alloc;
	buttonclass->releasefunc        = &frameclass_release;/* buttons and frames have the same release function */
	buttonclass->drawfunc           = &buttonclass_draw;
	buttonclass->setdefaultsfunc    = &buttonclass_setdefaults;
	buttonclass-> geomnotifyfunc    = &frameclass_geomnotify; /* buttons and frames have the same geomnotify function */
	ei_widgetclass_register(buttonclass);
	ei_bind(ei_ev_mouse_buttondown, NULL, "button", button_default_callback, NULL);
}

void    ei_toplevel_register_class	(void)
{

	/*registering of the toplevel widgetclass*/
	ei_widgetclass_t *toplevelclass         = malloc(sizeof(ei_widgetclass_t));
	strcpy(toplevelclass->name, "toplevel");
	toplevelclass->allocfunc                = &toplevelclass_alloc;
	toplevelclass->releasefunc              = &toplevelclass_release;
	toplevelclass->drawfunc                 = &toplevelclass_draw;
	toplevelclass->setdefaultsfunc          = &toplevelclass_setdefaults;
	toplevelclass->geomnotifyfunc           = &toplevelclass_geomnotify;
	ei_widgetclass_register(toplevelclass);

	/*callback specific to the toplevel widgetclass*/
	ei_bind(ei_ev_mouse_buttondown, NULL, "toplevel", &toplevel_default_callback, NULL);
}


