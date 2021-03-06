#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <sf/utils.h>

#include "../app.h"
#include "../window.h"
#include "../input_manager.h"
#include "../renderer2d.h"
#include "../ui.h"

static void on_glfw_error(int error, const char *desc) {
    return (void) fputs(desc, stderr);
}

static int glfw_init(void) {
    glfwSetErrorCallback(on_glfw_error);

    if (!glfwInit()) {
        return -1;
    }

    return 0;
}

static int init(void) {
    GLenum err;

    if (glfw_init() != 0) {
        return -1;
    }

    g_app.window = window_create(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!g_app.window) {
        glfwTerminate();
        return -1;
    }

    window_on_resize(app_on_resize);

    fprintf(stdout, "OpenGL Version: %s\n", glGetString(GL_VERSION));

    /*
     * GLEW has a problem with core contexts.
     * It calls glGetString(GL_EXTENSIONS), which causes GL_INVALID_ENUM
     * on GL 3.2+ core context as soon as glewInit() is called.
     * It also doesn't fetch the function pointers.
     */
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        return -1;
    }
    while ((err = glGetError()) != GL_NO_ERROR);

    g_app.im = input_manager_create(g_app.window);

    return 0;
}

static uint64_t sf_get_ticks(void) {
    struct timespec t;

    /*clock_gettime(CLOCK_REALTIME, &t);*/

    return t.tv_sec * 1e9 + t.tv_nsec;
}

int main(int argc, char *argv[]) {
    uint64_t cur_tick, last_tick;

    atexit((void (*)(void)) sf_memcheck);

    if (init() != 0) {
        return -1;
    }

    app_init(argv[0]);

    app_on_resize(g_app.window, g_app.window->w, g_app.window->h);

    cur_tick = sf_get_ticks();
    while (window_isopen()) {
        last_tick = cur_tick;
        cur_tick = sf_get_ticks();

        app_on_update((cur_tick - last_tick) * 1.0 / 1E9);

        app_on_render();

        glfwSwapInterval(0);
        glfwSwapBuffers(g_app.window->handle);
        glfwPollEvents();
    }

    app_destory();

    glfwTerminate();

    return 0;
}
