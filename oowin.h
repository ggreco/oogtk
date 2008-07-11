#ifndef OOWIN_H
#define OOWIN_H

namespace gtk {
    /** A Window can be one of these types. 
Most things you'd consider a "window" should have type WindowTopLevel; windows with this type are managed by the window manager and have a frame by default (call Window::Decorated(bool) to toggle the frame). Windows with type WindowPopup are ignored by the window manager; window manager keybindings won't work on them, the window manager won't decorate the window with a frame, many GTK+ features that rely on the window manager will not work (e.g. resize grips and maximization/minimization). WindowPopup is used to implement widgets such as Menu or tooltips that you normally don't think of as windows per se. Nearly all windows should be WindowTopLevel. In particular, do not use WindowPopup just to turn off the window borders; use Window::Decorated(bool) for that. 
*/
    enum WindowType
    {
        WindowTopLevel = GTK_WINDOW_TOPLEVEL /**< A regular window, such as a dialog. */,
        WindowPopup = GTK_WINDOW_POPUP /**< A special window such as a tooltip. */
    };

    /** Window placement can be influenced using this enumeration. 
Note that using PositionCenterAlways is almost always a bad idea. It won't necessarily work well with all window managers or on all windowing systems.
*/
    enum WindowPosition
    {
      PositionNone           = GTK_WIN_POS_NONE /**< No influence is made on placement. */,
      PositionCenter         = GTK_WIN_POS_CENTER /**< Windows should be placed in the center of the screen.  */,
      PositionMouse          = GTK_WIN_POS_MOUSE /**< Windows should be placed at the current mouse position. */,
      PositionCenterAlways   = GTK_WIN_POS_CENTER_ALWAYS /**< Keep window centered as it changes size, etc.  */,
      PositionCenterOnParent = GTK_WIN_POS_CENTER_ON_PARENT /**< Center the window on its transient parent (see Window::TransientFor()). */
    };

    class AccelGroup : public Object {
        public:
            operator  GtkAccelGroup *() const { return GTK_ACCEL_GROUP(obj_); }
            AccelGroup(GObject *obj) { Init(obj); }
    };


    /** Toplevel which can contain other widgets.
A Window is a toplevel widget that can contain other widgets. Nearly always, the type of the window should be WindowTopLevel (that is the default value of the constructor if you don't specify one). If you're implementing something like a popup menu from scratch (which is a bad idea, just use Menu), you might use WindowPopup. WindowPopup is not for dialogs, though in some other toolkits dialogs are called "popups". In GTK+, WindowPopup means a pop-up menu or pop-up tooltip. On X11, popup windows are not controlled by the window manager.

If you simply want an undecorated window (no window borders), use Window::Decorated(bool), don't use WindowPopup type.
    */
    class Window : public Bin
    {
        public:
/// DOXYS_OFF
            operator  GtkWindow *() const { return GTK_WINDOW(Obj()); }
            Window(const DerivedType &) {} // do nothing
            Window(GObject *obj) { Init(obj); }
/// DOXYS_ON
            /** Create a new Window object. 
            See Window class description for details.
            */
            Window(const std::string &title = "" /**< Window title*/, 
                   OneOf<GtkWindowType, WindowType> type = WindowTopLevel /**< Window type, defaults to WindowTopLevel */) {
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
            /** Sets the title of the GtkWindow. 
The title of a window will be displayed in its title bar; on the X Window System, the title bar is rendered by the window manager, so exactly how the title appears to users may vary according to a user's exact configuration. The title should help a user distinguish this window from other windows they may have open. A good title might include the application name and current document filename, for example.
               */
            void Title(const std::string &title /**< title of the window */) { 
                gtk_window_set_title(*this, title.c_str()); 
            }

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
            bool Active() const { return gtk_window_is_active(*this); }

            // shortcut GTK uses
            void Move(int x, int y) { Position(x, y); }
            void Resize(int width, int height) { Size(width, height); }

            /** Returns whether the window has been set to have decorations such as a title bar.
            \sa Window::Decorated(bool)
            \return true if the window has been set to have decorations 
            */
            bool Decorated() const {
                return gtk_window_get_decorated(*this);
            }
            /** Enable/disable window decorations.
            By default, windows are decorated with a title bar, resize controls, etc. Some window managers allow GTK+ to disable these decorations, creating a borderless window. If you set the decorated property to false using this function, GTK+ will do its best to convince the window manager not to decorate the window. Depending on the system, this function may not have any effect when called on a window that is already visible, so you should call it before calling Window::Show().

On Windows, this function always works, since there's no window manager policy involved.
            */
            void Decorated(bool flag /**< true to decorate the window */) {
                gtk_window_set_decorated(*this, flag);
            }

            /** Returns whether the window has been set to have a close button
            \sa Window::Deletable(bool);
            \return true if the window has been set to have a close button 
            \note Since GTK 2.10
*/
            bool Deletable() const {
                return gtk_window_get_deletable(*this);
            }
            /** Enable/disable window close button.
By default, windows have a close button in the window frame. Some window managers allow GTK+ to disable this button. If you set the deletable property tofalse using this function, GTK+ will do its best to convince the window manager not to show a close button. Depending on the system, this function may not have any effect when called on a window that is already visible, so you should call it before calling Window::Show().

On Windows, this function always works, since there's no window manager policy involved.
\note Since GTK 2.10
            */
            void Deletable(bool flag /**< true to enable the close widget for the window */) {
                gtk_window_set_deletable(*this, flag);
            }
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
                Init(gtk_file_chooser_dialog_new(title.c_str(), NULL, action, 
                                          NULL, NULL));
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
                char *msg;
                va_list va;

                va_start(va, msg_format);
                g_vasprintf(&msg, msg_format, va);
                va_end(va);

                Init(gtk_message_dialog_new_with_markup(*parent, flags, msgtype, buttontype,
                        msg));
                g_free(msg);
                Internal(true);
            }
            MessageDialog(va_list va, Window *parent, OneOf<GtkDialogFlags, DialogFlags> flags,
                          OneOf<GtkMessageType, MessageType> msgtype,
                          OneOf<GtkButtonsType, ButtonsType> buttontype,
                          const char *msg_format) : Dialog(DerivedType()) {
                char *msg;
                g_vasprintf(&msg, msg_format, va);

                Init(gtk_message_dialog_new_with_markup(*parent, flags, msgtype, buttontype,
                        msg));
                g_free(msg);
                Internal(true);
            }
            MessageDialog(const std::string &msg,
                          OneOf<GtkMessageType, MessageType> msgtype = MessageInfo,
                          OneOf<GtkButtonsType, ButtonsType> buttontype = ButtonsOk,
                          OneOf<GtkDialogFlags, DialogFlags> flags = DialogDestroyWithParent,
                          Window *parent = NULL) : Dialog(DerivedType()) {
                Init(gtk_message_dialog_new_with_markup(parent ? GTK_WINDOW(parent->Obj()) : NULL, flags, msgtype, buttontype,
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
}

#endif
