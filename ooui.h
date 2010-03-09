#ifndef OOUI_H

#define OOUI_H

namespace gtk {
    class ActionGroup;

    class Action : public Object {
            friend class ActionGroup;
        private:
            AbstractCbk *cbk_;
        public:
            operator  GtkAction *() const { return GTK_ACTION(obj_); }
            Action(GObject *obj) : cbk_(NULL) { Init(obj); }
            Action(const std::string &name,
                   const std::string &label,
                   const std::string &tooltip = "") : cbk_(NULL) {
                const char *l = NULL, *s = NULL;
                GtkStockItem item;

                if (gtk_stock_lookup(label.c_str(), &item))
                    s = label.c_str();
                else
                    l = label.c_str();

                Init(gtk_action_new(name.c_str(), l, s, tooltip.empty() ? NULL : tooltip.c_str()));
                Internal(true);
            }

            virtual ~Action() {
                if (cbk_ && Obj()->ref_count < 2) {
                    delete cbk_;
                    cbk_ = NULL;
                }
            }
            void Activate() { gtk_action_activate(*this); }

            // get/set
            bool Visible() const { return gtk_action_get_visible(*this); }
            bool IsVisible() const { return gtk_action_is_visible(*this); }
            void Visible(bool flag) { gtk_action_set_visible(*this, flag); }
            bool Sensitive() const { return gtk_action_get_sensitive(*this); }
            bool IsSensitive() const { return gtk_action_is_sensitive(*this); }
            void Sensitive(bool flag) { gtk_action_set_sensitive(*this, flag); }
            std::string Name() const { return gtk_action_get_name(*this); }
            std::string AccelPath() const { return gtk_action_get_accel_path(*this); }
            void AccelPath(const std::string &path) { gtk_action_set_accel_path(*this, path.c_str()); }
            void AccelGroup(const gtk::AccelGroup &group) { gtk_action_set_accel_group(*this, group); }

            // callback handling
            template <typename A>
            void Callback(void (A::*callback)(), A *base) {
                if (cbk_) delete cbk_;
                cbk_ = new CbkEvent<A,void>(base, callback);
            }
            template <typename A, typename B>
            void Callback(void (A::*callback)(B), A *base, B data) {
                if (cbk_) delete cbk_;
                cbk_ = new CbkEvent<A,void,B>(base, callback, data);
            }
    };

    struct ActionEntry {
        std::string name;
        std::string stock_id;
        std::string label;
        std::string tooltip;
        std::string accel;
        AbstractCbk *cbk;

        void Tooltip(const std::string &tip) { tooltip = tip; }
        void Accel(const std::string &acc) { accel = acc; }
        
        ActionEntry(const std::string &nam, const std::string &lab,
                    const std::string &ttip = "", const char *acc = NULL) : name(nam), tooltip(ttip), cbk(NULL) {
            GtkStockItem item;

            if (gtk_stock_lookup(lab.c_str(), &item))
                stock_id = lab;
            else
                label = lab;

            if (acc) accel = acc;
        }

        void Label(const std::string &l) { label = l; }
        void Icon(const std::string &icon) { stock_id = icon; }

        template <typename A, typename B>
        ActionEntry(const std::string &nam, const std::string &lab,
                    void (A::*callback)(B), A *base, B data,
                    const std::string &ttip = "", const char *acc = NULL) : name(nam), tooltip(ttip) {
            cbk = new CbkEvent<A,void,B>(base, callback, data);
            GtkStockItem item;

            if (gtk_stock_lookup(lab.c_str(), &item))
                stock_id = lab;
            else
                label = lab;

            if (acc) accel = acc;
        }
        template <typename A>
        ActionEntry(const std::string &nam, const std::string &lab,
                    void (A::*callback)(), A *base, 
                    const std::string &ttip = "", const char *acc = NULL) : name(nam), tooltip(ttip) {
            cbk = new CbkEvent<A,void>(base, callback);
            GtkStockItem item;

            if (gtk_stock_lookup(lab.c_str(), &item))
                stock_id = lab;
            else
                label = lab;

            if (acc) accel = acc;
        }
    };

    typedef std::vector<ActionEntry> ActionList;
    typedef std::vector<ActionEntry>::const_iterator ActionListIt;

    class ActionGroup : public Object {
            static void action_cbk(GtkAction *w, ActionGroup *group) {
                if (gtk::Action *a = dynamic_cast<gtk::Action *>(Object::Find((GObject *)w))) 
                    group->Dispatch(*a);
            }
        public:
            operator  GtkActionGroup *() const { return GTK_ACTION_GROUP(obj_); }
            ActionGroup(GObject *obj) { Init(obj); }
            ActionGroup(const std::string &name) {
                Init(gtk_action_group_new(name.c_str()));
                Internal(true);
            }

            void Dispatch(const gtk::Action &action) {
                if (action.cbk_)
                    action.cbk_->notify();
            }
            void Add(const ActionList &actions) {
                GtkActionEntry entries[actions.size()];
                int i = 0;
                memset(entries, 0, sizeof(GtkActionEntry) * actions.size());

                for (ActionListIt it = actions.begin(); it != actions.end(); ++it, ++i) {
                    entries[i].name = it->name.c_str();
                    if (!it->label.empty())
                        entries[i].label = it->label.c_str();
                    if (!it->stock_id.empty())
                        entries[i].stock_id = it->stock_id.c_str();
                    if (!it->tooltip.empty())
                        entries[i].tooltip = it->tooltip.c_str();
                    if (!it->accel.empty())
                        entries[i].accelerator = it->accel.c_str();
                    entries[i].callback = GCallback(ActionGroup::action_cbk);
                }

                gtk_action_group_add_actions(*this, entries, actions.size(), this);

                for (ActionListIt it = actions.begin(); it != actions.end(); ++it) {
                    if (gtk::Action *a = Action(it->name)) 
                        a->cbk_ = const_cast<AbstractCbk *>(it->cbk);
                }
            }

            void Add(const gtk::Action &action, const char *accel = NULL) {
                if (accel)
                    gtk_action_group_add_action_with_accel(*this, action, accel);
                else
                    gtk_action_group_add_action(*this, action);
            }
            void Remove(const gtk::Action &action) {
                gtk_action_group_remove_action(*this, action);
            }

            gtk::Action *Action(const std::string &actionname) {
                return dynamic_cast<gtk::Action *>(Object::Find((GObject *)
                            gtk_action_group_get_action(*this, actionname.c_str())));
            }
            // get/set attributes
            bool Visible() const { return gtk_action_group_get_visible(*this); }
            void Visible(bool flag) { gtk_action_group_set_visible(*this, flag); }
            bool Sensitive() const { return gtk_action_group_get_sensitive(*this); }
            void Sensitive(bool flag) { gtk_action_group_set_sensitive(*this, flag); }
            std::string Name() const { return gtk_action_group_get_name(*this); }
    };

    enum UIManagerItemType {
        UIManagerAuto = GTK_UI_MANAGER_AUTO,
        UIManagerMenuBar = GTK_UI_MANAGER_MENUBAR,
        UIManagerMenu = GTK_UI_MANAGER_MENU,
        UIManagerToolbar = GTK_UI_MANAGER_TOOLBAR,
        UIManagerPlaceholder = GTK_UI_MANAGER_PLACEHOLDER,
        UIManagerPopup =GTK_UI_MANAGER_POPUP,
        UIManagerMenuItem =GTK_UI_MANAGER_MENUITEM,
        UIManagerToolItem =GTK_UI_MANAGER_TOOLITEM,
        UIManagerSeparator =GTK_UI_MANAGER_SEPARATOR,
        UIManagerAccelerator =GTK_UI_MANAGER_ACCELERATOR
    };

    class UIManager : public Object {
        public:
            operator  GtkUIManager *() const { return GTK_UI_MANAGER(obj_); }

            // constructors
            UIManager(GObject *obj) { Init(obj); }
            UIManager() { Init(gtk_ui_manager_new()); Internal(true); }
            UIManager(const ActionGroup &group, const std::string &desc = "") { 
                Init(gtk_ui_manager_new()); Internal(true); 
                Append(group);
                if (!desc.empty()) FromString(desc);
            }

            // import datas
            int FromString(const std::string &desc) {
                GError *err = NULL;
                int rc = gtk_ui_manager_add_ui_from_string(*this, desc.c_str(), desc.length(), &err);
                if (rc == 0) {
                    std::string errorstring;
                    if (err)
                        errorstring = err->message;
                    else
                        errorstring = "FromString UIManager error";

                    g_error_free(err);
                    throw std::runtime_error(errorstring);
                }
                return rc;
            }
            int FromFile(const std::string &filename) {
                GError *err = NULL;
                int rc = gtk_ui_manager_add_ui_from_file(*this, filename.c_str(), &err);
                if (rc == 0) {
                    std::string errorstring;
                    if (err)
                        errorstring = err->message;
                    else
                        errorstring = "FromString UIManager error";

                    g_error_free(err);
                    throw std::runtime_error(errorstring);
                }
                return rc;
            }

            std::string UI() {
                std::string res;
                if (gchar *ui = gtk_ui_manager_get_ui(*this)) {
                    res = ui;
                    g_free(ui);
                }
                return res;
            }
            void RemoveUI(int id) { gtk_ui_manager_remove_ui(*this, id); }
            

            gtk::AccelGroup &AccelGroup() { 
                if (gtk::AccelGroup *a = dynamic_cast<gtk::AccelGroup *>(
                            Object::Find((GObject*)gtk_ui_manager_get_accel_group(*this))))
                    return *a;
                else
                    throw std::runtime_error("UIManager without accelgroup!");
            }
            void Append(const ActionGroup &group) {
                gtk_ui_manager_insert_action_group(*this, group, 1000); // to be sure we are at the end
            }
            void Insert(const ActionGroup &group, int position = 0) {
                gtk_ui_manager_insert_action_group(*this, group, 0);
            }
            void Remove(const ActionGroup &group) { gtk_ui_manager_remove_action_group(*this, group); }


            gtk::Widget *Widget(const std::string &path) {
                return dynamic_cast<gtk::Widget *>(Object::Find((GObject*)
                           gtk_ui_manager_get_widget(*this, path.c_str())));
            }
            gtk::Action *Action(const std::string &path) {
                return dynamic_cast<gtk::Action *>(Object::Find((GObject*)
                           gtk_ui_manager_get_action(*this, path.c_str())));
            }

    };
}

#endif
