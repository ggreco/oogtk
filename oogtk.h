#ifndef OOGTK_H

#define OOGTK_H

#include <gtk/gtk.h>
#include <assert.h>
#include <string>
#include <iostream>
#include <list>
#include <map>
#include "oogdk.h"

namespace gtk
{
    class Widget;

    typedef std::list<Widget *> WidgetList;
    
    enum PositionType
    {
      PosLeft = GTK_POS_LEFT,
      PosRight = GTK_POS_RIGHT,
      PosTop = GTK_POS_TOP,
      PosBottom = GTK_POS_BOTTOM
    };

    class Widget : public Object
    {
        public:
            operator  GtkWidget *() const { return GTK_WIDGET(obj_); }
            operator  GdkDrawable *() const { return GDK_DRAWABLE(GTK_WIDGET(obj_)->window); }
            Widget &Ref() { g_object_ref(obj_); return *this; }
            void Hide() { gtk_widget_hide(*this); }
            void Show() { gtk_widget_show(*this); }
            void ShowAll() { gtk_widget_show_all(*this); }
            void Sensitive() { gtk_widget_set_sensitive(*this, TRUE); }
            void Insensitive() { Sensitive(false); }
            void Sensitive(bool flag) { gtk_widget_set_sensitive(*this, flag); }
            void Tooltip(const std::string &tip) { gtk_widget_set_tooltip_markup(*this, tip.c_str()); }

            void SizeRequest(int width, int height) { gtk_widget_set_size_request(*this, width, height); }
            void ModifyBase(Color &color, GtkStateType type = GTK_STATE_NORMAL) {
                gtk_widget_modify_base(*this, type, color);
            }
            void ModifyText(Color &color, GtkStateType type = GTK_STATE_NORMAL) {
                 gtk_widget_modify_text(*this, type, color);
            }
            void ModifyFont(FontDesc &font) {
                gtk_widget_modify_font(*this, font);
            }
    };

 
    class Misc : public Widget
    {
        public:
            operator  GtkMisc *() const { return GTK_MISC(Obj()); }

            void Padding(int xpad, int ypad = 0) { gtk_misc_set_padding(*this, xpad, ypad); }
            void Padding(Point &coords) { Padding(coords.x, coords.y); }
            Point Padding() const {
                Point c;
                gtk_misc_get_padding(*this, &c.x, &c.y);
                return c; 
            }

            void Alignment(float xalign, float yalign = 0.5f) { gtk_misc_set_alignment(*this, xalign, yalign); }
            void Alignment(Align &align) { Alignment(align.first, align.second); }
            Align Alignment() const {
                Align a;
                gtk_misc_get_alignment(*this, &a.first, &a.second);
                return a;
            }
    };

   enum IconSize
   {
        IconSizeInvalid = GTK_ICON_SIZE_INVALID,
        IconSizeMenu = GTK_ICON_SIZE_MENU,
        IconSizeSmallToolbar = GTK_ICON_SIZE_SMALL_TOOLBAR,
        IconSizeLargeToolbar = GTK_ICON_SIZE_LARGE_TOOLBAR,
        IconSizeButton = GTK_ICON_SIZE_BUTTON,
        IconSizeDnD = GTK_ICON_SIZE_DND,
        IconSizeDialog = GTK_ICON_SIZE_DIALOG
    };

    class Image : public Misc
    {
        public:
            operator  GtkImage *() const { return GTK_IMAGE(Obj()); }

            Image(GObject *obj) { Init(obj); }

            Image(const std::string &name) {
                Init(gtk_image_new_from_file(name.c_str()));
                Internal(true);
            }
            Image(Pixbuf &pixbuf) {
                Init(gtk_image_new_from_pixbuf(pixbuf));
                Internal(true);
            }
            Image(IconSize size, const char *stockid) {
                Init(gtk_image_new_from_stock(stockid, (GtkIconSize)size));
                Internal(true);
            }
            Image() {
                Init(gtk_image_new());
                Internal(true);
            }

            void Set(const std::string &name) { gtk_image_set_from_file(*this, name.c_str()); }
            void Set(IconSize size, const char *stockid) { gtk_image_set_from_stock(*this, stockid, (GtkIconSize)size); }
            void Set(Pixbuf &pixbuf) { gtk_image_set_from_pixbuf(*this, pixbuf); }
    };

    enum Justification
    {
      JustifyLeft = GTK_JUSTIFY_LEFT,
      JustifyRight = GTK_JUSTIFY_RIGHT,
      JustifyCenter = GTK_JUSTIFY_CENTER,
      JustifyFill = GTK_JUSTIFY_FILL
    };

    class Label : public Misc
    {
        public:
            operator  GtkLabel *() const { return GTK_LABEL(Obj()); }

            Label(GObject *obj) { Init(obj); }
            Label(const std::string &label = "") {
                Init(gtk_label_new(NULL));

                if (!label.empty())
                    gtk_label_set_markup(*this, label.c_str());

                Internal(true);
            }


            void Set(const std::string &label) { gtk_label_set_markup(*this, label.c_str()); }
            std::string Get() const { return gtk_label_get_text(*this); }

            void Selectable(bool flag) { gtk_label_set_selectable(*this, flag); }
            bool Selectable() const { return gtk_label_get_selectable(*this); }

            void Ellipsize(PangoEllipsizeMode mode) { gtk_label_set_ellipsize(*this, mode); }
            PangoEllipsizeMode Ellipsize() const { return gtk_label_get_ellipsize(*this); }

            OneOf<GtkJustification, Justification> Justify() const {
                return gtk_label_get_justify(*this); 
            }
            void Justify(OneOf<GtkJustification, Justification> mode) {
                gtk_label_set_justify(*this, mode); 
            }

            int WidthChars() const { return gtk_label_get_width_chars(*this); }
            void WidthChars(int length) { return gtk_label_set_width_chars(*this, length); }
    };

    class Separator : public Widget
    {
    };

    class HSeparator : public Separator
    {
        public:
            HSeparator(GObject *obj) { Init(obj); }
            HSeparator() {
                Init(gtk_hseparator_new());
                Internal(true);
            }
    };

    class VSeparator : public Separator
    {
        public:
            VSeparator(GObject *obj) { Init(obj); }

            VSeparator() {
                Init(gtk_vseparator_new());
                Internal(true);
            }
    };

    class Editable : public Widget
    {
        public:
            int InsertText(const std::string &text, int position) {
                gtk_editable_insert_text(GTK_EDITABLE(Obj()), text.c_str(), text.length(), &position);
                return position;
            }
            void DeleteText(int start, int end = -1) {
                gtk_editable_delete_text(GTK_EDITABLE(Obj()), start, end);
            }

            std::string GetChars(int start, int end = -1) const {
                std::string res;
                
                if (char *result = gtk_editable_get_chars(GTK_EDITABLE(Obj()), start, end)) {
                    res = result;
                    g_free(result);
                }

                return res;
            }

            int Position() const {
                return gtk_editable_get_position(GTK_EDITABLE(Obj()));
            }
            void Position(int pos = - 1) {
                return gtk_editable_set_position(GTK_EDITABLE(Obj()), pos);
            }
    };

    class Entry: public Editable
    {
        public:
            operator  GtkEntry *() const { return GTK_ENTRY(Obj()); }

            Entry(GObject *obj) { Init(obj); }

            Entry(int maxlen = -1) {
                if (maxlen != -1) {
                    Init(gtk_entry_new_with_max_length(maxlen));
                }
                else
                    Init(gtk_entry_new());
                Internal(true);
            }
            void Set(const std::string &name) { gtk_entry_set_text(*this, name.c_str()); }
            std::string Get() const { return gtk_entry_get_text(*this); }
    };

    class DrawingArea : public Widget {
        public:
            operator GtkDrawingArea *() const { return GTK_DRAWING_AREA(Obj()); }
            DrawingArea(GObject *obj) { Init(obj); }
            DrawingArea() {
                Init(gtk_drawing_area_new());
                Internal(true);
            }
    };

    typedef int CbkId;

    class Application
    {
        public:
            typedef std::map<CbkId, AbstractCbk*> CbkMap;
            typedef CbkMap::iterator CbkIt;

            Application(int &a, char **&b) { gtk_init(&a,&b); }

            Application() { gtk_init(NULL, NULL); }
            void Run() { gtk_main(); }
            void Flush() { while (gtk_events_pending()) gtk_main_iteration(); }
            void Quit() { gtk_main_quit(); }

            // helpers for signals that need a fixed answer.
            bool True() { return true; }
            bool False() { return false; }

            // AddTimer... four variants
            template <typename T, typename R>
            CbkId AddTimer(int msec, R (T::*fnc)(void), T* obj, int rc = true) { return AddTimer(new CbkEvent<T,R>(obj, fnc, rc), msec); }
            template <typename T, typename R, typename J>
            CbkId AddTimer(int msec, R (T::*fnc)(J), T* obj, J data, int rc = true) { return AddTimer(new CbkEvent<T,R,J>(obj, fnc, data, rc), msec); }

            template <typename T>
            CbkId AddOneTimeEvent(int msec, void (T::*fnc)(void), T* obj) { return AddTimer(new CbkEvent<T,void>(obj, fnc, false), msec); }
            template <typename T, typename J>
            CbkId AddOneTimeEvent(int msec, void (T::*fnc)(J), T* obj, J data) { return AddTimer(new CbkEvent<T,void,J>(obj, fnc, data, false), msec); }
    
            // AddIdle... four variants
            template <typename T, typename R>
            CbkId AddIdle(R (T::*fnc)(void), T* obj) { return AddIdle(new CbkEvent<T,R>(obj, fnc)); }
            template <typename T, typename R, typename J>
            CbkId AddIdle(R (T::*fnc)(J), T* obj, J data) { return AddIdle(new CbkEvent<T,R,J>(obj, fnc, data)); }
            template <typename T>
            void RunOneTimeEvent(void (T::*fnc)(void), T* obj) { AddIdle(new CbkEvent<T,void>(obj, fnc, false)); }
            template <typename T, typename J>
            void RunOneTimeEvent(void (T::*fnc)(J), T* obj, J data) { AddIdle(new CbkEvent<T,void, J>(obj, fnc, data, false)); }
           
            void DelSource(CbkId id) {
                CbkIt it = Callbacks().find(id); 

                if (it != Callbacks().end()) {
                    delete it->second;
                    Callbacks().erase(id);
                    g_source_remove(id); 
                }
            }

            void DelTimer(CbkId id) { DelSource(id);  }
            void DelIdle(CbkId id) { DelIdle(id);  }

        private:
            static CbkMap &Callbacks() { static CbkMap objects; return objects; }

            CbkId AddIdle(AbstractCbk *cbk) {
                int id = g_idle_add((gboolean (*)(void*))AbstractCbk::real_callback_0, cbk);
                Callbacks().insert(CbkMap::value_type(id, cbk));
                return id;
            }
            CbkId AddTimer(AbstractCbk *cbk, int msec) {
                int id = g_timeout_add(msec, (gboolean (*)(void*))AbstractCbk::real_callback_0, cbk);
                Callbacks().insert(CbkMap::value_type(id, cbk));
                return id;
            }

    };

    class Builder
    {
        public:
            enum BuildSource {File, String};

            Builder(const std::string &ui = "", BuildSource source = File) {
                builder_ = gtk_builder_new();

                if (!ui.empty()) {                    
                    if (!Load(ui, source)) {
                        g_object_unref(builder_);
                        throw std::runtime_error("Builder error: " + error_);
                    }
                }
            }

            bool Load(const std::string &ui, BuildSource source = File) {
                GError *err = NULL;

                int rc;

                if (source == File)
                    rc = gtk_builder_add_from_file(builder_, ui.c_str(), &err);
                else
                    rc = gtk_builder_add_from_string(builder_, ui.c_str(), ui.length(), &err);

                if (!rc) {
                    if (err)
                        error_ = err->message;
                    else
                        error_ = "generic error";

                    g_free(err);
                    return false;
                }

                if (g_module_supported())
                    gtk_builder_connect_signals (builder_, this);

                return true;
            }

            bool FromString(const std::string &ui) { return Load(ui, String); }
            bool FromFile(const std::string &ui) { return Load(ui, File); }

            ~Builder() { g_object_unref(builder_); }

            Widget *
            Get(const char *label) {
                if (GObject *obj = gtk_builder_get_object(builder_, label)) {
                    return dynamic_cast<Widget *>(Object::Find(obj));
                }
                else
                    return NULL;
            }

            template<typename T> 
            T *Get(const char *label) {
                return dynamic_cast<T *>(Get(label));
            }
        private:
            GtkBuilder *builder_;
            std::string error_;
    };

}

#include "oocont.h"
#include "oobutton.h"
#include "ootree.h"
#include "ootext.h"
#include "oomenu.h"

// questo va incluso alla fine per problemi di forward declarations

namespace gtk {
    inline Object *Object::
    Find(GObject *o)
    {
         if (o) { // elencare solo i nodi terminali qui!
            if (void *v = g_object_get_data(o, "object")) {
                return (Object *)v;
            } else if (GTK_IS_ENTRY(o)) {
                return new Entry(o);
            } else if (GTK_IS_FILE_CHOOSER_DIALOG(o)) {
                return new FileChooserDialog(o);
            } else if (GTK_IS_MESSAGE_DIALOG(o)) {
                return new MessageDialog(o);
            } else if (GTK_IS_DIALOG(o)) {
                return new Dialog(o);
            } else if (GTK_IS_WINDOW(o)) {
                return new Window(o);
            } else if (GTK_IS_CHECK_MENU_ITEM(o)) { // menu handling
                return new CheckMenuItem(o);
            } else if (GTK_IS_IMAGE_MENU_ITEM(o)) {
                return new ImageMenuItem(o);
            } else if (GTK_IS_SEPARATOR_MENU_ITEM(o)) {
                return new SeparatorMenuItem(o);
            } else if (GTK_IS_MENU_ITEM(o)) { 
                return new MenuItem(o);
            } else if (GTK_IS_MENU_BAR(o)) {
                return new MenuBar(o);
            } else if (GTK_IS_MENU(o)) { 
                return new Menu(o);
            } else if (GTK_IS_RADIO_BUTTON(o)) { // button handling
                return new RadioButton(o);
            } else if (GTK_IS_LINK_BUTTON(o)) {
                return new LinkButton(o);
            } else if (GTK_IS_CHECK_BUTTON(o)) {
                return new CheckButton(o);
            } else if (GTK_IS_TOGGLE_BUTTON(o)) {
                return new ToggleButton(o);
            } else if (GTK_IS_BUTTON(o)) {
                return new Button(o);
            } else if (GTK_IS_STATUSBAR(o)) {
                return new Statusbar(o);
            } else if (GTK_IS_VBUTTON_BOX(o)) {
                return new VButtonBox(o);
            } else if (GTK_IS_HBUTTON_BOX(o)) {
                return new HButtonBox(o);
            } else if (GTK_IS_IMAGE(o)) {
                return new Image(o);
            } else if (GTK_IS_VBOX(o)) {
                return new VBox(o);
            } else if (GTK_IS_HBOX(o)) {
                return new HBox(o);
            } else if (GTK_IS_TEXT_MARK(o)) { // textview handing, from ootext.h
                return new TextMark(o);
            } else if (GTK_IS_TEXT_TAG(o)) {
                return new TextTag(o);
            } else if (GTK_IS_TEXT_TAG_TABLE(o)) {
                return new TextTagTable(o);
            } else if (GTK_IS_TEXT_BUFFER(o)) {
                return new TextBuffer(o);
            } else if (GTK_IS_TEXT_VIEW(o)) {
                return new TextView(o);
            } else if (GTK_IS_TREE_SELECTION(o)) {
                return new TreeSelection(o);
            } else if (GTK_IS_CELL_RENDERER_TEXT(o)) {
                return new CellRendererText(o);
            } else if (GTK_IS_CELL_RENDERER_TOGGLE(o)) {
                return new CellRendererToggle(o);
            } else if (GTK_IS_CELL_RENDERER_PIXBUF(o)) {
                return new CellRendererPixbuf(o);
            } else if (GTK_IS_TREE_VIEW_COLUMN(o)) {
                return new TreeViewColumn(o);
            } else if (GTK_IS_SCROLLED_WINDOW(o)) {
                return new ScrolledWindow(o);
            } else if (GTK_IS_TREE_VIEW(o)) {
                return new TreeView(o);
            } else if (GTK_IS_SEPARATOR_TOOL_ITEM(o)) {
                return new SeparatorToolItem(o);
            } else if (GTK_IS_TOOL_BUTTON(o)) {
                return new ToolButton(o);
            } else if (GTK_IS_TOOL_ITEM(o)) {
                return new ToolItem(o);
            } else if (GTK_IS_TOOLBAR(o)) {
                return new Toolbar(o);
            } else if (GTK_IS_ASPECT_FRAME(o)) {
                return new AspectFrame(o);
            } else if (GTK_IS_FRAME(o)) {
                return new Frame(o);
            } else if (GTK_IS_DRAWING_AREA(o)) {
                return new DrawingArea(o);
            } else if (GTK_IS_TABLE(o)) {
                return new Table(o);
            } else if (GTK_IS_LABEL(o)) {
                return new Label(o);
            } else if (GTK_IS_TREE_SELECTION(o)) {
                return new TreeSelection(o);
            } else if (GTK_IS_TREE_STORE(o)) {
                return new TreeStore(o);
            } else if (GTK_IS_LIST_STORE(o)) {
                return new ListStore(o);
            } else
                std::cerr << "Undefined type " << g_type_name(GTK_OBJECT_TYPE(o));
        }

        return NULL;
    }

}
#endif
