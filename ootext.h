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
                             OneOf<GtkTextSearchFlags, TextSearchFlags> flags = TextSearchTextOnly,
                             TextIter *limit = NULL) {
                return gtk_text_iter_forward_search(&it_, text.c_str(), flags,
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

    class TextBuffer : public Object
    {
        public:
            operator  GtkTextBuffer *() const { return GTK_TEXT_BUFFER(Obj()); }

            TextBuffer(GObject *o) { Init(o); }

            TextBuffer() {
                Init(gtk_text_buffer_new(NULL)); 
                Internal(true);
            }

            TextBuffer(const TextTagTable &table) {
                Init(gtk_text_buffer_new(table)); 
                Internal(true);
            }

            TextTagTable &TagTable() { 
                GtkTextTagTable *t = gtk_text_buffer_get_tag_table(*this);

                if (!t)
                    throw std::runtime_error("Text buffer without tagtable");

                return * (dynamic_cast<TextTagTable *>(Object::Find((GObject *)t)));
            }
            int Lines() const { return gtk_text_buffer_get_line_count(*this); }
            int Characters() const { return gtk_text_buffer_get_char_count(*this); }

            void Insert(const TextIter &position, const std::string &text) {
                gtk_text_buffer_insert(*this, const_cast<TextIter &>(position), 
                                text.c_str(), text.length());
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

            std::string Get(const TextRange &range, bool include_hidden = false) {
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
            TextIter Begin() { TextIter it; gtk_text_buffer_get_start_iter(*this, it); return it; }
            TextIter End() { TextIter it; gtk_text_buffer_get_end_iter(*this, it); return it; }
            TextRange Bounds() { TextRange r; gtk_text_buffer_get_bounds(*this, r.first, r.second); return r; }

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

    class TextView : public Container
    {
        public:
            operator  GtkTextView *() const { return GTK_TEXT_VIEW(Obj()); }

            TextView(GObject *o) { Init(o); }
            TextView() { Init(gtk_text_view_new()); Internal(true); }
            TextView(TextBuffer &buffer) { Init(gtk_text_view_new_with_buffer(buffer)); Internal(true); }

            // scroll functionalities
            void Scroll(const TextMark &mark) { gtk_text_view_scroll_mark_onscreen(*this, mark); }
            void Scroll(const TextMark &mark, double margin, bool use_align = false,
                        Align align = Align(0.5, 0.5)) {
                gtk_text_view_scroll_to_mark(*this, mark, margin, use_align, align.first, align.second); 
            }
            void Scroll(TextIter &it, double margin = 0.0f, bool use_align = false,
                        Align align = Align(0.5, 0.5)) {
                gtk_text_view_scroll_to_iter(*this, it, margin, use_align, align.first, align.second); 
            }

            // buffer handling
            TextBuffer &Buffer() { 
                GtkTextBuffer *b = gtk_text_view_get_buffer(*this); 

                if (!b) {
                    b = gtk_text_buffer_new(NULL);
                    gtk_text_view_set_buffer(*this, b);
                }

                return dynamic_cast<TextBuffer &>(*Object::Find((GObject *)b));
            }
            void Buffer(const TextBuffer &buffer) { gtk_text_view_set_buffer(*this, buffer); }


            // cursor position/text buffer conversions
            OneOf<GtkTextWindowType, TextWindowType> WindowType(GdkWindow *window) {
                return gtk_text_view_get_window_type(*this, window);
            }
            Point BufferToWindow(int x, int y, 
                    OneOf<GtkTextWindowType, TextWindowType> type = TextWindowWidget) {
                Point dest;
                gtk_text_view_buffer_to_window_coords(*this, 
                        type, x, y, 
                        &dest.x, &dest.y);
                return dest;
            }
            Rect Location(const TextIter &it) {
                GdkRectangle rect;
                gtk_text_view_get_iter_location(*this, it, &rect);
                return rect;
            }
            Rect VisibleRect() {
                GdkRectangle rect;
                gtk_text_view_get_visible_rect(*this, &rect);
                return rect; 
            }
            Point WindowToBuffer(int x, int y, 
                     OneOf<GtkTextWindowType, TextWindowType> type = TextWindowWidget) {
                Point dest;
                gtk_text_view_window_to_buffer_coords(*this, 
                        type, x, y, 
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
            OneOf<GtkWrapMode, gtk::WrapMode> WrapMode() const { return gtk_text_view_get_wrap_mode(*this); }
            void WrapMode(OneOf<GtkWrapMode, gtk::WrapMode> mode) { return gtk_text_view_set_wrap_mode(*this, mode); }
            OneOf<GtkJustification, gtk::Justification> Justification() const { return gtk_text_view_get_justification(*this); }
            void Justification(OneOf<GtkJustification, gtk::Justification> mode) { return gtk_text_view_set_justification(*this, mode); }

            // misc CUSTOM predicates
            void WordWrap() { WrapMode(WrapWord); }


            // signals
            BUILD_VOID_EVENT(OnCursorMove, "move-cursor");
            BUILD_VOID_EVENT(OnViewportMove, "move-viewport");
    };
}

#endif
