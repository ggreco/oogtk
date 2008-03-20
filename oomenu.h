#ifndef OOMENU_H
#define OOMENU_H

namespace gtk {

    class MenuShell : public Container
    {
        public:
            operator GtkMenuShell *() const { return GTK_MENU_SHELL(Obj()); }

            void Append(const Widget &child) { gtk_menu_shell_append(*this, child); }
            void Prepend(const Widget &child) { gtk_menu_shell_prepend(*this, child); }
            void Insert(const Widget &child, int pos) { gtk_menu_shell_insert(*this, child, pos); }
            void Deactivate() { gtk_menu_shell_deactivate(*this); }
            void Select(const Widget &menu_item) { gtk_menu_shell_select_item(*this, menu_item); }
            void SelectFirst(bool search_sensitive = true) { 
                gtk_menu_shell_select_first(*this, search_sensitive); 
            }
            void Activate(const Widget &menu_item, bool force_deactivate = false) { 
                gtk_menu_shell_activate_item(*this, menu_item, force_deactivate); 
            }
            void Deselect() { gtk_menu_shell_deselect(*this); }
            void Cancel() { gtk_menu_shell_cancel(*this); }
            void TakeFocus(bool flag) { gtk_menu_shell_set_take_focus(*this, flag); }
            bool TakeFocus() const { return gtk_menu_shell_get_take_focus(*this); }
    };

    class MenuBar : public MenuShell
    {
        public:
            operator GtkMenuBar *() const { return GTK_MENU_BAR(Obj()); }
            MenuBar(GObject *obj) { Init(obj); }
            MenuBar() {
                Init(gtk_menu_bar_new());
                Internal(true);
            }
    };

    class Menu : public MenuShell
    {
        public:
            operator GtkMenu *() const { return GTK_MENU(Obj()); }
            Menu(GObject *obj) { Init(obj); }
            Menu() {
                Init(gtk_menu_new());
                Internal(true);
            }
    };

    // TODO AccelPath stuff
    class MenuItem : public Item
    {
        public:
            operator GtkMenuItem *() const { return GTK_MENU_ITEM(Obj()); }
            MenuItem(const DerivedType &) {}
            MenuItem(GObject *obj) { Init(obj); }
            MenuItem(const std::string &label) {
                if (label.empty())
                    Init(gtk_menu_item_new());
                else
                    Init(gtk_menu_item_new_with_label(label.c_str()));

                Internal(true);
            }
            void RightJustified(bool flag) { gtk_menu_item_set_right_justified(*this, flag); }
            bool RightJustified() { return gtk_menu_item_get_right_justified(*this); }
            void RemoveSubmenu() { gtk_menu_item_remove_submenu(*this); }
            void Activate() { gtk_menu_item_activate(*this); }
            void Submenu(const Widget &submenu) { gtk_menu_item_set_submenu(*this, submenu); }
            Widget *Submenu() const {
                return dynamic_cast<Widget *>(
                        Object::Find((GObject *)gtk_menu_item_get_submenu(*this)));
            }

            // callbacks
            template <typename T>
            void OnActivate(void (T::*cbk)(), T *base ) {
                callback("activate", cbk, base);
            }
            template <typename T, typename J>
            void OnActivate(void (T::*cbk)(J), T *base, J data ) {
                callback("activate", cbk, base, data);
            }
            template <typename T>
            void OnActivate(void (T::*cbk)(Widget &), T *base ) {
                callback("activate", cbk, base);
            }
            template <typename T, typename J>
            void OnActivate(void (T::*cbk)(Widget &, J), T *base, J data ) {
                callback("activate", cbk, base, data);
            }

    };

    class SeparatorMenuItem : public MenuItem // COMPLETE API
    {
        public:
            operator GtkSeparatorMenuItem *() const { return GTK_SEPARATOR_MENU_ITEM(Obj()); }
            SeparatorMenuItem(GObject *obj) : MenuItem(DerivedType()) { Init(obj); }
            SeparatorMenuItem() : MenuItem(DerivedType()) {
                Init(gtk_separator_menu_item_new());
                Internal(true);
            }
    };

    class ImageMenuItem : public MenuItem
    {
        public:
            operator GtkImageMenuItem *() const { return GTK_IMAGE_MENU_ITEM(Obj()); }
            ImageMenuItem(GObject *obj) : MenuItem(DerivedType()) { Init(obj); }
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
            void Set(Widget &image) { gtk_image_menu_item_set_image(*this, image); }
            Widget *Get(Widget &image) { 
                return dynamic_cast<Widget *>(
                        Object::Find((GObject *)gtk_image_menu_item_get_image(*this)));
            }
    };

    class CheckMenuItem : public MenuItem
    {
        public:
            operator GtkCheckMenuItem *() const { return GTK_CHECK_MENU_ITEM(Obj()); }
            CheckMenuItem(GObject *obj) : MenuItem(DerivedType()) { Init(obj); }

            void Active(bool flag) { gtk_check_menu_item_set_active(*this, flag); }
            bool Active() const { return gtk_check_menu_item_get_active(*this); }
            void Inconsistent(bool flag) { gtk_check_menu_item_set_inconsistent(*this, flag); }
            bool Inconsistent() const { return gtk_check_menu_item_get_inconsistent(*this); }
            void DrawAsRadio(bool flag) { gtk_check_menu_item_set_draw_as_radio(*this, flag); }
            bool DrawAsRadio() const { return gtk_check_menu_item_get_draw_as_radio(*this); }

            void Toggled() { gtk_check_menu_item_toggled(*this); }
            void ShowToggle(bool flag) { gtk_check_menu_item_set_show_toggle(*this, flag); }
    };
}
#endif
