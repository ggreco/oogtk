#include "oogtk.h"

using namespace gtk;

class MyApp : public Application
{
    public:
        // our application constructor, we pass a title to the gtk::Window constructor
        // and the buffer to our textview.
        MyApp() : m_win("TextView & Toolbar example"), m_text(m_buffer) {

            // First, let's create a few buttons, some are stocks
            StockButton quit(GTK_STOCK_QUIT);
            Button setup("Insert some tagged text!");
            StockButton clear(GTK_STOCK_CLEAR);
            Button play("Dump");

            // add a tooltip for the only button whose meaning isn't obvious.
            play.Tooltip("This button dumps the content of the textview to stderr using iterators.");

            // let's add a callback for each button
            play.OnClick(&MyApp::stuff, this);
            clear.OnClick(&MyApp::clear, this);
            setup.OnClick(&MyApp::setup, this);
            // note that you can use the Application built in quit directly, both on button
            // and on the delete signal on the window
            quit.OnClick(&Application::Quit, dynamic_cast<Application *>(this));
            m_win.OnDelete(&Application::Quit, dynamic_cast<Application *>(this), true);

            // let's add a nice toolbar
            
            ToolButton tnew(GTK_STOCK_NEW);
            ToolButton tclear(GTK_STOCK_CLEAR);
            ToolButton tprint(GTK_STOCK_PRINT);
            ToolButton tquit(GTK_STOCK_QUIT);
            ToolButton thelp(GTK_STOCK_HELP);

            // let's place Help on the right side with the "help" of a separator
            SeparatorToolItem tsep;
            tsep.Draw(false);
            tsep.Expand(true);

            // now we can build our toolbar
            Toolbar toolbar(tnew, tclear, tprint, tquit, tsep, thelp);
            
            tquit.OnClick(&Application::Quit, dynamic_cast<Application *>(this));
            tnew.OnClick(&MyApp::setup, this);
            tclear.OnClick(&MyApp::clear, this);
            tprint.OnClick(&MyApp::stuff, this);
            thelp.OnClick(&MyApp::help, this);

            // let's place all the buttons in a horizontal box
            HBox buttonbox(setup, play, clear, quit);

            // create a scrolled window for the text view
            ScrolledWindow win;
            win.Child(m_text);

            // create a box with not homogeneous layout to host the textview and the buttonbox
            VBox box(false, 8);
            box.PackStart(toolbar, false, false); // a Toolbar expanded is really ugly!
            box.PackStart(win);
            box.PackStart(HSeparator(), false, false);
            box.PackStart(buttonbox, false, false); // button must not grow vertically if we resize

            // let's add the box to our window, this will create a reference to the object that
            // will avoid the GtkVBox to be destroyed when "box" goes out of scope.
            m_win.Child(box);
          
            tags(); // create some tags

            m_win.ShowAll();
        }

        void help() {
            MessageDialog diag(&m_win,
                               DialogModal,
                               MessageQuestion,
                               ButtonsClose,
                               "<b>OOGtk TextView and Toolbar demo</b>\n\n"
                               "This small program shows how to use a TextView widget and a Toolbar widget in a <b>oogtk</b> enviroment, the code is very simple and heavily commented, the textview is read/write, you can dump what you write to a shell with the <b>Print</b> icon or the <b>Dump</b> button.");
            diag.Run();
        }

        void stuff() {
            // you can use iteratos mostly like STL containers on text buffers,
            // one thing that is important to be remembered is that a change to
            // the buffer invalidates all the iterators (but not the marks).
            for (TextIter it = m_buffer.Begin(); it != m_buffer.End(); ++it) {
                std::cerr << (char)*it;
            }
        }

        void clear() {
            m_buffer.Clear();
        }

        void tags() {
            // here we create some named tags, we can also create them unnamed, but in that
            // case we'll need to keep a reference in our class or somewhere else.
            TextTag bold("bold");
            TextTag italic("italic");
            TextTag red("red");

            // set some properties on our just created objects
            bold.Set("weight", PANGO_WEIGHT_BOLD);
            italic.Set("style", PANGO_STYLE_ITALIC);
            red.Set("foreground", "red");
            red.Set("background", "black");

            // this is a tag defined in the class, it's not named, so we should refer to it by reference
            m_hyper.Set("underline", PANGO_UNDERLINE_LOW);
            m_hyper.Set("foreground", "blue");

            // we MUST add all our tags to the tagtable of our buffer if we want to use them!
            m_buffer.Add(bold);
            m_buffer.Add(italic);
            m_buffer.Add(red);
            m_buffer.Add(m_hyper);

            // the insertion in the tagtable create a reference to our tags, so that they dont get destroyed
        }
        
        void setup() {
            m_text.WordWrap();

            // some TextBuffer methods
            
            m_buffer.Set("The quick brown fox jumps over the lazy dog\n\n");

            m_buffer.AppendTags("Some other text with different background and foreground colors!", "red");
                   
            // now let's test word wrap :)
            m_buffer.Append("\n\nAnd now, for something completely different, a very long line that will most surely wraps around, maybe also twice or, if we are lucky also three or more times, if the window is really small... and I don't see why it shouldn't 'cause we don't specify any width and GTK by default force the smallest possible allocation for his widgets.\n\nThis text seems an hyperlink, isn't it?\n");

            // add something at a particular offset, tags may be referenced by name or by object
            m_buffer.InsertTags(m_buffer.End(), "\nI'm a bold italic line!\n", "bold", "italic");

            // the following line will insert text at the beginning of our buffer...
            m_buffer.InsertTags(m_buffer.Begin(), "\nI'm a fake url!\n\n", m_hyper);

            m_buffer.Append("Let's add another match for the word GTK, maybe two... GTK ! :)");

            // now let's apply some other tags
            TextIter it = m_buffer.Begin();
            TextRange r;

            if (it.Search("fox", r))
                m_buffer.Apply("bold", r);

            if (it.Search("lazy dog", r))
                m_buffer.Apply("italic", r);

            // you can search also for whole sentences
            if (it.Search("three or more", r))
                m_buffer.Apply("bold", r);

            // you can use internal objects instead of names for tags you use often
            if (it.Search("This text", r))
                m_buffer.Apply(m_hyper, r);

            // combine more tags on a single range, and for multiple occurences
            while (it.Search("GTK", r)) {
                m_buffer.Apply("bold", r);
                m_buffer.Apply("red", r);
                m_buffer.Apply("italic", r);
                it = r.second; // this is needed to find the next match
            }

        }
    private:
        // declare here just the objects you will need in callbacks
        Window m_win;
        TextBuffer m_buffer;
        TextView m_text;
        TextTag m_hyper;
};

int main()
{
    MyApp app;

    app.Run();
}
