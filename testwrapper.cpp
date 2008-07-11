#include "oogtk.h"

class MyApp : public gtk::Application {
    gtk::Window window_;
public:
    MyApp() : window_("Wrapper test!") { 
        window_.Child(gtk::Label("My window body"));
        window_.ShowAll();
    }
    gtk::Label &Label() {
        if (gtk::Label *l = dynamic_cast<gtk::Label *>(window_.Child()))
            return *l;
        else
            throw "AAAARGH, error!";
    }
};
int main() {
    MyApp app;
    std::cerr << "Label text: " << app.Label() << "\n";
}
