/*
 * Module Name:		surface.c
 * Description:		Surface Manager
 */

#include <stdlib.h>

#include CONFIG_H
#include <stutter/macros.h>
#include <stutter/memory.h>
#include <stutter/signal.h>
#include <stutter/frontend/common/surface.h>

static int surface_initialized = 0;
static struct fe_surface *surface_list = NULL;
static struct fe_surface *current_surface = NULL;

struct fe_surface_type fe_surface_type = { {
	NULL,
	"surface",
	sizeof(struct fe_surface),
	NULL,
	(object_init_t) fe_surface_init,
	(object_release_t) fe_surface_release },
	(fe_surface_print_t) NULL,
	(fe_surface_clear_t) NULL,
	(fe_surface_move_t) NULL,
	(fe_surface_control_t) NULL
};

int init_surface(void)
{
	if (surface_initialized)
		return(0);
	surface_initialized = 1;
	return(0);
}

int release_surface(void)
{
	struct fe_surface *cur, *tmp;

	if (!surface_initialized)
		return(0);
	for (cur = surface_list; cur; cur = tmp) {
		tmp = cur->next;
		destroy_object(OBJECT_S(cur));
	}
	surface_initialized = 0;
	return(0);
}

int fe_surface_init(struct fe_surface *surface, const char *params, va_list va)
{
	surface->next = surface_list;
	surface_list = surface;
	if (!current_surface)
		current_surface = surface;
	return(0);
}

void fe_surface_release(struct fe_surface *surface)
{
	struct fe_surface *cur, *prev;
	// TODO should we still have these signals?
	//emit_signal(surface, "purge_object", NULL);
	//remove_signal(surface, NULL);

	for (cur = surface_list, prev = NULL; cur; prev = cur, cur = cur->next) {
		if (cur == surface) {
			if (prev)
				prev->next = cur->next;
			else
				surface_list = cur->next;
			break;
		}
	}
	if (surface == current_surface)
		current_surface = NULL;
}

/**
 * Refresh the given surface or all surfaces if NULL is given as the surface.
 */
void fe_surface_refresh(struct fe_surface *surface)
{
	struct fe_surface *cur;

	if (surface)
		FE_SURFACE_CONTROL(surface, SCC_REFRESH, 0);
	else {
		for (cur = surface_list; cur; cur = cur->next) {
			FE_SURFACE_CONTROL(cur, SCC_REFRESH, 0);
		}
	}
}

/**
 * Returns the current surface.
 */
struct fe_surface *fe_surface_get_current(void)
{
	return(current_surface);
}

/**
 * Sets the current surface.
 */
void fe_surface_set_current(struct fe_surface *surface)
{
	current_surface = surface;
}


