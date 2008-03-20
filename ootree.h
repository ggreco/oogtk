#ifndef OOTREE_H
#define OOTREE_H

#include "oogtk.h"
#include <vector>
#include <list>
#include <stdarg.h>

namespace gtk {
    typedef GtkTreeIter TreeIter;
    struct ValidIter : public TreeIter {
        operator  GtkTreeIter *() { return dynamic_cast<GtkTreeIter *>(this); }
        ValidIter() : valid(false) {}
        bool valid;
        bool IsValid() { return valid; }
    };

    enum SelectionMode {
        SelectionNone = GTK_SELECTION_NONE,                             /* Nothing can be selected */
        SelectionSingle = GTK_SELECTION_SINGLE,
        SelectionBrowse = GTK_SELECTION_BROWSE,
        SelectionMultiple = GTK_SELECTION_MULTIPLE,
        SelectionExtended = GTK_SELECTION_EXTENDED
    };

    class TreePath // complete API 
    {
         public:
            operator  GtkTreePath *() const { return obj_; }
            TreePath(GtkTreePath *o) { obj_ = o; }

            TreePath(const std::string &path) {
                if (!path.empty())
                    obj_ = gtk_tree_path_new_from_string(path.c_str());
                else
                    obj_ = gtk_tree_path_new();
            }
            ~TreePath() { gtk_tree_path_free(obj_); }

            std::string str() const {
                std::string ret;
                if (gchar *c = gtk_tree_path_to_string (*this)) {
                    ret = c;
                    g_free(c);
                }
                return ret;
            }
            std::string ToString() const { return str(); }
            void Append(int idx) { gtk_tree_path_append_index(*this, idx); }
            void Prepend(int idx) { gtk_tree_path_prepend_index(*this, idx); }

            int Depth() const { return gtk_tree_path_get_depth(*this); }
            bool operator==(const TreePath &p) { return gtk_tree_path_compare(*this, p); }
            bool operator!=(const TreePath &p) { return !gtk_tree_path_compare(*this, p); }

            TreePath &operator++() { gtk_tree_path_next(*this); return *this; }
            TreePath &operator--() { gtk_tree_path_prev(*this); return *this; }

            // to emulate GTK api we offer also Next/Pred
            void Next() { gtk_tree_path_next(*this); }
            void Pred() { gtk_tree_path_prev(*this); }
            void Up() { gtk_tree_path_up(*this); }
            void Down() { gtk_tree_path_down(*this); }

            bool IsAncestor(const TreePath &descendant) const { return gtk_tree_path_is_ancestor(*this, descendant); }
            bool IsDescendant(const TreePath &ancestor) const { return gtk_tree_path_is_descendant(*this, ancestor); }
        private:
            GtkTreePath *obj_;
    };

    class TreeView;

    class TreeSelection : public Object
    {
         public:
            operator  GtkTreeSelection *() const { return GTK_TREE_SELECTION(obj_); }
            TreeSelection(GObject *o) { Init(o); }

            OneOf<SelectionMode,GtkSelectionMode> Mode() const { return gtk_tree_selection_get_mode(*this); }
            void Mode(OneOf<SelectionMode,GtkSelectionMode> mode) { gtk_tree_selection_set_mode(*this, mode); }
            gtk::TreeView &TreeView();

            int Count() const {
                return gtk_tree_selection_count_selected_rows(*this);
            }

            // select/unselect functions
            void Select(const TreePath &path) { gtk_tree_selection_select_path(*this, path); }
            void Unselect(const TreePath &path) { gtk_tree_selection_unselect_path(*this, path); }
            bool IsSelected(const TreePath &path) const { return gtk_tree_selection_path_is_selected(*this, path); }
            void Select(const TreeIter &it) { gtk_tree_selection_select_iter(*this, const_cast<TreeIter *>(&it)); }
            void Unselect(const TreeIter &it) { gtk_tree_selection_unselect_iter(*this, const_cast<TreeIter *>(&it)); }
            bool IsSelected(const TreeIter &it) const { return gtk_tree_selection_iter_is_selected(*this, const_cast<TreeIter *>(&it)); }

            void SelectAll() { gtk_tree_selection_select_all(*this); }
            void UnselectAll() { gtk_tree_selection_unselect_all(*this); }

            void SelectRange(const TreePath &begin, const TreePath &end) {
                gtk_tree_selection_select_range(*this, begin, end);
            }
    };

    class TreeModel : public Object
    {
        public:
            operator  GtkTreeModel *() const { return GTK_TREE_MODEL(obj_); }
            void Get(const TreeIter &it, ...) {
                va_list va;
                va_start(va, it);
                gtk_tree_model_get_valist(*this, const_cast<TreeIter *>(&it), va);
                va_end(va);
            }
            TreeIter Root() { return First(); }
            TreeIter First() {
                GtkTreeIter it;
                gtk_tree_model_get_iter_first(*this, &it);
                return it;
            }
            bool Next(TreeIter &it) {
                return gtk_tree_model_iter_next(*this, &it);
            }
            ValidIter Children(TreeIter &parent) {
                ValidIter it;
                it.valid = gtk_tree_model_iter_children(*this, it, &parent);
            }
            ValidIter Children() {
                ValidIter it;
                it.valid = gtk_tree_model_iter_children(*this, it, NULL);
            }
            bool HasChildren(TreeIter &it) const {
                return gtk_tree_model_iter_has_child(*this, &it);
            }
            int ChildrenNumber(TreeIter &it) const {
                return gtk_tree_model_iter_n_children(*this, &it);
            }
            int ChildrenNumber() const {
                return gtk_tree_model_iter_n_children(*this, NULL);
            }
            ValidIter GetNthChild(TreeIter &parent, int position) {
                ValidIter it;
                it.valid = gtk_tree_model_iter_nth_child (*this, it, &parent, position);
            }
            ValidIter Parent(TreeIter &child) {
                ValidIter it;
                it.valid = gtk_tree_model_iter_parent (*this, it, &child);
            }
            int Columns() const {
                return gtk_tree_model_get_n_columns(*this);
            }
    };

    typedef std::vector<GType> TypeList;

    class ListStore : public TreeModel
    {
        public:
            operator GtkListStore *() const { return GTK_LIST_STORE(obj_); }            
            ListStore(GObject *o) { Init(o); }

            ListStore(int size, GType *types) {
                Init(gtk_list_store_newv(size, types));
                Internal(true);
            }
            ListStore(const TypeList &types) {
                Init(gtk_list_store_newv(types.size(), (GType *)&types[0]));
                Internal(true);
            }
            void Set(const TreeIter &it, ...) {
                va_list va;
                va_start(va, it);
                gtk_list_store_set_valist(*this, const_cast<TreeIter *>(&it), va);
                va_end(va);
            }
            void AddTail(...) {
                TreeIter it;
                gtk_list_store_append(*this, &it);
                va_list va;
                va_start(va, this);
                gtk_list_store_set_valist(*this, &it, va);
                va_end(va);
            }
            void AddFront(...) {
                TreeIter it;
                gtk_list_store_prepend(*this, &it);
                va_list va;
                va_start(va, this);
                gtk_list_store_set_valist(*this, &it, va);
                va_end(va);
            }
            TreeIter Append() {
                TreeIter it;
                gtk_list_store_append(*this, &it);
                return it;
            }
            TreeIter Prepend() {
                TreeIter it;
                gtk_list_store_prepend(*this, &it);
                return it;
            }
            TreeIter Insert(int position) {
                TreeIter it;
                gtk_list_store_insert(*this, &it, position);
                return it;
            }            
            void Remove(TreeIter &it) {
                gtk_list_store_remove(*this, &it);
            }
            void Swap(TreeIter &a, TreeIter &b) {
                gtk_list_store_swap(*this, &a, &b);
            }

            bool IsValid(TreeIter &it) const {
                return gtk_list_store_iter_is_valid(*this, &it);
            }
            void Clear() {
                gtk_list_store_clear(*this);
            }
    };

    class TreeStore : public TreeModel
    {
        public:
            operator GtkTreeStore *() const { return GTK_TREE_STORE(obj_); }            
            TreeStore(GObject *o) { Init(o); }

            TreeStore(int size, GType *types) {
                Init(gtk_tree_store_newv(size, types));
                Internal(true);
            }
            TreeStore(TypeList &types) {
                Init(gtk_tree_store_newv(types.size(), (GType *)&types[0]));
                Internal(true);
            }
            void Set(const TreeIter &it, ...) {
                va_list va;
                va_start(va, it);
                gtk_tree_store_set_valist(*this, const_cast<TreeIter *>(&it), va);
                va_end(va);
            }
            void AddFront(const TreeIter &parent, ...) {
                TreeIter it;
                gtk_tree_store_prepend(*this, &it, const_cast<TreeIter *>(&parent));
                va_list va;
                va_start(va, parent);
                gtk_tree_store_set_valist(*this, &it, va);
                va_end(va);
            }

            void AddTail(const TreeIter &parent, ...) {
                TreeIter it;
                gtk_tree_store_append(*this, &it, const_cast<TreeIter *>(&parent));
                va_list va;
                va_start(va, parent);
                gtk_tree_store_set_valist(*this, &it, va);
                va_end(va);
            }
            void AddFront(...) {
                TreeIter it;
                gtk_tree_store_prepend(*this, &it, NULL);
                va_list va;
                va_start(va, this);
                gtk_tree_store_set_valist(*this, &it, va);
                va_end(va);
            }

            void AddTail(...) {
                TreeIter it;
                gtk_tree_store_append(*this, &it, NULL);
                va_list va;
                va_start(va, this);
                gtk_tree_store_set_valist(*this, &it, va);
                va_end(va);
            }

            TreeIter Append() {
                TreeIter it;
                gtk_tree_store_append(*this, &it, NULL);
                return it;
            }
            TreeIter Prepend() {
                TreeIter it;
                gtk_tree_store_prepend(*this, &it, NULL);
                return it;
            }
            TreeIter Append(const TreeIter &parent) {
                TreeIter it;
                gtk_tree_store_append(*this, &it, const_cast<TreeIter *>(&parent));
                return it;
            }
            TreeIter Prepend(const TreeIter &parent) {
                TreeIter it;
                gtk_tree_store_prepend(*this, &it, const_cast<TreeIter *>(&parent));
                return it;
            }
            TreeIter Insert(int position) {
                TreeIter it;
                gtk_tree_store_insert(*this, &it, NULL, position);
                return it;
            }
            TreeIter Insert(const TreeIter &parent, int position) {
                TreeIter it;
                gtk_tree_store_insert(*this, &it, const_cast<TreeIter *>(&parent), position);
                return it;
            }
            void Remove(TreeIter &it) {
                gtk_tree_store_remove(*this, &it);
            }
            void Swap(TreeIter &a, TreeIter &b) {
                gtk_tree_store_swap(*this, &a, &b);
            }

            // query sull'albero
            int Depth(TreeIter &it) const {
                return gtk_tree_store_iter_depth(*this, &it);
            }
            bool IsValid(TreeIter &it) const {
                return gtk_tree_store_iter_is_valid(*this, &it);
            }
            bool IsAncestor(TreeIter &it, TreeIter &descendant) const {
                return gtk_tree_store_is_ancestor(*this, &it, &descendant);
            }
            void Clear() {
                gtk_tree_store_clear(*this);
            }
    };

    class CellRenderer : public Object
    {
        public:
            operator  GtkCellRenderer *() const { return GTK_CELL_RENDERER(obj_); }            
    };

    class CellRendererText : public CellRenderer
    {
        public:
            CellRendererText(GObject *obj) { Init(obj); }
            CellRendererText() { 
                Init(gtk_cell_renderer_text_new()); 
                Internal(true);
            }
    };

    class CellRendererPixbuf : public CellRenderer
    {
        public:
            CellRendererPixbuf(GObject *obj) { Init(obj); }
            CellRendererPixbuf() { 
                Init(gtk_cell_renderer_pixbuf_new()); 
                Internal(true);
            }
    };

    class CellRendererToggle : public CellRenderer
    {
        public:
            operator  GtkCellRendererToggle *() const { return GTK_CELL_RENDERER_TOGGLE(obj_); }            

            CellRendererToggle(GObject *obj) { Init(obj); }
            CellRendererToggle() { 
                Init(gtk_cell_renderer_toggle_new()); 
                Internal(true);
            }

            void Radio(bool flag) { gtk_cell_renderer_toggle_set_radio(*this, flag); }
            bool Radio() const { return gtk_cell_renderer_toggle_get_radio(*this); }
            void Active(bool flag) { gtk_cell_renderer_toggle_set_active(*this, flag); }
            bool Active() const { gtk_cell_renderer_toggle_get_active(*this); }
    };

    typedef std::list<CellRenderer *> RendererList;

    class TreeViewColumn : public Object
    {
        public:
            operator  GtkTreeViewColumn *() const { return GTK_TREE_VIEW_COLUMN(obj_); }

            TreeViewColumn(GObject *obj) { Init(obj); }

            TreeViewColumn() {
                Init(gtk_tree_view_column_new());
                Internal(true);
            }

            bool Visible() const { gtk_tree_view_column_get_visible(*this); }
            void Visible(bool flag) { gtk_tree_view_column_set_visible(*this, flag); }
            void Title(const std::string &title) {
                gtk_tree_view_column_set_widget(*this, Label(title).Ref());
            }
            void GetRenderers(RendererList &objs) {
                GList *list = gtk_tree_view_column_get_cell_renderers(*this), *l;

                l = list;

                while (l) {
                    if (CellRenderer *o = dynamic_cast<CellRenderer *>(Object::Find((GObject *)l->data)))
                        objs.push_back(o);

                    l = l->next;
                }

                g_list_free(list);
            }
    };

    typedef std::list<TreeViewColumn *> ColumnList;

    class TreeView : public Container
    {
        public:
            operator  GtkTreeView *() const { return GTK_TREE_VIEW(Obj()); }

            TreeView(GObject *obj) { Init(obj); }
            TreeView(TreeModel &model) {
                model.Ref();
                Init(gtk_tree_view_new_with_model(model));
                Internal(true);
            }
            TreeView() {
                Init(gtk_tree_view_new());
                Internal(true);
            }
            void Model(TreeModel &model) {
                gtk_tree_view_set_model(*this, model);
            }

            TreeModel *Model() {
                GtkTreeModel *m = gtk_tree_view_get_model(*this);

                return dynamic_cast<TreeModel *>(Object::Find((GObject *)m));
            }
            TreeViewColumn *Get(int pos) {
                if (pos < 0)
                    return NULL;

                GtkTreeViewColumn *c = gtk_tree_view_get_column (*this, pos);

                return dynamic_cast<TreeViewColumn *>(Object::Find((GObject *)c));
            }

            TreeViewColumn *AddTextColumn(const std::string &title, int id) {
                CellRenderer *r = new CellRendererText();
                int col = gtk_tree_view_insert_column_with_attributes(*this, -1,
                        title.c_str(), *r, "markup", id,
                        NULL);

                return Get(col - 1);
            }
            TreeViewColumn *AddBooleanColumn(const std::string &title, int id,
                                             int inconsistent = -1, int visibility = -1)
            {

                CellRenderer *r = new CellRendererToggle();
                gint col;

                if (visibility != -1) {
                    if (inconsistent != -1) 
                        col = gtk_tree_view_insert_column_with_attributes (*this, -1, title.c_str(),
                                *r, "active", id, "inconsistent", inconsistent, "visible", visibility,
                                NULL);
                    else
                        col = gtk_tree_view_insert_column_with_attributes (*this, -1, title.c_str(),
                                *r, "active", id, "visible", visibility,
                                NULL);
                }
                else if (inconsistent != -1)
                    col = gtk_tree_view_insert_column_with_attributes (*this, -1, title.c_str(),
                            *r, "active", id, "inconsistent", inconsistent,
                            NULL);
                else 
                    col = gtk_tree_view_insert_column_with_attributes (*this, -1, title.c_str(),
                            *r, "active", id,
                            NULL);

                return Get(col -1);
            }

            TreeViewColumn *AddPixColumn(const std::string &title, int id) {
                CellRenderer *r = new CellRendererPixbuf();
                int col = gtk_tree_view_insert_column_with_attributes(*this, -1,
                        title.c_str(), *r, "pixbuf", id,
                        NULL);

                return Get(col - 1);
            }

            void Append(ColumnList &cols) {
                for(ColumnList::iterator it = cols.begin(); it != cols.end(); ++it)
                    Append(*(*it));
            }

            void Append(TreeViewColumn &col) {
                gtk_tree_view_append_column(*this, col);
            }
            void Insert(TreeViewColumn &col, int position = -1) {
                gtk_tree_view_insert_column(*this, col, position);
            }
            void Remove(TreeViewColumn &col)
            {
                gtk_tree_view_remove_column(*this, col);
            }
            void GetColumns(ColumnList &cols) {
                GList *list = gtk_tree_view_get_columns(*this), *l;

                while (l) {
                    if (TreeViewColumn *o = dynamic_cast<TreeViewColumn *>(
                                Object::Find((GObject *)l->data)))
                        cols.push_back(o);

                    l = l->next;
                }
            }
            void MoveAfter(TreeViewColumn &col, TreeViewColumn &ref) {
                gtk_tree_view_move_column_after(*this, col, ref);
            }
            TreeSelection &Selection() { // un treeview non puo' essere creato senza una TreeSelection
                GtkTreeSelection *s = gtk_tree_view_get_selection(*this);

                if (!s) throw std::runtime_error("TreeView without TreeSelection!");
                return dynamic_cast<TreeSelection&>(*(Object::Find((GObject *)s)));
            }
    };

    gtk::TreeView &TreeSelection::
    TreeView() {
        GtkTreeView *tv = gtk_tree_selection_get_tree_view(*this);
        gtk::TreeView *t = dynamic_cast<gtk::TreeView *>(Object::Find((GObject *)tv));
        if (!t)
            throw std::runtime_error("TreeSelection without treeview!");

        return *t;
    }
}

#endif
