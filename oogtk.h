/**
 \mainpage "OOGtk - A modern GTK C++ lightweighted wrapper"
 OOGtk is a GTK+ object oriented C++ wrapper. It's crossplatform, and it tries to give the user access to the complete API of GTK. The library is header only, so there are no architecture dependant downloads. At the moment the wrapper requires GTK 2.12, but with minor changes it can be made compatible with GTK 2.8+.

 */
#ifndef OOGTK_H

#define OOGTK_H

#include <gtk/gtk.h>
#include <glib/gprintf.h>
#include <assert.h>
#include <string>
#include <iostream>
#include <sstream>
#include <list>
#include <map>
#include <iomanip>
#include "oogdk.h"

namespace gtk
{
    class Widget;

    typedef std::list<Widget *> WidgetList;
   
#define BUILD_EVENT(method, signal) \
            template <typename T, typename R> \
            void method(R (T::*cbk)(), T *base, bool rc = false ) { callback(signal, cbk, base, rc); } \
            template <typename T, typename R, typename J> \
            void method(R (T::*cbk)(J), T *base, J data, bool rc = false ) { callback(signal, cbk, base, data, rc); } \
            template <typename T, typename R> \
            void method(R (T::*cbk)(Widget &), T *base, bool rc = false ) {  callback(signal, cbk, base, rc); } \
            template <typename T, typename R, typename J> \
            void method(R (T::*cbk)(Widget &, J), T *base, J data, bool rc = false ) { callback(signal, cbk, base, data, rc); }
#define BUILD_VOID_EVENT(method, signal) \
            template <typename T> \
            void method(void (T::*cbk)(), T *base) { callback(signal, cbk, base); } \
            template <typename T, typename J> \
            void method(void (T::*cbk)(J), T *base, J data) { callback(signal, cbk, base, data); } \
            template <typename T> \
            void method(void (T::*cbk)(Widget &), T *base) {  callback(signal, cbk, base); } \
            template <typename T, typename J> \
            void method(void (T::*cbk)(Widget &, J), T *base, J data) { callback(signal, cbk, base, data); }

#define BUILD_EVENTED_EVENT(method, signal) \
            template <typename T, typename R> \
            void method(R (T::*cbk)(Event &), T *base, bool rc = false) {  callback(signal, cbk, base, rc); } \
            template <typename T, typename R, typename J> \
            void method(R (T::*cbk)(Event &, J), T *base, J data, bool rc = false) { callback(signal, cbk, base, data, rc); }

    /// This type indicates the current state of a widget; the state determines how the widget is drawn. The GtkStateType enumeration is also used to identify different colors in a GtkStyle for drawing, so states can be used for subparts of a widget as well as entire widgets.
    enum StateType {
        StateNormal = GTK_STATE_NORMAL /**< State during normal operation. */,
        StateActive = GTK_STATE_ACTIVE /**< State of a currently active widget, such as a depressed button. */,
        StatePrelight = GTK_STATE_PRELIGHT /**<	State indicating that the mouse pointer is over the widget and the widget will respond to mouse clicks. */,
        StateSelected = GTK_STATE_SELECTED /**<	State of a selected item, such the selected row in a list. */,
        StateInsensitive = GTK_STATE_INSENSITIVE /**<	State indicating that the widget is unresponsive to user actions. */
    };

    /** The RcStyle class is used to represent a set of information about the appearance of a widget. This can later be composited together with other RcStyle structures to form a Style.
    */
    class RcStyle : public Object
    {
        public:
/// DOXYS_OFF
            operator GtkRcStyle *() const { return GTK_RC_STYLE(obj_); }
            RcStyle(GObject *obj) { Init(obj); }
            RcStyle(GtkRcStyle *obj) { Init((GObject *)obj); }
/// DOXYS_ON
    };

    class Widget : public Object
    {
        public:
/// DOXYS_OFF
            operator  GtkWidget *() const { return GTK_WIDGET(obj_); }
            operator  GdkDrawable *() const { return GDK_DRAWABLE(GTK_WIDGET(obj_)->window); }
/// DOXYS_ON
            Widget &Ref() { g_object_ref(obj_); return *this; }
            void Hide() { gtk_widget_hide(*this); }
            void Show(bool flag = true) { if (flag) gtk_widget_show(*this); else gtk_widget_hide(*this); }
            void ShowAll() { gtk_widget_show_all(*this); }
            void Insensitive() { Sensitive(false); }
            void Sensitive(bool flag = true) { gtk_widget_set_sensitive(*this, flag); }
            void Tooltip(const std::string &tip) { gtk_widget_set_tooltip_markup(*this, tip.c_str()); }

            void SizeRequest(int width, int height) { gtk_widget_set_size_request(*this, width, height); }

            /** Modifies style values on the widget. 
Modifications made using this technique take precedence over style values set via an RC file, however, they will be overriden if a style is explicitely set on the widget using Widget::SetStyle(). The RcStyle structure is designed so each field can either be set or unset, so it is possible, using this function, to modify some style values and leave the others unchanged.

Note that modifications made with this function are not cumulative with previous calls to Widget::ModifyStyle() or with such functions as Widget::ModifyFg(). If you wish to retain previous values, you must first call Widget::ModifierStyle(), make your modifications to the returned style, then call Widget::ModifyStyle() with that style. On the other hand, if you first call Widget::ModifyStyle(), subsequent calls to such functions Widget::ModifyFg() will have a cumulative effect with the initial modifications.
*/
            void ModifyStyle(const RcStyle &style /**< the RcStyle holding the style modifications */) {
                gtk_widget_modify_style(*this, style);
            }

            /** Returns the current modifier style for the widget (As set by Widget::ModifyStyle()). 
If no style has previously set, a new GtkRcStyle will be created with all values unset, and set as the modifier style for the widget. If you make changes to this rc style, you must call Widget::ModifyStyle(), passing in the returned rc style, to make sure that your changes take effect.
*/
            RcStyle ModifierStyle() {
                return RcStyle(gtk_widget_get_modifier_style(*this));
            }

            /** Sets the base color for a widget in a particular state.
All other style values are left untouched. The base color is the background color used along with the text color (see Widget::ModifyText()) for widgets such as Entry and TextView. See also Widget::ModifyStyle().

Note that "no window" widgets (which have the GTK_NO_WINDOW flag set) draw on their parent container's window and thus may not draw any background themselves. This is the case for e.g. Label. To modify the background of such widgets, you have to set the base color on their parent; if you want to set the background of a rectangular area around a label, try placing the label in a EventBox widget and setting the base color on that.
*/
            void ModifyBase(const Color &color, /**< the color to assign */
                            OneOf<GtkStateType, StateType> type = StateNormal /**< 	 the state for which to set the text color */) {
                gtk_widget_modify_base(*this, type, color);
            }
            /** Sets the text color for a widget in a particular state.
All other style values are left untouched. The text color is the foreground color used along with the base color (see Widget::ModifyBase()) for widgets such as Entry and TextView.
\sa  See also Widget::ModifyStyle().
             */
            void ModifyText(const Color &color, /**< the color to assign */
                            OneOf<GtkStateType, StateType> type = StateNormal /**< 	 the state for which to set the text color */) {
                 gtk_widget_modify_text(*this, type, color);
            }
            /** Sets the font to use for a widget. All other style values are left untouched. */
            void ModifyFont(const FontDesc &font /**< the font description to use */) { 
                gtk_widget_modify_font(*this, font); 
            }

            /** Sets the foreground color for a widget in a particular state. All other style values are left untouched. */
            void ModifyFg(const Color &color, /**< the color to assign */
                          OneOf<GtkStateType, StateType> type = StateNormal /**< the state for which to set the text color */
                    ) {
                gtk_widget_modify_fg(*this, type, color);
            }
            /** Sets the background color for a widget in a particular state. 
All other style values are left untouched. See also Widget::ModifyStyle().

Note that "no window" widgets (which have the GTK_NO_WINDOW flag set) draw on their parent container's window and thus may not draw any background themselves. This is the case for e.g. Label. To modify the background of such widgets, you have to set the background color on their parent; if you want to set the background of a rectangular area around a label, try placing the label in a EventBox widget and setting the background color on that.
*/
            void ModifyBg(const Color &color, /**< the color to assign */
                          OneOf<GtkStateType, StateType> type = StateNormal /**< the state for which to set the text color */
                    ) {
                gtk_widget_modify_bg(*this, type, color);
            }
            void GrabFocus() { gtk_widget_grab_focus(*this); }
            void GrabDefault() { gtk_widget_grab_default(*this); }

            BUILD_EVENT(OnFocusIn,  "focus-in-event");
            BUILD_EVENT(OnFocusOut, "focus-out-event");
            BUILD_EVENT(OnExpose,   "expose-event");
            BUILD_EVENTED_EVENT(OnExpose,   "expose-event");
            BUILD_EVENTED_EVENT(OnConfigure,   "configure-event");

            BUILD_EVENTED_EVENT(OnButtonRelease, "button-release-event");
            BUILD_EVENTED_EVENT(OnButtonPress, "button-press-event");
            BUILD_EVENTED_EVENT(OnKeyRelease, "key-release-event");
            BUILD_EVENTED_EVENT(OnKeyPress, "key-press-event");

    };

 
    class Misc : public Widget
    {
        public:
            operator  GtkMisc *() const { return GTK_MISC(Obj()); }

            void Padding(int xpad, int ypad = 0) { gtk_misc_set_padding(*this, xpad, ypad); }
            void Padding(Point &coords) { Padding(coords.x, coords.y); }
            Point Padding() const {
                Point c;
                gtk_misc_get_padding(*this, &c.x, &c.y);
                return c; 
            }

            void Alignment(float xalign, float yalign = 0.5f) { gtk_misc_set_alignment(*this, xalign, yalign); }
            void Alignment(Align &align) { Alignment(align.first, align.second); }
            Align Alignment() const {
                Align a;
                gtk_misc_get_alignment(*this, &a.first, &a.second);
                return a;
            }
    };

   enum IconSize
   {
        IconSizeInvalid = GTK_ICON_SIZE_INVALID,
        IconSizeMenu = GTK_ICON_SIZE_MENU,
        IconSizeSmallToolbar = GTK_ICON_SIZE_SMALL_TOOLBAR,
        IconSizeLargeToolbar = GTK_ICON_SIZE_LARGE_TOOLBAR,
        IconSizeButton = GTK_ICON_SIZE_BUTTON,
        IconSizeDnD = GTK_ICON_SIZE_DND,
        IconSizeDialog = GTK_ICON_SIZE_DIALOG
    };

    class Image : public Misc
    {
        public:
            operator  GtkImage *() const { return GTK_IMAGE(Obj()); }

            Image(GObject *obj) { Init(obj); }

            Image(const std::string &name) {
                Init(gtk_image_new_from_file(name.c_str()));
                Internal(true);
            }
            Image(Pixbuf &pixbuf) {
                Init(gtk_image_new_from_pixbuf(pixbuf));
                Internal(true);
            }
            Image(IconSize size, const char *stockid) {
                Init(gtk_image_new_from_stock(stockid, (GtkIconSize)size));
                Internal(true);
            }
            Image() {
                Init(gtk_image_new());
                Internal(true);
            }

            void Set(const std::string &name) { gtk_image_set_from_file(*this, name.c_str()); }
            void Set(IconSize size, const char *stockid) { gtk_image_set_from_stock(*this, stockid, (GtkIconSize)size); }
            void Set(Pixbuf &pixbuf) { gtk_image_set_from_pixbuf(*this, pixbuf); }
    };

    /// Used for justifying the text inside a Label widget. (See also Alignment). 
    enum Justification
    {
      JustifyLeft = GTK_JUSTIFY_LEFT /**< The text is placed at the left edge of the label. */,
      JustifyRight = GTK_JUSTIFY_RIGHT /**< The text is placed at the right edge of the label. */,
      JustifyCenter = GTK_JUSTIFY_CENTER /**< The text is placed in the center of the label. */,
      JustifyFill = GTK_JUSTIFY_FILL /**< The text is placed is distributed across the label. */
    };

/** A widget that displays a small to medium amount of text.
The Label widget displays a small amount of text. As the name implies, most labels are used to label another widget such as a Button, a MenuItem, or a OptionMenu. 

!Markup (styled text)

To make it easy to format text in a label (changing colors, fonts, etc.), label text can be provided in a simple markup format. Here's how to create a label with a small font:

\example
gtk::Label label("<small>Small text</small>");
\endexample

(See complete documentation of available tags in the Pango manual.)

The markup passed to the Label must be valid; for example, literal </>/& characters must be escaped as &lt;, &gt;, and &amp;. If you pass text obtained from the user, file, or a network to a label, you'll want to escape it with g_markup_escape_text() or g_markup_printf_escaped().

Markup strings are just a convenient way to set the PangoAttrList on a label; Label::Attributes() may be a simpler way to set attributes in some cases. Be careful though; PangoAttrList tends to cause internationalization problems, unless you're applying attributes to the entire string (i.e. unless you set the range of each attribute to [0, G_MAXINT)). The reason is that specifying the start_index and end_index for a PangoAttribute requires knowledge of the exact string being displayed, so translations will cause problems. 

!Selectable labels

Labels can be made selectable with Label::Selectable(bool). Selectable labels allow the user to copy the label contents to the clipboard. Only labels that contain useful-to-copy information — such as error messages — should be made selectable.

!Text layout

A label can contain any number of paragraphs, but will have performance problems if it contains more than a small number. Paragraphs are separated by newlines or other paragraph separators understood by Pango.

Labels can automatically wrap text if you call Label::LineWrap(bool).

Label::Justify() sets how the lines in a label align with one another. If you want to set how the label as a whole aligns in its available space, see Misc::Alignment().

*/
    class Label : public Misc
    {
        public:
/// DOXYS_OFF            
            operator  GtkLabel *() const { return GTK_LABEL(Obj()); }
            Label(GObject *obj) { Init(obj); }
/// DOXYS_ON

            /** Create a new label.
If the optional string parameter is not specified the label is created without any associated text, you'll have to add the text yourself with Label::Set() or Label::operator<<() if you want a marked up label or Label::Text() if you want a plain text label.
            */
            Label(const std::string &label = "" /**< Optional markup for the Label */) {
                Init(gtk_label_new(NULL));

                if (!label.empty())
                    gtk_label_set_markup(*this, label.c_str());

                Internal(true);
            }

            /** Sets the markup of a Label from a standard STL stream. */
            template<typename T>
            Label &operator<<(const T &type) {
                std::ostringstream os(Get(), std::ios_base::app);
                os << type;
                Set(os.str());
                return *this;
            }

            /** Parses str which is marked up with the Pango text markup language, setting the label's text and attribute list based on the parse results. If the str is external data, you may need to escape it with g_markup_escape_text() or g_markup_printf_escaped().
             */
            void Set(const std::string &label /**< 	a markup string (see Pango markup format) */) { 
                gtk_label_set_markup(*this, label.c_str()); 
            }
            /** Fetches the text from a label widget including any embedded underlines indicating mnemonics and Pango markup. If you want only the text you should use Label::Text() instead.
            \return the text of the label widget with Pango markup if present.
             */
            std::string Get() const { return gtk_label_get_label(*this); }
            
            /** Sets the text of the label with markup with printf styled args. */
            void SetF(const char *format, ...) {
                char *buffer;
                va_list va;
                va_start(va, format);
                g_vasprintf(&buffer, format, va);
                va_end(va);
                Set(buffer);
                g_free(buffer);
            }

            /** Sets the text within the Label widget, no markup allowed. 
It overwrites any text that was there before.

This will also clear any previously set mnemonic accelerators.

If you need to add marked up text use Label::Set(const std::string &), Label::SetF() or the << operator.
            */
            void Text(const std::string &plaintext) const { gtk_label_set_text(*this, plaintext.c_str()); }
            /** Fetches the text from a label widget, as displayed on the screen.
This does not include any embedded underlines indicating mnemonics or Pango markup. (See Label::Get() if you need also the markup)
\return A string containing the text contained in the label.
\sa Label::Text(const std::string &)
            */
            std::string Text() const { return gtk_label_get_text(*this); }

            /** Sets the Label to be selectable.
Selectable labels allow the user to select text from the label, for copy-and-paste.
*/
            void Selectable(bool flag /**< true to allow selecting text in the label */) { gtk_label_set_selectable(*this, flag); }
            /** Gets if the label is selectable for copy-n-paste or not 
            \return true if the user can copy text from the label
            */
            bool Selectable() const { return gtk_label_get_selectable(*this); }

            /** Sets the mode used to ellipsize (add an ellipsis: "...") to the text if there is not enough space to render the entire string. */
            void Ellipsize(PangoEllipsizeMode mode /**< a PangoEllipsizeMode */) { 
                gtk_label_set_ellipsize(*this, mode); 
            }
            /** Returns the ellipsizing position of the label. */
            PangoEllipsizeMode Ellipsize() const { return gtk_label_get_ellipsize(*this); }

            /** Returns the justification of the label
             \sa Label::Justify(OneOf<GtkJustification, Justification>)
             \return a Justification or a GtkJustification valid enum value.
             */
            OneOf<GtkJustification, Justification> Justify() const {
                return gtk_label_get_justify(*this); 
            }
            /** Sets the alignment of the lines in the text of the label relative to each other. 
            JustifyLeft is the default value when the widget is first created with any constructor. If you instead want to set the alignment of the label as a whole, use Misc::Alignment() instead. Label::Justify() has no effect on labels containing only a single line.
            */
            void Justify(OneOf<GtkJustification, Justification> mode) {
                gtk_label_set_justify(*this, mode); 
            }

            /** Returns whether lines in the label are automatically wrapped. 
             \sa Label::LineWrap(bool)
             \return true if the lines of the label are automatically wrapped. 
             */
            bool LineWrap() const { return gtk_label_get_line_wrap(*this); }

            /** Toggles line wrapping within the Label widget.
true makes it break lines if text exceeds the widget's size. false lets the text get cut off by the edge of the widget if it exceeds the widget size.

Note that setting line wrapping to true does not make the label wrap at its parent container's width, because GTK+ widgets conceptually can't make their requisition depend on the parent container's size. For a label that wraps at a specific position, set the label's width using Widget::SizeRequest().
            */
            void LineWrap(bool flag /**< the setting */) { gtk_label_set_line_wrap(*this, flag); }

            /** Retrieves the desired width of label, in characters.
            \sa Label::WidthChars(int), Label::MaxWidthChars(int)
            \return the width of the label in characters.
            */
            int WidthChars() const { return gtk_label_get_width_chars(*this); }
            /** Sets the desired width in characters of label to length characters. */
            void WidthChars(int length /**< the new desired width, in characters.  */) { 
                gtk_label_set_width_chars(*this, length); 
            }

            /** Sets the desired maximum width in characters of label to length.
             */
            void MaxWidthChars(int length /**< 	 the new desired maximum width, in characters. */) {
                gtk_label_set_max_width_chars(*this, length);
            }

            /** Sets a PangoAttrList; the attributes in the list are applied to the label text.
The attributes set with this function will be ignored if the "use-underline" or "use-markup" properties are set to true.
*/
            void Attributes(const PangoAttrList &attrs /* a PangoAttrList */) {
                gtk_label_set_attributes(*this, const_cast<PangoAttrList *>(&attrs));
            }
    };

    class Separator : public Widget
    {
    };

    class HSeparator : public Separator
    {
        public:
            HSeparator(GObject *obj) { Init(obj); }
            HSeparator() {
                Init(gtk_hseparator_new());
                Internal(true);
            }
    };

    class VSeparator : public Separator
    {
        public:
            VSeparator(GObject *obj) { Init(obj); }

            VSeparator() {
                Init(gtk_vseparator_new());
                Internal(true);
            }
    };

    class Editable : public Widget
    {
        public:
            int InsertText(const std::string &text, int position) {
                gtk_editable_insert_text(GTK_EDITABLE(Obj()), text.c_str(), text.length(), &position);
                return position;
            }
            void DeleteText(int start, int end = -1) {
                gtk_editable_delete_text(GTK_EDITABLE(Obj()), start, end);
            }

            std::string GetChars(int start, int end = -1) const {
                std::string res;
                
                if (char *result = gtk_editable_get_chars(GTK_EDITABLE(Obj()), start, end)) {
                    res = result;
                    g_free(result);
                }

                return res;
            }

            int Position() const {
                return gtk_editable_get_position(GTK_EDITABLE(Obj()));
            }
            void Position(int pos = - 1) {
                return gtk_editable_set_position(GTK_EDITABLE(Obj()), pos);
            }
    };

    const std::string EntryNumeric = "0123456789";
    const std::string EntryIpAddress = "0123456789.";
    const std::string EntryUpperCase = "ABCDEFGHIJKLMNOPQRSTUVXYWZ";
    const std::string EntryLowerCase = "abcdefghijklmnopqrstuvxywz";
    const std::string EntryLetters = EntryUpperCase + EntryLowerCase;
    const std::string EntryAlphaNumeric = EntryLetters + EntryNumeric;

    class Entry: public Editable
    {
            static void
            insert_filtered_handler(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, std::string *user_data) {
                int i;
                std::string result;

                for (i=0; i < new_text_length; i++) {
                    if (user_data->find(new_text[i]) == std::string::npos)
                        continue;
                    result.push_back(new_text[i]);
                }

                if (!result.empty()) {
                    gtk_signal_handler_block_by_func (GTK_OBJECT (editable),
                            GTK_SIGNAL_FUNC (insert_filtered_handler), user_data);
                    gtk_editable_insert_text (editable, result.c_str(), result.length(), position);
                    gtk_signal_handler_unblock_by_func (GTK_OBJECT (editable),
                            GTK_SIGNAL_FUNC (insert_filtered_handler), user_data);
                }
                gtk_signal_emit_stop_by_name (GTK_OBJECT (editable), "insert_text");
            }
            std::string m_filter;
        public:
            operator  GtkEntry *() const { return GTK_ENTRY(Obj()); }

            Entry(GObject *obj) { Init(obj); }

            Entry(int maxlen = -1) {
                if (maxlen != -1) {
                    Init(gtk_entry_new_with_max_length(maxlen));
                }
                else
                    Init(gtk_entry_new());
                Internal(true);
            }
            template <typename T>
            void Set(const T &t) {
                std::ostringstream os;
                os << t;
                gtk_entry_set_text(*this, os.str().c_str());
            }
            template<typename T>
            Entry &operator<<(const T &t) {
                std::ostringstream os(Get(), std::ios_base::app);
                os << t;
                Set(os.str());
                return *this;
            }
            void Set(const std::string &name) { gtk_entry_set_text(*this, name.c_str()); }
            void SetF(const char *format, ...) {
                char *buffer;
                va_list va;
                va_start(va, format);
                g_vasprintf(&buffer, format, va);
                va_end(va);
                gtk_entry_set_text(*this, buffer);
                g_free(buffer);
            }
            std::string Get() const { return gtk_entry_get_text(*this); }

            // some get/set methods
            void Alignment(float xalign) { gtk_entry_set_alignment(*this, xalign); }
            float Alignment() const { return gtk_entry_get_alignment(*this); }
            void MaxLength(int chars) { gtk_entry_set_max_length(*this, chars); }
            int MaxLength() const { return gtk_entry_get_max_length(*this); }
            void Visibility(bool flag) { gtk_entry_set_visibility(*this, flag); }
            bool Visibility() const { return gtk_entry_get_visibility(*this); }
            uint32_t InvisibleChar() const { return gtk_entry_get_invisible_char(*this); } 
            void InvisibleChar(uint32_t unicode_char) { gtk_entry_set_invisible_char(*this, unicode_char); }
            void WidthChars(int chars) { gtk_entry_set_width_chars(*this, chars); }
            int WidthChars() const { return gtk_entry_get_width_chars(*this); }
            void Editable(bool flag) { gtk_editable_set_editable(GTK_EDITABLE(Obj()), flag); }
            bool Editable() const { return gtk_editable_get_editable(GTK_EDITABLE(Obj())); }
          
            void Accepts(const std::string &chars, int maxlength = -1) {
                if (chars.empty()) {
                    if (!m_filter.empty())
                        g_signal_handlers_disconnect_by_func(Obj(), (void*)insert_filtered_handler, &m_filter);

                    return;
                }

                if (m_filter.empty())
                    g_signal_connect(Obj(), "insert_text", GCallback(insert_filtered_handler), &m_filter);
               
                if (maxlength > 0)
                    MaxLength(maxlength);

                m_filter = chars;
            }

            void ActivatesDefault(bool flag) { gtk_entry_set_activates_default(*this, flag); }
            // callbacks
            BUILD_EVENT(OnActivate, "activate");
            BUILD_EVENT(OnChanged, "changed");
    };

    enum ProgressBarOrientation
    {
      ProgressLeftToRight = GTK_PROGRESS_LEFT_TO_RIGHT,
      ProgressRightToLeft = GTK_PROGRESS_RIGHT_TO_LEFT,
      ProgressBottomToTop = GTK_PROGRESS_BOTTOM_TO_TOP,
      ProgressTopToBottom = GTK_PROGRESS_TOP_TO_BOTTOM
    };

    class ProgressBar : public Widget {
        public:
            operator GtkProgressBar *() const { return GTK_PROGRESS_BAR(Obj()); }
            ProgressBar(GObject *obj) { Init(obj); }
            ProgressBar() {
                Init(gtk_progress_bar_new());
                Internal(true);
            }
            void Pulse() { gtk_progress_bar_pulse(*this); }

            // get/set methods
            void Text(const std::string &text) { gtk_progress_bar_set_text(*this, text.c_str()); }
            void Fraction(double value) { gtk_progress_bar_set_fraction(*this, value); }
            void PulseStep(double value) { gtk_progress_bar_set_pulse_step(*this, value); }
            void Orientation(OneOf<ProgressBarOrientation, GtkProgressBarOrientation> mode) {
                gtk_progress_bar_set_orientation(*this, mode);
            }
            std::string Text() const { return gtk_progress_bar_get_text(*this); }
            double Fraction() const { return gtk_progress_bar_get_fraction(*this); }
            double PulseStep() const { return gtk_progress_bar_get_pulse_step(*this); }
            OneOf<ProgressBarOrientation, GtkProgressBarOrientation> Orientation() const {
                return gtk_progress_bar_get_orientation(*this);
            }
    };

    class DrawingArea : public Widget {
        public:
            operator GtkDrawingArea *() const { return GTK_DRAWING_AREA(Obj()); }
            DrawingArea(GObject *obj) { Init(obj); }
            DrawingArea() {
                Init(gtk_drawing_area_new());
                Internal(true);
            }
    };

    typedef int CbkId;

    class Application
    {
        public:
            typedef std::map<CbkId, AbstractCbk*> CbkMap;
            typedef CbkMap::iterator CbkIt;

            Application(int &a, char **&b) { gtk_init(&a,&b); }

            Application() { gtk_init(NULL, NULL); }
            static void Run() { gtk_main(); }
            static void Flush() { while (gtk_events_pending()) gtk_main_iteration(); }
            static void Quit() { gtk_main_quit(); }

            // helpers for signals that need a fixed answer.
            void QuitLoop() { gtk_main_quit(); }
            bool True() { return true; }
            bool False() { return false; }

            // AddTimer... four variants
            template <typename T, typename R>
            CbkId AddTimer(int msec, R (T::*fnc)(void), T* obj, int rc = true) { return AddTimer(new CbkEvent<T,R>(obj, fnc, rc), msec); }
            template <typename T, typename R, typename J>
            CbkId AddTimer(int msec, R (T::*fnc)(J), T* obj, J data, int rc = true) { return AddTimer(new CbkEvent<T,R,J>(obj, fnc, data, rc), msec); }

            template <typename T>
            CbkId AddOneTimeEvent(int msec, void (T::*fnc)(void), T* obj) { return AddTimer(new CbkEvent<T,void>(obj, fnc, false), msec); }
            template <typename T, typename J>
            CbkId AddOneTimeEvent(int msec, void (T::*fnc)(J), T* obj, J data) { return AddTimer(new CbkEvent<T,void,J>(obj, fnc, data, false), msec); }
    
            // AddIdle... four variants
            template <typename T, typename R>
            CbkId AddIdle(R (T::*fnc)(void), T* obj) { return AddIdle(new CbkEvent<T,R>(obj, fnc)); }
            template <typename T, typename R, typename J>
            CbkId AddIdle(R (T::*fnc)(J), T* obj, J data) { return AddIdle(new CbkEvent<T,R,J>(obj, fnc, data)); }
            template <typename T>
            void RunOneTimeEvent(void (T::*fnc)(void), T* obj) { AddIdle(new CbkEvent<T,void>(obj, fnc, false)); }
            template <typename T, typename J>
            void RunOneTimeEvent(void (T::*fnc)(J), T* obj, J data) { AddIdle(new CbkEvent<T,void, J>(obj, fnc, data, false)); }
           
            void DelSource(CbkId id) {
                CbkIt it = Callbacks().find(id); 

                if (it != Callbacks().end()) {
                    delete it->second;
                    Callbacks().erase(id);
                    g_source_remove(id); 
                }
            }

            void DelTimer(CbkId id) { DelSource(id);  }
            void DelIdle(CbkId id) { DelIdle(id);  }

        private:
            static CbkMap &Callbacks() { static CbkMap objects; return objects; }

            CbkId AddIdle(AbstractCbk *cbk) {
                int id = g_idle_add((gboolean (*)(void*))AbstractCbk::real_callback_0, cbk);
                Callbacks().insert(CbkMap::value_type(id, cbk));
                return id;
            }
            CbkId AddTimer(AbstractCbk *cbk, int msec) {
                int id = g_timeout_add(msec, (gboolean (*)(void*))AbstractCbk::real_callback_0, cbk);
                Callbacks().insert(CbkMap::value_type(id, cbk));
                return id;
            }

    };

    class Builder
    {
        public:
            enum BuildSource {File, String};

            Builder(const std::string &ui = "", BuildSource source = File) {
                builder_ = gtk_builder_new();

                if (!ui.empty()) {                    
                    if (!Load(ui, source)) {
                        g_object_unref(builder_);
                        throw std::runtime_error("Builder error: " + error_);
                    }
                }
            }

            bool Load(const std::string &ui, BuildSource source = File) {
                GError *err = NULL;

                int rc;

                if (source == File)
                    rc = gtk_builder_add_from_file(builder_, ui.c_str(), &err);
                else
                    rc = gtk_builder_add_from_string(builder_, ui.c_str(), ui.length(), &err);

                if (!rc) {
                    if (err)
                        error_ = err->message;
                    else
                        error_ = "generic error";

                    g_free(err);
                    return false;
                }

                if (g_module_supported())
                    gtk_builder_connect_signals (builder_, this);

                return true;
            }

            bool FromString(const std::string &ui) { return Load(ui, String); }
            bool FromFile(const std::string &ui) { return Load(ui, File); }

            ~Builder() { g_object_unref(builder_); }

            Widget *
            Get(const char *label) {
                if (GObject *obj = gtk_builder_get_object(builder_, label)) {
                    return dynamic_cast<Widget *>(Object::Find(obj));
                }
                else
                    return NULL;
            }

            template<typename T> 
            T *Get(const char *label) {
                return dynamic_cast<T *>(Get(label));
            }
        private:
            GtkBuilder *builder_;
            std::string error_;
    };

}

#include "oocont.h"
#include "oowin.h"
#include "oobutton.h"
#include "ootree.h"
#include "ootext.h"
#include "ooui.h"
#include "oomenu.h"

// questo va incluso alla fine per problemi di forward declarations

namespace gtk {
    inline Object *Object::
    Find(GObject *o)
    {
         if (o) { // elencare solo i nodi terminali qui!
            if (void *v = g_object_get_data(o, "object")) {
                return (Object *)v;
            } else if (GTK_IS_ENTRY(o)) {
                return new Entry(o);
            } else if (GTK_IS_FILE_CHOOSER_DIALOG(o)) { // window's derived stuff
                return new FileChooserDialog(o);
            } else if (GTK_IS_MESSAGE_DIALOG(o)) {
                return new MessageDialog(o);
            } else if (GTK_IS_DIALOG(o)) {
                return new Dialog(o);
            } else if (GTK_IS_WINDOW(o)) {
                return new Window(o);
            } else if (GTK_IS_CHECK_MENU_ITEM(o)) { // menu handling
                return new CheckMenuItem(o);
            } else if (GTK_IS_IMAGE_MENU_ITEM(o)) {
                return new ImageMenuItem(o);
            } else if (GTK_IS_SEPARATOR_MENU_ITEM(o)) {
                return new SeparatorMenuItem(o);
            } else if (GTK_IS_MENU_ITEM(o)) { 
                return new MenuItem(o);
            } else if (GTK_IS_MENU_BAR(o)) {
                return new MenuBar(o);
            } else if (GTK_IS_MENU(o)) { 
                return new Menu(o);
            } else if (GTK_IS_RADIO_BUTTON(o)) { // button handling
                return new RadioButton(o);
            } else if (GTK_IS_LINK_BUTTON(o)) {
                return new LinkButton(o);
            } else if (GTK_IS_CHECK_BUTTON(o)) {
                return new CheckButton(o);
            } else if (GTK_IS_TOGGLE_BUTTON(o)) {
                return new ToggleButton(o);
            } else if (GTK_IS_BUTTON(o)) {
                return new Button(o);
            } else if (GTK_IS_STATUSBAR(o)) {
                return new Statusbar(o);
            } else if (GTK_IS_VBUTTON_BOX(o)) {
                return new VButtonBox(o);
            } else if (GTK_IS_HBUTTON_BOX(o)) {
                return new HButtonBox(o);
            } else if (GTK_IS_IMAGE(o)) {
                return new Image(o);
            } else if (GTK_IS_TEXT_MARK(o)) { // textview handing, from ootext.h
                return new TextMark(o);
            } else if (GTK_IS_TEXT_TAG(o)) {
                return new TextTag(o);
            } else if (GTK_IS_TEXT_TAG_TABLE(o)) {
                return new TextTagTable(o);
            } else if (GTK_IS_TEXT_BUFFER(o)) {
                return new TextBuffer(o);
            } else if (GTK_IS_TEXT_VIEW(o)) {
                return new TextView(o);
            } else if (GTK_IS_TREE_SELECTION(o)) { // treeview stuff
                return new TreeSelection(o);
            } else if (GTK_IS_CELL_RENDERER_TEXT(o)) {
                return new CellRendererText(o);
            } else if (GTK_IS_CELL_RENDERER_TOGGLE(o)) {
                return new CellRendererToggle(o);
            } else if (GTK_IS_CELL_RENDERER_PIXBUF(o)) {
                return new CellRendererPixbuf(o);
            } else if (GTK_IS_TREE_VIEW_COLUMN(o)) {
                return new TreeViewColumn(o);
            } else if (GTK_IS_TREE_STORE(o)) {
                return new TreeStore(o);
            } else if (GTK_IS_LIST_STORE(o)) {
                return new ListStore(o);
            } else if (GTK_IS_TREE_VIEW(o)) {
                return new TreeView(o);
            } else if (GTK_IS_SCROLLED_WINDOW(o)) {
                return new ScrolledWindow(o);
            } else if (GTK_IS_SEPARATOR_TOOL_ITEM(o)) { // toolbar stuff
                return new SeparatorToolItem(o);
            } else if (GTK_IS_TOOL_BUTTON(o)) {
                return new ToolButton(o);
            } else if (GTK_IS_TOOL_ITEM(o)) {
                return new ToolItem(o);
            } else if (GTK_IS_TOOLBAR(o)) {
                return new Toolbar(o);
            } else if (GTK_IS_COMBO_BOX(o)) {
                return new ComboBox(o);
            } else if (GTK_IS_ASPECT_FRAME(o)) {
                return new AspectFrame(o);
            } else if (GTK_IS_FRAME(o)) {
                return new Frame(o);
            } else if (GTK_IS_VBOX(o)) {
                return new VBox(o);
            } else if (GTK_IS_HBOX(o)) {
                return new HBox(o);
            } else if (GTK_IS_DRAWING_AREA(o)) {
                return new DrawingArea(o);
            } else if (GTK_IS_TABLE(o)) {
                return new Table(o);
            } else if (GTK_IS_LABEL(o)) {
                return new Label(o);
            } else if (GTK_IS_UI_MANAGER(o)) { // UI Manager stuff
                return new UIManager(o);
            } else if (GTK_IS_ACCEL_GROUP(o)) {
                return new AccelGroup(o);
            } else if (GTK_IS_ACTION_GROUP(o)) {
                return new ActionGroup(o);
            } else if (GTK_IS_ACTION(o)) {
                return new Action(o);
            } else if (GTK_IS_SIZE_GROUP(o)) {
                return new SizeGroup(o);
            } else
                std::cerr << "Undefined type " << g_type_name(GTK_OBJECT_TYPE(o));
        }

        return NULL;
    }

}
// stream operations
inline std::ostream& operator<<(std::ostream& os, const gtk::Label &label) {
    os << label.Get();
    return os;
}
inline std::ostream& operator<<(std::ostream& os, const gtk::Entry &entry) {
    os << entry.Get();
    return os;
}

#endif
