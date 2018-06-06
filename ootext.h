#ifndef OOTEXT_H
#define OOTEXT_H

#include "oocont.h"

namespace gtk {
    class TextBuffer;

    class TextTag : public Object 
    {
        public:
            operator  GtkTextTag *() const { return GTK_TEXT_TAG(Obj()); }

            TextTag(GObject *o) { Init(o); }

            TextTag(const std::string &name = "") {
                Init(gtk_text_tag_new(name.empty() ? NULL : name.c_str()));
                Internal(true);
            }

            void Priority(int p) { gtk_text_tag_set_priority(*this, p); }
            int Priority() const { return gtk_text_tag_get_priority(*this); }

    };

    enum TextSearchFlags {
      TextSearchVisibleOnly = GTK_TEXT_SEARCH_VISIBLE_ONLY,
      TextSearchTextOnly = GTK_TEXT_SEARCH_TEXT_ONLY
      /* Possible future plans: SEARCH_CASE_INSENSITIVE, SEARCH_REGEXP */
    };

    inline TextSearchFlags operator|(const TextSearchFlags &l, const TextSearchFlags &r) {
        return (TextSearchFlags)( ((int)l)|((int)r) );
    }
/** A text buffer iterator


*/
    class TextIter {
            typedef std::pair<TextIter, TextIter> TextRange;
        public:
            operator const GtkTextIter *() const { return &it_; }
            operator GtkTextIter *() { return &it_; }
            TextIter &operator++() { gtk_text_iter_forward_char(&it_); return *this; }
            TextIter &operator--() { gtk_text_iter_backward_char(&it_); return *this; }
            bool operator==(const TextIter &rhs) { return gtk_text_iter_equal(&it_, rhs); }
            bool operator!=(const TextIter &rhs) { return !gtk_text_iter_equal(&it_, rhs); }
            bool operator<(const TextIter &rhs) { return gtk_text_iter_compare(&it_, rhs) < 0; }
            TextIter &operator=(const TextIter &rhs) { it_ = rhs.it_; return *this; }
            int operator*() const { return gtk_text_iter_get_char(&it_); }

            void Next() { ++(*this); }
            void Pred() { --(*this); }
            int Char() const { return *(*this); }

            bool InRange(const TextIter &lhs, const TextIter &rhs) const {
                return gtk_text_iter_in_range(&it_, lhs, rhs);
            }
            bool InRange(const TextRange &range) const {
                return InRange(range.first, range.second);
            }
            bool Search(const std::string &text, TextRange &result,
                             TextSearchFlags flags = TextSearchTextOnly,
                             TextIter *limit = NULL) {
                return gtk_text_iter_forward_search(&it_, text.c_str(), (GtkTextSearchFlags)flags,
                                             &result.first.it_, &result.second.it_, 
                                             limit ? &limit->it_ : NULL);
            }
                             
            int Line() const { return gtk_text_iter_get_line(&it_); }
            int Offset() const { return gtk_text_iter_get_offset(&it_); }
            int LineOffset() const { return gtk_text_iter_get_line_offset(&it_); }

            TextBuffer &Buffer();
            bool HasTag(const TextTag &tag) const {
                return gtk_text_iter_has_tag(&it_, tag);
            }
            bool IsStart() const { return gtk_text_iter_is_start(&it_); }
            bool IsEnd() const { return gtk_text_iter_is_end(&it_); }
            void AtLineOffset(int offset) { gtk_text_iter_set_line_offset(&it_, offset); }
            void AtOffset(int offset) { gtk_text_iter_set_offset(&it_, offset); }
            void AtLine(int line) { gtk_text_iter_set_line(&it_, line); }
        private:
            GtkTextIter it_;
    };

    typedef std::pair<TextIter, TextIter> TextRange;

    class TextTagTable : public Object
    {
        public:
            operator  GtkTextTagTable *() const { return GTK_TEXT_TAG_TABLE(Obj()); }

            TextTagTable(GObject *o) { Init(o); }

            TextTagTable() { 
                Init(gtk_text_tag_table_new());
                Internal(true);
            }

            void Add(const TextTag &tag) { gtk_text_tag_table_add(*this, tag); }
            void Remove(TextTag &tag) { gtk_text_tag_table_remove(*this, tag); }
            TextTag *Find(const std::string &name) { 
                return dynamic_cast<TextTag *>(Object::Find((GObject *)gtk_text_tag_table_lookup(*this, name.c_str())));
            }
            int Size() const { return gtk_text_tag_table_get_size(*this); }
    };

    class TextMark : public Object  // COMPLETE API
    {
        public:
            operator  GtkTextMark *() const { return GTK_TEXT_MARK(Obj()); }
            TextMark(GObject *o) { Init(o); }
     
            TextMark(const std::string &name = "", bool left_gravity = true) {
                Init(gtk_text_mark_new(name.empty() ? NULL : name.c_str(), left_gravity));
                Internal(true);
            }
            bool LeftGravity() const { return gtk_text_mark_get_left_gravity(*this); }
            bool RightGravity() const { return !gtk_text_mark_get_left_gravity(*this); }
            bool Deleted() const { return gtk_text_mark_get_deleted(*this); }
            bool Visible() const { return gtk_text_mark_get_visible(*this); }
            void Visible(bool flag) { return gtk_text_mark_set_visible(*this, flag); }

            std::string Name() { return gtk_text_mark_get_name(*this); }
            TextBuffer &Buffer();
    };

/** Stores attributed text for display in a TextView.

You may wish to begin by reading the text widget conceptual overview in TextView which gives an overview of all the objects and data types related to the text widget and how they work together. 
*/
    class TextBuffer : public Object
    {
        public:
/// DOXYS_OFF            
            operator  GtkTextBuffer *() const { return GTK_TEXT_BUFFER(Obj()); }

            TextBuffer(GObject *o) { Init(o); }
/// DOXYS_ON

/// Creates a new text buffer.
            TextBuffer() {
                Init(gtk_text_buffer_new(NULL)); 
                Internal(true);
            }

/// Creates a new text buffer with an associated TextTagTable.
            TextBuffer(const TextTagTable &table) {
                Init(gtk_text_buffer_new(table)); 
                Internal(true);
            }

/// Retrieve the TextTagTable associated with the buffer.
            TextTagTable &TagTable() { 
                GtkTextTagTable *t = gtk_text_buffer_get_tag_table(*this);

                if (!t)
                    throw std::runtime_error("Text buffer without tagtable");

                return * (dynamic_cast<TextTagTable *>(Object::Find((GObject *)t)));
            }
            
/** Obtains the number of lines in the buffer.

This value is cached, so the function is very fast.  

\return number of lines in the buffer 
*/
            int Lines() const { return gtk_text_buffer_get_line_count(*this); }
/** Gets the number of characters in the buffer.

Note that characters and bytes are not the same, you can't e.g. expect the contents of the buffer in string form to be this many bytes long. The character count is cached, so this function is very fast.

\return number of characters in the buffer 
*/
            int Characters() const { return gtk_text_buffer_get_char_count(*this); }
/** Inserts a string at position iter.

Emits the "insert-text" signal; insertion actually occurs in the default handler for the signal. iter is invalidated when insertion occurs (because the buffer contents change), but the default signal handler revalidates it to point to the end of the inserted text.
*/
            void Insert(const TextIter &position /**< a position in the buffer */, 
                        const std::string &text  /**< UTF-8 format string to insert */) {
                gtk_text_buffer_insert(*this, const_cast<TextIter &>(position), 
                                text.c_str(), text.length());
            }
            void InsertAtCursor(const std::string &text /**< UTF-8 format string to insert */) {
                gtk_text_buffer_insert_at_cursor(*this, text.c_str(), text.length());
            }
            // by tag reference
            void InsertTags(const TextIter &position, const std::string &text,
                    const TextTag &t1) {
                gtk_text_buffer_insert_with_tags(*this, const_cast<TextIter &>(position), 
                                text.c_str(), text.length(), t1, NULL);
            }
            void InsertTags(const TextIter &position, const std::string &text,
                    const TextTag &t1, const TextTag &t2) {
                gtk_text_buffer_insert_with_tags(*this, const_cast<TextIter &>(position), 
                                text.c_str(), text.length(), t1, t2.Obj(), NULL);
            }
            void InsertTags(const TextIter &position, const std::string &text,
                    const TextTag &t1, const TextTag &t2, const TextTag &t3) {
                gtk_text_buffer_insert_with_tags(*this, const_cast<TextIter &>(position), 
                                text.c_str(), text.length(), t1, t2.Obj(), t3.Obj(), NULL);
            }
            // by tag name
            void InsertTags(const TextIter &position, const std::string &text,
                    const std::string &t1) {
                gtk_text_buffer_insert_with_tags_by_name(*this, const_cast<TextIter &>(position), 
                                text.c_str(), text.length(), t1.c_str(), NULL);
            }
            void InsertTags(const TextIter &position, const std::string &text,
                    const std::string &t1, const std::string &t2) {
                gtk_text_buffer_insert_with_tags_by_name(*this, const_cast<TextIter &>(position), 
                                text.c_str(), text.length(), t1.c_str(), t2.c_str(), NULL);
            }
            void InsertTags(const TextIter &position, const std::string &text,
                    const std::string &t1, const std::string &t2, const std::string &t3) {
                gtk_text_buffer_insert_with_tags_by_name(*this, const_cast<TextIter &>(position), 
                                text.c_str(), text.length(), t1.c_str(), t2.c_str(), t3.c_str(), NULL);
            }

            void Append(const std::string &text) {
                Insert(End(), text);
            }
            void AppendTags(const std::string &text, const TextTag &t1) {
                InsertTags(End(), text, t1);
            }
            void AppendTags(const std::string &text, const TextTag &t1, const TextTag &t2) {
                InsertTags(End(), text, t1, t2);
            }
            void AppendTags(const std::string &text, const TextTag &t1, const TextTag &t2, const TextTag &t3) {
                InsertTags(End(), text, t1, t2, t3);
            }
            void AppendTags(const std::string &text, const std::string &t1) {
                InsertTags(End(), text, t1);
            }
            void AppendTags(const std::string &text, const std::string &t1, const std::string &t2) {
                InsertTags(End(), text, t1, t2);
            }
            void AppendTags(const std::string &text, const std::string &t1, const std::string &t2, const std::string &t3) {
                InsertTags(End(), text, t1, t2, t3);
            }

            void Set(const std::string &text) {
                gtk_text_buffer_set_text(*this, text.c_str(), text.length());
            }
            
            void Clear() { TextRange r = Bounds(); Delete(r); }
            /// Gets the entire contents of the textbuffer
            std::string Get() const {
                std::string res;
                TextRange r = Bounds();
                if (gchar *t = gtk_text_buffer_get_text(*this, r.first, r.second, FALSE)) {
                    res = t;
                    g_free(t);
                }
                return res;
            }
            std::string Get(const TextRange &range, bool include_hidden = false) const {
                std::string res;
                if (gchar *t = gtk_text_buffer_get_text(*this, range.first, range.second,
                                    include_hidden)) {
                    res = t;
                    g_free(t);
                }
                return res;
            }

            bool Modified() const { return gtk_text_buffer_get_modified(*this); }
            void Modified(bool flag) const { return gtk_text_buffer_set_modified(*this, flag); }

            void Delete(TextIter &begin, TextIter &end) {
                gtk_text_buffer_delete(*this, begin, end);
            }
            void Delete(TextRange &range) {
                Delete(range.first, range.second);
            }

            // tag handling
            void Apply(const TextTag &tag, const TextIter &begin, const TextIter &end) {
                gtk_text_buffer_apply_tag(*this, tag, begin, end);
            }
            void Apply(const TextTag &tag, const TextRange &range) { Apply(tag, range.first, range.second); }
            void Apply(const std::string &tagname, const TextIter &begin, const TextIter &end) {
                gtk_text_buffer_apply_tag_by_name(*this, tagname.c_str(), begin, end);
            }
            void Apply(const std::string &tagname, const TextRange &range) { Apply(tagname, range.first, range.second); }
            void Remove(const TextTag &tag, const TextIter &begin, const TextIter &end) {
                gtk_text_buffer_remove_tag(*this, tag, begin, end);
            }
            void Remove(const TextTag &tag, const TextRange &range) { Remove(tag, range.first, range.second); }
            void Remove(const std::string &tagname, const TextIter &begin, const TextIter &end) {
                gtk_text_buffer_remove_tag_by_name(*this, tagname.c_str(), begin, end);
            }
            void Remove(const std::string &tagname, const TextRange &range) { Remove(tagname, range.first, range.second); }
            void RemoveTags(const TextIter &begin, const TextIter &end) {
                gtk_text_buffer_remove_all_tags(*this,begin, end);
            }
            void RemoveTags(const TextRange &range) { RemoveTags(range.first, range.second); }
            void Add(const TextTag &tag) {
                TagTable().Add(tag);
            }

            // selection
            void Select(const TextIter &begin, const TextIter &end) { gtk_text_buffer_select_range(*this, begin, end); }
            void Select(const TextRange &range) { Select(range.first, range.second); }
            bool HasSelection() const { return gtk_text_buffer_get_has_selection(*this); }
            bool SelectionBounds(TextRange &range) const { 
                return gtk_text_buffer_get_selection_bounds(*this, range.first, range.second);
            }
            // mark operations
            void Move(TextMark &mark, const TextIter &where) { gtk_text_buffer_move_mark(*this, mark, where); }
            void Add(TextMark &mark, const TextIter &where) { gtk_text_buffer_add_mark(*this, mark, where); }
            void Delete(TextMark &mark) { gtk_text_buffer_delete_mark(*this, mark); }
            TextMark &CreateMark(const TextIter &where, bool left_gravity = true, const std::string &name = "") {
                GtkTextMark *o = gtk_text_buffer_create_mark(*this, name.empty() ? NULL : name.c_str(),
                                            where, left_gravity);
                return *(dynamic_cast<TextMark *>(Object::Find((GObject *)o)));
            }

            // getting new iterators
            TextIter Begin() const { TextIter it; gtk_text_buffer_get_start_iter(*this, it); return it; }
            TextIter End() const { TextIter it; gtk_text_buffer_get_end_iter(*this, it); return it; }
            TextRange Bounds() const { TextRange r; gtk_text_buffer_get_bounds(*this, r.first, r.second); return r; }

            TextMark *Cursor() { return dynamic_cast<TextMark *>(Object::Find((GObject *)
                                        gtk_text_buffer_get_insert(*this))); }

            bool CursorIter(TextIter &it) { 
                if (GtkTextMark *m = gtk_text_buffer_get_insert(*this)) {
                    gtk_text_buffer_get_iter_at_mark(*this, it, m);
                    return true;
                }
                return false;
            }
            TextIter AtMark(const TextMark &mark) { 
                TextIter it; 
                gtk_text_buffer_get_iter_at_mark(*this, it, mark);
                return it;
            }
            TextIter AtLine(int line, int offset = -1) {
                TextIter it;
                if (offset != -1)
                    gtk_text_buffer_get_iter_at_line_offset(*this, it, line, offset);
                else
                    gtk_text_buffer_get_iter_at_line(*this, it, line);
                return it;
            }
            TextIter AtCharacter(int offset) {
                TextIter it;
                gtk_text_buffer_get_iter_at_offset(*this, it, offset);
                return it;
            }

            void PlaceCursor(const TextIter &position) { gtk_text_buffer_place_cursor(*this, position); }

            BUILD_VOID_EVENT(OnChanged, "changed");
            BUILD_VOID_EVENT(OnMarkSet, "mark-set");
    };

    inline TextBuffer &TextIter::Buffer() { 
        if (TextBuffer *b = dynamic_cast<TextBuffer *>(Object::Find((GObject *)
                        gtk_text_iter_get_buffer(&it_)))) {
            return *b;
        }
        else throw std::runtime_error("Text iterator with invalid buffer");
    }

    inline TextBuffer &TextMark::Buffer() { 
        if (TextBuffer *b = dynamic_cast<TextBuffer *>(Object::Find((GObject *)
                        gtk_text_mark_get_buffer(*this)))) {
            return *b;
        }
        else throw std::runtime_error("Deleted text marks don't have buffers!");
    }

    enum TextWindowType
    {
        TextWindowPrivate = GTK_TEXT_WINDOW_PRIVATE,
        TextWindowWidget = GTK_TEXT_WINDOW_WIDGET,
        TextWindowText = GTK_TEXT_WINDOW_TEXT,
        TextWindowLeft = GTK_TEXT_WINDOW_LEFT,
        TextWindowRight = GTK_TEXT_WINDOW_RIGHT,
        TextWindowTop = GTK_TEXT_WINDOW_TOP,
        TextWindowBottom = GTK_TEXT_WINDOW_BOTTOM
    };

    enum WrapMode
    {
        WrapNone = GTK_WRAP_NONE,
        WrapChar = GTK_WRAP_CHAR,
        WrapWord = GTK_WRAP_WORD,
        WrapWordChar = GTK_WRAP_WORD_CHAR
    };

/** A widget to display multiple lines of text.

GTK+ has an extremely powerful framework for multiline text editing. The primary objects involved in the process are TextBuffer, which represents the text being edited, and TextView, a widget which can display a TextBuffer. Each buffer can be displayed by any number of views.

One of the important things to remember about text in GTK+ is that it's in the UTF-8 encoding. This means that one character can be encoded as multiple bytes. Character counts are usually referred to as offsets, while byte counts are called indexes. If you confuse these two, things will work fine with ASCII, but as soon as your buffer contains multibyte characters, bad things will happen.

Text in a buffer can be marked with tags. A tag is an attribute that can be applied to some range of text. For example, a tag might be called "bold" and make the text inside the tag bold. However, the tag concept is more general than that; tags don't have to affect appearance. They can instead affect the behavior of mouse and key presses, "lock" a range of text so the user can't edit it, or countless other things. A tag is represented by a GtkTextTag object. One GtkTextTag can be applied to any number of text ranges in any number of buffers.

Each tag is stored in a TextTagTable. A tag table defines a set of tags that can be used together. Each buffer has one tag table associated with it; only tags from that tag table can be used with the buffer. A single tag table can be shared between multiple buffers, however.

Tags can have names, which is convenient sometimes (for example, you can name your tag that makes things bold "bold"), but they can also be anonymous (which is convenient if you're creating tags on-the-fly).

Most text manipulation is accomplished with iterators, represented by a TextIter. An iterator represents a position between two characters in the text buffer. TextIter is a class designed to be allocated on the stack and manipulated like a STL iterator; it's guaranteed to be copiable by value and never contain any heap-allocated data. Iterators are not valid indefinitely; whenever the buffer is modified in a way that affects the number of characters in the buffer, all outstanding iterators become invalid. (Note that deleting 5 characters and then reinserting 5 still invalidates iterators, though you end up with the same number of characters you pass through a state with a different number).

Because of this, iterators can't be used to preserve positions across buffer modifications. To preserve a position, the TextMark object is ideal. You can think of a mark as an invisible cursor or insertion point; it floats in the buffer, saving a position. If the text surrounding the mark is deleted, the mark remains in the position the text once occupied; if text is inserted at the mark, the mark ends up either to the left or to the right of the new text, depending on its gravity. The standard text cursor in left-to-right languages is a mark with right gravity, because it stays to the right of inserted text.

Like tags, marks can be either named or anonymous. There are two marks built-in to TextBuffer; these are named "insert" and "selection_bound" and refer to the insertion point and the boundary of the selection which is not the insertion point, respectively. If no text is selected, these two marks will be in the same position. You can manipulate what is selected and where the cursor appears by moving these marks around. [2]

Text buffers always contain at least one line, but may be empty (that is, buffers can contain zero characters). The last line in the text buffer never ends in a line separator (such as newline); the other lines in the buffer always end in a line separator. Line separators count as characters when computing character counts and character offsets. Note that some Unicode line separators are represented with multiple bytes in UTF-8, and the two-character sequence "\r\n" is also considered a line separator. 
*/
    class TextView : public Container
    {
        public:
/// DOXYS_OFF            
            operator  GtkTextView *() const { return GTK_TEXT_VIEW(Obj()); }

            TextView(GObject *o) { Init(o); }
/// DOXYS_ON
/** Creates a new TextView. 

If you don't call TextView::Buffer(TextBuffer &) before using the text view, an empty default buffer will be created for you. Get the buffer with TextView::Buffer(). If you want to specify your own buffer, consider construct the object with the appropriate constructor TextView::TextView(TextBuffer &).
*/
            TextView() { Init(gtk_text_view_new()); Internal(true); }
/** Creates a new TextView widget displaying the buffer buffer. 

One buffer can be shared among many widgets. The text view adds its own reference count to the buffer; it does not take over an existing reference.      
*/
            TextView(TextBuffer &buffer /**< A TextBuffer to be displayed in this widget */) { 
                Init(gtk_text_view_new_with_buffer(buffer)); Internal(true); 
            }

            // scroll functionalities
            /** Scrolls the text view the minimum distance such that mark is contained within the visible area of the widget. */
            void Scroll(const TextMark &mark /**< A TextMark in the buffer of the text view */) { gtk_text_view_scroll_mark_onscreen(*this, mark); }
/** Scrolls the text view so that mark is on the screen in the position indicated by xalign and yalign. 

An alignment of 0.0 indicates left or top, 1.0 indicates right or bottom, 0.5 means center. If use_align is false, the text scrolls the minimal distance to get the mark onscreen, possibly not scrolling at all. The effective screen for purposes of this function is reduced by a margin of size within_margin.
*/
            void Scroll(const TextMark &mark /**< A valid TextMark */, 
                        double margin /**< 	 margin as a [0.0,0.5) fraction of screen size */, 
                        bool use_align = false /**< whether to use alignment arguments (if false, just get the mark onscreen), defaults to false */,
                        Align align = Align(0.5, 0.5) /**< horizontal and vertical alignment of mark within visible area */) {
                gtk_text_view_scroll_to_mark(*this, mark, margin, use_align, align.first, align.second); 
            }
/** Scrolls the text view so that iter is on the screen in the position indicated by xalign and yalign.

An alignment of 0.0 indicates left or top, 1.0 indicates right or bottom, 0.5 means center. If use_align is false, the text scrolls the minimal distance to get the mark onscreen, possibly not scrolling at all. The effective screen for purposes of this function is reduced by a margin of size within_margin.

Note that this function uses the currently-computed height of the lines in the text buffer. Line heights are computed in an idle handler; so this function may not have the desired effect if it's called before the height computations. To avoid oddness, consider using TextView::Scroll(const TextMark &, double, bool, Align) which saves a point to be scrolled to after line validation.
*/          
            void Scroll(TextIter &it /**< An initialized TextIter */, 
                       double margin = 0.0f  /**< 	 margin as a [0.0,0.5) fraction of screen size */, 
                       bool use_align = false /**< whether to use alignment arguments (if false, just get the mark onscreen), defaults to false */,
                       Align align = Align(0.5, 0.5) /**< horizontal and vertical alignment of mark within visible area */
                       ) {
                gtk_text_view_scroll_to_iter(*this, it, margin, use_align, align.first, align.second); 
            }

            // buffer handling
/** Returns the TextBuffer being displayed by this text view. 
The reference count on the buffer is not incremented; the caller of this function won't own a new reference. 
*/
            TextBuffer &Buffer() { 
                GtkTextBuffer *b = gtk_text_view_get_buffer(*this); 

                if (!b) {
                    b = gtk_text_buffer_new(NULL);
                    gtk_text_view_set_buffer(*this, b);
                }

                return dynamic_cast<TextBuffer &>(*Object::Find((GObject *)b));
            }
            /** Sets buffer as the buffer being displayed by text_view.
The previous buffer displayed by the text view is unreferenced, and a reference is added to buffer. If you owned a reference to buffer before passing it to this function, you must remove that reference yourself; TextView will not "adopt" it.
*/
            void Buffer(const TextBuffer &buffer /**< A TextBuffer */) { gtk_text_view_set_buffer(*this, buffer); }


            // cursor position/text buffer conversions
            TextWindowType WindowType(GdkWindow *window) {
                return (TextWindowType)gtk_text_view_get_window_type(*this, window);
            }
/** Converts coordinates from buffer system to window system.

Converts coordinate (buffer_x, buffer_y) to coordinates for the window type win, and stores the result in Point.

Note that you can't convert coordinates for a nonexisting window.
*/
            Point BufferToWindow(int x /**< buffer x coordinate */, 
                                 int y /**< buffer y coordinate */, 
                    TextWindowType type = TextWindowWidget /**< a TextWindowType, defaults to TextWindowWidget */) {
                Point dest;
                gtk_text_view_buffer_to_window_coords(*this, 
                        (GtkTextWindowType)type, x, y, 
                        &dest.x, &dest.y);
                return dest;
            }
/** Gets the TextIter at the start of the line containing the coordinate y.

y is in buffer coordinates, convert from window coordinates with TreeView::BufferToWindow(). 
*/
            TextIter LineAtY(int y /**< a y coordinate */) const { 
                TextIter it;
                gtk_text_view_get_line_at_y(*this, it, y, NULL); 
                return it; 
            }
/** Gets a rectangle which roughly contains the character at iter.

The rectangle position is in buffer coordinates; use TextView::BufferToWindow() to convert these coordinates to coordinates for one of the windows in the text view.            

\return a Rect which roughly contains the character at iter.
*/
            Rect Location(const TextIter &it /**< An initialized valid TextIter */) {
                GdkRectangle rect;
                gtk_text_view_get_iter_location(*this, it, &rect);
                return rect;
            }
            /** Returns a Rect with the currently-visible region of the buffer, in buffer coordinates. Convert to window coordinates with TreeView::BufferToWindow().
\return a Rect containing the currently-visible region of the buffer, in buffer coordinates.
             */
            Rect VisibleRect() {
                GdkRectangle rect;
                gtk_text_view_get_visible_rect(*this, &rect);
                return rect; 
            }
            Point WindowToBuffer(int x, int y, 
                                 TextWindowType type = TextWindowWidget) {
                Point dest;
                gtk_text_view_window_to_buffer_coords(*this, 
                        (GtkTextWindowType)type, x, y, 
                        &dest.x, &dest.y);
                return dest;
            }

            TextIter AtLocation(int x, int y) {
                TextIter it;
                gtk_text_view_get_iter_at_location(*this, it, x, y);
                return it;
            }
          
            // flags
            bool Editable() const { return gtk_text_view_get_editable(*this); }
            void Editable(bool flag) { gtk_text_view_set_editable(*this, flag); }
            bool Overwrite() const { return gtk_text_view_get_overwrite(*this); }
            void Overwrite(bool flag) { gtk_text_view_set_overwrite(*this, flag); }
            bool CursorVisible() const { return gtk_text_view_get_cursor_visible(*this); }
            void CursorVisible(bool flag) { gtk_text_view_set_cursor_visible(*this, flag); }
            bool AcceptsTab() const { return gtk_text_view_get_accepts_tab(*this); }
            void AcceptsTab(bool flag) { gtk_text_view_set_accepts_tab(*this, flag); }

            // margins
            int LeftMargin() const { return gtk_text_view_get_left_margin(*this); }
            void LeftMargin(int pixels) { gtk_text_view_set_left_margin(*this, pixels); }
            int RightMargin() const { return gtk_text_view_get_right_margin(*this); }
            void RightMargin(int pixels) { gtk_text_view_set_right_margin(*this, pixels); }
            int Indent() const { return gtk_text_view_get_indent(*this); }
            void Indent(int pixels) { gtk_text_view_set_indent(*this, pixels); }
             
            // line spacing
            int PixelsAbove() const { return gtk_text_view_get_pixels_above_lines(*this); }
            void PixelsAbove(int pixels) { gtk_text_view_set_pixels_above_lines(*this, pixels); }
            int PixelsBelow() const { return gtk_text_view_get_pixels_below_lines(*this); }
            void PixelsBelow(int pixels) { gtk_text_view_set_pixels_below_lines(*this, pixels); }
            // misc
            gtk::WrapMode WrapMode() const { return (gtk::WrapMode)gtk_text_view_get_wrap_mode(*this); }
            void WrapMode(gtk::WrapMode mode) { gtk_text_view_set_wrap_mode(*this, (GtkWrapMode)mode); }
            gtk::Justification Justification() const { return (gtk::Justification)gtk_text_view_get_justification(*this); }
            void Justification(gtk::Justification mode) { return gtk_text_view_set_justification(*this, (GtkJustification)mode); }

            // misc CUSTOM predicates
            void WordWrap() { WrapMode(WrapWord); }


            // signals
            BUILD_VOID_EVENT(OnCursorMove, "move-cursor");
            BUILD_VOID_EVENT(OnViewportMove, "move-viewport");
    };
}

#endif
