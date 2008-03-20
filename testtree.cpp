#include "oogtk.h"
#include <sstream>

GType columns[] = { G_TYPE_STRING, G_TYPE_STRING};

class MyApp : public gtk::Application
{
    gtk::Window win;
    gtk::Button bt;
    gtk::Button next;
    gtk::VBox box;
    gtk::HBox bbox;
    gtk::ListStore list;
    gtk::TreeView tv;
    gtk::ScrolledWindow sw;
public:
    MyApp() : win("Test TreeView"), bt("Uscita"), next("Prova un albero"), list(2, columns), tv(list) {
        win.Child(box);
        win.Border(8);
        box.PackStart(sw);
        sw.Border(4);
        sw.Child(tv);
        box.PackEnd(bbox, false);
        box.Spacing(4);
        bbox.PackStart(next);
        bbox.PackEnd(bt);
        bbox.Spacing(8);
        box.Homogeneous(false);
        bt.OnClick(&MyApp::myclick, this);
        next.OnClick(&MyApp::mynext, this);

        tv.AddTextColumn("Test", 0);
        tv.AddTextColumn("Prova", 1);

        for (int i = 0; i < 10; ++i) {
            std::ostringstream os1, os2;
            os1 << "Campo1 " << i;
            os2 << "Campo2 " << i;
            list.AddTail( 0, os1.str().c_str(),
                          1, os2.str().c_str(),
                          -1);
        }

        win.ShowAll();
    }
    void myclick()
    {
        Quit();
    }
    void mynext()
    {
        gtk::TreeStore *st = new gtk::TreeStore(2, columns);

        for (int i = 0; i < 4; ++i) {
            std::ostringstream os1;
            os1 << "Livello1 " << i;
            gtk::TreeIter it = st->Append();
            st->Set( it, 0, os1.str().c_str(), -1);

            for (int j = 0; j < 8; j++) {
                std::ostringstream os1, os2;
                os1 << "Livello2a" << i << "-" << j;
                os2 << "Livello2b " << (i * 8) + j;

                gtk::TreeIter sub = st->Append(it);
                st->Set( sub, 0, os1.str().c_str(), 
                               1, os2.str().c_str(),
                               -1);
            }
        }
        tv.Model(*st);
    }
};

int main(int argc, char *argv[]) {
    MyApp a;   

    a.Run();

}

