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
            TreePath(const TreePath &o) { obj_ = gtk_tree_path_copy(o); }
            TreePath(GtkTreePath *o) { obj_ = o; }

            TreePath(const std::string &path = "") {
                if (!path.empty())
                    obj_ = gtk_tree_path_new_from_string(path.c_str());
                else
                    obj_ = gtk_tree_path_new();
            }
            ~TreePath() { gtk_tree_path_free(obj_); }

            TreePath &operator=(const TreePath &orig) {
                gtk_tree_path_free(obj_);
                obj_ = gtk_tree_path_copy(orig);
                return *this;
            }
            TreePath &operator=(const GtkTreePath &orig) {
                gtk_tree_path_free(obj_);
                obj_ = gtk_tree_path_copy(&orig);
                return *this;
            }
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

    class TreeRowReference;

    typedef std::list<TreeRowReference> RefVec;

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

            RefVec SelectedRows();
            BUILD_VOID_EVENT(OnChanged, "changed");
    };

    class TreeModel : public Object
    {
        public:
            operator  GtkTreeModel *() const { return GTK_TREE_MODEL(obj_); }

            void GetValue(const TreeIter &it, int idx, int &value) {
                gtk_tree_model_get(*this, 
                        const_cast<TreeIter *>(&it), idx, &value, -1);
            }
            void GetValue(const TreeIter &it, int idx, void *&value) {
                gtk_tree_model_get(*this, 
                        const_cast<TreeIter *>(&it), idx, &value, -1);
            }
            void GetValue(const TreeIter &it, int idx, std::string &value) {
                gchar *field;
                gtk_tree_model_get(*this, 
                        const_cast<TreeIter *>(&it), idx, &field, -1);
                value = field;
                g_free(field);
            }

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
                return it;
            }

            virtual void Remove(const TreeIter &it) = 0;
            virtual void Set(const TreeIter &it, ...) = 0;
            virtual bool IsValid(const TreeIter &it) const = 0;
            virtual void SetValue(const TreeIter &it, int idx, int value) = 0;
            virtual void SetValue(const TreeIter &it, int idx, const std::string &value) = 0;
            virtual void SetValue(const TreeIter &it, int idx, void *value) = 0;

            ValidIter Children() {
                ValidIter it;
                it.valid = gtk_tree_model_iter_children(*this, it, NULL);
                return it;
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
                return it;
            }
            ValidIter Parent(TreeIter &child) {
                ValidIter it;
                it.valid = gtk_tree_model_iter_parent (*this, it, &child);
                return it;
            }
            int Columns() const {
                return gtk_tree_model_get_n_columns(*this);
            }
            TreePath Path(const TreeIter &it) {
                return TreePath(gtk_tree_model_get_path(*this, const_cast<TreeIter *>(&it)));
            }
            bool Iter(TreeIter &iter, const TreePath &path) {
                return gtk_tree_model_get_iter(*this, &iter, path);
            }
    };

    typedef std::vector<GType> TypeList;

    class ListStore : public TreeModel
    {
        public:
            operator GtkListStore *() const { return GTK_LIST_STORE(obj_); }            
            ListStore(GObject *o) { Init(o); }

            ListStore(int size, const GType *types) {
                Init(gtk_list_store_newv(size, const_cast<GType *>(types)));
                Internal(true);
            }
            ListStore(size_t size, ...) {
                va_list va;
                TypeList types;
                va_start(va, size);
                while (size--)
                    types.push_back(va_arg(va, GType));
                va_end(va);
                Init(gtk_list_store_newv(types.size(), (GType *)&types[0]));
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

            void SetValue(const TreeIter &it, int idx, int value) {
                gtk_list_store_set(*this, 
                        const_cast<TreeIter *>(&it), idx, &value, -1);
            }
            void SetValue(const TreeIter &it, int idx, void *value) {
                gtk_list_store_set(*this, 
                        const_cast<TreeIter *>(&it), idx, value, -1);
            }
            void SetValue(const TreeIter &it, int idx, const std::string &value) {
                gtk_list_store_set(*this, 
                        const_cast<TreeIter *>(&it), idx, value.c_str(), -1);
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
            void Remove(const TreeIter &it) {
                gtk_list_store_remove(*this, const_cast<TreeIter *>(&it));
            }
            void Swap(TreeIter &a, TreeIter &b) {
                gtk_list_store_swap(*this, &a, &b);
            }

            bool IsValid(const TreeIter &it) const {
                return gtk_list_store_iter_is_valid(*this, const_cast<TreeIter *>(&it));
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
            TreeStore(size_t size, ...) {
                va_list va;
                TypeList types;
                va_start(va, size);
                while (size--)
                    types.push_back(va_arg(va, GType));
                va_end(va);
                Init(gtk_list_store_newv(types.size(), (GType *)&types[0]));
                Internal(true);
            }
            void Set(const TreeIter &it, ...) {
                va_list va;
                va_start(va, it);
                gtk_tree_store_set_valist(*this, const_cast<TreeIter *>(&it), va);
                va_end(va);
            }
            void SetValue(const TreeIter &it, int idx, int value) {
                gtk_tree_store_set(*this, 
                        const_cast<TreeIter *>(&it), idx, &value, -1);
            }
            void SetValue(const TreeIter &it, int idx, void *value) {
                gtk_tree_store_set(*this, 
                        const_cast<TreeIter *>(&it), idx, value, -1);
            }
            void SetValue(const TreeIter &it, int idx, const std::string &value) {
                gtk_tree_store_set(*this, 
                        const_cast<TreeIter *>(&it), idx, value.c_str(), -1);
            }
            
            TreeIter AddFront(const TreeIter &parent, ...) {
                TreeIter it;
                gtk_tree_store_prepend(*this, &it, const_cast<TreeIter *>(&parent));
                va_list va;
                va_start(va, parent);
                gtk_tree_store_set_valist(*this, &it, va);
                va_end(va);

                return it;
            }

            TreeIter AddTail(const TreeIter &parent, ...) {
                TreeIter it;
                gtk_tree_store_append(*this, &it, const_cast<TreeIter *>(&parent));
                va_list va;
                va_start(va, parent);
                gtk_tree_store_set_valist(*this, &it, va);
                va_end(va);

                return it;
            }
            TreeIter AddFront(...) {
                TreeIter it;
                gtk_tree_store_prepend(*this, &it, NULL);
                va_list va;
                va_start(va, this);
                gtk_tree_store_set_valist(*this, &it, va);
                va_end(va);

                return it;
            }

            TreeIter AddTail(...) {
                TreeIter it;
                gtk_tree_store_append(*this, &it, NULL);
                va_list va;
                va_start(va, this);
                gtk_tree_store_set_valist(*this, &it, va);
                va_end(va);

                return it;
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
            void Remove(const TreeIter &it) {
                gtk_tree_store_remove(*this, const_cast<TreeIter *>(&it));
            }
            void Swap(TreeIter &a, TreeIter &b) {
                gtk_tree_store_swap(*this, &a, &b);
            }

            // query sull'albero
            int Depth(const TreeIter &it) const {
                return gtk_tree_store_iter_depth(*this, const_cast<TreeIter *>(&it));
            }
            bool IsValid(const TreeIter &it) const {
                return gtk_tree_store_iter_is_valid(*this, const_cast<TreeIter *>(&it));
            }
            bool IsAncestor(TreeIter &it, TreeIter &descendant) const {
                return gtk_tree_store_is_ancestor(*this, &it, &descendant);
            }
            void Clear() {
                gtk_tree_store_clear(*this);
            }
    };

    class TreeRowReference
    {
            friend class TreeSelection;
            // only for internal use
            TreeRowReference(GtkTreeModel *model, GtkTreePath *path) {
                obj_ = gtk_tree_row_reference_new(model, path);
            }
        public:
            operator  GtkTreeRowReference *() const { return obj_; }
            TreeRowReference(const TreeModel &model, const TreePath &path) {
                obj_ = gtk_tree_row_reference_new(model, path);
            }
            // copy constructor
            TreeRowReference(const TreeRowReference &src) {
                obj_ = gtk_tree_row_reference_copy(src);
            }
            TreeModel &Model() const {
                if (TreeModel *m = dynamic_cast<TreeModel *>(
                            Object::Find((GObject *)gtk_tree_row_reference_get_model(obj_))))
                    return *m;
                else
                    throw std::runtime_error("TreeRowReference without Model!!!");
            }
            TreeIter Iter() const {
                if (GtkTreePath *p = gtk_tree_row_reference_get_path(obj_)) {
                    TreeIter it;
                    if (gtk_tree_model_get_iter(gtk_tree_row_reference_get_model(obj_), 
                                                &it, p))
                        return it;
                }
                throw std::runtime_error("Unable to get the Iter from the reference!");
            }
            TreePath Path() const {
                return TreePath(gtk_tree_row_reference_get_path(obj_));
            }
            bool Valid() const {
                return gtk_tree_row_reference_valid(obj_);
            }
            TreeRowReference &operator=(const TreeRowReference &right) {
                gtk_tree_row_reference_free(obj_);
                obj_ = gtk_tree_row_reference_copy(right);
                return *this;
            }
            TreeRowReference &operator=(const GtkTreeRowReference &right) {
                gtk_tree_row_reference_free(obj_);
                obj_ = gtk_tree_row_reference_copy(const_cast<GtkTreeRowReference *>(&right));
                return *this;
            }
            ~TreeRowReference() {
                gtk_tree_row_reference_free(obj_);
            }
        private:
            GtkTreeRowReference *obj_;
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
            bool Active() const { return gtk_cell_renderer_toggle_get_active(*this); }
    };

    typedef std::list<CellRenderer *> RendererList;

    enum SortType
    {
        SortAscending  = GTK_SORT_ASCENDING,
        SortDescending = GTK_SORT_DESCENDING
    };

    class TreeViewColumn : public Object
    {
        public:
            operator  GtkTreeViewColumn *() const { return GTK_TREE_VIEW_COLUMN(obj_); }

            TreeViewColumn(GObject *obj) { Init(obj); }

            TreeViewColumn() {
                Init(gtk_tree_view_column_new());
                Internal(true);
            }

            void Title(const std::string &title) {
                Label l(title);
                gtk_tree_view_column_set_widget(*this, l);
                l.Show();
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

            // get/set methods
            bool Visible() const { return gtk_tree_view_column_get_visible(*this); }
            void Visible(bool flag) { gtk_tree_view_column_set_visible(*this, flag); }

            bool Clickable() const { return gtk_tree_view_column_get_clickable(*this); }
            void Clickable(bool flag) { gtk_tree_view_column_set_clickable(*this, flag); }

            bool Expand() const { return gtk_tree_view_column_get_expand(*this); }
            void Expand(bool flag) { gtk_tree_view_column_set_expand(*this, flag); }

            bool Resizable() const { return gtk_tree_view_column_get_resizable(*this); }
            void Resizable(bool flag) { gtk_tree_view_column_set_resizable(*this, flag); }

            int Spacing() const { return gtk_tree_view_column_get_spacing(*this); }
            void Spacing(int pixels) { gtk_tree_view_column_set_spacing(*this, pixels); }

            bool FixedWidth() const { return gtk_tree_view_column_get_fixed_width(*this); }
            void FixedWidth(bool flag) { gtk_tree_view_column_set_fixed_width(*this, flag); }

            int  Width() const { return gtk_tree_view_column_get_width(*this); }
            //void Width(int pixels) { gtk_tree_view_column_set_width(*this, pixels); }

#if 0
            // at the moment we support only widget based column titles.
            std::string Title() const { return gtk_tree_view_column_get_title(*this); }
            void Title(const std::string &title) { gtk_tree_view_column_set_title(*this, title); }
#endif
            bool Reorderable() const { return gtk_tree_view_column_get_reorderable(*this); }
            void Reorderable(bool flag) { gtk_tree_view_column_set_reorderable(*this, flag); }

            int  MinWidth() const { return gtk_tree_view_column_get_min_width(*this); }
            void MinWidth(int pixels) { gtk_tree_view_column_set_min_width(*this, pixels); }

            int  MaxWidth() const { return gtk_tree_view_column_get_max_width(*this); }
            void MaxWidth(int pixels) { gtk_tree_view_column_set_max_width(*this, pixels); }
            
            float Alignment() const { return gtk_tree_view_column_get_alignment(*this); }
            void Alignment(float xalign) { gtk_tree_view_column_set_alignment(*this, xalign); }

            // some sorting related methods
            bool SortIndicator() const { return gtk_tree_view_column_get_sort_indicator(*this); }
            void SortIndicator(bool flag) { gtk_tree_view_column_set_sort_indicator(*this, flag); }

            OneOf<gtk::SortType, GtkSortType> SortType() const { return gtk_tree_view_column_get_sort_order(*this); }
            void SortType(OneOf<gtk::SortType, GtkSortType> type) { gtk_tree_view_column_set_sort_order(*this, type); }
            
            int SortColumnId() const { return gtk_tree_view_column_get_sort_column_id(*this); }
            void SortColumnId(int sort_col_id) { gtk_tree_view_column_set_sort_column_id(*this, sort_col_id); }

            gtk::TreeView *TreeView() const;
            void PackStart(const CellRenderer &cell, bool expand = true) { gtk_tree_view_column_pack_start(*this, cell, expand); }
            void AddAttribute(const CellRenderer &cell, const char *name, int column) {
                gtk_tree_view_column_add_attribute(*this, cell, name, column);
            }
    };

    typedef std::list<TreeViewColumn *> ColumnList;

    class TreeView : public Container
    {
        public:
            operator  GtkTreeView *() const { return GTK_TREE_VIEW(Obj()); }

            TreeView(GObject *obj) { Init(obj); }
            TreeView(const TreeModel &model) {
                TreeModel &m = const_cast<TreeModel &>(model);
                m.Ref();
                Init(gtk_tree_view_new_with_model(m));
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

            TreeViewColumn *AddSortableTextColumn(const std::string &title, int id)
            { 
                if (TreeViewColumn *column = AddTextColumn(title, id)) {
                    column->Clickable(true);
                    column->SortIndicator(true);
                    column->SortColumnId(id);

//   gtk_tree_sortable_set_sort_column_id(
//            GTK_TREE_SORTABLE(m_model), id, GTK_SORT_ASCENDING);
                    return column;
                }
                return NULL;
            }


            TreeViewColumn *AddTextColumn(const std::string &title, int id) {
                CellRendererText r;
                int col = gtk_tree_view_insert_column_with_attributes(*this, -1,
                        title.c_str(), r, "markup", id,
                        NULL);

                return Get(col - 1);
            }
            TreeViewColumn *AddBooleanColumn(const std::string &title, int id,
                                             int inconsistent = -1, int visibility = -1)
            {

                CellRendererToggle r;
                gint col;

                if (visibility != -1) {
                    if (inconsistent != -1) 
                        col = gtk_tree_view_insert_column_with_attributes (*this, -1, title.c_str(),
                                r, "active", id, "inconsistent", inconsistent, "visible", visibility,
                                NULL);
                    else
                        col = gtk_tree_view_insert_column_with_attributes (*this, -1, title.c_str(),
                                r, "active", id, "visible", visibility,
                                NULL);
                }
                else if (inconsistent != -1)
                    col = gtk_tree_view_insert_column_with_attributes (*this, -1, title.c_str(),
                            r, "active", id, "inconsistent", inconsistent,
                            NULL);
                else 
                    col = gtk_tree_view_insert_column_with_attributes (*this, -1, title.c_str(),
                            r, "active", id,
                            NULL);

                return Get(col -1);
            }

            TreeViewColumn *AddPixTextColumn(const std::string &title, int textid, int pixid) {
                CellRendererPixbuf r1;
                CellRendererText r2;

                TreeViewColumn *col = new TreeViewColumn();

                col->Title(title);
                col->PackStart(r1, false);
                col->PackStart(r2);
                col->AddAttribute(r1, "pixbuf", pixid);
                col->AddAttribute(r2, "markup", textid);
                Append(*col);

                return col;
            }
            TreeViewColumn *AddPixColumn(const std::string &title, int id) {
                CellRendererPixbuf r;
                int col = gtk_tree_view_insert_column_with_attributes(*this, -1,
                        title.c_str(), r, "pixbuf", id,
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
                l = list;
                while (l) {
                    if (TreeViewColumn *o = dynamic_cast<TreeViewColumn *>(
                                Object::Find((GObject *)l->data)))
                        cols.push_back(o);

                    l = l->next;
                }
                g_list_free(list);
            }
            void MoveAfter(TreeViewColumn &col, TreeViewColumn &ref) {
                gtk_tree_view_move_column_after(*this, col, ref);
            }
            void ExpanderColumn(TreeViewColumn &col) { gtk_tree_view_set_expander_column(*this, col); }
            TreeViewColumn *ExpanderColumn() const {
                return dynamic_cast<TreeViewColumn *>(Object::Find((GObject *)
                            gtk_tree_view_get_expander_column(*this)));
            }
            void ExpandAll() { gtk_tree_view_expand_all(*this); }
            void CollapseAll() { gtk_tree_view_collapse_all(*this); }
            void ExpandRow(const TreePath &path, bool recursive = true) { gtk_tree_view_expand_row(*this, path, recursive); }
            void CollapseRow(const TreePath &path) { gtk_tree_view_collapse_row(*this, path); }
            void ScrollTo(int x, int y) { gtk_tree_view_scroll_to_point(*this, x, y); }
            void ScrollTo(const Point &point) { ScrollTo(point.x, point.y); }

            TreeSelection &Selection() { // un treeview non puo' essere creato senza una TreeSelection
                GtkTreeSelection *s = gtk_tree_view_get_selection(*this);

                if (!s) throw std::runtime_error("TreeView without TreeSelection!");
                return dynamic_cast<TreeSelection&>(*(Object::Find((GObject *)s)));
            }

            bool PathAt(TreePath &path, const Point &position) {
                GtkTreePath *p;
                if(gtk_tree_view_get_path_at_pos(*this, position.x, position.y,
                            &p, NULL, NULL, NULL)) {
                    path = *p;
                    gtk_tree_path_free(p);
                    return true;
                }
                return false;
            }

            // get/set methods
            bool HeadersClickable() const { return gtk_tree_view_get_headers_clickable(*this); }
            void HeadersClickable(bool flag) const { gtk_tree_view_set_headers_clickable(*this, flag); }
            bool HeadersVisible() const { return gtk_tree_view_get_headers_visible(*this); }
            void HeadersVisible(bool flag) const { gtk_tree_view_set_headers_visible(*this, flag); }
            bool RulesHint() const { return gtk_tree_view_get_rules_hint(*this); }
            void RulesHint(bool flag) { gtk_tree_view_set_rules_hint(*this, flag); }

            int  LevelIndentation() const { return gtk_tree_view_get_level_indentation(*this); }
            void LevelIndentation(int pixels) { gtk_tree_view_set_level_indentation(*this, pixels); }
            bool ShowExpanders() const { return gtk_tree_view_get_show_expanders(*this); }
            void ShowExpanders(bool flag) { gtk_tree_view_set_show_expanders(*this, flag); }
            
            bool Reorderable() const { return gtk_tree_view_get_reorderable(*this); }
            void Reorderable(bool flag) { gtk_tree_view_set_reorderable(*this, flag); }

            bool EnableSearch() const { return gtk_tree_view_get_enable_search(*this); }
            void EnableSearch(bool flag) { gtk_tree_view_set_enable_search(*this, flag); }

            int SearchColumn() const { return gtk_tree_view_get_search_column(*this); }
            void SearchColumn(int col) { gtk_tree_view_set_search_column(*this, col); }

            bool FixedHeightMode() const { return gtk_tree_view_get_fixed_height_mode(*this); }
            void FixedHeightMode(bool flag) { gtk_tree_view_set_fixed_height_mode(*this, flag); }
    };

    inline gtk::TreeView &TreeSelection::
    TreeView() {
        GtkTreeView *tv = gtk_tree_selection_get_tree_view(*this);
        gtk::TreeView *t = dynamic_cast<gtk::TreeView *>(Object::Find((GObject *)tv));
        if (!t)
            throw std::runtime_error("TreeSelection without treeview!");

        return *t;
    }

    inline gtk::TreeView *TreeViewColumn::
    TreeView() const
    {
        return dynamic_cast<gtk::TreeView *>(Object::Find((GObject *)
                    gtk_tree_view_column_get_tree_view(*this)));
    }

    inline RefVec TreeSelection::
    SelectedRows() {
        RefVec result;

        if (GtkTreeView *tv = gtk_tree_selection_get_tree_view(*this)) {
            if (GtkTreeModel *model = gtk_tree_view_get_model(tv)) {
                if (GList *list = gtk_tree_selection_get_selected_rows(*this,
                            &model)) {
                    GList *l = list;
                    while (l) {
                        result.push_back(TreeRowReference(model, (GtkTreePath *)l->data));
                        l = l->next;
                    }
                    g_list_foreach (list, (void (*)(void *,void *))gtk_tree_path_free, NULL);
                    g_list_free (list);
                }
            }
        }

        return result;
    }

    class ComboBox : public Bin
    {
        public:
            operator  GtkComboBox *() const { return GTK_COMBO_BOX(Obj()); }

            ComboBox(const DerivedType &) {}
            ComboBox(GObject *obj) { Init(obj); }

            ComboBox() {
                Init(gtk_combo_box_new());
                Internal(true);
            }
            ComboBox(const TreeModel &model) {
                Init(gtk_combo_box_new_with_model(model));
                Internal(true);
            }
            void Model(const TreeModel &model) { gtk_combo_box_set_model(*this, model); }
            TreeModel *Model() {
                return dynamic_cast<TreeModel *>(Object::Find((GObject *)
                        gtk_combo_box_get_model(*this)));
            }
            int Active() const { return gtk_combo_box_get_active(*this); }
            void Active(int row) { gtk_combo_box_set_active(*this, row); }

            void Popup() { gtk_combo_box_popup(*this); }
            void Popdown() { gtk_combo_box_popdown(*this); }

            bool FocusOnClick() const { return gtk_combo_box_get_focus_on_click(*this); }
            void FocusOnClick(bool flag) { gtk_combo_box_set_focus_on_click(*this, flag); }

            int WrapWidth() const { return gtk_combo_box_get_wrap_width(*this); }
            void WrapWidth(int width) const { gtk_combo_box_set_wrap_width(*this, width); }

            virtual void SetActiveIter(const TreeIter &it) { 
                gtk_combo_box_set_active_iter(*this, const_cast<TreeIter *>(&it));
            }
            virtual bool GetActiveIter(TreeIter &it) {
                return gtk_combo_box_get_active_iter(*this, &it);
            }

            BUILD_EVENT(OnChanged, "changed");
   };

    class ComboBoxText : public ComboBox
    {
        public:
            operator  GtkComboBox *() const { return GTK_COMBO_BOX(Obj()); }

            ComboBoxText() : ComboBox(DerivedType()) {
                Init(gtk_combo_box_new_text());
                Internal(true);
            }
            void Append(const std::string &text) {
                gtk_combo_box_append_text(*this, text.c_str());
            }
            void Prepend(const std::string &text) {
                gtk_combo_box_prepend_text(*this, text.c_str());
            }
            void Insert(const std::string &text, int position) {
                gtk_combo_box_insert_text(*this, position, text.c_str());
            }
            void Remove(int row) { gtk_combo_box_remove_text(*this, row); }

            std::string ActiveText() { 
                std::string res;
                if (gchar *c = gtk_combo_box_get_active_text(*this)) {
                    res = c; 
                    g_free(c);
                }
                return res;
            }
            virtual void SetActiveIter(const TreeIter &it) { 
                throw std::runtime_error("Iter not available on ComboBoxText items!");
            }
            virtual bool GetActiveIter(TreeIter &it) {
                throw std::runtime_error("Iter not available on ComboBoxText items!");
            }
    };
}

#endif
