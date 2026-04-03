#include "window_group.h"

void createWindowGroup(Display *dpy, Window root, WindowGroup *group, int x, int y, int width, int height, int vGap, int hGap, Layout layout)
{
    const unsigned int BORDER_WIDTH = 4;
    const unsigned long BORDER_COLOR = 0xff0000;
    const unsigned long BG_COLOR = 0x0000ff;

    Window window = XCreateSimpleWindow(dpy, root, x, y, width, height, BORDER_WIDTH, BORDER_COLOR, BG_COLOR);
    XSelectInput(dpy, window, SubstructureRedirectMask | SubstructureNotifyMask | KeyPressMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask);
    XMapWindow(dpy, window);

    group->window = window;
    group->x = x;
    group->y = y;
    group->width = width;
    group->height = height;
    group->vGap = vGap;
    group->hGap = hGap;
    group->layout = layout;
    group->clinetsCount = 0;
    group->clinets = malloc(sizeof(Client) * 10);
}