#include "oogtk.h"
#include <fstream>
#include <sstream>

using namespace gtk;

typedef std::vector<std::string> StrVec;

class OoEdit;

class OoView : public TextView
{
    private:
        std::string m_name;
        Label m_label;
        HBox m_cont;
        ScrolledWindow m_scrolled;
        OoEdit *m_app;
    public:
        OoView(OoEdit *app, const std::string &filename = "");
        const std::string &Filename() { return m_name; }
        void UpdateLabel() {
            std::string filename = m_name;
            int pos = filename.find_last_of("/");

            if (filename.empty())
                m_label.Set("<b>New Document</b>");
            if (pos == std::string::npos)
                m_label.Set(filename);
            else
                m_label.Set(filename.substr(pos+1));
        }

        void Filename(const std::string &filename) {
            m_name = filename;
            UpdateLabel();
        }

        Widget &GetLabel() { return m_cont; }
        Widget &GetContainer() { return m_scrolled; }
        bool CheckModified();
};

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

        void closeview(OoView *view) {
            if (view->CheckModified())
                return;

            int page = m_notebook.Page(view->GetContainer());
            for (ViewVec::iterator it = m_views.begin();
                 it != m_views.end(); ++it) {
                if (*it == view) {
                    m_views.erase(it);
                    break;
                }
            }
            delete view;
            m_notebook.Remove(page);
        }

        void checkquit(void) {
            for (ViewVec::iterator it = m_views.begin();
                 it != m_views.end(); ++it) {
                if ((*it)->CheckModified())
                    return;
            }

            Quit();
        }

        void opendoc(const std::string &filename = "") {
            OoView *v = new OoView(this, filename);
            m_views.push_back(v);
            int newpage = m_notebook.Append(v->GetContainer(), v->GetLabel());
            m_notebook.Current(newpage);

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
            v->Buffer().Modified(false);
        }

        void newdoc() {
            opendoc();
        }
        void savedoc(int page, const std::string &filename) {
            TextRange bounds = m_views[page]->Buffer().Bounds();
            std::ofstream os(filename.c_str(), std::ios::binary);

            if (os) {
                for (TextIter it = bounds.first;
                        it !=  bounds.second; ++it) 
                    os << (char) *it;

                m_views[page]->Buffer().Modified(false);
                m_views[page]->Filename(filename);
            }
        }

        void open() {
            FileChooserDialog dialog("Choose a text file to load...", &m_window);

            dialog.AddButton(GTK_STOCK_OK, ResponseOk);
            dialog.AddButton(GTK_STOCK_CANCEL, ResponseCancel);
            dialog.ShowAll();

            if (dialog.Run() == ResponseOk)
                opendoc(dialog.Filename());
        }
        void save() {
            int page = m_notebook.Current();

            if (page >=0 && page < m_views.size()) {
                ButtonVec buttons;
                buttons.push_back(ButtonData(GTK_STOCK_OK, ResponseOk));
                buttons.push_back(ButtonData(GTK_STOCK_CANCEL, ResponseCancel));

                FileChooserDialog dialog("Select a name for the file...", &m_window, 
                        FileChooserActionSave, buttons);

                if (!m_views[page]->Filename().empty())
                    dialog.Filename(m_views[page]->Filename());

                dialog.ShowAll();

                if (dialog.Run() == ResponseOk) {
                    savedoc(page, dialog.Filename());
                }
            }
        }
        void switched() {
            UpdatePosition();
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
};

Toolbar OoEdit::
build_toolbar()
{
    ToolButton tnew(GTK_STOCK_NEW);
    ToolButton topen(GTK_STOCK_OPEN);
    ToolButton tsave(GTK_STOCK_SAVE);
    ToolButton tfind(GTK_STOCK_FIND);
    ToolButton tquit(GTK_STOCK_QUIT);
    ToolButton thelp(GTK_STOCK_HELP);

    tnew.OnClick(&OoEdit::newdoc, this);
    topen.OnClick(&OoEdit::open, this);
    tsave.OnClick(&OoEdit::save, this);
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
    Menu filemenu;
    file.Submenu(filemenu);
    ImageMenuItem mnew (GTK_STOCK_NEW);
    ImageMenuItem mopen(GTK_STOCK_OPEN);
    ImageMenuItem msave(GTK_STOCK_SAVE);
    ImageMenuItem mquit(GTK_STOCK_QUIT);

    mnew.OnActivate(&OoEdit::newdoc, this);
    mopen.OnActivate(&OoEdit::open, this);
    msave.OnActivate(&OoEdit::save, this);
    mquit.OnActivate(&OoEdit::checkquit, this);

    filemenu.Append(mnew);
    filemenu.Append(SeparatorMenuItem());
    filemenu.Append(mopen);
    filemenu.Append(msave);
    filemenu.Append(SeparatorMenuItem());
    filemenu.Append(mquit);

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
    bottom.PackStart(Statusbar()); // used only for the resize corner

    box.PackStart(build_menubar(), false, false);
    box.PackStart(build_toolbar(), false, false);
    box.PackStart(m_notebook, true, true);
    box.PackStart(bottom, false, false);
    m_window.SizeRequest(600, 400);
    m_window.OnDelete(&OoEdit::checkquit, this, true);
    m_window.Child(box);
    m_window.ShowAll();

    m_notebook.OnPageSwitch(&OoEdit::switched, this);

    for (StrVec::const_iterator it = files.begin(); it != files.end(); ++it)
        opendoc(*it);
}

OoView::OoView(OoEdit *app, const std::string &filename) : 
   m_name(filename), m_cont(false, 2), m_app(app)
{
    UpdateLabel();

    Button button;
    Image img(IconSizeMenu, GTK_STOCK_CLOSE);
    button.Image(img);
    button.Relief(ReliefNone);
    button.OnClick(&OoEdit::closeview, app, this);
    m_cont.PackStart(m_label, true, true);
    m_cont.PackStart(button, false, false);
    m_cont.ShowAll();
    m_scrolled.Child(*this);
    m_scrolled.ShowAll();
    CursorVisible(true);
    TextView::OnCursorMove(&OoEdit::UpdatePosition, app);
}

bool OoView::CheckModified()
{
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

    for (int i = 1; i < argc; ++i)
        files.push_back(argv[i]);

    OoEdit edit(files);

    edit.Run();
}
