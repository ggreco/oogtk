#ifndef OOCONT_H
#define OOCONT_H

#include "oogtk.h"

#include <string>
#include <vector>

namespace gtk {
    class Container : public Widget
    {
        public:
            operator  GtkContainer *() const { return GTK_CONTAINER(Obj()); }

            void Children(WidgetList &list) {
                GList *lst, *l = gtk_container_get_children(*this);
                lst = l;

                while (l) {
                    if (Widget *w = dynamic_cast<Widget *>(Object::Find((GObject *)l->data)))
                        list.push_back(w);

                    l = l->next;
                }

                g_list_free(lst);
            }
            void Add(const Widget &w) { gtk_container_add(*this, w); }
            void Border(int size) { gtk_container_set_border_width(*this, size); }
            int Border() const { return gtk_container_get_border_width(*this); }
    };

    class Bin : public Container // COMPLETE API
    {
        public:
            operator  GtkBin *() const { return GTK_BIN(Obj()); }

            Widget *Child() {
                return dynamic_cast<Widget *>(Object::Find((GObject *)gtk_bin_get_child(*this))); 
            }
            void Child(const Widget &w) { Add(w); }
    };

    class Item : public Bin // COMPLETE API
    {
        public:
            operator  GtkItem *() const { return GTK_ITEM(Obj()); }
            void Select() { gtk_item_select(*this); }
            void Deselect() { gtk_item_deselect(*this); }
            void Toggle() { gtk_item_toggle(*this); }
    };

    enum PolicyType
    {
      PolicyAlways = GTK_POLICY_ALWAYS,
      PolicyAutomatic = GTK_POLICY_AUTOMATIC,
      PolicyNever =GTK_POLICY_NEVER
    };
    typedef std::pair<OneOf<GtkPolicyType, PolicyType>, 
                      OneOf<GtkPolicyType, PolicyType> > PolicyTypes;

    class ScrolledWindow : public Bin
    {
        public:
            operator GtkScrolledWindow *() const { return GTK_SCROLLED_WINDOW(Obj()); }

            ScrolledWindow(GObject *obj) { Init(obj); }
            ScrolledWindow() {
                Init(gtk_scrolled_window_new(NULL, NULL));
                Internal(true);
            }

            void Policy(PolicyTypes &policy) {
                gtk_scrolled_window_set_policy(*this, 
                        policy.first, policy.second);
            }
            
            PolicyTypes Policy() const {
                GtkPolicyType w, h;
                gtk_scrolled_window_get_policy(*this, &w, &h);
                return PolicyTypes(w, h);
            }
    };

    class Box : public Container { // COMPLETE API
        public:
            operator GtkBox *() const { return GTK_BOX(Obj()); }
            void PackStart(const Widget &w, bool expand = true, bool fill = true, int padding = 0) {  
                gtk_box_pack_start(*this, w, expand, fill, padding); 
            }
            void PackEnd(const Widget &w, bool expand = true, bool fill = true, int padding = 0) { 
                gtk_box_pack_end(*this, w, expand, fill, padding); 
            }

            void Spacing(int space) { gtk_box_set_spacing(*this, space); }
            int Spacing(void) const { gtk_box_get_spacing(*this); }
            void Homogeneous(bool flag) { gtk_box_set_homogeneous(*this, flag); }
            bool Homogeneous(void) const { gtk_box_get_homogeneous(*this); }

            void ReorderChild(Widget &child, int position = -1) {
                gtk_box_reorder_child(*this, child, position);
            }
    };

    class VBox : public Box { // COMPLETE API
        public:
            VBox(GObject *obj) { Init(obj); }
            VBox(bool homogenous = true, int spacing = 0) { Init(gtk_vbox_new(homogenous, spacing)); Internal(true); }
            VBox(const Widget &w1) {
                Init(gtk_vbox_new(true, 0)); Internal(true);
                PackStart(w1);
            }
            VBox(const Widget &w1, const Widget &w2) {
                Init(gtk_vbox_new(false, 0)); Internal(true);
                PackStart(w1);
                PackStart(w2);
            }
            VBox(const Widget &w1, const Widget &w2, const Widget &w3) {
                Init(gtk_vbox_new(false, 0)); Internal(true);
                PackStart(w1);
                PackStart(w2);
                PackStart(w3);
            }
            VBox(const Widget &w1, const Widget &w2, const Widget &w3, const Widget &w4) {
                Init(gtk_vbox_new(false, 0)); Internal(true);
                PackStart(w1);
                PackStart(w2);
                PackStart(w3);
                PackStart(w4);
            }
            VBox(const Widget &w1, const Widget &w2, const Widget &w3, const Widget &w4, const Widget &w5) {
                Init(gtk_vbox_new(false, 0)); Internal(true);
                PackStart(w1);
                PackStart(w2);
                PackStart(w3);
                PackStart(w4);
                PackStart(w5);
            }
    };

    class HBox : public Box { // COMPLETE API
        public:
            HBox(const DerivedType &) {} // do nothing
            HBox(GObject *obj) { Init(obj); }
            HBox(bool homogenous = true, int spacing = 0) { Init(gtk_hbox_new(homogenous, spacing)); Internal(true); }
            
            HBox(const Widget &w1) {
                Init(gtk_hbox_new(true, 0)); Internal(true);
                PackStart(w1);
            }
            HBox(const Widget &w1, const Widget &w2) {
                Init(gtk_hbox_new(false, 0)); Internal(true);
                PackStart(w1);
                PackStart(w2);
            }
            HBox(const Widget &w1, const Widget &w2, const Widget &w3) {
                Init(gtk_hbox_new(false, 0)); Internal(true);
                PackStart(w1);
                PackStart(w2);
                PackStart(w3);
            }
            HBox(const Widget &w1, const Widget &w2, const Widget &w3, const Widget &w4) {
                Init(gtk_hbox_new(false, 0)); Internal(true);
                PackStart(w1);
                PackStart(w2);
                PackStart(w3);
                PackStart(w4);
            }
            HBox(const Widget &w1, const Widget &w2, const Widget &w3, const Widget &w4, const Widget &w5) {
                Init(gtk_hbox_new(false, 0)); Internal(true);
                PackStart(w1);
                PackStart(w2);
                PackStart(w3);
                PackStart(w4);
                PackStart(w5);
            }
    };

    enum ButtonBoxStyle
    {
        ButtonBoxDefaultStyle = GTK_BUTTONBOX_DEFAULT_STYLE,
        ButtonBoxSpread = GTK_BUTTONBOX_SPREAD,
        ButtonBoxEdge   = GTK_BUTTONBOX_EDGE,
        ButtonBoxStart  = GTK_BUTTONBOX_START,
        ButtonBoxEnd    = GTK_BUTTONBOX_END,
        ButtonBoxCenter = GTK_BUTTONBOX_CENTER
    };

    class ButtonBox : public Box { // COMPLETE API
        public:
            operator GtkButtonBox *() const { return GTK_BUTTON_BOX(Obj()); }

            void Secondary(Widget &child, bool flag) {
                gtk_button_box_set_child_secondary(*this, child, flag);
            }
            bool Secondary(Widget &child) const {
                return gtk_button_box_get_child_secondary(*this, child);
            }

            void Layout(OneOf<GtkButtonBoxStyle, ButtonBoxStyle> style) {
                gtk_button_box_set_layout(*this, style);
            }
            OneOf<GtkButtonBoxStyle, ButtonBoxStyle> Layout() const {
                return gtk_button_box_get_layout(*this);
            }

            void ChildPadding(int x, int y) {
                gtk_button_box_set_child_ipadding(*this, x, y);
            }
            void ChildPadding(const Point &pad) {
                gtk_button_box_set_child_ipadding(*this, pad.x, pad.y);
            }
            Point ChildPadding() const {
                Point pad;
                gtk_button_box_get_child_ipadding(*this, &pad.x, &pad.y);
                return pad;
            }

            void ChildSize(int min_width, int min_height) {
                gtk_button_box_set_child_size(*this, min_width, min_height);
            }
            void ChildSize(const Point &minsize) {
                gtk_button_box_set_child_size(*this, minsize.x, minsize.y);
            }
            Point ChildSize() const {
                Point minsize;
                gtk_button_box_get_child_size(*this, &minsize.x, &minsize.y);
                return minsize;
            }
    };

    class HButtonBox : public ButtonBox { // COMPLETE API
        public:
            HButtonBox(GObject *obj) { Init(obj); }
            HButtonBox() { 
                Init(gtk_hbutton_box_new());
                Internal(true);
            }
    };
    class VButtonBox : public ButtonBox { // COMPLETE API
        public:
            VButtonBox(GObject *obj) { Init(obj); }
            VButtonBox() { 
                Init(gtk_vbutton_box_new());
                Internal(true);
            }
    };
    
    enum WindowType
    {
        WindowTopLevel = GTK_WINDOW_TOPLEVEL,
        WindowPopup = GTK_WINDOW_POPUP
    };

    enum WindowPosition
    {
      PositionNone           = GTK_WIN_POS_NONE,
      PositionCenter         = GTK_WIN_POS_CENTER,
      PositionMouse          = GTK_WIN_POS_MOUSE,
      PositionCenterAlways   = GTK_WIN_POS_CENTER_ALWAYS,
      PositionCenterOnParent = GTK_WIN_POS_CENTER_ON_PARENT
    };

    class AccelGroup : public Object {
        public:
            operator  GtkAccelGroup *() const { return GTK_ACCEL_GROUP(obj_); }
            AccelGroup(GObject *obj) { Init(obj); }
    };


    class Window : public Bin
    {
        public:
            operator  GtkWindow *() const { return GTK_WINDOW(Obj()); }
            Window(const DerivedType &) {} // do nothing
            Window(GObject *obj) { Init(obj); }
            Window(const std::string &title, 
                   OneOf<GtkWindowType, WindowType> type = WindowTopLevel) {
                Init(gtk_window_new(type));

                if (!title.empty())
                    gtk_window_set_title(GTK_WINDOW(obj_), title.c_str());
                Internal(true);
            }

            // windows are special objects and must be destroyed
            virtual ~Window() { if (ObjType() == InternalObj) gtk_widget_destroy(GTK_WIDGET(Obj())); }
            // window signals
            BUILD_EVENT(OnDelete, "delete_event");

            // accel group stuff
            void Add(const AccelGroup &group) { gtk_window_add_accel_group(*this, group); }
            void Remove(const AccelGroup &group) { gtk_window_remove_accel_group(*this, group); }

            // flags get/set 
            bool Resizable() const { return gtk_window_get_resizable(*this); }
            void Resizable(bool flag) { gtk_window_set_resizable(*this, flag); }

            bool Modal() const { return gtk_window_get_modal(*this); }
            void Modal(bool flag) { gtk_window_set_modal(*this, flag); }

            std::string Title() const { return std::string(gtk_window_get_title(*this)); }
            void Title(const std::string &title) { gtk_window_set_title(*this, title.c_str()); }

            Point Size() const { 
                Point pos;
                gtk_window_get_size(*this, &pos.x, &pos.y); 
                return pos;
            }
            void Size(const Point &size) { Size(size.x, size.y); }
            void Size(int width, int height) { gtk_window_resize(*this, width, height); }

            Point Position() const { 
                Point pos;
                gtk_window_get_position(*this, &pos.x, &pos.y);
                return pos;
            }
            void Position(int x, int y) { gtk_window_move(*this, x, y); }
            void Position(const Point &origin) { Position(origin.x, origin.y); }
            void Position(OneOf<GtkWindowPosition, WindowPosition> pos) { 
                gtk_window_set_position(*this, pos); 
            }

            void TransientFor(Window &parent) { gtk_window_set_transient_for(*this, parent); }
            bool Active() const { gtk_window_is_active(*this); }

            // shortcut che GTK si aspetta
            void Move(int x, int y) { Position(x, y); }
            void Resize(int width, int height) { Size(width, height); }

    };

    typedef std::pair<std::string, int> ButtonData;
    typedef std::vector<ButtonData> ButtonVec;

    enum DialogFlags
    {
        DialogModal = GTK_DIALOG_MODAL, /* call gtk_window_set_modal (win, TRUE) */
        DialogDestroyWithParent = GTK_DIALOG_DESTROY_WITH_PARENT, /* call gtk_window_set_destroy_with_parent () */
        DialogNoSeparator = GTK_DIALOG_NO_SEPARATOR
    };

    inline DialogFlags operator|(DialogFlags a, DialogFlags b) { return (DialogFlags)(((int)a)|((int)b)); }

    enum ResponseType
    {
        /* GTK returns this if a response widget has no response_id,
         * or if the dialog gets programmatically hidden or destroyed.
         */
        ResponseNone = GTK_RESPONSE_NONE,

        /* GTK won't return these unless you pass them in
         * as the response for an action widget. They are
         * for your convenience.
         */
        ResponseReject = GTK_RESPONSE_REJECT,
        ResponseAccept = GTK_RESPONSE_ACCEPT,

        /* If the dialog is deleted. */
        ResponseDeleteEvent = GTK_RESPONSE_DELETE_EVENT,

        /* These are returned from GTK dialogs, and you can also use them
         * yourself if you like.
         */
        ResponseOk = GTK_RESPONSE_OK,
        ResponseCancel = GTK_RESPONSE_CANCEL,
        ResponseClose = GTK_RESPONSE_CLOSE,
        ResponseYes = GTK_RESPONSE_YES,
        ResponseNo = GTK_RESPONSE_NO,
        ResponseApply = GTK_RESPONSE_APPLY,
        ResponseHelp = GTK_RESPONSE_HELP
    };

    class Dialog : public Window
    {
        public:
            operator  GtkDialog *() const { return GTK_DIALOG(Obj()); }

            Dialog(GObject *obj) : Window(DerivedType()) { Init(obj); }
            Dialog(const DerivedType &d) : Window(d) {}

            Dialog() : Window(DerivedType()) {
                Init(gtk_dialog_new());
                Internal(true);
            }
            Dialog(const std::string &title, const ButtonVec &buttons, 
                   Window *parent = NULL) : Window(DerivedType()) {
                Init(gtk_dialog_new());
                AddButtons(buttons);
                Title(title);
                Internal(true);
                if (parent) 
                    Window::TransientFor(*parent);
            }
            void Body(Widget &widget) {
                gtk_container_add(GTK_CONTAINER(GTK_DIALOG(Obj())->vbox), 
                                  widget);
            }
            void AddButton(const std::string &label, int rc) {
                gtk_dialog_add_button(*this, label.c_str(), rc);
            }
            void AddButton(const ButtonData &button) {
                AddButton(button.first, button.second);
            }
            void AddButtons(const ButtonVec &buttons) {
                for (ButtonVec::const_iterator it = buttons.begin(); 
                                               it != buttons.end(); ++it)
                    AddButton(*it);
            }

            int Run() { return gtk_dialog_run(*this); }
            void Response(int response_id) { gtk_dialog_response(*this, response_id); }
            void DefaultResponse(int id) { gtk_dialog_set_default_response(*this, id); }

            void Separator(bool flag = false) { gtk_dialog_set_has_separator(*this, flag); }
            bool Separator() { return gtk_dialog_get_has_separator(*this); }
    };

    enum FileChooserAction
    {
        FileChooserActionOpen = GTK_FILE_CHOOSER_ACTION_OPEN,
        FileChooserActionSave = GTK_FILE_CHOOSER_ACTION_SAVE,
        FileChooserActionSelectFolder = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
        FileChooserActionCreateFolder = GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER
    };

    class FileChooser {
        public:
            virtual GtkFileChooser *getobj() const = 0; // this is needed to make it pure virtual
            virtual ~FileChooser() {}

            // get/set methods
            std::string Filename() const {
                std::string file;
                if (gchar *name = gtk_file_chooser_get_filename(getobj())) {
                    file = name;
                    g_free(name);
                }
                return file;
            }
            bool Filename(const std::string &name) {
                return gtk_file_chooser_set_filename(getobj(), name.c_str());
            }

            std::string CurrentFolder() const {
                std::string result;
                if (gchar *folder = gtk_file_chooser_get_current_folder(getobj())) {
                    result = folder;
                    g_free(folder);
                }
                return result;
            }
            bool CurrentFolder(const std::string &folder) {
                 return gtk_file_chooser_set_current_folder(getobj(), folder.c_str());
            }

            bool LocalOnly() const { return gtk_file_chooser_get_local_only(getobj()); }
            void LocalOnly(bool flag) { gtk_file_chooser_set_local_only(getobj(), flag); }
            bool SelectMultiple() const { return gtk_file_chooser_get_select_multiple(getobj()); }
            void SelectMultiple(bool flag) { gtk_file_chooser_set_select_multiple(getobj(), flag); }
            bool ShowHidden() const { return gtk_file_chooser_get_show_hidden(getobj()); }
            void ShowHidden(bool flag) { gtk_file_chooser_set_show_hidden(getobj(), flag); }

            OneOf<GtkFileChooserAction, FileChooserAction> Action() const { 
                return gtk_file_chooser_get_action(getobj()); 
            }
            void Action(OneOf<GtkFileChooserAction, FileChooserAction> action) { 
                gtk_file_chooser_set_action(getobj(), action); 
            }

            bool OverwriteConfirmation() const { return gtk_file_chooser_get_do_overwrite_confirmation(getobj()); }
            void OverwriteConfirmation(bool flag) { gtk_file_chooser_set_do_overwrite_confirmation(getobj(), flag); }

            // selections
            void SelectAll() { gtk_file_chooser_select_all(getobj()); }
            void UnselectAll() { gtk_file_chooser_unselect_all(getobj()); }
            bool SelectFilename(const std::string &name) {
                return gtk_file_chooser_select_filename(getobj(), name.c_str());
            }
            void UnselectFilename(const std::string &name) {
                gtk_file_chooser_unselect_filename(getobj(), name.c_str());
            }

            void Filenames(std::vector<std::string> &results) {
                if (GSList *l = gtk_file_chooser_get_filenames(getobj())) {
                    GSList *it = l;

                    while (it) {
                        results.push_back((char *)it->data);
                        g_free(it->data);
                        it = it->next;
                    }
                    g_slist_free(l);
                }
            }
    };

    class FileChooserDialog : public Dialog, public FileChooser {
        public:
            operator  GtkFileChooserDialog *() const { return GTK_FILE_CHOOSER_DIALOG(Obj()); }

            FileChooserDialog(GObject *obj) : Dialog(DerivedType()) { Init(obj); }
         
            FileChooserDialog(const std::string &title = "", Window *parent_window = NULL,
                              OneOf<GtkFileChooserAction, FileChooserAction> action =  FileChooserActionOpen,
                              const ButtonVec &buttons = ButtonVec()) : 
                Dialog(DerivedType()) {
                Init(gtk_file_chooser_dialog_new(title.c_str(), NULL,
                                                 action, NULL));
                AddButtons(buttons);
                Internal(true);
            } 
            virtual GtkFileChooser *getobj() const { return GTK_FILE_CHOOSER(Obj()); }
    };

    enum MessageType
    {
        MessageInfo = GTK_MESSAGE_INFO,
        MessageWarning = GTK_MESSAGE_WARNING,
        MessageQuestion = GTK_MESSAGE_QUESTION,
        MessageError = GTK_MESSAGE_ERROR,
        MessageOther = GTK_MESSAGE_OTHER
    };

    enum ButtonsType
    {
        ButtonsNone = GTK_BUTTONS_NONE,
        ButtonsOk = GTK_BUTTONS_OK,
        ButtonsClose = GTK_BUTTONS_CLOSE,
        ButtonsCancel = GTK_BUTTONS_CANCEL,
        ButtonsYesNo = GTK_BUTTONS_YES_NO,
        ButtonsOkCancel = GTK_BUTTONS_OK_CANCEL
    };

    class MessageDialog : public Dialog
    {
        public:
            operator  GtkMessageDialog *() const { return GTK_MESSAGE_DIALOG(Obj()); }

            MessageDialog(GObject *obj) : Dialog(DerivedType()) { Init(obj); }

            MessageDialog(Window *parent, OneOf<GtkDialogFlags, DialogFlags> flags,
                          OneOf<GtkMessageType, MessageType> msgtype,
                          OneOf<GtkButtonsType, ButtonsType> buttontype,
                          const char *msg_format, ...) : Dialog(DerivedType()) {
                char msg[4096];
                va_list va;

                va_start(va, msg_format);
                vsnprintf(msg, sizeof(msg), msg_format, va);
                va_end(va);

                Init(gtk_message_dialog_new_with_markup(*parent, flags, msgtype, buttontype,
                        msg));
                Internal(true);
            }
            MessageDialog(const std::string &msg,
                          OneOf<GtkMessageType, MessageType> msgtype = MessageInfo,
                          OneOf<GtkButtonsType, ButtonsType> buttontype = ButtonsOk,
                          OneOf<GtkDialogFlags, DialogFlags> flags = DialogDestroyWithParent,
                          Window *parent = NULL) : Dialog(DerivedType()) {
                Init(gtk_message_dialog_new_with_markup(*parent, flags, msgtype, buttontype,
                            msg.c_str()));
                Internal(true);
            }
            void Text(const std::string &text) {
                gtk_message_dialog_set_markup(*this, text.c_str());
            }
            void Secondary(const std::string &text) {
                gtk_message_dialog_format_secondary_markup(*this, text.c_str());
            }
            void Image(gtk::Image &image) {
                 gtk_message_dialog_set_image(*this, image);
            }
    };

    class Notebook : public Container
    {
         public:
            operator  GtkNotebook *() const { return GTK_NOTEBOOK(Obj()); }

            Notebook(GObject *obj) { Init(obj); }

            Notebook() {
                Init(gtk_notebook_new());
                Internal(true);
            }

            int Current() const { return gtk_notebook_get_current_page(*this); }
            void Current(int page) { gtk_notebook_set_current_page(*this, page); }

            int Append(const Widget &contents, const Widget &label) {
                return gtk_notebook_append_page(*this, contents, label);
            }
            int Append(const Widget &contents) {
                return gtk_notebook_append_page(*this, contents, NULL);
            }
            int Prepend(const Widget &contents, const Widget &label) {
                return gtk_notebook_prepend_page(*this, contents, label);
            }
            int Prepend(const Widget &contents) {
                return gtk_notebook_prepend_page(*this, contents, NULL);
            }
            int Insert(int position, const Widget &contents, const Widget &label) {
                return gtk_notebook_insert_page(*this, contents, label, position);
            }                
            int Insert(int position, const Widget &contents) {
                    return gtk_notebook_insert_page(*this, contents, NULL, position);
            }
            void Remove(int pos = -1) {
                gtk_notebook_remove_page(*this, pos);
            }
            int Page(const Widget &contents) const { return gtk_notebook_page_num(*this, contents); }

            Widget *GetNth(int page) { 
                return dynamic_cast<Widget *>(
                        Object::Find((GObject *)gtk_notebook_get_nth_page(*this, page)));
            }

            template <typename T>
            void OnPageSwitch(void (T::*cbk)(), T *base ) {
                 callback("switch-page", cbk, base);
            }
            template <typename T, typename J>
            void OnPageSwitch(void (T::*cbk)(J), T *base, J user_data) {
                 callback("switch-page", cbk, base, user_data);
            }

            Widget &CurrentPage() {
                if (Widget *w = GetNth(Current()))
                    return *w;
                else
                    throw std::runtime_error("Asked CurrentPage for a notebook without pages?");
            }

    };

    enum AttachOptions
    {
        Expand = GTK_EXPAND,
        Shrink = GTK_SHRINK,
        Fill   = GTK_FILL
    };

    inline AttachOptions operator|(const AttachOptions &a, const AttachOptions &b) {
        return (AttachOptions)(((int)a)|((int)b));
    }

    class Table : public Container { // COMPLETE API
        public:
            operator  GtkTable *() const { return GTK_TABLE(Obj()); }

            Table(GObject *obj) { Init(obj); }
            Table(int rows, int columns, bool homogeneous = false) {
                Init(gtk_table_new(rows, columns, homogeneous));
                Internal(true);
            }

            void Resize(int rows, int columns) { gtk_table_resize(*this, rows, columns); }

            void Spacing(int space) {
                RowSpacing(space);
                ColSpacing(space);
            }
            void RowSpacing(int space, int row = -1) {
                if (row > -1)
                    gtk_table_set_row_spacing(*this, row, space);
                else
                    gtk_table_set_row_spacings(*this, space);
            }
            int RowSpacing(int row = -1) const {
                if (row == -1)
                    return gtk_table_get_default_row_spacing(*this);
                else
                    return gtk_table_get_row_spacing(*this, row);
            }

            void Attach(const Widget &child, int left, int top, int right = -1, int bottom = -1,
                        OneOf<GtkAttachOptions, AttachOptions> xoptions = (Expand|Fill),
                        OneOf<GtkAttachOptions, AttachOptions> yoptions = (Expand|Fill),
                        int xpadding = 0, int ypadding = 0
                        ) {
                gtk_table_attach(*this, child, 
                        left, right != -1 ? right : left + 1,
                        top, bottom != -1 ? bottom : top + 1,
                        xoptions, yoptions, xpadding, ypadding);
            }

            void ColSpacing(int space, int column = -1) {
                if (column > -1)
                    gtk_table_set_col_spacing(*this, column, space);
                else
                    gtk_table_set_col_spacings(*this, space);
            }
            int ColSpacing(int column = -1) const {
                if (column == -1)
                    return gtk_table_get_default_col_spacing(*this);
                else
                    return gtk_table_get_col_spacing(*this, column);
            }

            void Homogeneous(bool flag) { gtk_table_set_homogeneous(*this, flag); }
            bool Homogeneous() const { return gtk_table_get_homogeneous(*this); }
    };

    enum ShadowType
    {
        ShadowNone = GTK_SHADOW_NONE,
        ShadowIn = GTK_SHADOW_IN,
        ShadowOut = GTK_SHADOW_OUT,
        ShadowEtchedIn = GTK_SHADOW_ETCHED_IN,
        ShadowEtchedOut = GTK_SHADOW_ETCHED_OUT
    };

    class Frame : public Bin { // COMPLETE API
        public:
            operator  GtkFrame *() const { return GTK_FRAME(Obj()); }
            Frame(const DerivedType &) {}
            Frame(GObject *obj) { Init(obj); }

            Frame(const std::string &label) {
                Init(gtk_frame_new(label.empty() ? NULL : label.c_str()));
                Internal(true);
            }
            Frame(const Widget &child, const std::string &label) {
                Init(gtk_frame_new(label.empty() ? NULL : label.c_str()));
                Child(child);
                Internal(true);
            }

            void Label(const std::string &label) { gtk_frame_set_label(*this, label.c_str()); }
            void Label(Widget &widget) { LabelWidget(widget); }
            void LabelWidget(Widget &label) { gtk_frame_set_label_widget(*this, label); }
            std::string Label() const { return gtk_frame_get_label(*this); }
            Widget *LabelWidget() const { 
                return dynamic_cast<Widget *>(
                        Object::Find((GObject *)gtk_frame_get_label_widget(*this))); 
            }

            void LabelAlign(const Align &align) { gtk_frame_set_label_align(*this, align.first, align.second); }
            Align LabelAlign() const {
                Align align;
                gtk_frame_get_label_align(*this, &align.first, &align.second);
                return align;
            }

            void Shadow(OneOf<GtkShadowType, ShadowType> shadow) { gtk_frame_set_shadow_type(*this, shadow); }
            OneOf<GtkShadowType, ShadowType> Shadow() const { return gtk_frame_get_shadow_type(*this); }
    };

    class AspectFrame : public Frame {  // COMPLETE API
        public:
            operator  GtkAspectFrame *() const { return GTK_ASPECT_FRAME(Obj()); }
            AspectFrame(GObject *obj) : Frame(DerivedType()) { Init(obj); }

            AspectFrame(float ratio, const std::string &label = "",
                        const Align &align = Align(0.5, 0.5), 
                        bool obey_child = false) : Frame(DerivedType()) {
                Init(gtk_aspect_frame_new(label.empty() ? NULL : label.c_str(),
                                     align.first, align.second, ratio, obey_child));
                Internal(true);
            }

            void Set(float ratio, 
                     const Align &align = Align(0.5, 0.5), 
                     bool obey_child = false) {
                gtk_aspect_frame_set(*this, align.first, align.second, ratio, obey_child);
            }
    };

    enum ToolbarStyle
    {
        ToolbarIcons = GTK_TOOLBAR_ICONS,
        ToolbarText = GTK_TOOLBAR_TEXT,
        ToolbarBoth = GTK_TOOLBAR_BOTH,
        ToolbarBothHoriz = GTK_TOOLBAR_BOTH_HORIZ
    };

    class ToolItem : public Bin {
        public:
            operator  GtkToolItem *() const { return GTK_TOOL_ITEM(Obj()); }
            ToolItem(GObject *obj) { Init(obj); }
            ToolItem(const DerivedType &) {} // do nothing
            ToolItem() { Init(gtk_tool_item_new()); Internal(true); }

            bool Expand() const { return gtk_tool_item_get_expand(*this); }
            void Expand(bool flag) { return gtk_tool_item_set_expand(*this, flag); }
    };

    class ToolButton : public ToolItem // COMPLETE API
    {
        public:
            operator  GtkToolButton *() const { return GTK_TOOL_BUTTON(Obj()); }
            ToolButton(GObject *obj) : ToolItem(DerivedType()) { Init(obj); }

            ToolButton() :
                ToolItem(DerivedType()) {
                Init(gtk_tool_button_new(NULL, NULL)); 
                Internal(true); 
            }
            ToolButton(const Widget &icon, char *label = NULL) :
                ToolItem(DerivedType()) {
                Init(gtk_tool_button_new(icon, label)); 
                Internal(true); 
            }
            ToolButton(const char *stock_id) : 
                ToolItem(DerivedType()) {
                Init(gtk_tool_button_new_from_stock(stock_id));
                Internal(true); 
            }

            void Label(const std::string &label) {
                gtk_tool_button_set_label(*this, label.c_str());
            }
            std::string Label() const { 
                if (const gchar *label = gtk_tool_button_get_label(*this)) 
                    return label;
                else
                    return "";
            }
            void StockId(const std::string &label) {
                gtk_tool_button_set_stock_id(*this, label.c_str());
            }
            std::string StockId() const {
                if (const gchar *id = gtk_tool_button_get_stock_id(*this)) 
                    return id; 
                else
                    return "";
            }

            void Icon(const Widget &icon) { gtk_tool_button_set_icon_widget(*this, icon); }
            Widget *Icon() const { 
                return dynamic_cast<Widget *>(
                        Object::Find((GObject *)gtk_tool_button_get_icon_widget(*this))); 
            }
            void Underline(bool flag) { gtk_tool_button_set_use_underline(*this, flag); }
            bool Underline() const { return gtk_tool_button_get_use_underline(*this); }

            template <typename T>
            void OnClick(void (T::*cbk)(), T *base ) {
                callback("clicked", cbk, base);
            }

    };

    class SeparatorToolItem : public ToolItem // COMPLETE API
    {
        public:
            operator  GtkSeparatorToolItem *() const { return GTK_SEPARATOR_TOOL_ITEM(Obj()); }
            SeparatorToolItem(GObject *obj) : ToolItem(DerivedType()) { Init(obj); }
            SeparatorToolItem() : ToolItem(DerivedType()) {
                Init(gtk_separator_tool_item_new()); 
                Internal(true); 
            }

            bool Draw() const { return gtk_separator_tool_item_get_draw(*this); }
            void Draw(bool flag) { return gtk_separator_tool_item_set_draw(*this, flag); }
    };

    class Toolbar : public Container {
        private:
            void newitem() { Init(gtk_toolbar_new()); Internal(true); }
        public:
            operator  GtkToolbar *() const { return GTK_TOOLBAR(Obj()); }
            Toolbar(GObject *obj) { Init(obj); }
            Toolbar() { newitem(); }

            Toolbar(const ToolItem &i1) { 
                newitem(); Append(i1);
            }
            Toolbar(const ToolItem &i1, const ToolItem &i2) { 
                newitem(); Append(i1); Append(i2);
            }
            Toolbar(const ToolItem &i1, const ToolItem &i2, const ToolItem &i3) { 
                newitem(); Append(i1); Append(i2); Append(i3);
            }
            Toolbar(const ToolItem &i1, const ToolItem &i2, const ToolItem &i3, const ToolItem &i4) { 
                newitem(); Append(i1); Append(i2); Append(i3); Append(i4);
            }
            Toolbar(const ToolItem &i1, const ToolItem &i2, const ToolItem &i3, const ToolItem &i4, const ToolItem &i5) { 
                newitem(); Append(i1); Append(i2); Append(i3); Append(i4); Append(i5);
            }
            Toolbar(const ToolItem &i1, const ToolItem &i2, const ToolItem &i3, const ToolItem &i4, const ToolItem &i5, const ToolItem &i6) { 
                newitem(); Append(i1); Append(i2); Append(i3); Append(i4); Append(i5); Append(i6);
            }
            Toolbar(const ToolItem &i1, const ToolItem &i2, const ToolItem &i3, const ToolItem &i4, const ToolItem &i5, const ToolItem &i6, const ToolItem &i7) { 
                newitem(); Append(i1); Append(i2); Append(i3); Append(i4); Append(i5); Append(i6); Append(i7);
            }

            void IconSize(OneOf<GtkIconSize, gtk::IconSize> size) {
                gtk_toolbar_set_icon_size(*this, size);
            }
            OneOf<GtkIconSize, gtk::IconSize> IconSize() const {
                return gtk_toolbar_get_icon_size(*this);
            }
            void Style(OneOf<GtkToolbarStyle, ToolbarStyle> style) {
                gtk_toolbar_set_style(*this, style);
            }
            OneOf<GtkToolbarStyle, ToolbarStyle> Style() const {
                return gtk_toolbar_get_style(*this);
            }
            ToolItem *GetNth(int idx) { 
                return dynamic_cast<ToolItem *>(
                        Object::Find((GObject *)gtk_toolbar_get_nth_item(*this, idx)));
            }
            void Insert(const ToolItem &item, int pos) {
                gtk_toolbar_insert(*this, item, pos);
            }
            void Append(const ToolItem &item) { Insert(item, -1); }
    };

    class Statusbar : public HBox // complete API
    {
        public:
            operator  GtkStatusbar *() const { return GTK_STATUSBAR(Obj()); }
            Statusbar(GObject *obj) : HBox(DerivedType()) { Init(obj); }
            Statusbar() : HBox(DerivedType()) { 
                Init(gtk_statusbar_new());
                Internal(true);
            }

            int ContextId(const std::string &desc) {
                return gtk_statusbar_get_context_id(*this, desc.c_str());
            }

            int Push(int context_id, const std::string &text) {
                return gtk_statusbar_push(*this, context_id, text.c_str());
            }
            void Pop(int context_id) { gtk_statusbar_pop(*this, context_id); }
            void Remove(int context_id, int message_id) {
                gtk_statusbar_remove(*this, context_id, message_id);
            }

            void ResizeGrip(bool flag) { gtk_statusbar_set_has_resize_grip(*this, flag); }
            bool ResizeGrip() const { return gtk_statusbar_get_has_resize_grip(*this); }
    };
}

#endif
