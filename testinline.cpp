#include "oogtk.h"

class MyApp : public gtk::Application
{
    private:
        gtk::Window m_win;
        gtk::Label  m_label;

    public:
        MyApp() :
            m_win("OOGtk TEST by Gabriele Greco @ DARTS"),
            m_label("This is a VERY long <i>test</i> label.\n\n"
                    "It spreads on more lines and it uses \n"
                    "<big>Pango text <b>markup</b></big> to show it's capabilities.\n\n"
                    "Obviously <span foreground=\"blue\">Pango</span> can do <b>far</b> more than <u>this</u>...")
        {
            // let's create a pair of buttons
            gtk::Button button("Change label");
            gtk::StockButton quit(GTK_STOCK_QUIT);

            // let's create a box for our widgets
            gtk::VBox vbox(false, 8);

// the properties in the constructor may be also not specified, but added in a second
// moment with the following methods.
//            vbox.Spacing(8);
//            vbox.Homogeneous(false);

            vbox.Border(8); // let's give the vbox a border, it's a method available on every widget.
            m_label.Padding(16,8); // and let's give the label some padding

            button.OnClick(&MyApp::mycbk, this);

            // we can add a tooltip to every widget if we want.
            button.Tooltip("If you click on this button...\nThe label in the window will change... hurrah!");

            // you can use also object provided by the default classes as callbacks. 
            quit.OnClick(&gtk::Application::QuitLoop, dynamic_cast<gtk::Application *>(this));

            // you can define a void function for ondelete (that require a return code), and
            // specify the return code on the method line
            m_win.OnDelete(&gtk::Application::QuitLoop, dynamic_cast<gtk::Application *>(this), false);

            // you can create box in this way if you are ok with the default packing values..
            gtk::HBox buttonbox(button, quit);

            // ...or in this way if you want to control 
            vbox.PackStart(m_label, true, true); // default values, "true, true" can be omitted 
            vbox.PackStart(gtk::HSeparator()); // let's build inline a separator.
            vbox.PackEnd(buttonbox, false, false); // don't expand nor fill...

            // let's attach the box to the window and show all of our widgets
            m_win.Child(vbox);
            m_win.ShowAll();
        }

        void mycbk() {
            static int clicked = 0;
            clicked ++;
            // let's give a new content to our label if the user click the button!
            m_label.Set("<big><b>Changed!</b></big>");
            
            m_label << "\n\nJust to show you a <b>callback</b> at work (and streamed labels)!\n\n(Clicked <b>" << clicked << "</b> times)";
        }
};

int main(int argc, char *argv[])
{
    // create our interface
    MyApp app;

    // run it entering in the GTK main loop
    app.Run();
}

