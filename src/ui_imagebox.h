#ifndef UI_IMAGEBOX_H
#define UI_IMAGEBOX_H


#include "ui.h"

struct texture;

struct ui_imagebox {
    struct ui           ui;         /* inherit from ui, so this must be
                                     * the first element of the struct  */
    struct texture     *image;
};


/**
 * w and h specify the width and height of the image box. If w (or h) is equal
 * to zero, then w (or h) is assign to the image's width (or height).
 */
int ui_imagebox_init(struct ui_imagebox *ib,
                     int w, int h, struct texture *img);

void ui_imagebox_set_image(struct ui_imagebox *ib, struct texture *img);


#endif /* UI_IMAGEBOX_H */
