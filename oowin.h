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
                   WindowType type = WindowTopLevel /**<A Window type value from WindowType, defaults to WindowTopLevel */) {
                Init(gtk_window_new((GtkWindowType)type));

                if (!title.empty())
                    gtk_window_set_title(GTK_WINDOW(obj_), title.c_str());
                Internal(true);
            }

            // windows are special objects and must be destroyed
            virtual ~Window() { /*if (ObjType() == InternalObj) */ gtk_widget_destroy(GTK_WIDGET(Obj())); }
            // window signals
            BUILD_EVENT(OnDelete, "delete_event");

            // accel group stuff
            /** Associate accel_group with window */
            void Add(const AccelGroup &group) { gtk_window_add_accel_group(*this, group); }
            /** Remove the accelgroup from the window */
            void Remove(const AccelGroup &group) { gtk_window_remove_accel_group(*this, group); }

            /** Find if the window is resizable or not */
            bool Resizable() const { return gtk_window_get_resizable(*this); }
            /** Set if the window is resizable or not */
            void Resizable(bool flag) { gtk_window_set_resizable(*this, flag); }

            /** Find if the window is modal or not */
            bool Modal() const { return gtk_window_get_modal(*this); }
            /** Set/reset a window as modal.
Modal windows prevent interaction with other windows in the same application. To keep modal dialogs on top of main application windows, use Window::TransientFor() to make the dialog transient for the parent; most window managers will then disallow lowering the dialog below the parent.
            */
            void Modal(bool flag /**< true to make the window modal, false otherwise */ ) { gtk_window_set_modal(*this, flag); }

            /** Gets the title of the Window. */
            std::string Title() const { return std::string(gtk_window_get_title(*this)); }
            /** Sets the title of the Window. 
The title of a window will be displayed in its title bar; on the X Window System, the title bar is rendered by the window manager, so exactly how the title appears to users may vary according to a user's exact configuration. The title should help a user distinguish this window from other windows they may have open. A good title might include the application name and current document filename, for example.
               */
            void Title(const std::string &title /**< title of the window */) { 
                gtk_window_set_title(*this, title.c_str()); 
            }
/// Assign a window to a particular screen            
            void Screen(GdkScreen *s) { gtk_window_set_screen(*this, s); }
/** Presents a window to the user. 

This may mean raising the window in the stacking order, deiconifying it, moving it to the current desktop, and/or giving it the keyboard focus, possibly dependent on the user's platform, window manager, and preferences.

If window is hidden, this function calls Window::Show() as well.

This function should be used when the user tries to open a window that's already open. Say for example the preferences dialog is currently open, and the user chooses Preferences from the menu a second time; use Window::Present() to move the already-open dialog where the user can see it. 
*/
            void Present() { gtk_window_present(*this); }
/** Obtains the current size of window. 

If window is not onscreen, it returns the size GTK+ will suggest to the window manager for the initial window size (but this is not reliably the same as the size the window manager will actually select). The size obtained by Window::Size() is the last size received in a EventConfigure, that is, GTK+ uses its locally-stored size, rather than querying the X server for the size. As a result, if you call Window::Resize() then immediately call Window::Size(), the size won't have taken effect yet. After the window manager processes the resize request, GTK+ receives notification that the size has changed via a configure event, and the size of the window gets updated.

Note 1: Nearly any use of this function creates a race condition, because the size of the window may change between the time that you get the size and the time that you perform some action assuming that size is the current size. To avoid race conditions, connect to "configure-event" on the window and adjust your size-dependent state to match the size delivered in the EventConfigure.

Note 2: The returned size does not include the size of the window manager decorations (aka the window frame or border). Those are not drawn by GTK+ and GTK+ has no reliable method of determining their size.

Note 3: If you are getting a window size in order to position the window onscreen, there may be a better way. The preferred way is to simply set the window's semantic type with Window::TypeHint(), which allows the window manager to e.g. center dialogs. Also, if you set the transient parent of dialogs with Window::TransientFor() window managers will often center the dialog over its parent window. It's much preferred to let the window manager handle these things rather than doing it yourself, because all apps will behave consistently and according to user prefs if the window manager handles it. Also, the window manager can take the size of the window decorations/border into account, while your application cannot.

In any case, if you insist on application-specified window positioning, there's still a better way than doing it yourself - Window::Position(const Point &) or better Window::Position(WindowPosition) will frequently handle the details for you.
*/
            Point Size() const { 
                Point pos;
                gtk_window_get_size(*this, &pos.x, &pos.y); 
                return pos;
            }
/** Resizes a window.

Symmetric shortcut for Window::Resize() added in OOGtk for API coherence.
\sa Window::Resize()
*/
            void Size(const Point &size /**< A point containing the wanted width and height for the window */) { Size(size.x, size.y); }
/** Resizes a window.
\sa Window::Resize()
*/
            void Size(int width /**< wanted width in pixels */, 
                      int height /**< wanted height in pixels */) 
            { gtk_window_resize(*this, width, height); }
/** Resizes the window as if the user had done so, obeying geometry constraints. 

The default geometry constraint is that windows may not be smaller than their size request; to override this constraint, call Widget::SizeRequest() to set the window's request to a smaller value.

If Window::Resize() is called before showing a window for the first time, it overrides any default size set.

Windows may not be resized smaller than 1 by 1 pixels.

\sa Window::Size(int, int), Window::Size(const Point &)
*/
            void Resize(int width /**< wanted width in pixels*/, 
                      int height /**< wanted height in pixels */) 
            { gtk_window_resize(*this, width, height); }
/** Obtain the position of a window.

This function returns the position you need to pass to Window::Position(const Point &) or Window::Position(int, int) to keep window in its current position. This means that the meaning of the returned value varies with window gravity. Window::Position(const Point &) for more details.

If you haven't changed the window gravity, its gravity will be GDK_GRAVITY_NORTH_WEST. This means that Window::Position() gets the position of the top-left corner of the window manager frame for the window. Window::Position(const Point &) sets the position of this same top-left corner.

Window::Position() is not 100% reliable because the X Window System does not specify a way to obtain the geometry of the decorations placed on a window by the window manager. Thus GTK+ is using a "best guess" that works with most window managers.

Moreover, nearly all window managers are historically broken with respect to their handling of window gravity. So moving a window to its current position as returned by Window::Position() tends to result in moving the window slightly. Window managers are slowly getting better over time.

\return a Point containing the top-left coordinates of the window.
*/
            Point Position() const { 
                Point pos;
                gtk_window_get_position(*this, &pos.x, &pos.y);
                return pos;
            }
            /** Move a window to (X,Y).
\sa Window::Position(const Point &)
*/
            void Position(int x /**< X coordinate of the top-left edge */, 
                          int y /**< Y coordinate of the top-left edge */) { gtk_window_move(*this, x, y); }
/** Asks the window manager to move window to the given position. 

Window managers are free to ignore this; most window managers ignore requests for initial window positions (instead using a user-defined placement algorithm) and honor requests after the window has already been shown.

Note: the position is the position of the gravity-determined reference point for the window. The gravity determines two things: first, the location of the reference point in root window coordinates; and second, which point on the window is positioned at the reference point.

By default the gravity is GDK_GRAVITY_NORTH_WEST, so the reference point is simply the x, y supplied to Window::Position(const Point &) or Window::Position(int, int). The top-left corner of the window decorations (aka window frame or border) will be placed at the x, y specified by the Point or directly in the function call. Therefore, to position a window at the top left of the screen, you want to use the default gravity (which is GDK_GRAVITY_NORTH_WEST) and move the window to 0,0.

To position a window at the bottom right corner of the screen, you would set GDK_GRAVITY_SOUTH_EAST, which means that the reference point is at x + the window width and y + the window height, and the bottom-right corner of the window border will be placed at that reference point. So, to place a window in the bottom right corner you would first set gravity to south east, then write: window->Position(gdk_screen_width() - window_width, gdk_screen_height() - window_height) (note that this example does not take multi-head scenarios into account).

The Extended Window Manager Hints specification at http://www.freedesktop.org/Standards/wm-spec has a nice table of gravities in the "implementation notes" section.           
*/
            void Position(const Point &origin /**< Top left corner window coordinates */) { Position(origin.x, origin.y); }

            /** Sets a position constraint for this window. 
If the old or new constraint is PositionCenterAlways, this will also cause the window to be repositioned to satisfy the new constraint.
*/            void Position(WindowPosition pos) { 
                gtk_window_set_position(*this, (GtkWindowPosition)pos); 
            }
/** Set a window as "transient" for a parent one.

Dialog windows should be set transient for the main application window they were spawned from. 

This allows window managers to e.g. keep the dialog on top of the main window, or center the dialog over the main window. Dialog::Dialog() and other convenience functions in GTK+ will sometimes call Window::TransientFor() on your behalf.

On Windows, this function puts the child window on top of the parent, much as the window manager would have done on X.
*/
            void TransientFor(Window &parent /**< parent window */ ) { gtk_window_set_transient_for(*this, parent); }
            bool Active() const { return gtk_window_is_active(*this); }

            // shortcut GTK uses
            void Move(int x, int y) { Position(x, y); }

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
            /** Create a new Pixbuf object with the window contents.
This is an utility method that can be used to create a Pixbuf object from a window to dump
it to disk with Pixbuf::Save or to do some other stuff to the pixel data.

\retval a pointer to a newly allocated Pixbuf object, NULL if something went wrong.
*/
            gtk::Pixbuf *Pixbuf() {
                Application::Flush();
                int w, h;
                gdk_drawable_get_size(*this, &w, &h);
                if (GdkPixbuf *buf = gdk_pixbuf_get_from_drawable(NULL,
                                    *this, 
                                    gdk_colormap_get_system(),
                                    0, 0, 0, 0, w, h)) 
                    return new gtk::Pixbuf((GObject *)buf);
                else
                    return NULL;
            }

            /** Asks to place window in the fullscreen state.

Note that you shouldn't assume the window is definitely full screen afterward, because other entities (e.g. the user or window manager) could unfullscreen it again, and not all window managers honor requests to fullscreen windows. But normally the window will end up fullscreen. Just don't write code that crashes if not.

You can track the fullscreen state via the "window-state-event" signal on gtk::Widget.
*/
            void Fullscreen() { gtk_window_fullscreen(*this); }
/** Asks to toggle off the fullscreen state for window.

Note that you shouldn't assume the window is definitely not full screen afterward, because other entities (e.g. the user or window manager) could fullscreen it again, and not all window managers honor requests to unfullscreen windows. But normally the window will end up restored to its normal state. Just don't write code that crashes if not.

You can track the fullscreen state via the "window-state-event" signal on gtk::Widget.   
*/
            void Unfullscreen() { gtk_window_unfullscreen(*this); }
            /** Sets the default size of a window. 
If the window's "natural" size (its size request, as set by Widget::SizeRequest() ) is larger than the default, the default will be ignored. More generally, if the default size does not obey the geometry hints for the window (Window::GeometryHints() can be used to set these explicitly), the default size will be clamped to the nearest permitted size.

Unlike Widget::SizeRequest(), which sets a size request for a widget and thus would keep users from shrinking the window, this function only sets the initial size, just as if the user had resized the window themselves. Users can still shrink the window again as they normally would. Setting a default size of -1 means to use the "natural" default size (the size request of the window).

For more control over a window's initial size and how resizing works, investigate Window::GeometryHints().

For some uses, Window::Resize() is a more appropriate function. Window::Resize() changes the current size of the window, rather than the size to be used on initial display. It always affects the window itself, not the geometry widget.

The default size of a window only affects the first time a window is shown; if a window is hidden and re-shown, it will remember the size it had prior to hiding, rather than using the default size.

Windows can't actually be 0x0 in size, they must be at least 1x1, but passing 0 for width and height is OK, resulting in a 1x1 default size.
*/
            void DefaultSize(int width /**< width in pixels, or -1 to unset the default width */, 
                             int height /**< height in pixels, or -1 to unset the default height */ ) { gtk_window_set_default_size(*this, width, height); }
    };

    typedef std::pair<std::string, int> ButtonData;
    typedef std::vector<ButtonData> ButtonVec;

    enum DialogFlags
    {
        DialogModal = GTK_DIALOG_MODAL /**< Make the constructed dialog modal, like calling Window::Modal(true) */,
        DialogDestroyWithParent = GTK_DIALOG_DESTROY_WITH_PARENT /**< Destroy the dialog when its parent is destroyed. */,
        DialogNoSeparator = GTK_DIALOG_NO_SEPARATOR /**< Don't put a separator between the action area and the dialog content. */
    };

    inline DialogFlags operator|(DialogFlags a, DialogFlags b) { return (DialogFlags)(((int)a)|((int)b)); }

    enum ResponseType
    {
        ResponseNone = GTK_RESPONSE_NONE /**< GTK returns this if a response widget has no response_id, or if the dialog gets programmatically hidden or destroyed. */,

        /* GTK won't return these unless you pass them in
         * as the response for an action widget. They are
         * for your convenience.
         */
        ResponseReject = GTK_RESPONSE_REJECT /**< Generic response ID, not used by gtk dialogs. */,
        ResponseAccept = GTK_RESPONSE_ACCEPT /**< Generic response ID, not used by gtk dialogs. */,

        /* If the dialog is deleted. */
        ResponseDeleteEvent = GTK_RESPONSE_DELETE_EVENT /**< Returned if dialog is deleted */,

        /* These are returned from GTK dialogs, and you can also use them
         * yourself if you like.
         */
        ResponseOk = GTK_RESPONSE_OK /**< Returned by OK buttons in GTK+ dialogs */,
        ResponseCancel = GTK_RESPONSE_CANCEL /**< Returned by Cancel buttons in GTK+ dialogs */,
        ResponseClose = GTK_RESPONSE_CLOSE /**< Returned by Close buttons in GTK+ dialogs */,
        ResponseYes = GTK_RESPONSE_YES /**< Returned by Yes buttons in GTK+ dialogs */,
        ResponseNo = GTK_RESPONSE_NO /**< Returned by No buttons in GTK+ dialogs */,
        ResponseApply = GTK_RESPONSE_APPLY /**< Returned by Apply buttons in GTK+ dialogs */,
        ResponseHelp = GTK_RESPONSE_HELP
    };

/** Create popup windows

Dialog boxes are a convenient way to prompt the user for a small amount of input, e.g. to display a message, ask a question, or anything else that does not require extensive effort on the user's part.

GTK+ treats a dialog as a window split vertically. The top section is a VBox, and is where widgets such as a Label or a Entry should be packed. The bottom area is known as the action_area. This is generally used for packing buttons into the dialog which may perform functions such as cancel, ok, or apply. The two areas are separated by a HSeparator.

Dialog boxes are created with a the relevant constructors, the constructor will allows you to set the dialog title, some convenient flags, and add simple buttons.

A 'modal' dialog (that is, one which freezes the rest of the application from user input), can be created by calling Window::Modal() on the dialog. When creating a dialog you can also pass the DialogModal flag to make a dialog modal.

If you add buttons to Dialog through the constructor, using Dialog::AddButton(), or Dialog::AddButtons() clicking the button will emit a signal called "response" with a response ID that you specified. GTK+ will never assign a meaning to positive response IDs; these are entirely user-defined. But for convenience, you can use the response IDs in the ResponseType enumeration (these all have values less than zero). If a dialog receives a delete event, the "response" signal will be emitted with a response ID of ResponseDeleteEvent.

If you want to block waiting for a dialog to return before returning control flow to your code, you can call Dialog::Run(). This function enters a recursive main loop and waits for the user to respond to the dialog, returning the response ID corresponding to the button the user clicked.

For the simple dialog in the following example, in reality you'd probably use GMessageDialog to save yourself some effort. But you'd need to create the dialog contents manually if you had more than a simple message in the dialog. 

\example
// Function to open a dialog box displaying the message provided. 

void quick_message (const char *message) {
   gtk::ButtonVec b;
   b.push_back(gtk::ButtonData(GTK_STOCK_OK, ResponseOk);

   gtk::Dialog dialog("Message", b);

   dialog.Body(gtk::Label(message));

   dialog.ShowAll();

   dialog.Run();
}

\endexample
*/
    class Dialog : public Window
    {
        public:
/// DOXYS_OFF
            operator  GtkDialog *() const { return GTK_DIALOG(Obj()); }

            Dialog(GObject *obj) : Window(DerivedType()) { Init(obj); }
            Dialog(const DerivedType &d) : Window(d) {}
/// DOXYS_ON

            /** Create a new dialog.
This constructor creates a new dialog without title or any special attribute, use Window::Title(const std::string &), Window::TransientFor(), Dialog::AddButton(), Window::Modal(bool)... to configure the dialog after you created it.
*/
            Dialog() : Window(DerivedType()) {
                Init(gtk_dialog_new());
                Internal(true);
            }
            /** Create a new dialog.
This constructor creates a new dialog with a title, an array of Button and an optional parent window, you can use Window::Title(const std::string &), Window::TransientFor(), Dialog::AddButton(), Window::Modal(bool)... to change dialog behaviour.
             */
            Dialog(const std::string &title /**< Title of the window */, 
                  const ButtonVec &buttons /**< A vector of ButtonData */, 
                   Window *parent = NULL /**< Optional parent window to be transient for (see also Window::TransientFor() ) */
                   ) : Window(DerivedType()) {
                Init(gtk_dialog_new());
                AddButtons(buttons);
                Title(title);
                Internal(true);
                if (parent) 
                    Window::TransientFor(*parent);
            }
/** Add a widget inside the dialog.

This function let you set the contents of the dialog body. A dialog is composed by a body, an horizontal separator and a button row, you can add buttons with Dialog::AddButton() or Dialog::AddButtons().
*/            
            void Body(const Widget &widget) {
                gtk_container_add(GTK_CONTAINER(GTK_DIALOG(Obj())->vbox), 
                                  widget);
            }
/** Adds a button to the dialog.

Add a button with the given text (or a stock button, if button_text is a stock ID) and sets things up so that clicking the button will emit the "response" signal with the given response_id. The button is appended to the end of the dialog's action area. 
*/
            void AddButton(const std::string &label /**< Label of the button or a valid stock ID */,
                           int rc /**< A ResponseType value or a custom value to be returned in Dialog::Run() if the user click this button */ ) {
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
/** Blocks in a recursive main loop until the dialog either emits the "response" signal, or is destroyed.

If the dialog is destroyed during the call to Dialog::Run(), the function returns ResponseNone. Otherwise, it returns the response ID from the ::response signal emission.

Before entering the recursive main loop, Dialog::Run() calls Window::Show() on the dialog for you. Note that you still need to show any children of the dialog yourself.

During Dialog::Run(), the default behavior of "delete-event" is disabled; if the dialog receives ::delete_event, it will not be destroyed as windows usually are, and Dialog::Run() will return GTK_RESPONSE_DELETE_EVENT. Also, during Dialog::Run() the dialog will be modal. You can force Dialog::Run() to return at any time by calling Dialog::Response() to emit the ::response signal. Destroying the dialog during Dialog::Run() is a very bad idea, because your post-run code won't know whether the dialog was destroyed or not.

After Dialog::Run() returns, you are responsible for hiding or destroying the dialog if you wish to do so.

Typical usage of this function might be:

\example
  gint result = dialog.Run();
  switch (result) {
    case gtk::ResponseAccept:
         do_application_specific_something ();
         break;
    default:
         do_nothing_since_dialog_was_cancelled ();
         break;
  }
\endexample

Note that even though the recursive main loop gives the effect of a modal dialog (it prevents the user from interacting with other windows in the same window group while the dialog is run), callbacks such as timeouts, IO channel watches, DND drops, etc, will be triggered during a Dialog::Run() call.

\return one of the predefined ResponseType values or an user defined one associated with a button (see Dialog::AddButton() )
*/
            int Run() { return gtk_dialog_run(*this); }

/** Emits the "response" signal with the given response ID.

Used to indicate that the user has responded to the dialog in some way; typically either you or Dialog::Run() will be monitoring the ::response signal and take appropriate action.            
*/
            void Response(int response_id /**< One of the ResponseType predefined value or an integer value of your choice, it will be the return value of Dialog::Run() */ ) { gtk_dialog_response(*this, response_id); }

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
                gtk_file_filter_add_custom(*this, (GtkFileFilterFlags)flags, (GtkFileFilterFunc)filter_wrapper, cbk, NULL);
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
                gtk_file_filter_add_custom(*this, (GtkFileFilterFlags)flags, (GtkFileFilterFunc)filter_wrapper, cbk, NULL);
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

/** Sets the current name in the file selector, as if entered by the user. 
  
Note that the name passed in here is a UTF-8 string rather than a filename. This function is meant for such uses as a suggested name in a "Save As..." dialog.

If you want to preselect a particular existing file, you should use FileChooser::Filename() instead. Please see the documentation for those functions for an example of using FileChooser::CurrentName() as well.

*/  
            void CurrentName(const std::string &name /**< the filename to use, as a UTF-8 string. */) {
                 gtk_file_chooser_set_current_name(getobj(), name.c_str());
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
            FileChooser::ActionValue Action() const { 
                return  (FileChooser::ActionValue)gtk_file_chooser_get_action(getobj()); 
            }
            /// Sets the type of operation that the chooser is performing; the user interface is adapted to suit the selected action. For example, an option to create a new folder might be shown if the action is FileChooser::ActionSave but not if the action is FileChooser::ActionOpen.
            void Action(FileChooser::ActionValue action /**< the action that the file selector is performing */) { 
                gtk_file_chooser_set_action(getobj(), (GtkFileChooserAction)action); 
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
/** A file chooser dialog, suitable for "File/Open" or "File/Save" commands.

FileChooserDialog is a dialog box suitable for use with "File/Open" or "File/Save as" commands. This widget works by putting a FileChooserWidget inside a Dialog. It exposes the FileChooser interface, so you can use all of the FileChooser functions on the file chooser dialog as well as those for Dialog.

Note that FileChooserDialog does not have any methods of its own. Instead, you should use the functions that work on a FileChooser.

In the simplest of cases, you can the following code to use GtkFileChooserDialog to select a file for opening.

\example

std::string open() {
    gtk::FileChooserDialog dlg("Open a file...");
    dlg.AddButton(GTK_STOCK_OK, ResponseOk);
    dlg.AddButton(GTK_STOCK_CANCEL, ResponseCancel);

    if (dlg.Run() == gtk::ResponseOk)
        return dlg.Filename();
    
    return std::string();
}
\endexample
*/
    class FileChooserDialog : public Dialog, public FileChooser {
        public:
/// DOXYS_OFF
            operator  GtkFileChooserDialog *() const { return GTK_FILE_CHOOSER_DIALOG(Obj()); }

            FileChooserDialog(GObject *obj) : Dialog(DerivedType()) { Init(obj); }
/// DOXYS_ON

            /** Create a new FileChooserDialog. */
            FileChooserDialog(const std::string &title = "" /**< Title of the window, it can be empty. */,
                              Window *parent_window = NULL /**< Transient parent of the dialog, or NULL. */,
                              FileChooser::ActionValue action =  FileChooser::ActionOpen /**< Open or save mode for the dialog */,
                              const ButtonVec &buttons = ButtonVec() /**< Vector of buttons to be added to the dialog, you can also use Dialog::AddButton() after the object is created to do this (look at the example) */ ) : 
                Dialog(DerivedType()) {
                Init(gtk_file_chooser_dialog_new(title.c_str(), NULL, (GtkFileChooserAction)action, 
                                          NULL, NULL));
                AddButtons(buttons);
                Internal(true);
            } 
            virtual GtkFileChooser *getobj() const { return GTK_FILE_CHOOSER(Obj()); }
    };

/** The FileChooserButton is a widget that lets the user select a file. 
  
It implements the FileChooser interface. Visually, it is a file name with a button to bring up a FileChooserDialog. The user can then use that dialog to change the file associated with that button. This widget does not support setting the "select-multiple" property to TRUE.

The FileChooserButton supports the FileChooser Actions FileChooser::ActionOpen and FileChooser::ActionSelectFolder.

Important

The FileChooserButton will ellipsize the label, and thus will thus request little horizontal space. To give the button more space, you should callWidget::SizeRequest(), FileChooserButton::WidthChars(int), or pack the button in such a way that other interface elements give space to the widget.

*/
    class FileChooserButton : public HBox, public FileChooser {
        public:
/// DOXYS_OFF
            operator  GtkFileChooserButton *() const { return GTK_FILE_CHOOSER_BUTTON(Obj()); }

            FileChooserButton(GObject *obj) : HBox(DerivedType()) { Init(obj); }
/// DOXYS_ON

            /** Create a new FileChooserDialog. */
            FileChooserButton(const std::string &title = "" /**< Title of the popup FileChooserDialog window, it can be empty. */,
                              FileChooser::ActionValue action =  FileChooser::ActionOpen /**< Open or select folder mode for the button */) : 
                HBox(DerivedType()) {
                Init(gtk_file_chooser_button_new(title.c_str(), (GtkFileChooserAction)action));
                Internal(true);
            } 
            virtual GtkFileChooser *getobj() const { return GTK_FILE_CHOOSER(Obj()); }
            /// Modifies the title of the browse dialog.
            void Title(const std::string &title /**< the new browse dialog title. */) { gtk_file_chooser_button_set_title(*this, title.c_str()); }
            /// Retrieves the title of the browse dialog.
            std::string Title() const { return gtk_file_chooser_button_get_title(*this); }
            /// Retrieves the width in characters of the button widget's entry
            int WidthChars() const { return gtk_file_chooser_button_get_width_chars(*this); }
            /// Sets the width (in characters) that button will use to n_chars.
            void WidthChars(int n_chars /**< the new width, in characters. */) { gtk_file_chooser_button_set_width_chars(*this, n_chars); }
            /// Fire a callback when the user changes the file in the button
            BUILD_EVENT(OnFileSet, "file-set");
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

            /**
A message dialog is a simple dialog with an icon indicating the dialog type (error, warning, etc.) and some text which is marked up with the Pango text markup language. When the user clicks a button a "response" signal is emitted with response IDs from gtk::ResponseType. See gtk::Dialog for more details. 
             */
    class MessageDialog : public Dialog
    {
        public:
/// DOXYS_OFF
            operator  GtkMessageDialog *() const { return GTK_MESSAGE_DIALOG(Obj()); }

            MessageDialog(GObject *obj) : Dialog(DerivedType()) { Init(obj); }
/// DOXYS_ON
/// Creates a new message dialog with print-f style message text
            MessageDialog(Window *parent, DialogFlags flags,
                          MessageType msgtype,
                          ButtonsType buttontype,
                          const char *msg_format, ...) : Dialog(DerivedType()) {
                char *msg;
                va_list va;

                va_start(va, msg_format);
                g_vasprintf(&msg, msg_format, va);
                va_end(va);

                Init(gtk_message_dialog_new_with_markup(parent ? GTK_WINDOW(parent->Obj()) : NULL, (GtkDialogFlags)flags, (GtkMessageType)msgtype, 
                            (GtkButtonsType)buttontype, "."));
                gtk_message_dialog_set_markup(*this, msg);
                g_free(msg);
                Internal(true);
            }
/// Creates a new message dialog
            MessageDialog(va_list va, Window *parent, DialogFlags flags,
                          MessageType msgtype,
                          ButtonsType buttontype,
                          const char *msg_format) : Dialog(DerivedType()) {
                char *msg;
                g_vasprintf(&msg, msg_format, va);

                Init(gtk_message_dialog_new_with_markup(parent ? GTK_WINDOW(parent->Obj()) : NULL,
                            (GtkDialogFlags)flags, (GtkMessageType)msgtype, (GtkButtonsType)buttontype,
                            "."));
                gtk_message_dialog_set_markup(*this, msg);
                g_free(msg);
                Internal(true);
            }
/// Creates a new message dialog with as few arguments as possible
            MessageDialog(const std::string &msg,
                          MessageType msgtype = MessageInfo,
                          ButtonsType buttontype = ButtonsOk,
                          DialogFlags flags = DialogDestroyWithParent,
                          Window *parent = NULL) : Dialog(DerivedType()) {
                Init(gtk_message_dialog_new_with_markup(parent ? GTK_WINDOW(parent->Obj()) : NULL, 
                            (GtkDialogFlags)flags, (GtkMessageType)msgtype, (GtkButtonsType)buttontype, "."));
                gtk_message_dialog_set_markup(*this, msg.c_str());
                Internal(true);
            }
            /// Sets the text of the dialog area (with printf style and optional pango markup)
            void TextF(const char *format/**< print-f style format string */, ...) {
                va_list va; char *msg;
                va_start(va, format);
                g_vasprintf(&msg, format, va);
                va_end(va);
               
                gtk_message_dialog_set_markup(*this, msg);
                g_free(msg);
            }
            // Sets the text of the dialog area.
            void Text(const std::string &text /**< markup string (see Pango markup format) */) {
                gtk_message_dialog_set_markup(*this, text.c_str());
            }
            /**
             Sets the secondary text of the message dialog to be message_format (with printf()-style).

\note Setting a secondary text makes the primary text become bold, unless you have provided explicit markup.
*/
            void Secondary(const std::string &text/**< print-f style format string */) {
                gtk_message_dialog_format_secondary_markup(*this, "%s", text.c_str());
            }
            /// Sets the dialog image to a gtk::Image 
            void Image(gtk::Image &image) {
                 gtk_message_dialog_set_image(*this, image);
            }
    };
}

#endif
