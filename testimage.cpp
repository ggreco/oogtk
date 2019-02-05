#include "oogtk.h"

class MyApp : public gtk::Application
{
    private:
        gtk::Window m_win;
        gtk::Image  m_image;
        gtk::StockButton m_previous, m_next;
        std::vector<std::string> m_images;
        int m_current = 0;

    public:
        MyApp(const std::vector<std::string> &img) :
            m_previous(GTK_STOCK_GO_BACK),
            m_next(GTK_STOCK_GO_FORWARD),
            m_images(img)
        {
            // let's create a pair of buttons
            gtk::StockButton quit(GTK_STOCK_QUIT);

            // let's create a box for our widgets
            gtk::VBox vbox(false, 2);

            vbox.Border(8); // let's give the vbox a border, it's a method available on every widget.
            m_image.Padding(16,16); // and let's give the label some padding

            // you can use also object provided by the default classes as callbacks.
            quit.OnClick(&gtk::Application::QuitLoop, dynamic_cast<gtk::Application *>(this));

            // you can define a void function for ondelete (that require a return code), and
            // specify the return code on the method line
            m_win.OnDelete(&gtk::Application::QuitLoop, dynamic_cast<gtk::Application *>(this), false);

            m_previous.OnClick(&MyApp::do_previous, this);
            m_next.OnClick(&MyApp::do_next, this);

            // ...or in this way if you want to control
            vbox.PackStart(m_image, true, true); // default values, "true, true" can be omitted
            vbox.PackStart(gtk::HSeparator()); // let's build inline a separator.
            // you can create box in this way if you are ok with the default packing values..
            gtk::HBox buttonbox(m_previous, quit, m_next);
            buttonbox.Homogeneous(true);
            buttonbox.Spacing(16);
            buttonbox.Border(16);
            vbox.PackEnd(buttonbox, false, false); // don't expand nor fill...

            // let's attach the box to the window and show all of our widgets
            m_win.Child(vbox);
            m_win.ShowAll();

            update_image();
        }
        void do_next() {
            if (m_current < m_images.size() - 2) {
                m_current++;
                update_image();
            }
        }
        void do_previous() {
            if (m_current > 0) {
                m_current--;
                update_image();
            }
        }
        void update_image() {
            m_image.Set(m_images[m_current]);
            m_previous.Show(m_current > 0);
            m_next.Show(m_current < (m_images.size() - 1));
            m_win.Title(m_images[m_current]);
        }
};
int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cerr << "Usage:\n" << argv[0] << " <image>\n\n";
        return 0;
    }
    std::vector<std::string> imgs;
    for (int i = 1; i < argc; ++i)
        imgs.push_back(argv[i]);

    MyApp app(imgs);
    app.Run();
}
