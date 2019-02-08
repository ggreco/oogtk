#ifndef OOTREE_H
#define OOTREE_H

#include "oogtk.h"
#include <vector>
#include <list>
#include <stdarg.h>

namespace gtk {
    /**
      The TreeIter is the primary structure for accessing a tree/liststore.

      Models are expected to put a unique integer in the stamp member, and put model-specific data in the three user_data members, all these datas are opaque to the user.
    */
    typedef GtkTreeIter TreeIter;

    struct ValidIter : public TreeIter {
        operator  GtkTreeIter *() { return dynamic_cast<GtkTreeIter *>(this); }
        ValidIter() : valid(false) {}
        bool valid;
        bool IsValid() { return valid; }
    };

    /// Identifies how the user can interact with a particular cell.
    enum CellRendererMode {
        CellRendererModeInert = GTK_CELL_RENDERER_MODE_INERT /**< The cell is just for display and cannot be interacted with. Note that this doesn't mean that eg. the row being drawn can't be selected -- just that a particular element of it cannot be individually modified.  */,
        CellRendererModeActivatable = GTK_CELL_RENDERER_MODE_ACTIVATABLE /**< The cell can be clicked.  */,
        CellRendererModeEditable = GTK_CELL_RENDERER_MODE_EDITABLE /**< The cell can be edited or otherwise modified.  */
    };

    /// Used to control what selections users are allowed to make.
    enum SelectionMode {
        SelectionNone = GTK_SELECTION_NONE /**< No selection is possible. */,
        SelectionSingle = GTK_SELECTION_SINGLE /**< Zero or one element may be selected. A click on an unselected item selects that one and unselect the current selection. */,
        SelectionBrowse = GTK_SELECTION_BROWSE /**< Exactly one element is selected. In some circumstances, such as initially or during a search operation, it's possible for no element to be selected with SelectionBrowse. What is really enforced is that the user can't deselect a currently selected element except by selecting another element. */,
        SelectionMultiple = GTK_SELECTION_MULTIPLE /**< Any number of elements may be selected. Clicks toggle the state of an item. Any number of elements may be selected. The Ctrl key may be used to enlarge the selection, and Shift key to select between the focus and the child pointed to. Some widgets may also allow Click-drag to select a range of elements. */
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

    /// A list of TreeRowReference, useful for TreeStore/ListStore operations.
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
\sa TreeSelection::Mode(SelectionMode)
\return the current selection mode
            */
            SelectionMode Mode() const { return (SelectionMode)gtk_tree_selection_get_mode(*this); }
            /** Sets the selection mode of the selection.
If the previous type was SelectionMultiple, then the anchor is kept selected, if it was previously selected.
            */
            void Mode(SelectionMode mode /**< a SelectionMode valid value or a GtkSelectionMode value */) { gtk_tree_selection_set_mode(*this, (GtkSelectionMode)mode); }

            /** Get a reference to the TreeView this TreeSelection is associated to.
            \return a reference to the TreeView object paired with this TreeSelection.
            */
            gtk::TreeView &TreeView();

            /// Returns the number of rows that have been selected in tree.
            int Count() const {
                return gtk_tree_selection_count_selected_rows(*this);
            }

            /// Select the row at path.
            void Select(const TreePath &path /**< The TreePath to be selected. */) { gtk_tree_selection_select_path(*this, path); }
            /// Unselects the row at path.
            void Unselect(const TreePath &path /**< The TreePath to be unselected. */) { gtk_tree_selection_unselect_path(*this, path); }
            /// Returns true if the row pointed to by path is currently selected. If path does not point to a valid location, false is returned
            bool IsSelected(const TreePath &path /**< A TreePath to check selection on */) const { return gtk_tree_selection_path_is_selected(*this, path); }
            /// Selects the specified iterator.
            void Select(const TreeIter &it /**< The TreeIter to be selected */ ) { gtk_tree_selection_select_iter(*this, const_cast<TreeIter *>(&it)); }
            /// Unselects the specified iterator.
            void Unselect(const TreeIter &it /**< The TreeIter to be unselected */) { gtk_tree_selection_unselect_iter(*this, const_cast<TreeIter *>(&it)); }
            /// Returns true if the row at iter is currently selected.
            bool IsSelected(const TreeIter &it /**< A valid TreeIter */) const { return gtk_tree_selection_iter_is_selected(*this, const_cast<TreeIter *>(&it)); }

            /// Selects all the nodes. selection must be set to SelectionMultiple mode.
            void SelectAll() { gtk_tree_selection_select_all(*this); }
            /// Unselects all the nodes.
            void UnselectAll() { gtk_tree_selection_unselect_all(*this); }

            /// Selects a range of nodes, determined by start_path and end_path inclusive. selection must be set to SelectionMultiple mode.
            void SelectRange(const TreePath &begin /**< The initial node of the range. */,
                             const TreePath &end /**< The final node of the range. */) {
                gtk_tree_selection_select_range(*this, begin, end);
            }
            /// Unselects a range of nodes, determined by start_path and end_path inclusive.
            void UnselectRange(const TreePath &begin /**< The initial node of the range. */,
                             const TreePath &end /**< The final node of the range. */) {
                gtk_tree_selection_unselect_range(*this, begin, end);
            }

            //// Returns a RefVec containing a TreeRowReference for every line in the TreeSelection
            RefVec SelectedRows();

            /// Callback to call if the user change the state of the TreeSelection
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
            void GetValue(const TreeIter &it, int idx, gdouble &value) {
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
                if (field) {
                    value = field;
                    g_free(field);
                }
                else
                    value.clear();
            }
            void GetValue(const TreeIter &it, int idx, long long &value) {
                gtk_tree_model_get(*this,
                        const_cast<TreeIter *>(&it), idx, &value, -1);
            }

            void Get(TreeIter it, ...) {
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
            virtual void Set(TreeIter it, ...) = 0;
            virtual bool IsValid(const TreeIter &it) const = 0;
            virtual void SetValue(const TreeIter &it, int idx, int value) = 0;
            virtual void SetValue(const TreeIter &it, int idx, const std::string &value) = 0;
            virtual void SetValue(const TreeIter &it, int idx, void *value) = 0;
            virtual void SetValue(const TreeIter &it, int idx, bool value) = 0;

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
            TreePath Path(const TreeIter &it /**< a valid iterator for this TreeModel */) const {
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
            void Set(TreeIter it, ...) {
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
            void SetValue(const TreeIter &it, int idx, bool value) {
                gtk_list_store_set(*this,
                        const_cast<TreeIter *>(&it), idx, value ? TRUE : FALSE, -1);
            }

            template <typename T>
            void AddTail(gint col, T v, ...) {
                TreeIter it;
                gtk_list_store_append(*this, &it);
                gtk_list_store_set(*this, &it, col, v, -1);
                va_list va;
                va_start(va, v);
                gtk_list_store_set_valist(*this, &it, va);
                va_end(va);
            }
            template <typename T>
            void AddFront(gint col, T v, ...) {
                TreeIter it;
                gtk_list_store_prepend(*this, &it);
                gtk_list_store_set(*this, &it, col, v, -1);
                va_list va;
                va_start(va, v);
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
            void Set(TreeIter it, ...) {
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
            void SetValue(const TreeIter &it, int idx, bool value) {
                gtk_tree_store_set(*this,
                        const_cast<TreeIter *>(&it), idx, value ? TRUE : FALSE, -1);
            }

            TreeIter AddFront(TreeIter parent, ...) {
                TreeIter it;
                gtk_tree_store_prepend(*this, &it, const_cast<TreeIter *>(&parent));
                va_list va;
                va_start(va, parent);
                gtk_tree_store_set_valist(*this, &it, va);
                va_end(va);

                return it;
            }

            TreeIter AddTail(TreeIter parent, ...) {
                TreeIter it;
                gtk_tree_store_append(*this, &it, const_cast<TreeIter *>(&parent));
                va_list va;
                va_start(va, parent);
                gtk_tree_store_set_valist(*this, &it, va);
                va_end(va);

                return it;
            }
            template <typename T>
            TreeIter AddFront(gint col, T v, ...) {
                TreeIter it;
                gtk_tree_store_prepend(*this, &it, NULL);
                gtk_tree_store_set(*this, &it, col, v, -1);
                va_list va;
                va_start(va, v);
                gtk_tree_store_set_valist(*this, &it, va);
                va_end(va);

                return it;
            }

            template <typename T>
            TreeIter AddTail(gint col, T v, ...) {
                TreeIter it;
                gtk_tree_store_append(*this, &it, NULL);
                gtk_tree_store_set(*this, &it, col, v, -1);
                va_list va;
                va_start(va, v);
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
/// DOXYS_OFF
            friend class TreeSelection;
            friend class IconView;
            // only for internal use
            TreeRowReference(GtkTreeModel *model, GtkTreePath *path) {
                obj_ = gtk_tree_row_reference_new(model, path);
            }
/// DOXYS_ON
        public:
            operator  GtkTreeRowReference *() const { return obj_; }
/** Creates a row reference based on path.
This reference will keep pointing to the node pointed to by path, so long as it exists. It listens to all signals emitted by model, and updates its path appropriately. If path isn't a valid path in model, then an invalid path is created, you can check this with TreeRowReference::Valid().
*/
            TreeRowReference(const TreeModel &model, const TreePath &path) {
                obj_ = gtk_tree_row_reference_new(model, path);
            }
            // Creates a new TreeRowReference from an existing one
            TreeRowReference(const TreeRowReference &src) {
                obj_ = gtk_tree_row_reference_copy(src);
            }
            /// Returns the model that the row reference is monitoring.
            TreeModel &Model() const {
                if (TreeModel *m = dynamic_cast<TreeModel *>(
                            Object::Find((GObject *)gtk_tree_row_reference_get_model(obj_))))
                    return *m;
                else
                    throw std::runtime_error("TreeRowReference without Model!!!");
            }
            // Returns a TreeIter to the row the object currenttly points to, use TreeRowReference::Valid before this call if your code can bring here with an invalid item.
            TreeIter Iter() const {
                if (GtkTreePath *p = gtk_tree_row_reference_get_path(obj_)) {
                    TreeIter it;
                    if (gtk_tree_model_get_iter(gtk_tree_row_reference_get_model(obj_),
                                                &it, p))
                        return it;
                }
                throw std::runtime_error("Unable to get the Iter from the reference!");
            }
            /// Returns a path that the row reference currently points to, use TreeRowReference::Valid to check this object exists before calling this function.
            TreePath Path() const {
                return TreePath(gtk_tree_row_reference_get_path(obj_));
            }
            /// Returns true if the reference refers to a current valid path.
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

/** An object for rendering a single cell on a Drawable

The CellRenderer is a base class of a set of objects used for rendering a cell to a Drawable. These objects are used primarily by the TreeView widget, though they aren't tied to them in any specific way. It is worth noting that CellRenderer is not a Widget and cannot be treated as such.

The primary use of a CellRenderer is for drawing a certain graphical elements on a Drawable. Typically, one cell renderer is used to draw many cells on the screen. To this extent, it isn't expected that a CellRenderer keep any permanent state around. Instead, any state is set just prior to use using GObjects property system. Then, the cell is measured using CellRenderer::Size(). Finally, the cell is rendered in the correct location using CellRenderer::Render().

There are a number of rules that must be followed when writing a new CellRenderer. First and formost, it's important that a certain set of properties will always yield a cell renderer of the same size, barring a Style change. The CellRenderer also has a number of generic properties that are expected to be honored by all children.

Beyond merely rendering a cell, cell renderers can optionally provide active user interface elements. A cell renderer can be activatable like CellRendererToggle, which toggles when it gets activated by a mouse click, or it can be editable like CellRendererText, which allows the user to edit the text using a Entry. To make a cell renderer activatable or editable, you have to implement the activate or start_editing virtual functions, respectively.
*/
    class CellRenderer : public Object
    {
        public:
/// DOXYS_OFF
            operator  GtkCellRenderer *() const { return GTK_CELL_RENDERER(obj_); }
/// DOXYS_ON
/** Obtains the width and height needed to render the cell.

Used by view widgets to determine the appropriate size for the cell_area passed to CellRenderer::Render(). If cell_area is specified, the method will fill in the x and y offsets (if set) of the cell relative to this location.

\\
*/
        Rect Size(Widget &widget /**< the widget the renderer is rendering to */,
                  Rect   *cell_area = NULL /**< Optional area where the cell will be allocated */) {
            gint x = 0, y = 0, w, h;
            gtk_cell_renderer_get_size(*this, widget, cell_area,
                                       &x, &y, &w, &h);
            return Rect(x,y,w,h);
        }
    };
/** Renders text in a cell

A CellRendererText renders a given text in its cell, using the font, color and style information provided by its properties. The text will be ellipsized if it is too long and the ellipsize property allows it.

If the mode is CellRendererModeEditable, the CellRendererText allows to edit its text using an entry.
*/
    class CellRendererText : public CellRenderer
    {
/// DOXYS_OFF
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
            ~CellRendererText() { if (edited_) delete edited_; }
/// DOXYS_ON
/** Creates a new CellRendererText.

Adjust how text is drawn using object properties. Object properties can be set globally (with Object::Set()). Also, with TreeViewColumn, you can bind a property to a value in a TreeModel. For example, you can bind the "text" property on the cell renderer to a string value in the model, thus rendering a different string in each row of the TreeView
*/
            CellRendererText() : edited_(NULL) {
                Init(gtk_cell_renderer_text_new());
                Internal(true);
            }
            /// Callback to be called if the cell is edited
            template <typename T>
            void OnEdited(void (T::*cbk)(const std::string &, const std::string &), T* base) {
                if (edited_) delete edited_;
                edited_ = new EditedCbk<T>(*this, base, cbk);
            }
    };

    class CellRendererProgress : public CellRenderer
    {
        public:
            CellRendererProgress(GObject *obj) { Init(obj); }
            CellRendererProgress() {
                Init(gtk_cell_renderer_progress_new());
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
            class AbstractToggled {
                    Object &obj_;
                    int id_;
                    static void real_cbk(GtkCellRendererToggle *cell, gchar *path_string, AbstractToggled *base) {
                        if (base) base->notify(path_string);
                    }
                public:
                    AbstractToggled(Object &obj) : obj_(obj) { id_ = g_signal_connect(obj_.Obj(), "toggled", GCallback(real_cbk), this); }
                    virtual ~AbstractToggled() { g_signal_handler_disconnect(obj_.Obj(), id_); }
                    virtual void notify(const char *) const  = 0;
            };
            template <typename T>
            class ToggledCbk : public AbstractToggled {
                    T*myObj;
                    void (T::*myFnc)(const std::string &);
                public:
                    ToggledCbk(Object &w, T*obj, void (T::*fnc)(const std::string &)) :
                        AbstractToggled(w), myObj(obj), myFnc(fnc) {}
                    void notify(const char *a) const { (myObj->*myFnc)(a); }
            };

            AbstractToggled *toggled_;
        public:
            operator  GtkCellRendererToggle *() const { return GTK_CELL_RENDERER_TOGGLE(obj_); }

            CellRendererToggle(GObject *obj) : toggled_(NULL) { Init(obj); }
            CellRendererToggle() : toggled_(NULL) {
                Init(gtk_cell_renderer_toggle_new());
                Internal(true);
            }
            ~CellRendererToggle() { if (toggled_) delete toggled_; }

            void Radio(bool flag) { gtk_cell_renderer_toggle_set_radio(*this, flag); }
            bool Radio() const { return gtk_cell_renderer_toggle_get_radio(*this); }
            void Active(bool flag) { gtk_cell_renderer_toggle_set_active(*this, flag); }
            bool Active() const { return gtk_cell_renderer_toggle_get_active(*this); }
/// Callback to be called if the cell is edited
            template <typename T>
            void OnToggled(void (T::*cbk)(const std::string &), T* base) {
                if (toggled_) delete toggled_;
                toggled_ = new ToggledCbk<T>(*this, base, cbk);
            }
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
            /// The sizing method the column uses to determine its width. Please note that TreeViewColumn::Autosize are inefficient for large views, and can make columns appear choppy.
            typedef enum {
                GrowOnly = GTK_TREE_VIEW_COLUMN_GROW_ONLY /**< Columns only get bigger in reaction to changes in the model */,
                Autosize = GTK_TREE_VIEW_COLUMN_AUTOSIZE /**< Columns resize to be the optimal size everytime the model changes */,
                Fixed = GTK_TREE_VIEW_COLUMN_FIXED /**< Columns are a fixed numbers of pixels wide. */
            } SizingMode;

            operator  GtkTreeViewColumn *() const { return GTK_TREE_VIEW_COLUMN(obj_); }
            operator GtkCellLayout *() const { return GTK_CELL_LAYOUT(obj_); }
            TreeViewColumn(GObject *obj) { Init(obj); }

            /// Creates a new empty TreeViewColumn.
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
                GList *list = gtk_cell_layout_get_cells(*this), *l;

                l = list;

                while (l) {
                    if (CellRenderer *o = dynamic_cast<CellRenderer *>(Object::Find((GObject *)l->data)))
                        objs.push_back(o);

                    l = l->next;
                }

                g_list_free(list);
            }

            // get/set methods

            /// Returns true if tree_column is visible.
            bool Visible() const { return gtk_tree_view_column_get_visible(*this); }
            /// Sets the visibility of the tree column.
            void Visible(bool flag /**< whether the column is visible or not. If it is visible, then the tree will show the column. */) { gtk_tree_view_column_set_visible(*this, flag); }

            bool Clickable() const { return gtk_tree_view_column_get_clickable(*this); }
            void Clickable(bool flag) { gtk_tree_view_column_set_clickable(*this, flag); }

            bool Expand() const { return gtk_tree_view_column_get_expand(*this); }
            void Expand(bool flag) { gtk_tree_view_column_set_expand(*this, flag); }

            bool Resizable() const { return gtk_tree_view_column_get_resizable(*this); }
            void Resizable(bool flag) { gtk_tree_view_column_set_resizable(*this, flag); }

            /// Returns the spacing of tree_column.
            int Spacing() const { return gtk_tree_view_column_get_spacing(*this); }
            /// Sets the spacing field of tree_column, which is the number of pixels to place between cell renderers packed into it.
            void Spacing(int pixels /**< distance between cell renderers in pixels.*/) { gtk_tree_view_column_set_spacing(*this, pixels); }

            /// Gets the fixed width of the column. This value is only meaning may not be the actual width of the column on the screen, just what is requested.
            int FixedWidth() const { return gtk_tree_view_column_get_fixed_width(*this); }
/** Sets the size of the column in pixels.

This is meaningful only if the TreeViewColumn::SizingMode is TreeViewColumn::Fixed. The size of the column is clamped to the min/max width for the column. Please note that the min/max width of the column doesn't actually affect the "fixed_width" property of the widget, just the actual size when displayed.
 */
            void FixedWidth(int val /**< The size to set tree_column to. Must be greater than 0. */ ) { gtk_tree_view_column_set_fixed_width(*this, val); }

            /// Returns the current type of tree_column.
            SizingMode Sizing() const { return (SizingMode)gtk_tree_view_column_get_sizing(*this); }
            /// Sets the growth behavior of tree_column to type.
            void Sizing(SizingMode m) { gtk_tree_view_column_set_sizing(*this, (GtkTreeViewColumnSizing)m); }

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

            // Obtains the horizontal position and size of a cell in a column (or in the first cell of the column if cell is not passed). If the cell is not found in the column, or the column has no cell start_pos and width are not changed and false is returned.
            bool Position(int &start /**< return location for the horizontal position of cell within tree_column*/,
                          int &width /**<return location for the width of cell*/,
                          CellRenderer *cell = NULL /**<optional cell column */) {
                if (cell == NULL) {
                   GList *list = gtk_cell_layout_get_cells(*this);
                   if (!list) return false;
                   cell = dynamic_cast<CellRenderer *>(Object::Find((GObject *)list->data));
                   if (!cell) return false;
                   g_list_free(list);
                }
                return gtk_tree_view_column_cell_get_position(*this, *cell, &start, &width);
            }

            // some sorting related methods
            bool SortIndicator() const { return gtk_tree_view_column_get_sort_indicator(*this); }
            void SortIndicator(bool flag) { gtk_tree_view_column_set_sort_indicator(*this, flag); }

            gtk::SortType SortType() const { return (gtk::SortType)gtk_tree_view_column_get_sort_order(*this); }
            void SortType(gtk::SortType type) { gtk_tree_view_column_set_sort_order(*this, (GtkSortType)type); }

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

            /// Returns a pointer to the column actually displayed in the treeview at a particular
            /// x coordinate in pixels where 0 is the leftmost point of the widget.
            /// \return pointer to a TreeViewColumn or null if not found
            TreeViewColumn *ColumnAt(int x /**< X position inside the widget in pixels */) {
                int colx = 0;
                GtkTreeViewColumn *col = NULL;
                GList *columns = gtk_tree_view_get_columns(*this);

                for (GList *node = columns;  node != NULL && col == NULL;  node = node->next) {
                    GtkTreeViewColumn *checkcol = (GtkTreeViewColumn*) node->data;

                    if (x >= colx  &&  x < (colx + gtk_tree_view_column_get_width(checkcol)))
                        col = checkcol;
                    else
                        colx += gtk_tree_view_column_get_width(checkcol);
                }

                g_list_free(columns);
                if (col)
                    return dynamic_cast<TreeViewColumn*>(Object::Find((GObject *)col));

                return NULL;
            }
            TreeViewColumn *AddProgressColumn(const std::string &title, int id, int ptext = -1, int visibility_col = -1) {
                CellRendererProgress r;

                int col;

                if (ptext == -1) {
                    if (visibility_col == -1)
                        col = gtk_tree_view_insert_column_with_attributes(*this, -1,
                                 title.c_str(), r, "value", id,
                                 NULL);
                    else
                         col = gtk_tree_view_insert_column_with_attributes(*this, -1,
                                 title.c_str(), r, "value", id, "visible", visibility_col,
                                 NULL);
                } else {
                    if (visibility_col == -1)
                        col = gtk_tree_view_insert_column_with_attributes(*this, -1,
                                 title.c_str(), r, "value", id, "text", ptext,
                                 NULL);
                    else
                        col = gtk_tree_view_insert_column_with_attributes(*this, -1,
                                 title.c_str(), r, "value", id, "text", ptext, "visible", visibility_col,
                                 NULL);
                }
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
            TreeViewColumn *AddPixColumn(const std::string &title, int id, int hide_column = -1) {
                CellRendererPixbuf r;
                int col;

                if (hide_column == -1)
                    col = gtk_tree_view_insert_column_with_attributes(*this, -1,
                            title.c_str(), r, "pixbuf", id,
                            NULL);
                else
                    col = gtk_tree_view_insert_column_with_attributes(*this, -1,
                            title.c_str(), r, "pixbuf", id, "visible", hide_column,
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
            /// Resizes all columns to their optimal width. Only works after the treeview has been realized.
            void Autosize() {  gtk_tree_view_columns_autosize(*this);  }
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

            bool PathAt(TreePath &path, const Point &position, TreeViewColumn **c = NULL) {
                GtkTreePath *p;
                GtkTreeViewColumn *cc = NULL;
                if(gtk_tree_view_get_path_at_pos(*this, position.x, position.y,
                            &p, &cc, NULL, NULL)) {
                    path = *p;
                    gtk_tree_path_free(p);
                    if (c) {
                        if (cc)
                            *c = dynamic_cast<TreeViewColumn*>(Object::Find((GObject*)cc));
                        else
                            *c = NULL;
                    }
                    return true;
                }
                return false;
            }

            // DRAG & DROP Support

            /// Turns tree_view into a drop destination for automatic DND. Calling this method sets "reorderable" to FALSE.
            void EnableModelDragDest( const std::vector<TargetEntry> &targets, int32_t actions) {
                gtk_tree_view_enable_model_drag_dest(*this, &targets[0], targets.size(), (GdkDragAction)actions);
            }
            /// Turns tree_view into a drag source for automatic DND. Calling this method sets "reorderable" to FALSE.
            void EnableModelDragSource(int32_t mods, const std::vector<TargetEntry> &targets, int32_t actions) {                         gtk_tree_view_enable_model_drag_source(*this, (GdkModifierType)mods, &targets[0], targets.size(), (GdkDragAction)actions);
            }
            /// Undoes the effect of TreeView::EnableModelDragSource().
            void DisableModelDragSource() { gtk_tree_view_unset_rows_drag_source(*this); }
            /// Undoes the effect of TreeView::EnableModelDragDest().
            void DisableModelDragDest() { gtk_tree_view_unset_rows_drag_dest(*this); }

            // get/set methods
            bool HeadersClickable() const { return gtk_tree_view_get_headers_clickable(*this); }
            void HeadersClickable(bool flag) const { gtk_tree_view_set_headers_clickable(*this, flag); }
            bool HeadersVisible() const { return gtk_tree_view_get_headers_visible(*this); }
            void HeadersVisible(bool flag) const { gtk_tree_view_set_headers_visible(*this, flag); }

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
                if (GList *list = gtk_tree_selection_get_selected_rows(*this, &model)) {
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

    /** CellLayout is an interface to be implemented by all objects which want to provide a TreeViewColumn-like API for packing cells, setting attributes and data funcs.

One of the notable features provided by implementations of CellLayout are attributes. Attributes let you set the properties in flexible ways. They can just be set to constant values like regular properties. But they can also be mapped to a column of the underlying tree model with CellLayout::Attributes(), which means that the value of the attribute can change from cell to cell as they are rendered by the cell renderer. Finally, it is possible to specify a function with CellLayout::CellDataFunc() that is called to determine the value of the attribute for each cell that is rendered.
*/
    class CellLayout
    {
        protected:
            virtual ~CellLayout() {}
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

/**  A ComboBox is a widget that allows the user to choose from a list of valid choices. The ComboBox displays the selected choice. When activated, the ComboBox displays a popup which allows the user to make a new choice. The style in which the selected value is displayed, and the style of the popup is determined by the current theme. It may be similar to a OptionMenu, or similar to a Windows-style combo box.

Unlike its predecessors Combo and OptionMenu, the ComboBox uses the model-view pattern; the list of valid choices is specified in the form of a tree model, and the display of the choices can be adapted to the data in the model by using cell renderers, as you would in a tree view. This is possible since ComboBox implements the CellLayout interface. The tree model holding the valid choices is not restricted to a flat list, it can be a real tree, and the popup will reflect the tree structure.

In addition to the model-view API, ComboBox offers a simple API which is suitable for text-only combo boxes, and hides the complexity of managing the data in a model. It consists in the subclass ComboBoxText.
*/
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
/** Creates a new empty ComboBox.

The combobox must be populated with a TreeModel, via ComboBox::Model(const TreeModel &),
and how the model is viewed should be configured through the ComboBox CellLayout interface,
via CellLayout::PackStart and CellLayout::AddAttribute.

\sa ComboBox(const TreeModel &)
*/
            ComboBox() {
                Init(gtk_combo_box_new());
                Internal(true);
            }
/**  Creates a new ComboBox with the model initialized to model.
The model must implement TreeModel interface, to display your model contents
you'll need to attach to the ComboBox also one or more CellRenderer, the ComboBox
widget implements the CellLayout interface so, to attach a model to a widget, you'll
have to use CellLayout::PackStart or your CellRenderer, and connect it to the model
data using CellLayout::AddAttribute.

\example
void PopulateCombo(gtk::ComboBox &combo, const std::list<std::string> &items) {
    combo.Clear(); // clear previous renderers

    // create a simple ListStore with a single column
    gtk::ListStore store(1, G_TYPE_STRING);

    // create and setup a text renderer with markup support
    gtk::CellRendererText text;
    combo.PackStart(text, false);
    combo.AddAttribute(text, "markup", 0);

    for (std::list<std::string>::const_iterator it = items.begin();
                                                it != items.end(); ++it)
        store.AddTail(0, it->c_str(), -1);

    combo.Model(store);
}
\endexample
*/
            ComboBox(const TreeModel &model /**< A ListStore, TreeStore or a custom model that implements the TreeModel interface */ ) {
                Init(gtk_combo_box_new_with_model(model));
                Internal(true);
            }
/** Sets the model used by ComboBox to be model. Will unset a previously set model (if applicable).

\note that this function does not clear the cell renderers, you have to call CellLayout::Clear() yourself if you need to set up different cell renderers for the new model.

\sa  ComboBox(const TreeModel &)
*/
            void Model(const TreeModel &model /**< A ListStore, TreeStore or a custom model that implements the TreeModel interface */ ) { gtk_combo_box_set_model(*this, model); }
/** Returns the TreeModel which is acting as data source for the object.
\return A TreeModel which was passed during construction or through ComboBox::Model(const TreeModel&), NULL if no model has been set for this object.
*/
            TreeModel *Model() {
                return dynamic_cast<TreeModel *>(Object::Find((GObject *)
                        gtk_combo_box_get_model(*this)));
            }
/** Returns the index of the currently active item, or -1 if there's no active item.

If the model is a non-flat treemodel, and the active item is not an immediate child of the root of the tree, this function returns gtk_tree_path_get_indices (path)[0], where path is the TreePath of the active item.
*/
            int Active() const { return gtk_combo_box_get_active(*this); }
/** Sets the active item of the object to be the item at index. */
            void Active(int row /**< 	 An index in the model passed during construction, or -1 to have no active item */) { gtk_combo_box_set_active(*this, row); }
/** Pops up the menu or dropdown list of the object.

This function is mostly intended for use by accessibility technologies; applications should have little use for it.
*/
            void Popup() { gtk_combo_box_popup(*this); }
/** Hides the menu or dropdown list of the object.

This function is mostly intended for use by accessibility technologies; applications should have little use for it.
*/
            void Popdown() { gtk_combo_box_popdown(*this); }

            void AddTextColumn(int id, bool expand = true) {
                CellRendererText txt;
                PackStart(txt, expand);
                AddAttribute(txt, "markup", id);
            }
/** Returns the wrap width which is used to determine the number of columns for the popup menu. If the wrap width is larger than 1, the combo box is in table mode. */
            int WrapWidth() const { return gtk_combo_box_get_wrap_width(*this); }
/** Sets the wrap width of the object to be width. The wrap width is basically the preferred number of columns when you want the popup to be layed out in a table.
*/
            void WrapWidth(int width) const { gtk_combo_box_set_wrap_width(*this, width); }
/** Sets the current active item to be the one referenced by iter.
\note Iter must correspond to a path of depth one.
*/
            virtual void SetActiveIter(const TreeIter &it /* An initialized and valid TreeIter */) {
                gtk_combo_box_set_active_iter(*this, const_cast<TreeIter *>(&it));
            }
/** Sets it to point to the current active item, if it exists.
\return true, if it was set
*/
            virtual bool GetActiveIter(TreeIter &it /* Destination iter */ ) {
                return gtk_combo_box_get_active_iter(*this, &it);
            }
/** The changed signal is emitted when the active item is changed. The can be due to the user selecting a different item from the list, or due to a call to ComboBox::ActiveIter(). It will also be emitted while typing into a ComboBoxEntry, as well as when selecting an item from the ComboBoxEntry's list. */
            BUILD_EVENT(OnChanged, "changed");
   };

/**
A ComboBoxText is a simple variant of ComboBox that hides the model-view complexity for simple text-only use cases.
To create a ComboBoxText, use the constructor ComboBoxText::ComboBoxText().
You can add items to a ComboBoxText with ComboBoxText::Append(), ComboBoxText::Insert() or ComboBoxText::Prepend() and remove options with ComboBoxText::Remove().
If the ComboBoxText contains an entry (via the 'has-entry' property), its contents can be retrieved using ComboBoxText::ActiveText(). The entry itself can be accessed by calling Bin::Child() on the combo box.
 */
    class ComboBoxText : public ComboBox
    {
        public:
/// DOXYS_OFF
#if GTK_MINOR_VERSION > 23
            operator  GtkComboBoxText *() const { return GTK_COMBO_BOX_TEXT(Obj()); }
            ComboBoxText(GObject *obj) { Init(obj); }
#else
            operator  GtkComboBox *() const { return GTK_COMBO_BOX(Obj()); }
#endif
/// DOXYS_ON
            /// Creates a new ComboBoxText, which is a ComboBox just displaying strings. With GTK 2.24+ the combo box created by this function may have an optional entry.
#if GTK_MINOR_VERSION > 23
            ComboBoxText(bool with_entry = false  /**< optional entry for the ComboBoxText, defaults to false */
                        ) : ComboBox(DerivedType()) {
                Init(with_entry ? gtk_combo_box_text_new_with_entry() : gtk_combo_box_text_new());
                Internal(true);
            }
#else
            ComboBoxText() : ComboBox(DerivedType()) {
                Init(gtk_combo_box_new_text());
                Internal(true);
            }
#endif
            void Append(const std::string &text) {
#if GTK_MINOR_VERSION < 24
                gtk_combo_box_append_text(*this, text.c_str());
#else
                gtk_combo_box_text_append_text(*this, text.c_str());
#endif
            }
            void Prepend(const std::string &text) {
#if GTK_MINOR_VERSION < 24
                gtk_combo_box_prepend_text(*this, text.c_str());
#else
                gtk_combo_box_text_prepend_text(*this, text.c_str());
#endif
            }
            void Insert(const std::string &text, int position) {
#if GTK_MINOR_VERSION < 24
                gtk_combo_box_insert_text(*this, position, text.c_str());
#else
                gtk_combo_box_text_insert_text(*this, position, text.c_str());
#endif
            }
            void Remove(int row) {
#if GTK_MINOR_VERSION < 24
                gtk_combo_box_remove_text(*this, row);
#else
                gtk_combo_box_text_remove(*this, row);
#endif
            }

            std::string ActiveText() {
                std::string res;
                if (gchar *c =
#if GTK_MINOR_VERSION < 24
                        gtk_combo_box_get_active_text(*this)
#else
                        gtk_combo_box_text_get_active_text(*this)
#endif
                        ) {
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

/**
EntryCompletion is an auxiliary object to be used in conjunction with Entry to provide the completion functionality. It implements the CellLayout interface, to allow the user to add extra cells to the TreeView with completion matches.

"Completion functionality" means that when the user modifies the text in the entry, EntryCompletion checks which rows in the model match the current content of the entry, and displays a list of matches. By default, the matching is done by comparing the entry text case-insensitively against the text column of the model (see EntryCompletion::TextColumn() ), but this can be overridden with a custom match function (see EntryCompletion::MatchFunc() ).

When the user selects a completion, the content of the entry is updated. By default, the content of the entry is replaced by the text column of the model, but this can be overridden by connecting to the ::match-selected signal and updating the entry in the signal handler. Note that you should return TRUE from the signal handler to suppress the default behaviour.

To add completion functionality to an entry, use Entry::Completion(const EntryCompletion &).

In addition to regular completion matches, which will be inserted into the entry when they are selected, EntryCompletion also allows to display "actions" in the popup window. Their appearance is similar to menuitems, to differentiate them clearly from completion strings. When an action is selected, the ::action-activated signal is emitted.
 */
    class EntryCompletion: public Object, public CellLayout
    {
        protected:
            GtkCellLayout *getobj() const { return GTK_CELL_LAYOUT(Obj()); }
        public:
/// DOXYS_OFF
         operator  GtkEntryCompletion *() const { return GTK_ENTRY_COMPLETION(Obj()); }
         EntryCompletion(GObject *obj) { Init(obj); }
/// DOXYS_ON
            /// Creates a new EntryCompletion object.
            EntryCompletion() {
                Init(gtk_entry_completion_new());
                Internal(true);
            }
            /// Gets the entry completion has been attached to.
            Entry *GetEntry() { return dynamic_cast<Entry *>(Find((GObject*)gtk_entry_completion_get_entry(*this))); }
            /// Sets the model for this object. If completion already has a model set, it will remove it before setting the new model. If model is NULL, then it will unset the model.
            void Model(const TreeModel *m) {
                if (!m)
                    gtk_entry_completion_set_model(*this, NULL);
                else
                    gtk_entry_completion_set_model(*this, *m);
            }
            /// Get the model associated with the object, returns NULL if the model is unset.
            TreeModel *Model() {
                return dynamic_cast<TreeModel *>(Find((GObject*)gtk_entry_completion_get_model(*this)));
            }
            /// Sets the match function for completion to be func. The match function is used to determine if a row should or should not be in the completion list.
            void MatchFunc(GtkEntryCompletionMatchFunc func, gpointer func_data) {
                gtk_entry_completion_set_match_func(*this, func, func_data, NULL);
            }
            /// Requires the length of the search key for completion to be at least length. This is useful for long lists, where completing using a small key takes a lot of time and will come up with meaningless results anyway (ie, a too large dataset).
            void MinimumKeyLength(size_t t) { gtk_entry_completion_set_minimum_key_length(*this, t); }
            /// Returns the minimum key length as set for completion.
            int MinimumKeyLength() const { return gtk_entry_completion_get_minimum_key_length(*this); }
            /// Requests a completion operation, or in other words a refiltering of the current list with completions, using the current key. The completion list view will be updated accordingly.
            void Complete() const { gtk_entry_completion_complete(*this); }
            /// Convenience method for setting up the most used case of this code: a completion list with just strings. This function will set up completion to have a list displaying all (and just) strings in the completion list, and to get those strings from column in the model of completion.
            void TextColumn(size_t column /**< The column in the model of completion to get strings from. */) {
                gtk_entry_completion_set_text_column(*this, column);
            }
            /// Returns the column in the model of completion to get strings from.
            int TextColumn() const { return gtk_entry_completion_get_text_column(*this); }
    };
    inline void Entry::Completion(const EntryCompletion *completion) {
        if (!completion)
            gtk_entry_set_completion(*this, NULL);
        else
            gtk_entry_set_completion(*this, *completion);
    }
    inline EntryCompletion *Entry::Completion() const {
        return dynamic_cast<EntryCompletion *>(Find((GObject*)gtk_entry_get_completion(*this)));
    }

// 95% complete, only drag & drop handling is not implemented

/** a widget that provides an alternative view on a list model.
IconView provides an alternative view on a list model. It displays the model as a grid of icons with labels. Like TreeView, it allows to select one or multiple items (depending on the selection mode, see IconView::Mode()). In addition to selection with the arrow keys, IconView supports rubberband selection, which is controlled by dragging the pointer.
*/
    class IconView : public Container
    {
        public:
/// DOXYS_OFF
            operator  GtkIconView *() const { return GTK_ICON_VIEW(Obj()); }

            IconView(GObject *obj) { Init(obj); }
/// DOXYS_ON
            /// Creates a new IconView widget with a existing model.
            IconView(const TreeModel &model) {
                TreeModel &m = const_cast<TreeModel &>(model);
                m.Ref();
                Init(gtk_icon_view_new_with_model(m));
                Internal(true);
            }
            /// Creates a new IconView widget
            IconView() {
                Init(gtk_icon_view_new());
                Internal(true);
            }
            /** Sets the model for a IconView.
            If the icon_view already has a model set, it will remove it before setting the new model.
            */
            void Model(TreeModel &model /**< The model */) {
                gtk_icon_view_set_model(*this, model);
            }
            /** Returns the model the GtkIconView is based on.
            \note Returns NULL if the model is unset.
            \return A TreeModel, or NULL if none is currently being used.
            */
            TreeModel *Model() {
                GtkTreeModel *m = gtk_icon_view_get_model(*this);

                return dynamic_cast<TreeModel *>(Object::Find((GObject *)m));
            }
            /// Sets the column with text for icon_view to be column. The text column must be of type G_TYPE_STRING.
            void TextColumn(int column /**< A column in the currently used model, or -1 to display no text */) {
                gtk_icon_view_set_text_column(*this, column);
            }
            /// Returns the column with text for icon_view, -1 if it's unset.
            int TextColumn() const {
                return gtk_icon_view_get_text_column(*this);
            }
            /** Sets the column with markup information for icon_view to be column.
              The markup column must be of type G_TYPE_STRING. If the markup column is set to something,
              it overrides the text column set by IconView::TextColumn(int).
            */
            void MarkupColumn(int column /**< A column in the currently used model, or -1 to display no text */) {
                gtk_icon_view_set_markup_column(*this, column);
            }
            /// Returns the column with markup for icon_view, -1 if it's unset.
            int MarkupColumn() const {
                return gtk_icon_view_get_markup_column(*this);
            }

            /** Sets the column with pixbufs for icon_view.
            The pixbuf column must be of type GDK_TYPE_PIXBUF.
            */
            void PixbufColumn(int column /**< A column in the currently used model, or -1 to display no text */) {
                gtk_icon_view_set_pixbuf_column(*this, column);
            }
            /// Returns the column with pixbuf for icon_view, -1 if it's unset.
            int PixbufColumn() const {
                return gtk_icon_view_get_pixbuf_column(*this);
            }
            /** Finds the path at the point (x, y), relative to bin_window coordinates.
           See IconView::ConvertWidget2BinCoords() for converting widget coordinates to bin_window coordinates.
           */
            bool PathAt(TreePath &path, const Point &position) {
                if(GtkTreePath *p = gtk_icon_view_get_path_at_pos(*this, position.x, position.y)) {
                    path = *p;
                    gtk_tree_path_free(p);
                    return true;
                }
                return false;
            }
            /** Converts widget coordinates to coordinates for the bin_window.
            This method converts widget coordinates to coordinates for the bin_window as expected by e.g. IconView::PathAt().
            \return a Point rapresenting bin_window X and Y coordinates.
            */
            Point ConvertWidget2BinCoords(int wx /**< X coordinate relative to the widget */,
                                          int wy /**< Y coordinate relative to the widget */) const {
                Point p;
                gtk_icon_view_convert_widget_to_bin_window_coords(*this, wx, wy, &p.x, &p.y);
                return p;
            }

            /** Sets the current keyboard focus to be at path, and selects it.
This is useful when you want to focus the user's attention on a particular item. If start_editing is true, then editing should be started in the specified cell.

This function is often followed by Widget::GrabFocus (on the icon_view) in order to give keyboard focus to the widget.
\note Please note that editing can only happen when the widget is realized.
            */
            void Cursor(const TreePath &path /**< A path to the item to select */,
                        bool start_editing = false /**< true, if the specified cell should start being edited, defaults to false.*/) {
                gtk_icon_view_set_cursor(*this, path, NULL, start_editing ? TRUE : FALSE);
            }

            /** Fills in path with the current cursor path.
            If the cursor isn't currently set, then it will return false.
            */
            bool Cursor(TreePath &path /**< Return location for the current cursor path, untouched if the method returned false */) {
                GtkTreePath *p;
                GtkCellRenderer *c;
                gboolean rc = gtk_icon_view_get_cursor(*this, &p, &c);
                if (rc  == FALSE || p == NULL)
                    return false;
                path = *p;
                gtk_tree_path_free(p);
                return true;
            }
            /// Sets the selection mode of the icon_view.
            void Mode(SelectionMode mode  /**< a SelectionMode valid value or a GtkSelectionMode value */) {
                gtk_icon_view_set_selection_mode(*this, (GtkSelectionMode)mode);
            }
            /// Gets the selection mode of the icon_view.
            SelectionMode Mode() const { return (SelectionMode) gtk_icon_view_get_selection_mode(*this); }
            /** Set the maximum number of columns for the widget.
            Sets the ::columns property which determines in how many columns the icons are arranged. If columns is -1, the number of columns will be chosen automatically to fill the available area.
            */
            void Columns(int columns /**< the number of columns, -1 to let GTK choose it */) { gtk_icon_view_set_columns(*this, columns); }
            /// Returns the number of columns of the widget.
            int Columns() const { return gtk_icon_view_get_columns(*this); }
            /// Sets the ::item-width property which specifies the width to use for each item. If it is set to -1, the icon view will automatically determine a suitable item size.
            void ItemWidth(int width /**< the width in pixels for each item */) { gtk_icon_view_set_item_width(*this, width); }
            /// Returns the value of the ::item-width property (width in pixels of each item).
            int ItemWidth() const { return gtk_icon_view_get_item_width(*this); }
            /// Sets the ::spacing property which specifies the space which is inserted between the cells (i.e. the icon and the text) of an item.
            void Spacing(int size /**< the space between cells */ ) { gtk_icon_view_set_spacing(*this, size); }
            /// Returns the value of the ::spacing property.
            int Spacing() const { return gtk_icon_view_get_spacing(*this); }
            /// Sets the ::row-spacing property which specifies the space which is inserted between the rows of the icon view.
            void RowSpacing(int size /**< the row spacing */) { gtk_icon_view_set_row_spacing(*this, size); }
            /// Returns the value of the ::row-spacing property.
            int RowSpacing() const { return gtk_icon_view_get_row_spacing(*this); }
            /// Sets the ::column-spacing property which specifies the space which is inserted between the cloumns of the icon view.
            void ColumnSpacing(int size /**< the row spacing */) { gtk_icon_view_set_column_spacing(*this, size); }
            /// Returns the value of the ::column-spacing property.
            int ColumnSpacing() const { return gtk_icon_view_get_column_spacing(*this); }
            /// Sets the ::margin property which specifies the space which is inserted at the top, bottom, left and right of the icon view.
            void Margin(int size) { gtk_icon_view_set_margin(*this, size); }
            /// Returns the value of the ::margin property.
            int Margin() const { return gtk_icon_view_get_margin(*this); }
            /// Sets the "item-padding" property which specifies the padding around each of the icon view's items
#if GTK_MINOR_VERSION > 16
            void ItemPadding(int padding) { gtk_icon_view_set_item_padding(*this, padding); }
            /// Returns the value of the ::item-padding property.
            int ItemPadding() const { return gtk_icon_view_get_item_padding(*this); }
#endif
            /// Selects the item at path.
            void Select(const TreePath &path) { gtk_icon_view_select_path(*this, path); }
            /// Unselect the item at path
            void Unselect(const TreePath &path) { gtk_icon_view_unselect_path(*this, path); }
            /// Returns true if the icon pointed to by path is currently selected. If path does not point to a valid location, false is returned.
            bool Selected(const TreePath &path) { return gtk_icon_view_path_is_selected(*this, path); }
            /// Returns a RefVec containing a TreeRowReference for every item selected in the IconView
            RefVec SelectedItems() const {
                RefVec result;
                if (GtkTreeModel *model = gtk_icon_view_get_model(*this)) {
                    if (GList *list = gtk_icon_view_get_selected_items(*this)) {
                        GList *l = list;
                        while (l) {
                            result.push_back(TreeRowReference(model, (GtkTreePath *)l->data));
                            l = l->next;
                        }
                        g_list_foreach (list, (void (*)(void *,void *))gtk_tree_path_free, NULL);
                        g_list_free (list);
                    }
                }
                return result;
            }
            /// Selects all the icons. the object must have its selection mode set to SelectionMultiple.
            void SelectAll() { gtk_icon_view_select_all(*this); }
            /// Unselects all the icons.
            void UnselectAll() { gtk_icon_view_unselect_all(*this); }
            /// Activates the item determined by path.
            void Activates(const TreePath &path) { gtk_icon_view_item_activated(*this, path); }
            /** Moves the alignments of icon_view to the position specified by path.
The parameter row_align determines where the row is placed, and col_align determines where column is placed. Both are expected to be between 0.0 and 1.0. 0.0 means left/top alignment, 1.0 means right/bottom alignment, 0.5 means center.

If the alignament values are not specified, the tree does the minimum amount of work to scroll the item onto the screen. This means that the item will be scrolled to the edge closest to its current position. If the item is currently visible on the screen, nothing is done.

This function only works if the model is set, and path is a valid row on the model. If the model changes before the icon_view is realized, the centered path will be modified to reflect this change.

\note if you specify row_align you must also specify col_align or your row_align will be ignored.
*/
            void ScrollTo(const TreePath &path /**< The path of the item to move to. */,
                          float row_align = -1.0 /**< The optional vertical alignment of the item specified by path. */,
                          float col_align = -1.0 /**< The horizontal alignment of the item specified by path.*/) {
                gtk_icon_view_scroll_to_path(*this, path, (row_align >= 0.0 && col_align >= 0.0), row_align, col_align);
            }
            /// Sets start_path and end_path to be the first and last visible path. Note that there may be invisible paths in between. Returns false if no visible path is found, in this case start_path and end_path content is undefined.
            bool VisibleRange(TreePath &start_path, TreePath &end_path) {
                GtkTreePath *s, *e;
                if (gtk_icon_view_get_visible_range(*this, &s, &e)) {
                    if (s) {
                        start_path = *s;
                        gtk_tree_path_free(s);
                        if (e) {
                            end_path = *e;
                            gtk_tree_path_free(e);
                            return true;
                        }
                    }
                }
                return false;
            }
#if GTK_MINOR_VERSION > 12
            /// Sets the tip area of tooltip to be the area covered by the item at path. See also IconView::TooltipColumn() for a simpler alternative. See also Tooltip::Area().
            void Tooltip(const TreePath &path, const gtk::Tooltip &tip) {
                gtk_icon_view_set_tooltip_item(*this, tip, path);
            }
#endif
            /*** Sets simple tooltip handling through icon view model.
If you only plan to have simple (text-only) tooltips on full items, you can use this function to have IconView handle these automatically for you. column should be set to the column in icon_view's TreeModel containing the tooltip texts, or -1 to disable this feature.

When enabled, "has-tooltip" will be set to TRUE and icon_view will connect a "query-tooltip" signal handler.
            */
            void TooltipColumn(int column) { gtk_icon_view_set_tooltip_column(*this, column); }
            /// Returns the column of icon_view's model which is being used for displaying tooltips on icon_view's rows.
            int TooltipColumn() const { return gtk_icon_view_get_tooltip_column(*this); }
#if GTK_MINOR_VERSION > 20
            /// Gets the row in which the item path is currently displayed. Row numbers start at 0.
            int ItemRow(const TreePath &path) { return gtk_icon_view_get_item_row(*this, path); }
            /// Gets the column in which the item path is currently displayed. Column numbers start at 0.
            int ItemColumn(const TreePath &path) { return gtk_icon_view_get_item_column(*this, path); }
#endif
            /** This function is a convenience function to allow you to reorder models that support the TreeDragSourceIface and the TreeDragDestIface. Both TreeStore and ListStore support these. If reorderable is true, then the user can reorder the model by dragging and dropping rows. The developer can listen to these changes by connecting to the model's row_inserted and row_deleted signals. The reordering is implemented by setting up the icon view as a drag source and destination. Therefore, drag and drop can not be used in a reorderable view for any other purpose.

This function does not give you any degree of control over the order -- any reordering is allowed. If more control is needed, you should probably handle drag and drop manually.
            */
            void Reorderable(bool flag) { return gtk_icon_view_set_reorderable(*this, flag); }
            /// Retrieves whether the user can reorder the list via drag-and-drop.
            bool Reorderable() const { return gtk_icon_view_get_reorderable(*this); }
    };
}

#endif
