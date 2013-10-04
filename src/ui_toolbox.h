#ifndef UI_TOOLBOX_H
#define UI_TOOLBOX_H


#include <sf_list.h>

#include "ui.h"


struct ui_toolbox {
    struct ui           ui;         /* inherit from ui, so this must be
                                     * the first element of the struct  */
    uint8_t             background_color[4];
    struct sf_list     *buttons;    /* elt: (struct ui *) */
};


struct ui_toolbox *ui_toolbox_create(int w, int h, uint8_t r, uint8_t g,
                                     uint8_t b, uint8_t a);

void ui_toolbox_add_button(struct ui_toolbox *tb, struct ui *ui);

void ui_toolbox_resize(struct ui_toolbox *tb, int w, int h);

void ui_toolbox_move(struct ui_toolbox *tb, int x, int y);


#endif /* UI_TOOLBOX_H */