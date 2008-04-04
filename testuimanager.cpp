#include "oogtk.h"

using namespace gtk;

std::string ui_info =
        "<ui>"
        "  <menubar name='MenuBar'>"
        "    <menu action='FileMenu'>"
        "      <menuitem action='New'/>"
        "      <menuitem action='Open'/>"
        "      <menuitem action='Test'/>"
        "      <menuitem action='Save'/>"
        "      <separator/>"
        "      <menuitem action='Quit'/>"
        "    </menu>"
        "  </menubar>"
        "  <toolbar  name='ToolBar'>"
        "    <toolitem action='New'/>"
        "    <toolitem action='Open'/>"
        "    <toolitem action='Save'/>"
        "    <toolitem action='Test'/>"
        "      <separator/>"
        "    <toolitem action='Quit'/>"
        "  </toolbar>"
        "</ui>";

class MyApp : public Application
{
        Window m_main;
        UIManager m_manager;
    public:
        MyApp() : m_main("UIManager test") {
            VBox box(0, false);
            m_main.Child(box);

            ActionGroup group("actions");
            ActionList l;
            l.push_back(ActionEntry("FileMenu", "_File",  "", "<control>F"));
            l.push_back(ActionEntry("Open", GTK_STOCK_OPEN, &MyApp::open, this, 
                    "This will help you opening... nothing!", "<control>O"));
            l.push_back(ActionEntry("New", GTK_STOCK_NEW, &MyApp::new_, this, "", "<control>N"));
            l.push_back(ActionEntry("Save", GTK_STOCK_SAVE, &MyApp::save, this, "This element has a tooltip"));
            l.push_back(ActionEntry("Test", "_Test", &MyApp::test, this, 3, 
                    "A not stock element with user_data!", "<control>T"));
            l.push_back(ActionEntry("Quit", GTK_STOCK_QUIT, 
                        &Application::Quit, dynamic_cast<Application *>(this)));
            group.Add(l);
           
            m_manager.Insert(group);
            m_manager.FromString(ui_info);
            if (Widget *w = m_manager.Widget("/MenuBar"))
                box.PackStart(*w, false);
            if (Widget *w = m_manager.Widget("/ToolBar"))
                box.PackStart(*w, false);

            Label label("Menu and toolbar created through <b>GtkUIManager</b>");
            label.Padding(8, 32);
            box.PackStart(label, true, true);
            m_main.Add(m_manager.AccelGroup());
            m_main.ShowAll();
        }

        void save() { std::cerr << "clicked save\n"; }
        void open() { std::cerr << "clicked open\n"; }
        void new_() { std::cerr << "clicked new\n"; }
        void test(int val) { std::cerr << "clicked test: " << val << "\n"; }
};

int main()
{
    MyApp app;

    app.Run();
}


