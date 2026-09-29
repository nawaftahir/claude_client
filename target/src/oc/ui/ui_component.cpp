#include "common_types.h"
#include "imports.h"
#include "bytematch.h"
#include <string.h>

extern dvar_t *com_developer;
extern clientStatic_t cls;

extern UI_Component_data_t __ZN12UI_Component1gE;
#define UI_Component_g __ZN12UI_Component1gE
extern refexport_t re;
extern void *imp_com_developer;

void UI_Component_Init(void);

void UI_Component_Init(void)
{
    memset(&UI_Component_g, 0, sizeof(UI_Component_data_t));

    UI_Component_g.screenWidth = (float)cls.vidConfig.width;
    UI_Component_g.screenHeight = (float)cls.vidConfig.height;

    UI_Component_g.charWidth = 8.0f;
    UI_Component_g.charHeight = 16.0f;
    UI_Component_g.scrollBarSize = 16.0f;

    if (com_developer->current.integer)
    {
        UI_Component_g.cursor = re.RegisterMaterial("ui/assets/3_cursor3", 0x30, 1);
        UI_Component_g.filledCircle = re.RegisterMaterial("ui/assets/sliderbutt_1", 0x30, 1);
    }
}
