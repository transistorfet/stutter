/*
 * Header Name:		menu.h
 * Version:		0.1
 * Description:		Menu Manager Header
 */

#ifndef _WIN32_CORE_MENU_H
#define _WIN32_CORE_MENU_H

#include <windows.h>

#include <stutter/globals.h>
#include <stutter/frontend/common/layout.h>

struct menu_s;

int init_menu(void);
int release_menu(void);

int destroy_menu(struct menu_s *);
struct menu_s *generate_menu(void *, struct property_s *, struct layout_s *);
int attach_menu(struct menu_s *, HWND);

int menu_process_request(short);

#endif


