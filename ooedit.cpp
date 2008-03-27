#include "oogtk.h"
#include <fstream>
#include <sstream>

using namespace gtk;

typedef std::vector<std::string> StrVec;

class OoEdit;

// OoView is a class that represent the single document view in the editor interface
// it's derived from textview since textview (and buffer) are the elements most used
// by the editing operations
class OoView : public TextView
{
    private:
        // here we declare only the objects we need to remember since we need to change
        // them through callbacks.
        // Most callbacks are not in the view class itself but in the main program
        // class.
        std::string m_name;
        Label m_label;
        Label m_modified;
        HBox m_cont;
        ScrolledWindow m_scrolled;
        OoEdit *m_app;
    public:
        OoView(OoEdit *app, const std::string &filename = "");
        const std::string &Filename() { return m_name; }
        void UpdateLabel() {
            int pos = m_name.find_last_of("/");

            if (m_name.empty())
                m_label.Set("<b>New Document</b>");
            else if (pos == std::string::npos)
                m_label.Set(m_name);
            else
                m_label.Set(m_name.substr(pos+1));

            m_modified.Show(Buffer().Modified());
        }

        void Filename(const std::string &filename) {
            m_name = filename;
            UpdateLabel();
        }

        Widget &GetLabel() { return m_cont; } // we return the hbox as label for the notebook page
        Widget &GetContainer() { return m_scrolled; } // we return the scrolledwin as container to create the notebook page

        void Modified(bool flag) {
            Buffer().Modified(flag);
            m_modified.Show(flag);
        }
        bool CheckModified();
};

// the main program class. Derived from Application as it's better to do in a oogtk project.
// most of the callbacks are defined inside this class.
class OoEdit : public Application
{
        Window m_window;
        Notebook m_notebook;
        typedef std::vector<OoView *> ViewVec;
        ViewVec m_views;
        Toolbar build_toolbar();
        MenuBar build_menubar();
        Label m_line, m_column;
    public:
        OoEdit(const StrVec &files);

        Window &MainWindow() { return m_window; }

        // callback activated if you click on the close page widget
        void closeview(OoView *view) {
            if (view->CheckModified())
                return;

            int page = m_notebook.Page(view->GetContainer());
            ViewVec::iterator it = std::find(m_views.begin(), m_views.end(), view);
            if (it != m_views.end())
                m_views.erase(it);

            delete view;
            m_notebook.Remove(page);
        }

        // callback activated if you attempt to close the program, it scans the container
        // to find for changed documents.
        void checkquit(void) {
            for (ViewVec::iterator it = m_views.begin();
                 it != m_views.end(); ++it) {
                if ((*it)->CheckModified())
                    return;
            }

            Quit();
        }

        void opendoc(const std::string &filename = "") {
            // let's create a new document tab
            OoView *v = new OoView(this, filename);
            m_views.push_back(v);
            // let's give notebook widget the widgets to be used as body and label of the notebook page
            int newpage = m_notebook.Append(v->GetContainer(), v->GetLabel());
            m_notebook.Current(newpage);

            // read the file is the name is not empty (existing document)
            if (!filename.empty()) {
                std::ifstream f(filename.c_str());

                if (f) {
                    std::string line;

                    while(std::getline(f, line)) {
                        line += '\n';
                        v->Buffer().Append(line);
                    }
                }
            }
            // set the text buffer as not modified, this also hides the small * in the label.
            v->Modified(false);
            // move cursor to the beginning of the document
            v->Buffer().PlaceCursor(v->Buffer().Begin());
            // update the position and other datas in the bottombar
            UpdatePosition();
        }

        void newdoc() { opendoc(); }

        void savedoc(int page, const std::string &filename) {
            // get start and end iterator to the document of a particular page
            TextRange bounds = m_views[page]->Buffer().Bounds();
            std::ofstream os(filename.c_str(), std::ios::binary);

            // write document character by character, not the fastest way, but
            // it shows how a text iterator can be used similar to a STL one.
            if (os) {
                for (TextIter it = bounds.first;
                        it !=  bounds.second; ++it) 
                    os << (char) *it;

                m_views[page]->Modified(false);
                m_views[page]->Filename(filename);
            }
        }

        void open() {
            // let's create a new modal file chooser to load a file
            FileChooserDialog dialog("Choose a text file to load...", &m_window);

            // add a pair of buttons
            dialog.AddButton(GTK_STOCK_OK, ResponseOk);
            dialog.AddButton(GTK_STOCK_CANCEL, ResponseCancel);
            dialog.ShowAll();

            // run the dialog and if we click Ok then open the document
            if (dialog.Run() == ResponseOk)
                opendoc(dialog.Filename());
        }
        void save() {
            int page = m_notebook.Current();

            if (page >=0 && page < m_views.size()) {
                // create a pair of buttons for a Dialog with the alternative
                // API (look at open() implementation for the basic one).
                ButtonVec buttons;
                buttons.push_back(ButtonData(GTK_STOCK_OK, ResponseOk));
                buttons.push_back(ButtonData(GTK_STOCK_CANCEL, ResponseCancel));

                // Create a new dialog with the buttons defined above, specify "save"
                // as action type, this will change the browser.
                FileChooserDialog dialog("Select a name for the file...", &m_window, 
                        FileChooserActionSave, buttons);

                // if we have a filename give it to the dialog
                if (!m_views[page]->Filename().empty())
                    dialog.Filename(m_views[page]->Filename());

                dialog.ShowAll();

            // run the dialog and if we click Ok then SAVE the document
                if (dialog.Run() == ResponseOk) {
                    savedoc(page, dialog.Filename());
                }
            }
        }
        void switched() {
            UpdatePosition();
      
            int pos = m_notebook.Current();
            if (pos != -1)
                m_views[pos]->GrabFocus();
        }

        void about() {
            MessageDialog diag(&m_window,
                    DialogModal,
                    MessageInfo,
                    ButtonsClose,
                    "<b>OOGtk Editor v1.0</b>\n\n"
                    "This small text editor shows how to use oogtk for a complete program and not a simple demo. It isn't certainly a very complete editor but it's indeed a complete program!");
            diag.Run();        
        }

        void UpdatePosition() {
            int pos = m_notebook.Current();

            if (pos == -1) 
                return;

            TextBuffer &b = m_views[pos]->Buffer();
            TextIter it;

            if (b.CursorIter(it)) {
                std::ostringstream os;
                os << it.Line() << '/' << b.Lines();
                m_line.Set(os.str());
                os.str("");
                os << it.LineOffset();
                m_column.Set(os.str());
            }
        }

        // this is to force dialog response if a user hit enter on the search widget
        void diag_search_ok(Dialog *dialog) {
            dialog->Response(ResponseOk);
        }

        // this method will popup a search dialog and will handle it inline
        void find() {
            int pos = m_notebook.Current();

            // quit if we don't have a buffer!
            if (pos == -1) {
                MessageDialog diag(&m_window,
                        DialogDestroyWithParent,
                        MessageInfo,
                        ButtonsClose,
                        "You need an active buffer to search for something!");
                diag.Run();
                return;
            }

            // add two buttons to the dialog
            ButtonVec buttons;
            buttons.push_back(ButtonData(GTK_STOCK_FIND, ResponseOk));
            buttons.push_back(ButtonData(GTK_STOCK_CLOSE, ResponseCancel));

            // let's build a local search dialog.
            Dialog diag("OOEdit Find for " + m_views[pos]->Filename(), buttons, &m_window);
            Entry entry;
            VBox box(Label("Insert the text to search"), entry);
            box.Spacing(8);
            box.Border(16);
            diag.Body(box);
            diag.ShowAll();
            diag.Modal(false);

            // this callback allow us to start search pressing enter on the entry widget
            entry.OnActivate(&OoEdit::diag_search_ok, this, &diag);

            TextBuffer &b =  m_views[pos]->Buffer();
            TextIter it = b.Begin();
            bool ifoundit = false;

            // we do a cycle cause we want to stay here until we have no more matches or the user
            // clicks cancel
            for(;;) {
                int r = diag.Run();

                if (r == ResponseOk) {
                    // no search if the keyword is empty
                    if (entry.Get().empty()) 
                        continue;

                    TextRange result;

                    // this will search on the buffer and highlight the result
                    if (it.Search(entry.Get(), result)) {
                        it = result.second;
                        b.Select(result);
                        // our result may be out of the displayed part of the TextBuffer,
                        // so we scroll the textview to be sure it's visible.
                        m_views[pos]->Scroll(it);
                        ifoundit = true;
                    }
                    else { // if we don't found a result hide the search dialog...
                        diag.Hide();

                        // ... and display a message
                        MessageDialog diag2(&m_window,
                                DialogModal,
                                ifoundit ? MessageInfo : MessageWarning,
                                ButtonsClose,
                                ifoundit ? "No more occurencies of <b>%s</b> in the document"
                                : "The search keyword <b>%s</b> has not been found in the document",
                                entry.Get().c_str());
                        diag2.Run();
                        break;
                    }
                }
                else  // we break out of the loop if we don't receive OK!
                    break;
            }
        }
};

Toolbar OoEdit::
build_toolbar()
{
    // build some stock toolbutton items
    ToolButton tnew(GTK_STOCK_NEW);
    ToolButton topen(GTK_STOCK_OPEN);
    ToolButton tsave(GTK_STOCK_SAVE);
    ToolButton tfind(GTK_STOCK_FIND);
    ToolButton tquit(GTK_STOCK_QUIT);
    ToolButton thelp(GTK_STOCK_HELP);

    // give every button its callback
    tnew.OnClick(&OoEdit::newdoc, this);
    topen.OnClick(&OoEdit::open, this);
    tsave.OnClick(&OoEdit::save, this);
    tfind.OnClick(&OoEdit::find, this);
    tquit.OnClick(&OoEdit::checkquit, this);
    thelp.OnClick(&OoEdit::about, this);

      // let's place Help on the right side with the "help" of a separator
    SeparatorToolItem tsep;
    tsep.Draw(false);
    tsep.Expand(true);

    // now we can build our toolbar
    return Toolbar(tnew, topen, tsave, tfind, tquit, tsep, thelp);
}

MenuBar OoEdit::
build_menubar()
{
    MenuBar bar;

    // create a File menu
    MenuItem file("File");
    ImageMenuItem mnew (GTK_STOCK_NEW);
    ImageMenuItem mopen(GTK_STOCK_OPEN);
    ImageMenuItem msave(GTK_STOCK_SAVE);
    ImageMenuItem mquit(GTK_STOCK_QUIT);

    // give every entry its callback
    mnew.OnActivate(&OoEdit::newdoc, this);
    mopen.OnActivate(&OoEdit::open, this);
    msave.OnActivate(&OoEdit::save, this);
    mquit.OnActivate(&OoEdit::checkquit, this);

    // appends items to the menu
    Menu filemenu(mnew, SeparatorMenuItem(), mopen, msave, SeparatorMenuItem(), mquit);
    file.Submenu(filemenu);

    // Create an help menu
    MenuItem help("Help");
    Menu helpmenu;
    help.Submenu(helpmenu);
    ImageMenuItem about(GTK_STOCK_ABOUT);
    about.OnActivate(&OoEdit::about, this);
    helpmenu.Append(about);

    // append both menus to our bar and justify the help menu to the right
    bar.Append(file);
    bar.Append(help);
    help.RightJustified(true);
    return bar;
}

OoEdit::OoEdit(const StrVec &files) : 
    m_window("OoEdit - mini text editor"), m_line("----"), m_column("----")
{
    VBox box(false, 2);

    HBox bottom(false, 4);
    bottom.PackStart(Label(" Line:"), false, false);
    bottom.PackStart(m_line, false, false);
    bottom.PackStart(VSeparator(), false, false);
    bottom.PackStart(Label(" Column:"), false, false);
    bottom.PackStart(m_column, false, false);
    bottom.PackStart(VSeparator(), false, false);
    bottom.PackStart(Statusbar()); // used only for the resize corner

    box.PackStart(build_menubar(), false, false);
    box.PackStart(build_toolbar(), false, false);
    box.PackStart(m_notebook, true, true);
    box.PackStart(bottom, false, false);
    m_window.SizeRequest(600, 400);
    // check if we can quit if someone try to close the window
    m_window.OnDelete(&OoEdit::checkquit, this, true);
    m_window.Child(box);
    m_window.ShowAll();

    // update bottombar if we switch view.
    m_notebook.OnPageSwitch(&OoEdit::switched, this);

    for (StrVec::const_iterator it = files.begin(); it != files.end(); ++it)
        opendoc(*it);
}

OoView::OoView(OoEdit *app, const std::string &filename) : 
   m_name(filename), m_modified("*"), m_cont(false, 2), m_app(app)
{
    UpdateLabel();

    // create the notebook label with filename, modify indicator and a close button.
    Button button;
    m_modified.Padding(2);
    Image img(IconSizeMenu, GTK_STOCK_CLOSE);
    button.Image(img);
    button.Relief(ReliefNone);
    button.OnClick(&OoEdit::closeview, app, this);
    m_cont.PackStart(m_label, true, true);
    m_cont.PackStart(m_modified, false, false);
    m_cont.PackStart(button, false, false);
    m_cont.ShowAll();

    // this will add our textview to the scrolled window.
    m_scrolled.Child(*this);
    m_scrolled.ShowAll();
    CursorVisible(true);

    // autumatically call the Widget::Show(true) method if we modify the buffer...
    Buffer().OnChanged(&Widget::Show, dynamic_cast<Widget *>(&m_modified), true);
    // ... and update cursor position cause it can be moved
    Buffer().OnChanged(&OoEdit::UpdatePosition, app);
    // ... obviously we should update the cursor position also on cursormoive
    TextView::OnCursorMove(&OoEdit::UpdatePosition, app);
}

bool OoView::CheckModified()
{
    // if a view is modified show a dialog prompting if we want to quit anyway
    // or abort the quit view / program operation.
    if (Buffer().Modified()) {
        MessageDialog diag(&m_app->MainWindow(),
                DialogModal,
                MessageQuestion,
                ButtonsOkCancel,
                "<b>OOEdit</b>\n\n"
                "The file <b>%s</b> has been modified,\ndo you want to quit anyway?",
                Filename().empty() ? "New Document" : Filename().c_str());

        if ( diag.Run() == ResponseCancel)
            return true;
    }
    return false;
}

int main(int argc, char *argv[])
{
    StrVec files;

    // every argument is a file to open
    for (int i = 1; i < argc; ++i)
        files.push_back(argv[i]);

    OoEdit edit(files);

    // enter GTK main loop
    edit.Run();
}
