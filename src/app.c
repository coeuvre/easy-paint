#ifdef ANDROID
#include <GLES/gl.h>
#else /* ANDROID */
#include <GL/glew.h>
#endif /* ANDROID */
#include <sf_utils.h>
#include <sf_debug.h>

#include "app.h"
#include "user_paint_panel.h"
#include "user_learn_panel.h"
#include "ui_imagebox.h"
#include "ui_menu.h"
#include "texture.h"
#include "resource_manager.h"


struct app g_app;
static void menuicon_on_press(struct ui *ui, int n, int x[n], int y[n]) {
    ui_show((struct ui *) g_app.menu);
}

static void menu_on_press(struct ui *ui, int n, int x[n], int y[n]) {
    ui_hide((struct ui *) g_app.menu);
}

static void app_change_stage(int stage) {
    switch (stage) {
    case APP_STAGE_DOODLE:
        ui_hide((struct ui *) g_app.ulp);
        ui_show((struct ui *) g_app.upp);
        break;
    case APP_STAGE_TEACHING:
        ui_hide((struct ui *) g_app.upp);
        ui_show((struct ui *) g_app.ulp);
        break;
    }
}

static void menu_item_on_press(struct ui *ui, int n, int x[n], int y[n]) {
    int i = 0;
    SF_LIST_BEGIN(g_app.menu->items, struct ui *, p);
        struct ui *item = *p;
        if (item == ui) {
            app_change_stage(i);
            ui_hide((struct ui *) g_app.menu);
            return;
        }
        ++i;
    SF_LIST_END();
}

void app_load_resource(const char *rootpath) {
    int i;

    g_app.rm = resource_manager_create(rootpath);

    for (i = 0; i < RESOURCE_NTEXTURES; ++i) {
        resource_manager_load(g_app.rm, RESOURCE_TEXTURE, i);
    }
}

int app_init(void) {
    g_app.renderer2d = renderer2d_create(g_app.window->w, g_app.window->h);
    g_app.uim = ui_manager_create();

    g_app.upp = user_paint_panel_create(g_app.window->w, g_app.window->h,
                                        g_app.rm);
    ui_manager_push(g_app.uim, 0, 0, (struct ui *) g_app.upp);

    g_app.ulp = user_learn_panel_create(g_app.window->w, g_app.window->h,
                                        g_app.rm);
    ui_manager_push(g_app.uim, 0, 0, (struct ui *) g_app.ulp);

    g_app.menuicon = ui_imagebox_create(
        0, 0, resource_manager_get(g_app.rm, RESOURCE_TEXTURE,
                                   RESOURCE_TEXTURE_ICON_PARENT));
    UI_CALLBACK(g_app.menuicon, press, menuicon_on_press);
    ui_manager_push(g_app.uim, 0, g_app.window->h - g_app.menuicon->ui.area.h,
                    (struct ui *) g_app.menuicon);

    g_app.logo = ui_imagebox_create(
        0, 0, resource_manager_get(g_app.rm, RESOURCE_TEXTURE,
                                   RESOURCE_TEXTURE_ICON_LOGO));
    g_app.label1 = ui_imagebox_create(
        0, 0, resource_manager_get(g_app.rm, RESOURCE_TEXTURE,
                                   RESOURCE_TEXTURE_ICON_LABEL1));

    g_app.label2 = ui_imagebox_create(
        0, 0, resource_manager_get(g_app.rm, RESOURCE_TEXTURE,
                                   RESOURCE_TEXTURE_ICON_LABEL2));

    /*g_app.label3 = ui_imagebox_create(*/
        /*0, 0, resource_manager_get(g_app.rm, RESOURCE_TEXTURE,*/
                                   /*RESOURCE_TEXTURE_ICON_LABEL3));*/

    g_app.menu = ui_menu_create(256, g_app.window->h);
    UI_CALLBACK(g_app.menu, press, menu_on_press);
    ui_menu_set_background_color(g_app.menu, 64, 64, 64, 250);
    ui_menu_add_item(g_app.menu, (struct ui *) g_app.logo);
    ui_menu_add_item(g_app.menu, (struct ui *) g_app.label1);
    ui_menu_add_item(g_app.menu, (struct ui *) g_app.label2);
    /*ui_menu_add_item(g_app.menu, (struct ui *) g_app.label3);*/
    {
        int i = 0;
        SF_LIST_BEGIN(g_app.menu->items, struct ui *, p);
            struct ui *item = *p;
            if (i != 0) {
                UI_CALLBACK(item, press, menu_item_on_press);
            }
            ++i;
        SF_LIST_END();
    }
    ui_manager_push(g_app.uim, 0, 0, (struct ui *) g_app.menu);
    ui_hide((struct ui *) g_app.menu);

    app_change_stage(APP_STAGE_DOODLE);

    return 0;
}

void app_on_resize(struct window *win, int w, int h) {
    renderer2d_resize(g_app.renderer2d, w, h);

    user_paint_panel_resize(g_app.upp, w, h);

    user_learn_panel_resize(g_app.ulp, w, h);

    g_app.menuicon->ui.area.y = h - g_app.menuicon->ui.area.h;
    g_app.menu->ui.area.h = h;
}

#if 0
static void handle_mouse_button_right(void) {
    static int lastx, lasty;

    if (g_app.im->keys[KEY_MB_RIGHT] == KEY_PRESS) {
        lastx = g_app.im->mouse.x;
        lasty = g_app.im->mouse.y;
    }

    if (g_app.im->keys[KEY_MB_RIGHT] == KEY_REPEAT
        && (g_app.im->mouse.x != lastx || g_app.im->mouse.y != lasty)) {
        canvas_offset(&g_app.upp->canvas, lastx - g_app.im->mouse.x,
                      lasty - g_app.im->mouse.y);
        lastx = g_app.im->mouse.x;
        lasty = g_app.im->mouse.y;
    }

}
#endif

void app_on_update(double dt) {
#if 0
    handle_mouse_button_right();
    if (g_app.im->keys[KEY_UP] == KEY_PRESS) {
        int x, y;
        canvas_screen_to_canvas(g_app.canvas, g_app.im->mouse.x,
                                g_app.im->mouse.y, &x, &y);
        canvas_zoom_in(g_app.canvas, x, y);
    } else if (g_app.im->keys[KEY_DOWN] == KEY_PRESS) {
        int x, y;
        canvas_screen_to_canvas(g_app.canvas, g_app.im->mouse.x,
                                g_app.im->mouse.y, &x, &y);
        canvas_zoom_out(g_app.canvas, x, y);
    }
#endif
    static int cnt = 0;
    static double elapse = 0;

    ++cnt;
    elapse += dt;
    if (elapse > 1.0) {
        dprintf("FPS: %d\n", cnt);
        cnt = 0;
        elapse -= 1.0;
    }

    input_manager_update();
    ui_manager_update(g_app.uim, g_app.im, dt);
}

void app_on_render(void) {
    ui_manager_render(g_app.uim, g_app.renderer2d);
}
