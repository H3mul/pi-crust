#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scancodes.h"
#include <X11/XKBlib.h>

#include "keyboard.h"

enum params
{          //argv-indices:
    P_EXE, //executable name
    P_DEV, //device file
    NUM_P  //number of parameters
};

enum errors
{
    ERR_SUCCESS,  //no error
    ERR_ARGCOUNT, //wrong number of arguments
    ERR_SYMBOL,   //symbol not in look up table
    ERR_LAYOUT,   //parameter P_LAY does not contain a correct keyboard layout
    ERR_LAZY      //i haven't done this
};

int main(int argc, char **argv)
{
    if (argc != NUM_P)
    {
        fprintf(stderr, "Usage: %s <device file> <layout> <unicode>\n", argv[P_EXE]);
        fprintf(stderr, "Takes string to type from stdin\n");
        fprintf(stderr, "<device file>:\ton the Raspberry Pi usually /dev/hidg0\n");

        return ERR_ARGCOUNT;
    }

    Display *display;
    Window window;
    XEvent event;
    Keyboard kb = Keyboard(argv[P_DEV]);

    int eventLoopRunOnce = 0;
    int s;

    printf("Starting...\n");
    // printf("Opening %s\n", argv[P_DEV]);

    /* open connection with the server */
    display = XOpenDisplay(NULL);
    if (display == NULL)
    {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }

    s = DefaultScreen(display);

    /* get the display width and height */
    int width = DisplayWidth(display, s);
    int height = DisplayHeight(display, s);

    /* create fullscreen window */
    window = XCreateSimpleWindow(display, RootWindow(display, s), 0, 0, width, height, 1,
                                 BlackPixel(display, s), WhitePixel(display, s));

    /* select kind of events we are interested in */
    XSelectInput(display, window, KeyPressMask | KeyReleaseMask | EnterWindowMask | LeaveWindowMask);

    /* map (show) the window */
    XMapWindow(display, window);

    printf("Starting event loop...\n");

    /* event loop */
    while (1)
    {
        XNextEvent(display, &event);

        /* keyboard events */
        if (event.type == KeyPress)
        {
            KeySym keysym = XLookupKeysym(&event.xkey, 0);

            kb.key_down_handler(keysym);

            /* exit on ESC key press */
            if (event.xkey.keycode == 0x09)
                break;
        }
        else if (event.type == KeyRelease)
        {
            KeySym keysym = XLookupKeysym(&event.xkey, 0);

            kb.key_up_handler(keysym);
        }
        /* dummy x11 screen entry/exit events */
        else if (event.type == EnterNotify && !eventLoopRunOnce)
        {
            eventLoopRunOnce = 1;
        }
        else if (event.type == EnterNotify && eventLoopRunOnce)
        {
            printf("EnterWindow\n");
        }
        else if (event.type == LeaveNotify)
        {
            printf("LeaveWindow\n");
        }

        kb.send_keyboard_reports();
    }

    /* close connection to server */
    XCloseDisplay(display);

    return 0;
}
