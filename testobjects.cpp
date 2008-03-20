#include "oogtk.h"

using namespace gtk;

class TestApp : public Application
{
        Window m_win;
    public:
        TestApp() : m_win("Test objects") {
            Button b("Ciao"), c("Come"), d("Stai");
            Button a = b; // A and B are the same object
            m_win.Child(HBox(VBox(a, c, d), 
                        VBox(Label("Label 1"), Label("Label 2"), Label("Label 3"))));
            m_win.ShowAll();
        }
};

int main()
{
    TestApp app;

    app.Run();
}
