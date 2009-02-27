#include "oogtk.h"

using namespace gtk;

class MyApplication : public Application
{
    public:
        MyApplication() : m_window("Button Test") {

            // let's build a box with a radiogroup
            RadioButton b1("Test 1");
            RadioButton b2(b1, "Test 2");
            RadioButton b3(b1, "Test 3");

            VBox vbox1(b1, b2, b3);

            // let's build a second box with another radiogroup, using another method to set the group
            RadioGroup group;
            RadioButton b4(group, "Another Test 1");
            RadioButton b5(group, "Another Test 2");
            RadioButton b6(group, "Another Test 3");

            VBox vbox2(b4, b5, b6);
           
            // yes, there is also a third method to do this!
            RadioButton b7( "New Test 1");
            RadioButton b8(b7.Group(), "New Test 2");
            RadioButton b9(b7.Group(), "New Test 3");
            RadioButton b10(b7.Group(), "New Test 4");

            VBox vbox3(b7, b8, b9, b10);

            // let's place the boxes on an horizontal box...
            HBox hbox(Frame(vbox1, "ButtonGroup, method 1"),
                      Frame(vbox2, "ButtonGroup, method 2"),
                      Frame(vbox3, "ButtonGroup, method 3"));

            hbox.Spacing(16);

            // now let's try some other kind of buttons:
            
            Button bb1("I'm a basic button");
            CheckButton c1("Check me!");
            c1.OnToggle(&MyApplication::handlecheck, this);
            ToggleButton tb1("I'm a toggle");
            tb1.OnToggle(&MyApplication::handletoggle, this);
            StockButton sb1(GTK_STOCK_NEW);
            LinkButton l1("http://www.gtk.org", "I'm a clickabile link!");
            HBox hbox2(bb1, tb1, c1, sb1, l1);
            hbox2.Spacing(32);

            // let's add a quit button
            StockButton q(GTK_STOCK_QUIT);
            q.OnClick(&Application::QuitLoop, dynamic_cast<Application *>(this));

            // place everything in a vertical container
            VBox box(false, 8);
            box.PackStart(hbox, true, true);
            box.PackStart(Frame(hbox2, "Other button types..."), false, false);
            box.PackStart(HSeparator(), false, false);
            box.PackStart(q, false, false);

            // let set some active items
            b2.Active(true);
            b6.Active(true);
            b9.Active(true);

            // let's add a new snooper to monitor some keys
            AddKeySnooper(&MyApplication::snooper, this, (int)1234567);

            m_window.OnDelete(&Application::QuitLoop,
                    dynamic_cast<Application *>(this), true); // true here is needed to avoid the window to autoclose
            m_window.Child(box);
            m_window.ShowAll();
        }

        void snooper(Event &e, int userdata) {
            if (EventKey *k = e) {
                std::cerr << "Received " << (k->Press() ? "PRESS" : "RELEASE") 
                          << " event " << k->KeyVal() << " with userdata " << userdata << "\n";
            }
        }
        void handletoggle(Widget &toggle) {
            ToggleButton *b = dynamic_cast<ToggleButton *>(&toggle);

            if (b->Active())
                b->Label("I'm a PUSHED toggle");
            else
                b->Label("I'm a toggle");
        }
        void handlecheck(Widget &check) {
            CheckButton *b = dynamic_cast<CheckButton *>(&check);

            if (b->Active())
                b->Label("Uncheck me!");
            else
                b->Label("Check me!");
        }
    private:
        Window m_window;
};

int main()
{
    MyApplication app;

    app.Run();
}

