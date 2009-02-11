// test del builder
#include "oogtk.h"

class MyApp : public gtk::Application, public gtk::Builder
{
public:
    MyApp(const std::string &name) {
// this should be built with glade-3 or converted through gtk-builder-convert
        Load(name);
        
        std::cerr << "The interface has been loaded.\n";

// attach some callbacks or tooltips to the interface widget.
        Get("button1")->Tooltip("This widget <b>CLOSES</b> the app!");
        Get<gtk::Button>("button1")->OnClick(&MyApp::mycbk, this);
        Get("button2")->Tooltip("This widget does <b>NOTHING</b>!");

// glade-3 let you specify treestore/liststore in the interface definition.
	if (gtk::TreeStore *s = Get<gtk::TreeStore>("treestore1")) {
// add a pair of rows to the tree, using AddTail without iterator adds the row in the root node,
// in this example the treestore is used as a liststore, note that the odd parameters of the 
// function call are the index of the column in the model (starting from 0) 
	    s->AddTail(1, "One Two Three", 2, 123, -1);
	    s->AddTail(1, "Three Three Three", 2, 333, -1);

// add a pair of columns to the treeview, unfortunately glade don't let you do this yet.
	    if (gtk::TreeView *t = Get<gtk::TreeView>("treeview1")) {
		t->AddTextColumn("String", 1);
		t->AddTextColumn("Value", 2);
	    }
	    else
		std::cerr << "Unable to find TreeView!\n";
	}
	else 
  	    std::cerr << "Unable to find TreeStore!\n";

// make the app quit when the user closes the window
	Get<gtk::Window>("window1")->OnDelete(&Application::QuitLoop, dynamic_cast<Application *>(this), true);

	std::cerr << "The interface has been configured.\n";
    }
    void Show() {
        if (gtk::Window *w = Get<gtk::Window>("window1"))
            w->Show();
    }
    void mycbk() {
        Quit();
    }
};

int main()
{
// create our app
    MyApp app("config.ui");
// show the interface!
    app.Show();
// run the app!
    app.Run();
}

