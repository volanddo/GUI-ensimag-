#include "ei_gridder.h"

ei_grid_t *grid_chain   = NULL;
int n_default_raw       = 3;
int n_default_line      = 3;

ei_bool_t get_state_case(ei_grid_t *grid, int x, int y){
        return grid->occuped[y*grid->n_row + x];
}

void set_state_case(ei_grid_t *grid, int x, int y, ei_bool_t value){
    if(x >= 0 && x < grid->n_row && y >= 0 && y < grid->n_line)
        grid->occuped[y*grid->n_row + x] = value;
}

static void found_grid(ei_widget_t* widget, ei_grid_t **_previous, ei_grid_t **_current)
{
        ei_grid_t *prev = NULL;
        ei_grid_t *current = grid_chain;
        while (current != NULL && current->widget != widget) {
                prev    = current;
                current = current->next;
        }
        *_previous = prev;
        *_current = current;
}

static void remove_grid	(struct ei_widget_t *widget)
{
        // On supprime la grille du widget si elle exist
        ei_grid_t *grid = NULL;
        ei_grid_t *previous = NULL;
        found_grid(widget, &previous, &grid);
        if (grid)
        {
                if(!grid->next)
                {
                        free(grid);
                        previous->next = NULL;
                }

                else if(grid == grid_chain)
                {
                        grid_chain = grid->next;
                        free(grid);
                }
                else
                {
                        previous->next = grid->next;
                        free(grid);
                }
        }

}

ei_point_t get_avaliable_case(ei_grid_t *grid)
{
        for(int y = 0; y < grid->n_line; y ++)
        {
                for(int x = 0; x < grid->n_row; x ++)
                {
                        if(get_state_case(grid, x,y))
                                return ei_point(x,y);
                }
        }
        return ei_point(-1,-1);
}

/// register gridder
void 			ei_register_gridder_manager 	(void)
{
        ei_geometrymanager_t *gridder   = malloc(sizeof(ei_geometrymanager_t));
        strcpy(gridder->name, "gridder");
        gridder->runfunc                = &gridder_runfunc;
        gridder->releasefunc            = &gridder_realasefunc;
        ei_geometrymanager_register(gridder);
}

ei_grid_t *create_grid(ei_widget_t* widget, const int *n_row, const int *n_line)
{
        //remove the previous grid if one was already create.
        remove_grid(widget);
        // Create and initialise a new grid.
        ei_grid_t *new_grid     = malloc(sizeof (ei_grid_t));
        new_grid->top_left      = widget->content_rect->top_left;
        new_grid->width         = widget->content_rect->size.width;
        new_grid->height        = widget->content_rect->size.height;
        new_grid->widget        = widget;
        if (n_line)
                new_grid->n_line        = *n_line;
        else
                new_grid->n_line        = n_default_line;
        if (n_row)
                new_grid->n_row         = *n_row;
        else
                new_grid->n_row         = n_default_raw;
        new_grid->next  = grid_chain;
        grid_chain      = new_grid;

        new_grid->occuped = calloc((new_grid->n_row)*(new_grid->n_line),sizeof (ei_bool_t));
        for(int i = 0; i<(new_grid->n_row)*(new_grid->n_line); i ++){
                new_grid->occuped[i] = EI_TRUE;
        }
        return new_grid;
}

ei_grid_t * update_grid(ei_widget_t* widget)
{
        ei_grid_t *current = grid_chain;
        while(current != NULL && current->widget != widget) {
                current = current->next;
        }
        if (current) {
                current->top_left       = current->widget->content_rect->top_left;
                current->width          = current->widget->content_rect->size.width;
                current->height         = current->widget->content_rect->size.height;
        }
        return current;
}
void check_x_y(int *x, int *y, int *y_end, const int *x_end)
{
        if(x)           { x = *x >= 0? x: NULL; }
        if(y)           { y = *y >= 0? y: NULL; }
        if(y_end)       { y_end = *y_end >= 0? y_end: NULL; }
        if(x_end)       { x_end = *x_end >= 0? x_end: NULL; }
        if(x && x_end)  {
                int x_val = *x;
                int x_end_val = *x_end;
                *x = *x < *x_end? x_val: x_end_val;
        }
        if(y && y_end){
                int y_val = *y;
                int y_end_val = *y_end;
                *y = *y < *y_end? y_val: y_end_val;
        }

}
extern void			ei_grid			(ei_widget_t*		widget,
                                    int*			x,
                                    int*			y,
                                    int*			x_end,
                                    int*			y_end,
                                    const int*			width,
                                    const int*			height,
                                    const float*		rel_width,
                                    const float*		rel_height)
{
        // declaration of variable
        ei_grid_param_t *param_widget = NULL;
        ei_geometrymanager_t *current_widget_manager = NULL;
        //get the current geom manager and replace it by gridder.
        if(widget->geom_params)
        {
                current_widget_manager = widget->geom_params->manager;
        }
        ei_geometrymanager_t *gridder= ei_geometrymanager_from_name("gridder");
        if (!current_widget_manager) { /* si le widget n'est géré par aucun geometry manager */
                param_widget = malloc(sizeof(ei_grid_param_t));
        }
        else if (current_widget_manager != gridder) { /*si le widget est géré par un autre geometry manager que le gridder*/
                widget->geom_params->manager->releasefunc(widget);
                free(widget->geom_params);
                param_widget = malloc(sizeof(ei_grid_param_t));
        }
        else { /*si le widget est géré par le gridder*/
                param_widget = (ei_grid_param_t *) widget->geom_params;
        }

        ei_grid_t *parent_grid = update_grid(widget->parent);
        if (!parent_grid)
        {
                parent_grid = create_grid(widget->parent, NULL, NULL);
        }

        // Check validity of entry.
        check_x_y(x,y, y_end, x_end);

        // Set default param_widget setting
        param_widget->geomparam.manager = gridder;
        param_widget->x = x ? *x : 0;
        param_widget->y = y ? *y : 0;
        param_widget->x_end= x_end;
        param_widget->y_end = y_end;

        if(!x && !y && !y_end && !x_end){
                ei_point_t p = get_avaliable_case(parent_grid);

                param_widget->x = p.x;
                param_widget->y = p.y;
        }

        param_widget->rel_height = 0.0;
        param_widget->rel_width = 0.0;

        // Set param with param (absolute, relative or nothing is given).
        if (width){ param_widget->width = *width;}
        else if (rel_width){ param_widget->width = 0; }
        else { param_widget->rel_width = 1.0; }

        if (height){ param_widget->height = *height; }

        else if (rel_height){ param_widget->height = 0; }

        else { param_widget->rel_height = 1.0; }

        param_widget->rel_width         = rel_width ? *rel_width : param_widget->rel_width  ;
        param_widget->rel_height        = rel_height ? *rel_height : param_widget->rel_width;

        widget->geom_params = (ei_geometry_param_t *) param_widget;
        ei_app_invalidate_rect(&widget->screen_location);
        widget->geom_params->manager->runfunc(widget);
        ei_app_invalidate_rect(&widget->screen_location);
}

/**
 * \brief Returns the minimum between the integers @param a and @param b
 */
static int min_int(int a, int b)
{
        return a<b ? a : b;
}

extern void gridder_runfunc	(struct ei_widget_t *widget)
{
        int x, y;
        int y_end_nn = 0, x_end_nn = 0;
        ei_point_t  w_top_left_pos;
        ei_size_t w_size, max_size;
        ei_grid_t *parent_grid;
        ei_grid_param_t *gridder_param = (ei_grid_param_t *)widget->geom_params;

        parent_grid = update_grid(widget->parent);
        if (!parent_grid)
        {
                parent_grid = create_grid(widget->parent, NULL, NULL);
        }
        if (gridder_param->x_end)
        {
                x_end_nn = gridder_param->x_end ? *gridder_param->x_end-gridder_param->x: 0;
        }
        if (gridder_param->y_end)
        {
                y_end_nn = gridder_param->y_end ? *gridder_param->y_end-gridder_param->y: 0;
        }

        for(int i = gridder_param->x; i <= gridder_param->x + x_end_nn; i ++)
        {
                for(int j = gridder_param->y; j <= gridder_param->y + y_end_nn; j ++)
                {
                        set_state_case(parent_grid, i,j,EI_FALSE);
                }
        }

        max_size = ei_size(
             (parent_grid->width/parent_grid->n_row)*(1+x_end_nn) +1,
            (parent_grid->height/parent_grid->n_line)*(1+y_end_nn) +1
        );
        w_size = max_size;

        if(gridder_param->rel_width)
        {
                w_size.width = min_int((int)round(w_size.width*gridder_param->rel_width), w_size.width);

        }
        else
        {
                w_size.width = min_int(w_size.width, gridder_param->width);
        }

        if(gridder_param->rel_height)
        {
                w_size.height = min_int((int)round((double)w_size.height*gridder_param->rel_height), w_size.height);
        }
        else
        {
                w_size.height = min_int(w_size.height, gridder_param->height);

        }

        x = (int)round(((double)parent_grid->width/(double)parent_grid->n_row)*(double)gridder_param->x + (double)parent_grid->top_left.x);
        y = (int)round((((double)parent_grid->height/(double)parent_grid->n_line)*(double)gridder_param->y+(double)parent_grid->top_left.y));

        w_top_left_pos = ei_point( x + (max_size.width - w_size.width)/2,
                               y + (max_size.height -w_size.height)/2
        );

        widget->screen_location = ei_rect(w_top_left_pos, w_size);
        widget->wclass->geomnotifyfunc(widget);

        }

extern void gridder_realasefunc(struct ei_widget_t *widget)
{

        if(widget->content_rect)
        {
                return;
        }

        remove_grid(widget);
        ei_grid_param_t *gridder_param = (ei_grid_param_t *)widget->geom_params;
        int x_end_nn, y_end_nn;
        if (gridder_param->x_end)
        {
                x_end_nn = gridder_param->x_end ? *gridder_param->x_end-gridder_param->x: 0;
        }
        if (gridder_param->y_end)
        {
                y_end_nn = gridder_param->y_end ? *gridder_param->y_end-gridder_param->y: 0;
        }
        ei_grid_t *parent_grid = update_grid(widget->parent);
        for(int i = gridder_param->x; i <= gridder_param->x + x_end_nn; i ++)
        {
                for(int j = gridder_param->y; j <= gridder_param->y + y_end_nn; j ++)
                {
                        set_state_case(parent_grid, i,j,EI_TRUE);
                }
        }

}