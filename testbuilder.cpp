// test del builder
#include "oogtk.h"

class MyApp : public gtk::Application, public gtk::Builder
{
public:
    MyApp() {
        Load("./config.ui");
        
        std::cerr << "Interfaccia costruita.\n";

        Get("button1")->Tooltip("Questo widget non fa <b>NULLA</b>");
        Get<gtk::Button>("button1")->OnClick(&MyApp::mycbk, this);
    }
    void Show() {
        if (gtk::Window *w = Get<gtk::Window>("window_main"))
            w->Show();
    }
    void mycbk() {
        Quit();
    }
};

int main()
{
    MyApp app;
    app.Show();
    app.Run();
}

