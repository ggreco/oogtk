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
            bool operator==(const TreePath &p) { return gtk_tree_path_compare(*this, p) == 0; }
            bool operator!=(const TreePath &p) { return gtk_tree_path_compare(*this, p) != 0; }

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
/** The selection object for TreeView.
The TreeSelection object is a helper object to manage the selection for a TreeView widget. The TreeSelection object is automatically created when a new TreeView widget is created, and cannot exist independentally of this widget. The primary reason the TreeSelection objects exists is for cleanliness of code and API. That is, there is no conceptual reason all these functions could not be methods on the TreeView widget instead of a separate function.

The TreeSelection object is gotten from a TreeView by calling TreeView::Selection(). It can be manipulated to check the selection status of the tree, as well as select and deselect individual rows. Selection is done completely view side. As a result, multiple views of the same model can have completely different selections. Additionally, you cannot change the selection of a row on the model that is not currently displayed by the view without expanding its parents first.

One of the important things to remember when monitoring the selection of a view is that the "changed" signal is mostly a hint. That is, it may only emit one signal when a range of rows is selected. Additionally, it may on occasion emit a ::changed signal when nothing has happened (mostly as a result of programmers calling select_row on an already selected row). 
*/
    class TreeSelection : public Object
    {
         public:
/// DOXYS_OFF             
            operator  GtkTreeSelection *() const { return GTK_TREE_SELECTION(obj_); }
            TreeSelection(GObject *o) { Init(o); }
/// DOXYS_ON
            /** Gets the selection mode for selection.
\sa TreeSelection::Mode(OneOf<SelectionMode,GtkSelectionMode>)
\return the current selection mode 
            */
            OneOf<SelectionMode,GtkSelectionMode> Mode() const { return gtk_tree_selection_get_mode(*this); }
            /** Sets the selection mode of the selection. 
If the previous type was SelectionMultiple, then the anchor is kept selected, if it was previously selected.
            */
            void Mode(OneOf<SelectionMode,GtkSelectionMode> mode /**< a SelectionMode valid value or a GtkSelectionMode value */) { gtk_tree_selection_set_mode(*this, mode); }

            /** Get a reference to the TreeView this TreeSelection is associated to. 
            \return a reference to the TreeView object paired with this TreeSelection.
            */
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

    /** The tree interface used by TreeView.

The TreeModel interface defines a generic tree interface for use by the TreeView widget. It is an abstract interface, and is designed to be usable with any appropriate data structure. The programmer just has to implement this interface on their own data type for it to be viewable by a TreeView widget.

The model is represented as a hierarchical tree of strongly-typed, columned data. In other words, the model can be seen as a tree where every node has different values depending on which column is being queried. The type of data found in a column is determined by using the GType system (ie. G_TYPE_INT, GTK_TYPE_BUTTON, G_TYPE_POINTER, etc.). The types are homogeneous per column across all nodes. It is important to note that this interface only provides a way of examining a model and observing changes. The implementation of each individual model decides how and if changes are made.

In order to make life simpler for programmers who do not need to write their own specialized model, two generic models are provided — the TreeStore and the ListStore. To use these, the developer simply pushes data into these models as necessary. These models provide the data structure as well as all appropriate tree interfaces. As a result, implementing drag and drop, sorting, and storing data is trivial. For the vast majority of trees and lists, these two models are sufficient.

Models are accessed on a node/column level of granularity. One can query for the value of a model at a certain node and a certain column on that node. There are two structures used to reference a particular node in a model. They are the TreePath and the TreeIte. Most of the interface consists of operations on a TreeIter.

A path is essentially a potential node. It is a location on a model that may or may not actually correspond to a node on a specific model. The TreePath struct can be converted into either an array of unsigned integers or a string. The string form is a list of numbers separated by a colon. Each number refers to the offset at that level. Thus, the path “0” refers to the root node and the path “2:4” refers to the fifth child of the third node.

By contrast, a TreeIter is a reference to a specific node on a specific model. It is a generic struct with an integer and three generic pointers. These are filled in by the model in a model-specific way. One can convert a path to an iterator by calling TreeModel::Get(TreeIter &, const TreePath &). These iterators are the primary way of accessing a model and are similar to the iterators used by TextBuffer. They are generally statically allocated on the stack and only used for a short time. The model interface defines a set of operations using them for navigating the model.

It is expected that models fill in the iterator with private data. For example, the ListStore model, which is internally a simple linked list, stores a list node in one of the pointers. The TreeModelSort stores an array and an offset in two of the pointers. Additionally, there is an integer field. This field is generally filled with a unique stamp per model. This stamp is for catching errors resulting from using invalid iterators with a model.

The lifecycle of an iterator can be a little confusing at first. Iterators are expected to always be valid for as long as the model is unchanged (and doesn't emit a signal). The model is considered to own all outstanding iterators and nothing needs to be done to free them from the user's point of view. Additionally, some models guarantee that an iterator is valid for as long as the node it refers to is valid (most notably the TreeStore and ListStore). Although generally uninteresting, as one always has to allow for the case where iterators do not persist beyond a signal, some very important performance enhancements were made in the sort model. As a result, the GTK_TREE_MODEL_ITERS_PERSIST flag was added to indicate this behavior.

To help show some common operation of a model, some examples are provided. The first example shows three ways of getting the iter at the location “3:2:5”. While the first method shown is easier, the second is much more common, as you often get paths from callbacks. 
*/
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
            /** Sets iter to a valid iterator pointing to path_string, if it exists. 
Otherwise, iter is left invalid and false is returned.
\return true if there is a valid TreeIter associated with the specified path, false otherwise.
*/
            bool Get(TreeIter &iter /**< A TreeIter to be set */, 
                     const std::string &path /**< A TreePath string, ie: "0:0:1" */) {
                return gtk_tree_model_get_iter_from_string(*this, &iter, path.c_str());
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
            /** Returns the type of the column.
            \return The type of the column. 
             */
            GType ColumnType(int index /**< The column index. */) const {
                return gtk_tree_model_get_column_type(*this, index);
            }
            /// Returns the number of columns supported by this TreeModel.
            /// \return The number of columns. 
            int Columns() const {
                return gtk_tree_model_get_n_columns(*this);
            }
            /** Returns a TreePath referenced by iter.
            \return a TreePath.
            */
            TreePath Path(const TreeIter &it /**< a valid iterator for this TreeModel */) {
                return TreePath(gtk_tree_model_get_path(*this, const_cast<TreeIter *>(&it)));
            }
            /** Sets iter to a valid iterator pointing to path. 
            \return true if there is a valid TreeIter in this TreeModel for the specified path, false otherwise.
            */
            bool Iter(TreeIter &iter /**< An uninitialized TreeIter */, 
                      const TreePath &path /**< A TreePath */) {
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
                        const_cast<TreeIter *>(&it), idx, value, -1);
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
                Init(gtk_tree_store_newv(types.size(), (GType *)&types[0]));
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
            class AbstractEdited {
                    Object &obj_;
                    int id_;
                    static void real_cbk(GtkCellRendererText *cell, gchar *path_string, 
                                         gchar *new_text, AbstractEdited *base) {
                        if (base) base->notify(path_string, new_text);
                    }
                public:
                    AbstractEdited(Object &obj) : obj_(obj) { id_ = g_signal_connect(obj_.Obj(), "edited", GCallback(real_cbk), this); }
                    virtual ~AbstractEdited() { g_signal_handler_disconnect(obj_.Obj(), id_); }
                    virtual void notify(const char *, const char *) const  = 0;
            };
            template <typename T>
            class EditedCbk : public AbstractEdited {
                    T*myObj;
                    void (T::*myFnc)(const std::string &, const std::string &);
                public:
                    EditedCbk(Object &w, T*obj, void (T::*fnc)(const std::string &, const std::string &)) :
                        AbstractEdited(w), myObj(obj), myFnc(fnc) {}
                    void notify(const char *a, const char *b) const { (myObj->*myFnc)(a, b); }
            };

            AbstractEdited *edited_;
        public:
            CellRendererText(GObject *obj) : edited_(NULL) { Init(obj); }
            CellRendererText() : edited_(NULL) { 
                Init(gtk_cell_renderer_text_new()); 
                Internal(true);
            }
            ~CellRendererText() { if (edited_) delete edited_; }
            template <typename T>
            void OnEdited(void (T::*cbk)(const std::string &, const std::string &), T* base) {
                if (edited_) delete edited_;
                edited_ = new EditedCbk<T>(*this, base, cbk);
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

    /// A list of CellRendererer, used by TreeViewColumn::GetRenderers, may be empty.
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


    /// A list of pointer to TreeViewColumn objects, returned by a some TreeView functions.
    typedef std::list<TreeViewColumn *> ColumnList;

    /** A widget for displaying both trees and lists
Widget that displays any object that implements the TreeModel interface.

Please refer to the tree widget conceptual overview for an overview of all the objects and data types related to the tree widget and how they work together.

Several different coordinate systems are exposed in the TreeView API. These are: 

\image docpics/tree-view-coordinates.png "TreeView coordinate systems."
#Widget coordinates# -- coordinates relative to the widget (usually widget->window.
#Bin window coordinates# -- coordinates relative to the window that GtkTreeView renders to.
#Tree coordinates# -- coordinates relative to the entire scrollable area of GtkTreeView. These coordinates start at (0, 0) for row 0 of the tree.

Several functions are available for converting between the different coordinate systems. The most common translations are between widget and bin window coordinates and between bin window and tree coordinates. For the former you can use gtk_tree_view_convert_widget_to_bin_window_coords() (and vice versa), for the latter gtk_tree_view_convert_bin_window_to_tree_coords() (and vice versa). 
     */
    class TreeView : public Container
    {
        public:
/// DOXYS_OFF            
            operator  GtkTreeView *() const { return GTK_TREE_VIEW(Obj()); }

            TreeView(GObject *obj) { Init(obj); }
/// DOXYS_ON
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

            /** Add a new, editable, text column to the TreeView.

This API appends a new TreeViewColumn to the TreeView, the column is in text format with Pango markup enabled and can be made editable on cell bases specifying an edit_id boolean column of the associated TreeModel to select if a cell is editable or not or, if you don't specify edit_id, all the cells of this column will be editable.
\return a pointer to the newly created TreeViewColumn or NULL if some problem occurred.
*/
            TreeViewColumn *AddEditableColumn(const std::string &title /**< Title for the column */, 
                                              int id /**< The ID of the column where to retrieve this column text in the associated TreeModel */, 
                                              int edit_id = -1 /**< The optional ID of the column in the TreeModel associated to this TreeView that defines if the text is editable or not on cell basis, if not specified defaults to -1 that means that every cell of this column is editable. */) {
                CellRendererText r;
                int col;

                if (edit_id != -1) {
                    col = gtk_tree_view_insert_column_with_attributes(*this, -1,
                            title.c_str(), r, "markup", id,
                            "editable", edit_id,
                            NULL);
                }
                else {
                    col = gtk_tree_view_insert_column_with_attributes(*this, -1,
                            title.c_str(), r, "markup", id,
                            NULL);
                    r.Set("editable", true);
                }

                return Get(col - 1);
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
            TreeViewColumn *AddStockColumn(const std::string &title, int id) {
                CellRendererPixbuf r;
                int col = gtk_tree_view_insert_column_with_attributes(*this, -1,
                        title.c_str(), r, "stock-id", id,
                        NULL);
            
                return Get(col - 1);
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
            /// Expands the row at path. This will also expand all parent rows of path as necessary.
            void Expand(const TreePath &path /**< path to a row. */) { gtk_tree_view_expand_to_path(*this, path); }
            /// Recursively expands all nodes in the TreeView.
            void ExpandAll() { gtk_tree_view_expand_all(*this); }
            /// Recursively collapses all visible, expanded nodes in tree_view.
            void CollapseAll() { gtk_tree_view_collapse_all(*this); }
            /// Opens the row so its children are visible.
            void ExpandRow(const TreePath &path /**< path to a row */, 
                           bool recursive = true /**< whether to recursively expand, or just expand immediate children, defaults to expand recursively. */) { gtk_tree_view_expand_row(*this, path, recursive); }
            void CollapseRow(const TreePath &path) { gtk_tree_view_collapse_row(*this, path); }
            void ScrollTo(int x, int y) { gtk_tree_view_scroll_to_point(*this, x, y); }
            void ScrollTo(const Point &point) { ScrollTo(point.x, point.y); }

            /** Returns the selection associated with this TreeView.
            A TreeSelection is a convenient wrapper around a subset of TreeView methods
            related to selection handling, every TreeView has one TreeSelection, so this
            function returns a reference to it.
            \return A reference to this TreeView TreeSelection.
            */
            TreeSelection &Selection() { // a treeview always has his TreeSelection
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

            /// Converts widget coordinates to coordinates for the bin_window
            /// \return a Point containing X,Y Bin widget coordinates.
            Point WidgetToBin(const Point &widget_coords /**< X,Y coordinates relative to widget */
                    ) {
                Point dest;
                gtk_tree_view_convert_widget_to_bin_window_coords(*this, widget_coords.x, widget_coords.y,
                                                           &dest.x, &dest.y);
                return dest;
            }
            /// Converts widget coordinates to coordinates for the tree (the full scrollable area of the tree).
            /// \return a Point containing X,Y tree coordinates.
            Point WidgetToTree(const Point &widget_coords /**< X,Y coordinates relative to widget */
                    ) {
                Point dest;
                gtk_tree_view_convert_widget_to_tree_coords(*this, widget_coords.x, widget_coords.y,
                                                           &dest.x, &dest.y);
                return dest;
            }
            /// Converts tree coordinates (coordinates in full scrollable area of the tree) to widget coordinates.
            /// \return a Point containing X,Y widget coordinates.
            Point TreeToWidget(const Point &tree_coords /**< X,Y coordinates relative to tree */
                    ) {
                Point dest;
                gtk_tree_view_convert_tree_to_widget_coords(*this, tree_coords.x, tree_coords.y,
                                                           &dest.x, &dest.y);
                return dest;
            }
            /// Converts tree coordinates (coordinates in full scrollable area of the tree) to bin_window coordinates.
            /// \return a Point containing X,Y bin widget coordinates.
            Point TreeToBin(const Point &tree_coords /**< X,Y coordinates relative to tree */
                    ) {
                Point dest;
                gtk_tree_view_convert_tree_to_bin_window_coords(*this, tree_coords.x, tree_coords.y,
                                                           &dest.x, &dest.y);
                return dest;
            }
            /// Converts bin_window coordinates to widget relative coordinates.
            /// \return a Point containing X,Y widget coordinates.
            Point BinToWidget(const Point &bin_coords /**< X,Y coordinates relative to bin widget */
                    ) {
                Point dest;
                gtk_tree_view_convert_bin_window_to_widget_coords(*this, bin_coords.x, bin_coords.y,
                                                           &dest.x, &dest.y);
                return dest;
            }
            /// Converts bin_window coordinates for the tree (the full scrollable area of the tree).
            /// \return a Point containing X,Y tree coordinates.
            Point BinToTree(const Point &bin_coords /**< X,Y coordinates relative to bin widget */
                    ) {
                Point dest;
                gtk_tree_view_convert_bin_window_to_tree_coords(*this, bin_coords.x, bin_coords.y,
                                                           &dest.x, &dest.y);
                return dest;
            }            
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

    /** CellLayout is an interface to be implemented by all objects which want to provide a GtkTreeViewColumn-like API for packing cells, setting attributes and data funcs.

One of the notable features provided by implementations of CellLayout are attributes. Attributes let you set the properties in flexible ways. They can just be set to constant values like regular properties. But they can also be mapped to a column of the underlying tree model with CellLayout::Attributes(), which means that the value of the attribute can change from cell to cell as they are rendered by the cell renderer. Finally, it is possible to specify a function with CellLayout::CellDataFunc() that is called to determine the value of the attribute for each cell that is rendered.
*/
    class CellLayout
    {
        protected:
            virtual GtkCellLayout *getobj() const = 0;
        public:
            /** Packs the cell into the beginning of cell_layout.
If expand is false, then the cell is allocated no more space than it needs. Any unused space is divided evenly between cells for which expand is true, the default value of expand is false.

Note that reusing the same cell renderer is not supported.
*/
            void PackStart(CellRenderer &r /**< The cell renderer to pack at the start of this cell */, 
                           bool expand = false /**< if cell is to be given extra space allocated to cell_layout */) {
                gtk_cell_layout_pack_start(getobj(), r, expand);
            }
            /** Adds the cell to the end of cell_layout.
            \sa CellLayout::PackStart
             */
            void PackEnd(CellRenderer &r /**< The cell renderer to pack at the start of this cell */, 
                         bool expand = false /**< if cell is to be given extra space allocated to cell_layout */) {
                gtk_cell_layout_pack_end(getobj(), r, expand);
            }
/** Returns the cell renderers which have been added to cell_layout.
 */
            void GetRenderers(RendererList &objs /**< An application allocated RenderList */) {
                GList *list = gtk_cell_layout_get_cells(getobj()), *l;

                l = list;

                while (l) {
                    if (CellRenderer *o = dynamic_cast<CellRenderer *>(Object::Find((GObject *)l->data)))
                        objs.push_back(o);

                    l = l->next;
                }

                g_list_free(list);
            }
/** Adds an attribute mapping to the list in cell_layout. 

The column is the column of the model to get a value from, and the attribute is the parameter on cell to be set from the value. So for example if column 2 of the model contains strings, you could have the "text" attribute of a GtkCellRendererText get its values from column 2.
*/
            void AddAttribute(CellRenderer &r /**< A previously layout added renderer (with CellLayout::PackStart or CellLayout::PackEnd)*/,
                              const char *attribute /**< An attribute on the renderer.  */, 
                              int column /**< The column of the model to assign this attribute to */) {
                gtk_cell_layout_add_attribute(getobj(), r, attribute, column); 
            }
/** Unsets all the mappings on all renderers on cell_layout and removes all renderers from cell_layout. */            
            void Clear() { gtk_cell_layout_clear( getobj()); } 
/** Clears all existing attributes previously set.
This function clears all existing attributes previously set with CellLayout::AddAttribute for 
a particular renderer associated to this object.

\sa CellLayout::AddAttribute()
*/
            void ClearAttributes(CellRenderer &r /**< A previously layout added renderer */) {
                gtk_cell_layout_clear_attributes(getobj(), r);
            }
    };

    class ComboBox : public Bin, public CellLayout
    {
        protected:
            GtkCellLayout *getobj() const { return GTK_CELL_LAYOUT(Obj()); }
        public:
/// DOXYS_OFF             
            operator  GtkComboBox *() const { return GTK_COMBO_BOX(Obj()); }
            operator  GtkCellLayout *() const { return GTK_CELL_LAYOUT(Obj()); }

            ComboBox(const DerivedType &) {}
            ComboBox(GObject *obj) { Init(obj); }
/// DOXYS_ON             

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

            void AddTextColumn(int id, bool expand = true) {
                CellRendererText txt;
                PackStart(txt, expand);
                AddAttribute(txt, "markup", id);
            }
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
/// DOXYS_OFF             
            operator  GtkComboBox *() const { return GTK_COMBO_BOX(Obj()); }
/// DOXYS_ON

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
