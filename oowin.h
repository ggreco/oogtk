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
            See Window class description for details. The type of the window can be chosen from the values in the WindowType enumeration and defaults to WindowTopLevel.
            */
            Window(const std::string &title = "" /**< Window title*/, 
                   OneOf<GtkWindowType, WindowType> type = WindowTopLevel /**<A Window type value from WindowType, defaults to WindowTopLevel */) {
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



/** A FileFilter can be used to restrict the files being shown in a FileChooser. 

Files can be filtered based on their name (FileFilter::AddPattern()), on their mime type (with FileFilter::AddMimeType()), or by a custom filter function (with FileFilter::AddCustom()).

Filtering by mime types handles aliasing and subclassing of mime types; e.g. a filter for text/plain also matches a file with mime type application/rtf, since application/rtf is a subclass of text/plain. Note that FileFilter allows wildcards for the subtype of a mime type, so you can e.g. filter for image / *.

Normally, filters are used by adding them to a FileChooser, see FileChooser::Filter(), but it is also possible to manually use a filter on a file with FileFilter::Filter(). 
*/
    class FileFilter : public Object
    {
        public:
            /// These flags indicate what informations are needed by a FileFilter::Custom() callback. 
            enum Flags {
                Filename = GTK_FILE_FILTER_FILENAME /**< the filename of the file being tested */,
                URI = GTK_FILE_FILTER_URI /**< the URI for the file being tested  */,
                DisplayName = GTK_FILE_FILTER_DISPLAY_NAME /**< the string that will be used to display the file in the file chooser */,
                MimeType = GTK_FILE_FILTER_MIME_TYPE /**< the mime type of the file  */
            };

        /// Support struct for FileFilter::Custom()
            struct Info
            {
        /// DOXYS_OFF        
                Info(GtkFileFilterInfo *info) :
                    flags(info->contains) {
                    if (flags & FileFilter::Filename)
                        filename = info->filename;
                    if (flags & FileFilter::URI)
                        uri = info->uri;
                    if (flags & FileFilter::DisplayName)
                        displayname = info->display_name;
                    if (flags & FileFilter::MimeType)
                        mimetype = info->mime_type;
                }
                Info() : flags(0) {}
        /// DOXYS_ON
                uint32_t flags /**< Flags indicating which of the following fields need are filled */; 
                std::string filename /**< 	the filename of the file being tested */;
                std::string uri /**< 	the URI for the file being tested */;
                std::string displayname /**< 	the string that will be used to display the file in the file chooser */;
                std::string mimetype /**< 	the mime type of the file */;
            };

        private:
            class AbstractFilter {
                public:
                    virtual ~AbstractFilter() {}
                    virtual void notify(Info & info) = 0;
            };
            template <typename T, typename J>
            class FilterCbk : public AbstractFilter
            {
                public:
                    FilterCbk(T*obj, void (T::*func)(Info &)) : 
                        myObj(obj), myFnc0(func), myFnc1(NULL) {}
                    FilterCbk(T*obj, void (T::*func)(Info &), J data) :
                        myObj(obj), myFnc1(func), myFnc0(NULL), ma1(data) {}
                    void notify(Info &val) {
                        if (myFnc0)
                            (myObj->*myFnc0)(val);
                        else 
                            (myObj->*myFnc1)(val, ma1);
                    }
                private:
                    T* myObj;
                    void (T::*myFnc0)(Info &);
                    void (T::*myFnc1)(Info &, J);
                    J ma1;
            };
            static void filter_wrapper(GtkFileFilterInfo *info, AbstractFilter *cbk) {
                Info f(info);
                cbk->notify(f);
            }
            std::vector<AbstractFilter *> cbks_;
        public:
/// DOXYS_OFF       
            operator GtkFileFilter *() const { return GTK_FILE_FILTER(Obj()); }
            FileFilter(GObject *obj) { Init(obj); }     
            ~FileFilter() {
                for(std::vector<AbstractFilter *>::iterator it = cbks_.begin(); it != cbks_.end(); ++it)
                    delete (*it);
            }
/// DOXYS_ON
/** Creates a new FileFilter with no rules added to it. 

Such a filter doesn't accept any files, so is not particularly useful until you add rules with FileFilter::AddMimeType(), FileFilter::AddPattern(), or FileFilter::AddCustom(). To create a filter that accepts any file, use:

\example
FileFilter filter;
Filter::AddPattern("*");
\endexample
*/
            FileFilter(const std::string &name = "") { 
                Init(gtk_file_filter_new());
                if (!name.empty())
                    Name(name);
                Internal(true);
            }
/** Sets the human-readable name of the filter.

This is the string that will be displayed in the file selector user interface if there is a selectable list of filters.
*/
            void Name(const std::string &name /**< the name for this filter */) { gtk_file_filter_set_name(*this, name.c_str()); }
            /** Gets the human-readable name for the filter. 
            \sa FileFilter::Name(const std::string &)
            \return The human-readable name of the filter, or an empty string if none.
             */
            std::string Name() const { 
                if (const gchar *c = gtk_file_filter_get_name(*this)) 
                    return c; 
                else 
                    return "";
            }
            /// Adds a rule allowing a shell style glob to a filter.
            void AddPattern(const std::string &pattern /**< a shell style glob  */) {
                gtk_file_filter_add_pattern(*this, pattern.c_str());
            }
            /// Adds a rule allowing a given mime type to filter.
            void AddMimeType(const std::string &type /**< name of a MIME type */) {
                gtk_file_filter_add_mime_type(*this, type.c_str());
            }
            /// Adds a rule allowing image files in the formats supported by GdkPixbuf.
            void AddPixbufFormats() { gtk_file_filter_add_pixbuf_formats(*this); }
/** Adds rule to a filter that allows files based on a custom callback function. 

The bitfield needed which is passed in provides information about what sorts of information that the filter function needs; this allows GTK+ to avoid retrieving expensive information when it isn't needed by the filter.
*/
            template <typename T, typename J>
            void AddCustom(uint32_t flags /**< bitfield of FileFilter::Flags indicating the information that the custom filter function needs.  */, 
                        void (T::*callback)(Info &, J) /**< member function pointer to the function to call */,
                        T *base /**< pointer to the base of the class */, 
                        J data /**< user data for the function*/) {
                AbstractFilter *cbk = new FilterCbk<T,J>(base, callback, data);
                cbks_.push_back(cbk);
                gtk_file_filter_add_custom(*this, flags, filter_wrapper, cbk, NULL);
            }
/** Adds rule to a filter that allows files based on a custom callback function. 

The bitfield needed which is passed in provides information about what sorts of information that the filter function needs; this allows GTK+ to avoid retrieving expensive information when it isn't needed by the filter.
*/
            template <typename T>
            void AddCustom(uint32_t flags /**< bitfield of FileFilter::Flags indicating the information that the custom filter function needs.  */, 
                        void (T::*callback)(Info &) /**< member function pointer to the function to call */,
                        T *base /**< pointer to the base of the class */) {
                AbstractFilter *cbk = new FilterCbk<T, void>(base, callback);
                cbks_.push_back(cbk);
                gtk_file_filter_add_custom(*this, flags, filter_wrapper, cbk, NULL);
            }
    };

    /** File chooser interface used by FileChooserWidget and FileChooserDialog

FileChooser is an interface that can be implemented by file selection widgets. In GTK+, the main objects that implement this interface are FileChooserWidget, FileChooserDialog, and FileChooserButton. You do not need to write an object that implements the FileChooser interface unless you are trying to adapt an existing file selector to expose a standard programming interface.

FileChooser allows for shortcuts to various places in the filesystem. In the default implementation these are displayed in the left pane. It may be a bit confusing at first taht these shortcuts come from various sources and in various flavours, so lets explain the terminology here:

|Bookmarks|are created by the user, by dragging folders from the right pane to the left pane, or by using the "Add". Bookmarks can be renamed and deleted by the user.
|Shortcuts|can be provided by the application or by the underlying filesystem abstraction (e.g. both the gnome-vfs and the Windows filesystems provide "Desktop" shortcuts). Shortcuts cannot be modified by the user.
|Volumes|are provided by the underlying filesystem abstraction. They are the "roots" of the filesystem. 

!File Names and Encodings

When the user is finished selecting files in a FileChooser, your program can get the selected names either as filenames or as URIs. For URIs, the normal escaping rules are applied if the URI contains non-ASCII characters. However, filenames are always returned in the character set specified by the G_FILENAME_ENCODING environment variable. Please see the Glib documentation for more details about this variable. 
#IMPORTANT:# This means that while you can pass the result of FileChooser::Filename() to open(2) or fopen(3), you may not be able to directly set it as the text of a Label widget unless you convert it first to UTF-8, which all GTK+ widgets expect. You should use g_filename_to_utf8() to convert filenames into strings that can be passed to GTK+ widgets.

!Key Bindings

Internally, GTK+ implements a file chooser's graphical user interface with the private GtkFileChooserDefaultClass. This widget has several key bindings and their associated signals. This section describes the available key binding signals.

FileChooser key binding example

The default keys that activate the key-binding signals in GtkFileChooserDefaultClass are as follows:

||Signal name||Default key combinations
|location-popup|Control+L (empty path); / (path of "/")[a]; ~ (path of "~")
|up-folder|Alt+Up[b] ; Backspace
|down-folder|Alt+Down
|home-folder|Alt+Home
|desktop-folder|Alt+D
|quick-bookmark|Alt+1 through Alt+0

[a] Both the individual / key and the numeric keypad's "divide" key are supported.
[b] Both the individual Up key and the numeric keypad's Up key are supported.

You can change these defaults to something else. For example, to add a Shift modifier to a few of the default bindings, you can include the following fragment in your .gtkrc-2.0 file:

\example
binding "my-own-gtkfilechooser-bindings" {
	bind "<Alt><Shift>Up" {
		"up-folder" ()
	}
	bind "<Alt><Shift>Down" {
		"down-folder" ()
	}
	bind "<Alt><Shift>Home" {
		"home-folder" ()
	}
}

class "GtkFileChooserDefault" binding "my-own-gtkfilechooser-bindings"
\endexample	


*/
    class FileChooser {
        public:
            /// Describes whether a FileChooser is being used to open existing files or to save to a possibly new file. 
            enum ActionValue
            {
                ActionOpen = GTK_FILE_CHOOSER_ACTION_OPEN /**< Indicates open mode. The file chooser will only let the user pick an existing file.  */,
                ActionSave = GTK_FILE_CHOOSER_ACTION_SAVE /**< Indicates save mode. The file chooser will let the user pick an existing file, or type in a new filename. */,
                ActionSelectFolder = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER /**< Indicates an Open mode for selecting folders. The file chooser will let the user pick an existing folder. */,
                ActionCreateFolder = GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER /**< Indicates a mode for creating a new folder. The file chooser will let the user name an existing or new folder. */
            };

/// DOXYS_OFF
            virtual GtkFileChooser *getobj() const = 0; // this is needed to make it pure virtual
            virtual ~FileChooser() {}
/// DOXYS_ON
/** Gets the URI for the currently selected file in the file selector. 

If multiple files are selected, one of the filenames will be returned at random.

If the file chooser is in folder mode, this function returns the selected folder.

\return The currently selected URI, or an empty string if no file is selected. 
*/
            std::string URI() const {
                std::string file;
                if (gchar *name = gtk_file_chooser_get_uri(getobj())) {
                    file = name;
                    g_free(name);
                }
                return file;
            }      
/** Sets the file referred to by uri as the current file for the file chooser.
The operation is performed by changing to the URI's parent folder and actually selecting the URI in the list. If the chooser is FileChooser::ActionSave mode, the URI's base name will also appear in the dialog's file name entry.

If the URI isn't in the current folder of chooser, then the current folder of chooser will be changed to the folder containing uri. This is equivalent to a sequence of FileChooser::UnselectAll() followed by FileChooser::SelectURI(). 

\return true if both the folder could be changed and the URI was selected successfully, false otherwise. 
*/
            bool URI(const std::string &uri /**< the URI to set as current */) {
                return gtk_file_chooser_set_uri(getobj(), uri.c_str());
            }
/** Gets the filename for the currently selected file in the file selector.

If multiple files are selected, one of the filenames will be returned at random.
If the file chooser is in folder mode, this function returns the selected folder.
\return The currently selected filename, or an empty string if no file is selected, or the selected file can't be represented with a local filename.
*/
            std::string Filename() const {
                std::string file;
                if (gchar *name = gtk_file_chooser_get_filename(getobj())) {
                    file = name;
                    g_free(name);
                }
                return file;
            }
/** Sets filename as the current filename for the file chooser.

This operation is performed by changing to the file's parent folder and actually selecting the file in list. If the chooser is in FileChooser::ActionSave mode, the file's base name will also appear in the dialog's file name entry.

If the file name isn't in the current folder of chooser, then the current folder of chooser will be changed to the folder containing filename. This is equivalent to a sequence of FileChooser::UnselectAll() followed by FileChooser::SelectFilename().

Note that the file must exist, or nothing will be done except for the directory change.

If you are implementing a File/Save As... dialog, you should use this function if you already have a file name to which the user may save; for example, when the user opens an existing file and then does File/Save As... on it. If you don't have a file name already — for example, if the user just created a new file and is saving it for the first time, do not call this function. Instead, use something similar to this:

\example
if (document_is_new) {
    // the user just created a new document
    chooser.CurrentFolder(default_folder_for_saving);
    chooser.CurrentName("Untitled document");
} else {
    // the user edited an existing document 
    chooser.Filename(existing_filename);
}
\endexample

\return true if both the folder could be changed and the file was selected successfully, false otherwise. 
*/
            bool Filename(const std::string &name /**< 	 the filename to set as current */) {
                return gtk_file_chooser_set_filename(getobj(), name.c_str());
            }

/** Gets the current folder of chooser as a local filename.
 *
Note that this is the folder that the file chooser is currently displaying (e.g. "/home/username/Documents"), which is not the same as the currently-selected folder if the chooser is in FileChooser::SelectFolder mode (e.g. "/home/username/Documents/selected-folder/". To get the currently-selected folder in that mode, use FileChooser::URI() as the usual way to get the selection.

\return the full path of the current folder, or an empty string if the current path cannot be represented as a local filename. This function will also return an empty string if the file chooser was unable to load the last folder that was requested from it; for example, as would be for calling FileChooser::CurrentFolder(const std::string &) on a nonexistent folder. 
\sa FileChooser::CurrentFolder(const std::string &)
*/
            std::string CurrentFolder() const {
                std::string result;
                if (gchar *folder = gtk_file_chooser_get_current_folder(getobj())) {
                    result = folder;
                    g_free(folder);
                }
                return result;
            }
/** Sets the current folder for chooser from a local filename. 

The user will be shown the full contents of the current folder, plus user interface elements for navigating to other folders.

\return true if the folder could be changed successfully, false otherwise. 
 */
            bool CurrentFolder(const std::string &folder /**< the full path of the new current folder */) {
                 return gtk_file_chooser_set_current_folder(getobj(), folder.c_str());
            }

            /** Gets whether only local files can be selected in the file selector.
            \sa FileChooser::LocalOnly(bool)
            \return true if only local files can be selected 
            */
            bool LocalOnly() const { return gtk_file_chooser_get_local_only(getobj()); }
            /** Sets whether only local files can be selected in the file selector. 
If local_only is true (the default), then the selected file are files are guaranteed to be accessible through the operating systems native file file system and therefore the application only needs to worry about the filename functions in FileChooser, like FileChooser::Filename(), rather than the URI functions like FileChooser::Uri().
             */
            void LocalOnly(bool flag /**< true if only local files can be selected. */) { 
                gtk_file_chooser_set_local_only(getobj(), flag); 
            }
            /** Sets whether multiple files can be selected in the file selector. 
This is only relevant if the action is set to be FileChooser::SelectFolder or FileChooser::ActionOpen.
            */
            void SelectMultiple(bool flag /**< true if multiple files can be selected. */) { 
                gtk_file_chooser_set_select_multiple(getobj(), flag); 
            }
            /** Gets whether multiple files can be selected in the file selector. 
            \sa FileChooser::SelectMultiple(bool)
            \return true if multiple files can be selected. 
            */
            bool SelectMultiple() const { return gtk_file_chooser_get_select_multiple(getobj()); }
            /// Sets whether hidden files and folders are displayed in the file selector.
            void ShowHidden(bool flag /**< true if hidden files and folders should be displayed.  */) { 
                gtk_file_chooser_set_show_hidden(getobj(), flag); 
            }
            /// Gets whether hidden files and folders are displayed in the file selector. 
            /// \return true if hidden files and folders are displayed. 
            bool ShowHidden() const { return gtk_file_chooser_get_show_hidden(getobj()); }

            /// Gets the type of operation that the file chooser is performing
            /// \return the action that the file selector is performing 
            OneOf<GtkFileChooserAction, FileChooser::ActionValue> Action() const { 
                return gtk_file_chooser_get_action(getobj()); 
            }
            /// Sets the type of operation that the chooser is performing; the user interface is adapted to suit the selected action. For example, an option to create a new folder might be shown if the action is FileChooser::ActionSave but not if the action is FileChooser::ActionOpen.
            void Action(OneOf<GtkFileChooserAction, FileChooser::ActionValue> action /**< the action that the file selector is performing */) { 
                gtk_file_chooser_set_action(getobj(), action); 
            }

            /// Queries whether a file chooser is set to confirm for overwriting when the user types a file name that already exists.
            /// \return true if the file chooser will present a confirmation dialog; false otherwise. 
            bool OverwriteConfirmation() const { return gtk_file_chooser_get_do_overwrite_confirmation(getobj()); }

/** Sets whether a file chooser in FileChooser::ActionSave mode will present a confirmation dialog if the user types a file name that already exists. 

This is false by default.

Regardless of this setting, the chooser will emit the "confirm-overwrite" signal when appropriate.

If all you need is the stock confirmation dialog, set this property to true. You can override the way confirmation is done by actually handling the "confirm-overwrite" signal; please refer to its GTK+ documentation for the details.
*/
            void OverwriteConfirmation(bool flag /**< whether to confirm overwriting in save mode */) { 
                gtk_file_chooser_set_do_overwrite_confirmation(getobj(), flag); 
            }

            /// Selects all the files in the current folder of a file chooser.
            void SelectAll() { gtk_file_chooser_select_all(getobj()); }
            /// Unselects all the files in the current folder of a file chooser.
            void UnselectAll() { gtk_file_chooser_unselect_all(getobj()); }

            /** Selects a filename. 
 If the file name isn't in the current folder of chooser, then the current folder of chooser will be changed to the folder containing filename.

 \return true if both the folder could be changed and the file was selected successfully, false otherwise. 
 */
            bool SelectFilename(const std::string &name /**< the filename to select */) {
                return gtk_file_chooser_select_filename(getobj(), name.c_str());
            }
            /// Unselects a currently selected filename. If the filename is not in the current directory, does not exist, or is otherwise not currently selected, does nothing.
            void UnselectFilename(const std::string &name /**< the filename to unselect */) {
                gtk_file_chooser_unselect_filename(getobj(), name.c_str());
            }

/** Lists all the selected files and subfolders in the current folder of chooser. 
The returned names are full absolute paths. If files in the current folder cannot be represented as local filenames they will be ignored. (See FileChooser::URIs())
 */
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

            /// Adds filter to the list of filters that the user can select between. When a filter is selected, only files that are passed by that filter are displayed. 
            void AddFilter(const FileFilter &file /**< A FileFilter */) {
                gtk_file_chooser_add_filter(getobj(), file);
            }
            /// Removes filter from the list of filters that the user can select between.
            void RemoveFilter(const FileFilter &file /**< A FileFilter */) {
                gtk_file_chooser_remove_filter(getobj(), file);
            }

            /// Lists the current set of user-selectable filters.
            void ListFilters(std::vector<FileFilter *> &filters /**< A list of FileFilter pointers to be filled */) {
                GSList *list = gtk_file_chooser_list_filters(getobj()), 
                       *e = list;
                while (e) {
                    if (FileFilter *f = dynamic_cast<FileFilter *>(
                                    Object::Find((GObject *)e -> data)))
                        filters.push_back(f);                    
                    e = e -> next;
                }
                g_slist_free(list);
            }

            /** Sets the current filter; only the files that pass the filter will be displayed.

If the user-selectable list of filters is non-empty, then the filter should be one of the filters in that list. Setting the current filter when the list of filters is empty is useful if you want to restrict the displayed set of files without letting the user change it.
*/
            void Filter(const FileFilter &filter /**< A FileFilter class */) {
                gtk_file_chooser_set_filter(getobj(), filter);
            }
            /// Gets the current filter.
            /// \return the current filter or NULL if none is set
            FileFilter *Filter() const {
                return dynamic_cast<FileFilter *>(Object::Find((GObject *)
                            gtk_file_chooser_get_filter(getobj())));
            }
    };

    class FileChooserDialog : public Dialog, public FileChooser {
        public:
            operator  GtkFileChooserDialog *() const { return GTK_FILE_CHOOSER_DIALOG(Obj()); }

            FileChooserDialog(GObject *obj) : Dialog(DerivedType()) { Init(obj); }
         
            FileChooserDialog(const std::string &title = "", Window *parent_window = NULL,
                              OneOf<GtkFileChooserAction, FileChooser::ActionValue> action =  FileChooser::ActionOpen,
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
