/*
 * Module Name:		menu.c
 * System Requirements:	Windows OS
 * Description:		Win32 Menu Manager
 */

#include <string.h>
#include <stdarg.h>
#include <windows.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/memory.h>
#include <stutter/string.h>
#include <stutter/globals.h>
#include <stutter/frontend/common/layout.h>
#include "menu.h"

#define MENU_ITEMS_INIT_SIZE		100
#define MENU_ITEMS_GROW_FACTOR		1.75

struct menu_item_s {
	int id;
	char *cmd;
	char *args;
};

struct menu_s {
	HWND hwnd;
	HMENU hmenu;
};

static int menu_next_id = 0;
static struct menu_item_s **menu_items = NULL;
static int menu_items_size = MENU_ITEMS_INIT_SIZE;

static struct menu_item_s *create_menu_item(char *, char *);
static int menu_get_next_id(void);
static int menu_grow_items_list(int);

int init_menu(void)
{
	if (menu_items)
		return(0);
	if (!(menu_items = (struct menu_item_s **) memory_alloc(sizeof(struct menu_item_s *) * menu_items_size)))
		return(-1);
	memset(menu_items, '\0', sizeof(struct menu_item_s *) * MENU_ITEMS_INIT_SIZE);
	layout_register_type("menu", LAYOUT_RT_MENU, (layout_create_t) generate_menu, NULL);
	return(0);
}

int release_menu(void)
{
	int i;

	if (!menu_items)
		return(0);
	layout_unregister_type("menu");
	for (i = 0;i < menu_items_size;i++) {
		memory_free(menu_items[i]);
	}
	memory_free(menu_items);
	return(0);
}

int destroy_menu(struct menu_s *menu)
{
	if (menu->hmenu)
		DestroyMenu(menu->hmenu);
	memory_free(menu);
	return(0);
}

struct menu_s *generate_menu(void *ptr, struct property_s *props, struct layout_s *children)
{
	HMENU file;
	MENUITEMINFO info;
	struct menu_s *menu;
	struct menu_item_s *item;

	if (!(menu = (struct menu_s *) memory_alloc(sizeof(struct menu_s))))
		return(NULL);
	menu->hwnd = NULL;
	menu->hmenu = NULL;

// MOVE THIS vvvvvvvvvvv
	if (!(menu->hmenu = CreateMenu()))
		return(NULL);
	if (!(file = CreateMenu()))
		return(NULL);
	memset(&info, '\0', sizeof(MENUITEMINFO));
	info.cbSize = sizeof(MENUITEMINFO);

	item = create_menu_item("irc.server", "irc.neovanglist.net");
	info.fMask = MIIM_STRING | MIIM_ID;
	info.wID = item->id;
	info.dwTypeData = "&Connect to Otherside";
	InsertMenuItem(file, 0, 1, &info);

	item = create_menu_item("base.quit", "");
	info.fMask = MIIM_STRING | MIIM_ID;
	info.wID = item->id;
	info.dwTypeData = "E&xit";
	InsertMenuItem(file, 1, 1, &info);

	info.fMask = MIIM_STRING | MIIM_SUBMENU;
	info.wID = 0;
	info.hSubMenu = file;
	info.dwTypeData = "&File";
	InsertMenuItem(menu->hmenu, 0, 1, &info);
// MOVE THIS ^^^^^^^^^^

	return(menu);
}

int attach_menu(struct menu_s *menu, HWND hwnd)
{
	if (menu->hwnd)
		return(-1);
	menu->hwnd = hwnd;
	SetMenu(hwnd, menu->hmenu);
	DrawMenuBar(hwnd);
	return(0);
}

int menu_process_request(short id)
{
	char *args;

	if ((id >= menu_items_size) || (!menu_items[id]))
		return(-1);
	if (!(args = duplicate_string(menu_items[id]->args)))
		return(-1);
	util_evaluate_command(menu_items[id]->cmd, args);
	return(0);
}

/*** Local Functions ***/

static struct menu_item_s *create_menu_item(char *cmd, char *args)
{
	int id;
	int cmd_len = 0;
	struct menu_item_s *item;

	if ((id = menu_get_next_id()) == -1)
		return(NULL);
	if (cmd)
		cmd_len = strlen(cmd) + 1;
	if (!(item = (struct menu_item_s *) memory_alloc(sizeof(struct menu_item_s) + cmd_len + strlen(args) + 1)))
		return(NULL);
	item->id = id;
	item->cmd = (char *) (((size_t) item) + sizeof(struct menu_item_s));
	strcpy(item->cmd, cmd);
	item->args = (char *) (((size_t) item->cmd) + cmd_len);
	strcpy(item->args, args);
	menu_items[id] = item;
	return(item);
}

static int menu_get_next_id(void)
{
	int id;

	if (menu_items[menu_next_id])
		return(-1);
	id = menu_next_id++;
	for (;menu_next_id != id;menu_next_id++) {
		if (menu_next_id >= menu_items_size)
			menu_next_id = 0;
		if (!menu_items[menu_next_id])
			return(id);
	}
	menu_next_id = menu_items_size;
	if (menu_grow_items_list(menu_items_size * MENU_ITEMS_GROW_FACTOR))
		menu_next_id = 0;
	return(id);
}

static int menu_grow_items_list(int newsize)
{
	if (newsize <= menu_items_size)
		return(-1);
	if (!(menu_items = (struct menu_item_s **) memory_realloc(menu_items, sizeof(struct menu_item_s *) * newsize)))
		return(-1);
	memset(&menu_items[menu_items_size], '\0', sizeof(struct menu_item_s *) * (menu_items_size - newsize));
	menu_items_size = newsize;
	return(0);
}


