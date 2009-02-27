#include <iostream>
#include "oogtk.h"
#include <stdlib.h>

// this simple program test the use of OOGtk without the Application class
// helper.

static void clicked()
{
    std::cerr << "You clicked me!\n";
}

int main(int argc, char *argv[]) {
    // initialize GTK, you can provide argc / argv or ignore them 
    gtk::Application::Init(argc, argv);

    // create some objects
    gtk::Window win("Hello world window");
    gtk::Button button("Click me!");
    win.Border(16);
    win.Child(button);

    // attach a callback to them (to an external C function)
    win.OnDelete((GCallback)gtk::Application::Quit);
    // attach a callback to a local function
    button.OnClick((GCallback)clicked);

    // show the objects and run the app!
    win.ShowAll();

    gtk::Application::Run();

    exit(0);
}
