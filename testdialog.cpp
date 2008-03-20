#include "oogtk.h"

using namespace gtk;

class MyApp : public Application
{
    private:
        Dialog m_dialog;
    public:
        MyApp() {
            HBox box;
            Label l1("Ciao!");
            Label l2("Prova!");
            box.PackStart(l1);
            box.PackStart(l2);
            box.ShowAll();
            m_dialog.AddBody(box);
            m_dialog.AddButton("Ok", 0);
            m_dialog.AddButton("Fanculo", 1);
        }
        void RunDialog() {
            m_dialog.Run();
        }
};

int main()
{
    MyApp a;

    a.RunDialog();
}
