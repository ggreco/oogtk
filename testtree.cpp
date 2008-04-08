#include "oogtk.h"
#include <sstream>

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
    MyApp() : win("Test TreeView"), bt("Uscita"), next("Prova un albero"), 
              list(make_vector(G_TYPE_STRING)(G_TYPE_STRING)), tv(list) {
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
        win.OnDelete(&MyApp::myclick, this, true);
        tv.AddTextColumn("Test", 0);
        tv.AddTextColumn("Prova", 1);

        tv.OnButtonPress(&MyApp::event_clicked, this);

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
    void event_clicked(gtk::Event &e)
    {
        // the cast operator is defined not to match wrong types
        if (gtk::EventKey *k = e) {
            std::cerr << "Got keyevent!?!?!?\n";
        }
        else if (gtk::EventButton *b = e) {
            if (b->IsDoubleClick())
                std::cerr << "Got doubleclick in " << b->Coords() << "\n";
        }
    }

    void myclick()
    {
        Quit();
    }
    void mynext()
    {
        gtk::TreeStore *st = new gtk::TreeStore(make_vector(G_TYPE_STRING)(G_TYPE_STRING));

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

