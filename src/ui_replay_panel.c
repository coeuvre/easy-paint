#include <stdlib.h>

#include <sf_utils.h>

#include "ui_replay_panel.h"
#include "system.h"


#define TOOLBOX_HEIGHT 48
#define TOOLBOX_MIN_WIDTH 288

#define REPLAY_SPEED_MIN     128
#define REPLAY_SPEED_MAX     5120
#define REPLAY_SPEED_DEFAULT 512
#define REPLAY_SPPED_DELTA   128


static void ui_replay_panel_reset(struct ui_replay_panel *urp) {
    urp->isreplay = 0;
    ui_imagebox_set_image(&urp->replay, urp->play_image);

    urp->isstop = 1;
    while (record_canredo(&urp->record)) {
        record_redo(&urp->record, &urp->canvas);
    }
}

static void replay_on_press(struct ui_imagebox *replay,
                            int n, int x[n], int y[n]) {
    struct ui_replay_panel *urp =
        sf_container_of(replay, struct ui_replay_panel, replay);

    if (urp->isstop) {
        urp->isstop = 0;
        while (record_canundo(&urp->record)) {
            record_undo(&urp->record, &urp->canvas);
        }
    }

    if (urp->isreplay) {
        urp->isreplay = 0;
        ui_imagebox_set_image(replay, urp->play_image);
    } else {
        urp->isreplay = 1;
        ui_imagebox_set_image(replay, urp->pause_image);
    }
}

static void stop_on_press(struct ui_imagebox *stop,
                          int n, int x[n], int y[n]) {
    struct ui_replay_panel *urp =
        sf_container_of(stop, struct ui_replay_panel, stop);
    ui_replay_panel_reset(urp);
}

static void rewind_on_press(struct ui_imagebox *rewind,
                            int n, int x[n], int y[n]) {
    struct ui_replay_panel *urp =
        sf_container_of(rewind, struct ui_replay_panel, rewind);

    urp->replay_speed -= REPLAY_SPPED_DELTA;
    if (urp->replay_speed < REPLAY_SPEED_MIN) {
        urp->replay_speed = REPLAY_SPEED_MIN;
    }
}

static void fastforward_on_press(struct ui_imagebox *fastforward,
                                 int n, int x[n], int y[n]) {
    struct ui_replay_panel *urp =
        sf_container_of(fastforward, struct ui_replay_panel, fastforward);

    urp->replay_speed += REPLAY_SPPED_DELTA;
    if (urp->replay_speed > REPLAY_SPEED_MAX) {
        urp->replay_speed = REPLAY_SPEED_MAX;
    }

}

static void canvas_on_press(struct canvas *canvas,
                            int n, int x[n], int y[n]) {
}

static void canvas_on_update(struct canvas *canvas, struct input_manager *im,
                             double dt) {
    static double ntoreplay = 0;

    struct ui_replay_panel *urp =
        sf_container_of(canvas, struct ui_replay_panel, canvas);
#if 0
    if (im->keys[KEY_1] == KEY_PRESS) {
        record_save(&urp->record,
                    get_save_file_name("Record File(*.rec)\0*.rec\0"));
    } else
#endif
    if (im->keys[KEY_2] == KEY_PRESS) {
        if (record_load(&urp->record,
                    get_open_file_name("Record File(*.rec)\0*.rec\0")) == 0) {
            canvas_clear(&urp->canvas);
            record_adjust(&urp->record, 10, 10);
            ui_replay_panel_reset(urp);
        }
    }

    if (urp->isreplay) {
        if (record_canredo(&urp->record)) {
            int n;
            ntoreplay += urp->replay_speed * dt;
            n = ntoreplay;
            if (n > 0) {
                record_redo_n(&urp->record, &urp->canvas, n);
                ntoreplay -= n;
            }
        } else {
            ui_replay_panel_reset(urp);
        }
    }
}

static void ui_replay_panel_on_push(struct ui_replay_panel *urp,
                                    struct ui_manager *uim, int x, int y) {
    ui_manager_push(uim, x, y, (struct ui *) &urp->canvas);
    ui_manager_push(uim, x, y + urp->canvas.ui.area.h - TOOLBOX_HEIGHT,
                    (struct ui *) &urp->toolbox);
}

static void ui_replay_panel_on_show(struct ui_replay_panel *urp) {
    ui_show((struct ui *) &urp->canvas);
    ui_show((struct ui *) &urp->toolbox);
}

static void ui_replay_panel_on_hide(struct ui_replay_panel *urp) {
    ui_hide((struct ui *) &urp->canvas);
    ui_hide((struct ui *) &urp->toolbox);
}


struct ui_replay_panel *ui_replay_panel_create(int w, int h) {
    struct ui_replay_panel *urp;

    urp = malloc(sizeof(*urp));
    ui_replay_panel_init(urp, w, h);
    return urp;
}

int ui_replay_panel_init(struct ui_replay_panel *urp, int w, int h) {
    ui_init((struct ui *) urp, w, h);

    urp->isreplay = 0;
    urp->isstop = 1;
    urp->replay_speed = REPLAY_SPEED_DEFAULT;

    canvas_init(&urp->canvas, w, h);
    UI_CALLBACK(&urp->canvas, update, canvas_on_update);
    UI_CALLBACK(&urp->canvas, press, canvas_on_press);

    record_init(&urp->record);

    urp->stop_image = texture_load_2d("res/icons/stop.png");
    ui_imagebox_init(&urp->stop, 0, 0, urp->stop_image);
    UI_CALLBACK(&urp->stop, press, stop_on_press);

    urp->pause_image = texture_load_2d("res/icons/pause.png");
    urp->play_image = texture_load_2d("res/icons/play.png");
    ui_imagebox_init(&urp->replay, 0, 0, urp->play_image);
    UI_CALLBACK(&urp->replay, press, replay_on_press);

    urp->fastforward_image = texture_load_2d("res/icons/fastforward.png");
    ui_imagebox_init(&urp->fastforward, 0, 0, urp->fastforward_image);
    UI_CALLBACK(&urp->fastforward, press, fastforward_on_press);

    urp->rewind_image = texture_load_2d("res/icons/rewind.png");
    ui_imagebox_init(&urp->rewind, 0, 0, urp->rewind_image);
    UI_CALLBACK(&urp->rewind, press, rewind_on_press);

    ui_toolbox_init(&urp->toolbox, w, TOOLBOX_HEIGHT, 128, 128, 128, 240);
    ui_toolbox_add_button(&urp->toolbox, (struct ui *) &urp->replay);
    ui_toolbox_add_button(&urp->toolbox, (struct ui *) &urp->stop);
    ui_toolbox_add_button(&urp->toolbox, (struct ui *) &urp->rewind);
    ui_toolbox_add_button(&urp->toolbox, (struct ui *) &urp->fastforward);

    UI_CALLBACK(urp, push, ui_replay_panel_on_push);
    UI_CALLBACK(urp, show, ui_replay_panel_on_show);
    UI_CALLBACK(urp, hide, ui_replay_panel_on_hide);

    return 0;
}