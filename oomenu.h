#ifndef OOMENU_H
#define OOMENU_H

/*
\unit
!Menu handling
This file contains all the oogtk classes related to menu handling.
*/

namespace gtk {
/** A base class for menu objects.
A MenuShell is the abstract base class used to derive the Menu and MenuBar subclasses.

A MenuShell is a container of MenuItem objects arranged in a list which can be navigated, selected, and activated by the user to perform application functions. A MenuItem can have a submenu associated with it, allowing for nested hierarchical menus. 
 */
    class MenuShell : public Container
    {
        public:
/// DOXYS_OFF
            operator GtkMenuShell *() const { return GTK_MENU_SHELL(Obj()); }
/// DOXYS_ON
            /** Adds a new MenuItem to the end of the menu shell's item list. */
            void Append(const Widget &child /**< The MenuItem to add. */) { 
                gtk_menu_shell_append(*this, child); }
            /** Adds a new MenuItem to the beginning of the menu shell's item list. */
            void Prepend(const Widget &child /**< The MenuItem to add. */) { 
                gtk_menu_shell_prepend(*this, child); }
            /** Adds a new MenuItem to the menu shell's item list at the position indicated by position. */
            void Insert(const Widget &child /**< The MenuItem to add. */, 
                        int pos /**< The position in the item list where child is added. Positions are numbered from 0 to n-1. */
                    ) { gtk_menu_shell_insert(*this, child, pos); }
            /** Deactivates the menu shell. Typically this results in the menu shell being erased from the screen. */
            void Deactivate() { gtk_menu_shell_deactivate(*this); }
            /** Selects the menu item from the menu shell. */
            void Select(const Widget &menu_item /**< The MenuItem to select. */ ) { gtk_menu_shell_select_item(*this, menu_item); }
            /** Select the first visible or selectable child of the menu shell; don't select tearoff items unless the only item is a tearoff item. */
            void SelectFirst(bool search_sensitive = true /**< if true, search for the first selectable menu item, otherwise select nothing if the first item isn't sensitive. This should be false if the menu is being popped up initially, defaults to true. */ ) { 
                gtk_menu_shell_select_first(*this, search_sensitive); 
            }
            /** Activates the menu item within the menu shell. */
            void Activate(const Widget &menu_item /**< 	The MenuItem to activate. */, 
                          bool force_deactivate = false /**< If true, force the deactivation of the menu shell after the menu item is activated, defaults to false.*/) { 
                gtk_menu_shell_activate_item(*this, menu_item, force_deactivate); 
            }
            /** Deselects the currently selected item from the menu shell, if any. */
            void Deselect() { gtk_menu_shell_deselect(*this); }
            /** Cancels the selection within the menu shell. */
            void Cancel() { gtk_menu_shell_cancel(*this); }
            /** If TakeFocus is true (the default) the menu shell will take the keyboard focus so that it will receive all keyboard events which is needed to enable keyboard navigation in menus.

Setting TakeFocus to false is useful only for special applications like virtual keyboard implementations which should not take keyboard focus.

The TakeFocus state of a menu or menu bar is automatically propagated to submenus whenever a submenu is popped up, so you don't have to worry about recursively setting it for your entire menu hierarchy. Only when programmatically picking a submenu and popping it up manually, the take_focus property of the submenu needs to be set explicitely.

Note that setting it to false has side-effects:

If the focus is in some other app, it keeps the focus and keynav in the menu doesn't work. Consequently, keynav on the menu will only work if the focus is on some toplevel owned by the onscreen keyboard.

To avoid confusing the user, menus with TakeFocus set to false should not display mnemonics or accelerators, since it cannot be guaranteed that they will work. 
*/
            void TakeFocus(bool flag /**< true if the menu shell should take the keyboard focus on popup. */ ) {
                gtk_menu_shell_set_take_focus(*this, flag); }
            /** Returns true if the menu shell will take the keyboard focus on popup.
            \return true if the menu shell will take the keyboard focus on popup. 
            \sa MenuShell::TakeFocus(bool)
            */
            bool TakeFocus() const { return gtk_menu_shell_get_take_focus(*this); }
    };
/** A subclass widget for MenuShell which holds MenuItem widgets
The MenuBar is a subclass of MenuShell which contains one to many MenuItem. The result is a standard menu bar which can hold many menu items. MenuBar allows for a shadow type to be set for aesthetic purposes.
 */
    class MenuBar : public MenuShell
    {
        public:
/// DOXYS_OFF
            operator GtkMenuBar *() const { return GTK_MENU_BAR(Obj()); }
            MenuBar(GObject *obj) { Init(obj); }
/// DOXYS_ON
/// Create a new MenuBar widget.
            MenuBar() {
                Init(gtk_menu_bar_new());
                Internal(true);
            }
    };

    /** A menu widget.
A Menu is a MenuShell that implements a drop down menu consisting of a list of MenuItem objects which can be navigated and activated by the user to perform application functions.

A Menu is most commonly dropped down by activating a MenuItem in a MenuBar or popped up by activating a GtkMenuItem in another Menu.

A Menu can also be popped up by activating a OptionMenu. Other composite widgets such as the Notebook can pop up a Menu as well.

Applications can display a Menu as a popup menu by calling the Menu::Popup() function.
     */
    class Menu : public MenuShell
    {
        public:
/// DOXYS_OFF
            operator GtkMenu *() const { return GTK_MENU(Obj()); }
            Menu(GObject *obj) { Init(obj); }
/// DOXYS_ON
            /** Create a new EMPTY menu widget. 
Use MenuShell::Append(), MenuShell::Prepend(), MenuShell::Insert() to populate it.
            */
            Menu() {
                Init(gtk_menu_new());
                Internal(true);
            }
            /** Create a new menu widget with one child widget */
            Menu(const Widget &w1) {
                Init(gtk_menu_new());
                Internal(true);
                Append(w1);
            }
            /** Create a new menu widget with two child widgets */
            Menu(const Widget &w1, const Widget &w2) {
                Init(gtk_menu_new());
                Internal(true);
                Append(w1);
                Append(w2);
            }
            /** Create a new menu widget with three child widgets */
            Menu(const Widget &w1, const Widget &w2, const Widget &w3) {
                Init(gtk_menu_new());
                Internal(true);
                Append(w1);
                Append(w2);
                Append(w3);
            }
            /** Create a new menu widget with four child widgets */
            Menu(const Widget &w1, const Widget &w2, const Widget &w3, const Widget &w4) {
                Init(gtk_menu_new());
                Internal(true);
                Append(w1);
                Append(w2);
                Append(w3);
                Append(w4);
            }
            /** Create a new menu widget with five child widgets */
            Menu(const Widget &w1, const Widget &w2, const Widget &w3, const Widget &w4, const Widget &w5) {
                Init(gtk_menu_new());
                Internal(true);
                Append(w1);
                Append(w2);
                Append(w3);
                Append(w4);
                Append(w5);
            }
            /** Create a new menu widget with six child widgets */
            Menu(const Widget &w1, const Widget &w2, const Widget &w3, const Widget &w4, const Widget &w5, const Widget &w6) {
                Init(gtk_menu_new());
                Internal(true);
                Append(w1);
                Append(w2);
                Append(w3);
                Append(w4);
                Append(w5);
                Append(w6);
            }
            /** Create a new menu widget with seven child widgets */
            Menu(const Widget &w1, const Widget &w2, const Widget &w3, const Widget &w4, const Widget &w5, const Widget &w6, const Widget &w7) {
                Init(gtk_menu_new());
                Internal(true);
                Append(w1);
                Append(w2);
                Append(w3);
                Append(w4);
                Append(w5);
                Append(w6);
                Append(w7);
            }

            /** Sets the title string for the menu. The title is displayed when the menu is shown as a tearoff menu. If title is NULL, the menu will see if it is attached to a parent menu item, and if so it will try to use the same text as that menu item's label.
             */
            void Title(const std::string &title /**< a string containing the title for the menu.*/) { gtk_menu_set_title(*this, title.c_str()); }
            /** Returns the title of the menu.
             \return the title of the menu, or an empty string if the menu has no title set on it.
             \sa Menu::Title(const std::string)
            */
            std::string Title() const { 
                std::string res;
                if (const gchar *c = gtk_menu_get_title(*this)) 
                    res = c;
                return res;
            }

            /** Changes the tearoff state of the menu. A menu is normally displayed as drop down menu which persists as long as the menu is active. It can also be displayed as a tearoff menu which persists until it is closed or reattached. 
            */
            void TearoffState(bool state /**< 	If true, menu is displayed as a tearoff menu.*/) { 
                gtk_menu_set_tearoff_state(*this, state); 
            }
            /** Returns whether the menu is torn off. 
             \return true if the menu is currently torn off.
             \sa Menu::TearoffState(bool) 
             */
            bool TearoffState() const { return gtk_menu_get_tearoff_state(*this); }

            /**
            Displays a menu and makes it available for selection.
Applications can use this function to display context-sensitive menus. The default menu positioning function will position the menu at the current mouse cursor position. 

The button parameter should be the mouse button pressed to initiate the menu popup. If the menu popup was initiated by something other than a mouse button press, such as a mouse button release or a keypress, button should be 0.

The activate_time parameter is used to conflict-resolve initiation of concurrent requests for mouse/keyboard grab requests. To function properly, this needs to be the time stamp of the user event (such as a mouse click or key press) that caused the initiation of the popup. Only if no such event is available, gtk_get_current_event_time() is used instead.

If unsure about the values of the parameters simply call the function without parameters and accept the default ones.
*/
         void Popup(int button = 0 /**< 	 the mouse button which was pressed to initiate the event, defaults to 0.*/, 
                   guint32 activate_time = gtk_get_current_event_time() /**< the time at which the activation event occurred. */
               ) {
                gtk_menu_popup(*this, NULL, NULL, NULL, NULL, button, activate_time);
            }
         /** Removes the menu from the screen. */
         void Popdown() { gtk_menu_popdown(*this); }
    };

    // TODO AccelPath stuff
    /** The widget used for item in menus
The MenuItem widget and the derived widgets are the only valid childs for menus. Their function is to correctly handle highlighting, alignment, events and submenus.

As it derives from Bin it can hold any valid child widget, altough only a few are really useful. 
    */
    class MenuItem : public Item
    {
        public:
/// DOXYS_OFF
            operator GtkMenuItem *() const { return GTK_MENU_ITEM(Obj()); }
            MenuItem(const DerivedType &) {}
            MenuItem(GObject *obj) { Init(obj); }
/// DOXYS_ON
/** Create a new MenuItem, if you supply a label string it will be created a child
Label widget and placed inside the MenuItem, otherwise you'll have to add the child Widget yourself with Bin::Child(const Widget &).
*/
            MenuItem(const std::string &label = "") {
                if (label.empty())
                    Init(gtk_menu_item_new());
                else
                    Init(gtk_menu_item_new_with_label(label.c_str()));

                Internal(true);
            }
            /** Sets whether the menu item appears justified at the right side of a menu bar.
This was traditionally done for "Help" menu items, but is now considered a bad idea. (If the widget layout is reversed for a right-to-left language like Hebrew or Arabic, right-justified-menu-items appear at the left.) 
            */
            void RightJustified(bool flag /**< if true the menu item will appear at the far right if added to a menu bar. */) { 
                gtk_menu_item_set_right_justified(*this, flag); 
            }
            /** Gets whether the menu item appears justified at the right side of the menu bar. */
            bool RightJustified() { return gtk_menu_item_get_right_justified(*this); }
            /** Removes the widget's submenu. */
            void RemoveSubmenu() { gtk_menu_item_set_submenu(*this, NULL); }
            /** Emits the "activate" signal on the given item */
            void Activate() { gtk_menu_item_activate(*this); }
            /** Sets or replaces the menu item's submenu. */
            void Submenu(const Widget &submenu /**< the new submenu */) { gtk_menu_item_set_submenu(*this, submenu); }
            /** Gets the submenu underneath this menu item, if any. See MenuItem::Submenu*const Widget &).
            \return submenu for this menu item, or NULL if none.
            */
            Widget *Submenu() const {
                return dynamic_cast<Widget *>(
                        Object::Find((GObject *)gtk_menu_item_get_submenu(*this)));
            }

            /** Attach a callback to the MenuItem activate signal */
            BUILD_VOID_EVENT(OnActivate, "activate");
    };

    /** A separator used in menus.
The SeparatorMenuItem is a separator used to group items within a menu. It displays a horizontal line with a shadow to make it appear sunken into the interface.
     */
    class SeparatorMenuItem : public MenuItem // COMPLETE API
    {
        public:
/// DOXYS_OFF
            operator GtkSeparatorMenuItem *() const { return GTK_SEPARATOR_MENU_ITEM(Obj()); }
            SeparatorMenuItem(GObject *obj) : MenuItem(DerivedType()) { Init(obj); }
/// DOXYS_ON
            /** Create a new SeparatorMenuItem */
            SeparatorMenuItem() : MenuItem(DerivedType()) {
                Init(gtk_separator_menu_item_new());
                Internal(true);
            }
    };

    /** A menu item with an icon.
A ImageMenuItem is a menu item which has an icon next to the text label.

Note that the user can disable display of menu icons, so make sure to still fill in the text label.
     */
    class ImageMenuItem : public MenuItem
    {
        public:
/// DOXYS_OFF            
            operator GtkImageMenuItem *() const { return GTK_IMAGE_MENU_ITEM(Obj()); }
            ImageMenuItem(GObject *obj) : MenuItem(DerivedType()) { Init(obj); }
/// DOXYS_ON
/** Create a new ImageMenuItem, stock_id may be the ID of a stock item (for instance GTK_STOCK_OK or GTK_STOCK_APPLY), or a label for the item, you may also construct the object without parameters and add a label and an image yourself.
 */
            ImageMenuItem(const std::string &stock_id = "") : MenuItem(DerivedType()) {
                GtkStockItem item;
                if (!stock_id.empty()) {
                    if (gtk_stock_lookup(stock_id.c_str(), &item))
                        Init(gtk_image_menu_item_new_from_stock(stock_id.c_str(), NULL));
                    else
                        Init(gtk_image_menu_item_new_with_label(stock_id.c_str()));
                }
                else
                    Init(gtk_image_menu_item_new());
                Internal(true);
            }
            /** Sets the image of image_menu_item to the given widget. Note that it depends on the show-menu-images setting whether the image will be displayed or not.
             */
            void Set(Widget &image /**< a widget to set as the image for the menu item. */) { 
                gtk_image_menu_item_set_image(*this, image); 
            }
            /** Gets the widget that is currently set as the image of image_menu_item. 
            \return the widget set as image of image_menu_item, NULL if none.
            */
            Widget *Get() { 
                return dynamic_cast<Widget *>(
                        Object::Find((GObject *)gtk_image_menu_item_get_image(*this)));
            }
    };

    /** A menu item with a check box
A CheckMenuItem is a menu item that maintains the state of a boolean value in addition to a MenuItem's usual role in activating application code.

A check box indicating the state of the boolean value is displayed at the left side of the MenuItem. Activating the MenuItem toggles the value. 
     */
    class CheckMenuItem : public MenuItem
    {
        public:
/// DOXYS_OFF            
            operator GtkCheckMenuItem *() const { return GTK_CHECK_MENU_ITEM(Obj()); }
            CheckMenuItem(GObject *obj) : MenuItem(DerivedType()) { Init(obj); }
/// DOXYS_ON
            /** Create a new CheckMenuItem with an optional label. */
            CheckMenuItem(const std::string &label = "" /**< the string to use for the label, if you don't specify a label you'll have to fill the Bin widget yourself. */ ) {
                if (label.empty())
                    Init(gtk_check_menu_item_new());
                else
                    Init(gtk_check_menu_item_new_with_label(label.c_str()));
                Internal(true);
            }
            /** Sets the active state of the menu item's check box. */
            void Active(bool flag /**< 	boolean value indicating whether the check box is active. */) { 
                gtk_check_menu_item_set_active(*this, flag); 
            }
            /** Returns whether the check menu item is active. 
            \return true if the menu item is checked.
            */
            bool Active() const { return gtk_check_menu_item_get_active(*this); }
            /** Sets the widget in an inconsistent state.
If the user has selected a range of elements (such as some text or spreadsheet cells) that are affected by a boolean setting, and the current values in that range are inconsistent, you may want to display the check in an "in between" state. This function turns on "in between" display. Normally you would turn off the inconsistent state again if the user explicitly selects a setting. This has to be done manually,CheckMenuItem::Inconsistent(bool) only affects visual appearance, it doesn't affect the semantics of the widget.
*/
            void Inconsistent(bool flag /**< true to display an "inconsistent" third state check*/) { 
                gtk_check_menu_item_set_inconsistent(*this, flag); 
            }
            /** Retrieves the value set by CheckMenuItem::Inconsistent(bool). 
            \return true if inconsistent.
            */
            bool Inconsistent() const { return gtk_check_menu_item_get_inconsistent(*this); }
            /** Sets whether check_menu_item is drawn like a RadioMenuItem */
            void DrawAsRadio(bool flag /**< whether check_menu_item is drawn like a GtkRadioMenuItem */) 
            { gtk_check_menu_item_set_draw_as_radio(*this, flag); 
            }
            /** Returns whether check_menu_item looks like a RadioMenuItem
            \return true if check_menu_item looks like a RadioMenuItem
            */
            bool DrawAsRadio() const { return gtk_check_menu_item_get_draw_as_radio(*this); }
            /** Emits the "toggled" signal. */
            void Toggled() { gtk_check_menu_item_toggled(*this); }
            /** Controls whether the check box is shown at all times. Normally the check box is shown only when it is active or while the menu item is selected. 
            */
            void ShowToggle(bool flag /**< boolean value indicating whether to always show the check box. */) { 
                gtk_check_menu_item_set_show_toggle(*this, flag); 
            }
    };
}
#endif
