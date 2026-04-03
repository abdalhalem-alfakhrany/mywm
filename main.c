#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "utils.h"
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/Xutil.h>

Display *dpy;
Window root;
Cursor normal, move, horizontal_resize, vertical_resize;
XWindowAttributes rootWindowAttributes;
Atom isWindowWrapper;

const unsigned int BORDER_WIDTH = 0;
const unsigned long BORDER_COLOR = 0xff0000;
const unsigned long BG_COLOR = 0x0000ff;

typedef enum layout_
{
    Horinzontal,
    Vertical
} Layout;

typedef struct client_
{
    Window window;
    Window wrapper;
    int x, y, width, height, isFloating;
} Client;

typedef struct windowGroup_
{
    Window window;
    Client *clinets;
    int clinetsCount;
    int x, y, width, height, vGap, hGap;
    Layout layout;
} WindowGroup;

Client createClient(WindowGroup group, Window window)
{
    Client client = {0};

    Window wrapper = XCreateSimpleWindow(dpy, root, 0, 0, 1, 1, 0, 0x000000, BG_COLOR);
    XSelectInput(dpy, wrapper, SubstructureRedirectMask | SubstructureNotifyMask | KeyPressMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask);

    client.window = window;
    client.wrapper = wrapper;
    client.x = 0;
    client.y = 0;
    client.width = 0;
    client.height = 0;
    return client;
}

void drawClient(Display *dpy, Client client)
{
    XWindowChanges wrapperWindowChanges = {.x = client.x, .y = client.y, .width = client.width, .height = client.height};
    XConfigureWindow(dpy, client.wrapper, CWX | CWY | CWHeight | CWWidth, &wrapperWindowChanges);

    XWindowChanges clientWindowChanges = {.x = client.x, .y = client.y, .width = client.width, .height = client.height};
    XConfigureWindow(dpy, client.window, CWX | CWY | CWHeight | CWWidth, &clientWindowChanges);
    XReparentWindow(dpy, client.window, client.wrapper, 0, 0);
}

void drawWindowGroup(Display *dpy, WindowGroup group)
{
    XWindowChanges WindowGroupChanges = {.x = group.x, .y = group.y, .width = group.width, .height = group.height};
    XConfigureWindow(dpy, group.window, CWX | CWY | CWHeight | CWWidth, &WindowGroupChanges);
    for (size_t i = 0; i < group.clinetsCount; i++)
    {
        Client client = group.clinets[i];
        if (client.isFloating)
        {
        }
        else
        {
            if (group.layout == Horinzontal)
            {
                client.x = (i * group.width / group.clinetsCount) + ((i == 0) ? 2 : 0);
                client.y = 2;
                client.width = (group.width / group.clinetsCount) - ((i == 0) ? 4 : 0) - group.vGap;
                client.height = group.height - 2;
            }
            else if (group.layout == Vertical)
            {
                client.x = 4;
                client.y = (i * group.height / group.clinetsCount) + 4 + group.hGap;
                client.width = group.width - 8;
                client.height = (group.height / group.clinetsCount) - 8 - group.hGap;
            }
            drawClient(dpy, client);
        }
    }
}

void createWindowGroup(Display *dpy, Window root, WindowGroup *group, int x, int y, int width, int height, int vGap, int hGap, Layout layout)
{

    Window window = XCreateSimpleWindow(dpy, root, x, y, width, height, BORDER_WIDTH, BORDER_COLOR, BG_COLOR);
    XSelectInput(dpy, window, SubstructureRedirectMask | SubstructureNotifyMask | KeyPressMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask);
    XMapWindow(dpy, window);
    XFlush(dpy);
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

WindowGroup *groups;
WindowGroup *focusedGroup;

void initWindowGroups(int count)
{
    groups = malloc(sizeof(WindowGroup) * count);
    for (int i = 0; i < count; i++)
    {
        createWindowGroup(dpy, root, groups + i,
                          i * rootWindowAttributes.width / count + ((i == 0) ? 2 : 0),
                          2,
                          rootWindowAttributes.width / count - 4,
                          rootWindowAttributes.height - 4,
                          4, 4, Horinzontal);
        //   4, 4, i % 2 ? Vertical : Horinzontal);
        XClassHint hint;

        char buff1[256];

        sprintf(buff1, "window group %i", i + 1);
        hint.res_class = buff1;

        char buff2[256];
        sprintf(buff2, "WindowGroup %i", i + 1);
        hint.res_class = buff2;

        XSetClassHint(dpy, (groups + i)->window, &hint);
    }
}

int main()
{
    XEvent ev;

    if (!(dpy = XOpenDisplay(0x0)))
        return 1;

    root = DefaultRootWindow(dpy);
    XGetWindowAttributes(dpy, root, &rootWindowAttributes);
    normal = XCreateFontCursor(dpy, XC_left_ptr);
    move = XCreateFontCursor(dpy, XC_fleur);
    horizontal_resize = XCreateFontCursor(dpy, XC_sb_h_double_arrow);
    vertical_resize = XCreateFontCursor(dpy, XC_sb_v_double_arrow);

    XSelectInput(dpy, root, SubstructureRedirectMask | SubstructureNotifyMask | KeyPressMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask);
    XSync(dpy, 0);

    XDefineCursor(dpy, root, normal);
    XFlush(dpy);

    int running = 1;
    int groupsCount = 2;

    int pressedButton = -1;

    initWindowGroups(groupsCount);

    while (running)
    {
        XNextEvent(dpy, &ev);
        switch (ev.type)
        {
        case KeyPress:
        {
            KeySym keysym = XLookupKeysym(&ev.xkey, 0);

            if (ev.xkey.state & Mod1Mask)
            {
                switch (keysym)
                {
                case XK_r:
                {
                    // const char *xclock[] = {"rofi", "-show", "drun", NULL};
                    // spawn(xclock);

                    const char *xclock[] = {"xeyes", NULL};
                    spawn(xclock);
                    break;
                }
                case XK_q:
                {
                    printf("q pressed \n");
                    break;
                }
                case XK_Escape:
                {
                    running = 0;
                    break;
                }
                default:
                {
                    break;
                }
                }
                break;
            }
        }
        case MotionNotify:
        {
            // printf("is mod pressed %i\n", ev.xkey.state && Mod1Mask);
            printf("is left button pressed %i\n", pressedButton);
            if (ev.xkey.state && Mod1Mask && pressedButton == 3)
            {
                // XDefineCursor(dpy, root, horizontal_resize);
                // XFlush(dpy);
                focusedGroup->x += 10;
                for (size_t i = 0; i < groupsCount; i++)
                    drawWindowGroup(dpy, groups[i]);
            }
            // else
            // {
            //     XDefineCursor(dpy, root, normal);
            //     XFlush(dpy);
            // }
            break;
        }
        case ButtonRelease:
        {
            pressedButton = -1;
            XDefineCursor(dpy, root, normal);
            XFlush(dpy);
            break;
        }
        case ButtonPress:
        {
            // printf("is left button pressed %i\n", ev.xbutton.button);
            pressedButton = ev.xbutton.button;
            if (ev.xkey.state && Mod1Mask && pressedButton == 3)
            {
                XDefineCursor(dpy, root, horizontal_resize);
                XFlush(dpy);
            }
            // if (ev.xkey.state && Mod1Mask)
            // {
            //     XDefineCursor(dpy, root, move);
            //     XFlush(dpy);
            // }
            // else
            // {
            //     XDefineCursor(dpy, root, normal);
            //     XFlush(dpy);
            // }
            // XClassHint hint;
            // XGetClassHint(dpy, focusedGroup->window, &hint);
            // printf("instance: %s,class: %s\n", hint.res_name, hint.res_class);
            // break;
        }
        case EnterNotify:
        {
            int windowGroup = 0;
            for (size_t i = 0; i < groupsCount; i++)
            {
                if ((groups + i)->window == ev.xcrossing.window)
                {
                    windowGroup = 1;
                    focusedGroup = groups + i;
                    XSetWindowBorder(dpy, focusedGroup->window, 0x00ff00);
                }
            }
            if (!windowGroup)
                XSetWindowBorder(dpy, ev.xcrossing.window, 0xff00ff);
            break;
        }
        case LeaveNotify:
        {
            int windowGroup = 0;
            for (size_t i = 0; i < groupsCount; i++)
            {
                if ((groups + i)->window == ev.xcrossing.window)
                {
                    windowGroup = 1;
                    focusedGroup = groups + i;
                    XSetWindowBorder(dpy, focusedGroup->window, 0xff0000);
                }
            }
            if (!windowGroup)
                XSetWindowBorder(dpy, ev.xcrossing.window, 0xffff00);
            break;
        }
        case MapRequest:
        {
            focusedGroup->clinets[focusedGroup->clinetsCount] = createClient(*focusedGroup, ev.xmaprequest.window);
            XReparentWindow(dpy, focusedGroup->clinets[focusedGroup->clinetsCount].wrapper, focusedGroup->window, 0, 0);
            XMapWindow(dpy, focusedGroup->clinets[focusedGroup->clinetsCount].wrapper);
            XMapWindow(dpy, focusedGroup->clinets[focusedGroup->clinetsCount].window);
            focusedGroup->clinetsCount++;
            drawWindowGroup(dpy, *focusedGroup);
            break;
        }
        default:
            break;
        }
    }
}