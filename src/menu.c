#include "menu.h"
#include <raylib.h>
#include <stdbool.h>

#define MAX_NUM_BUTTONS 5

typedef void (*buttonAction)(void);

typedef struct
{
    bool isMenuOpen;
    size_t numButtons;
    buttonAction actions[MAX_NUM_BUTTONS];
    size_t numFuncs;
} MenuInfo;

static MenuInfo sMenuInfo = {.isMenuOpen = false};

/*
void openMenu(const char* title, size_t numButtons, void (*funcs)(void))
{
    sMenuInfo.isMenuOpen = true;
    
    sMenuInfo.funcs = (void (*)(void))calloc(numButtons, );
    
    for(int i = 0; i < numButtons; ++i)
    {
        funcs[i]
    }
}
*/