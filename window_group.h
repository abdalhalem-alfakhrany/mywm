#ifndef WINDOW_GROUP_H
#define WINDOW_GROUP_H

#include <X11/Xlib.h>
#include <stdlib.h>

#include "client.h"

typedef enum layout_
{
    Horinzontal,
    Vertical
} Layout;

typedef struct windowGroup_
{
    Window window;
    Client *clinets;
    int clinetsCount;
    int x, y, width, height, vGap, hGap;
    Layout layout;
} WindowGroup;

void createWindowGroup(Display *dpy, Window root, WindowGroup *group, int x, int y, int width, int height, int vGap, int hGap, Layout layout);
#endif