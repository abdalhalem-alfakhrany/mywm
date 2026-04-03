#ifndef CLIENT_H
#define CLIENT_H

#include <X11/Xlib.h>
#include <stdlib.h>
#include "window_group.h"

typedef struct client_
{
    Window window;
    Window wrapper;
    int x, y, width, height;
} Client;


// Client createClient(WindowGroup group, Window window);

#endif