#ifndef MENU_H
#define MENU_H
#include <stddef.h>//size_t

//Interface for an extremely basic menu at the center of the screen.

void openMenu(const char* title, size_t numButtons, void (*funcs)(void));

//openMenu() just adds a menu to a list to draw later with this function.
void drawMenus(void);

void closeMenu(const char* title);

#endif