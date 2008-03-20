#include "oogtk.h"
#include <fstream>

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
        void SetLabel(const std::string &filename) {
            int pos = filename.find_last_of("/");
            if (pos == std::string::npos)
                m_label.Set(filename);
            else
                m_label.Set(filename.substr(pos+1));
        }

        void Filename(const std::string &filename) {
            m_name = filename;
            SetLabel(filename);
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

    MenuItem file("File");
    Menu filemenu;
    file.Submenu(filemenu);
    filemenu.Append(ImageMenuItem(GTK_STOCK_NEW));
    filemenu.Append(SeparatorMenuItem());
    filemenu.Append(ImageMenuItem(GTK_STOCK_OPEN));
    filemenu.Append(ImageMenuItem(GTK_STOCK_SAVE));
    filemenu.Append(SeparatorMenuItem());
    filemenu.Append(ImageMenuItem(GTK_STOCK_QUIT));
    bar.Append(file);
    return bar;
}

OoEdit::OoEdit(const StrVec &files) : m_window("OoEdit - mini text editor")
{
    VBox box(false, 2);

    box.PackStart(build_menubar(), false, false);
    box.PackStart(build_toolbar(), false, false);
    box.PackStart(m_notebook, true, true);

    m_window.SizeRequest(600, 400);
    m_window.OnDelete(&OoEdit::checkquit, this, true);
    m_window.Child(box);
    m_window.ShowAll();

    for (StrVec::const_iterator it = files.begin(); it != files.end(); ++it)
        opendoc(*it);
}

OoView::OoView(OoEdit *app, const std::string &filename) : 
   m_name(filename), m_cont(false, 2), m_app(app)
{
    if (filename.empty())
        m_label.Set("<b>New Document</b>");
    else
        SetLabel(filename);

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
