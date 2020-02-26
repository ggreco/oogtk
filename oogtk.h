/**
 */
#ifndef OOGTK_H

#define OOGTK_H

#include <gtk/gtk.h>
#include <glib/gprintf.h>
#include <assert.h>
#include <stdint.h>
#include <string>
#include <iostream>
#include <sstream>
#include <list>
#include <map>
#include <iomanip>
#include <stdint.h>
#include "oogdk.h"
#include <mutex>

namespace gtk
{
    class Widget;

    /// a list of pointer to Widget objects, used by various OOGtk APIs.
    typedef std::list<Widget *> WidgetList;

#define BUILD_EVENT(method, signal) \
            template <typename T, typename R> \
            void method(R (T::*cbk)(), T *base, bool rc = false ) { callback(signal, cbk, base, rc); } \
            template <typename T, typename R, typename J> \
            void method(R (T::*cbk)(J), T *base, J data, bool rc = false ) { callback(signal, cbk, base, data, rc); } \
            template <typename T, typename R> \
            void method(R (T::*cbk)(Widget &), T *base, bool rc = false ) {  callback(signal, cbk, base, rc); } \
            template <typename T, typename R, typename J> \
            void method(R (T::*cbk)(Widget &, J), T *base, J data, bool rc = false ) { callback(signal, cbk, base, data, rc); } \
            void method(GCallback cbk, void *data = NULL) { g_signal_connect(Obj(), signal, cbk, data); }

#define BUILD_VOID_EVENT(method, signal) \
            template <typename T> \
            void method(void (T::*cbk)(), T *base) { callback(signal, cbk, base); } \
            template <typename T, typename J> \
            void method(void (T::*cbk)(J), T *base, J data) { callback(signal, cbk, base, data); } \
            template <typename T> \
            void method(void (T::*cbk)(Widget &), T *base) {  callback(signal, cbk, base); } \
            template <typename T, typename J> \
            void method(void (T::*cbk)(Widget &, J), T *base, J data) { callback(signal, cbk, base, data); } \
            void method(GCallback cbk, void *data = NULL) { g_signal_connect(Obj(), signal, cbk, data); }

#define BUILD_EVENTED_EVENT(method, signal) \
            template <typename T, typename R> \
            void method(R (T::*cbk)(Event &), T *base, bool rc = false) {  callback(signal, cbk, base, rc); } \
            template <typename T, typename R, typename J> \
            void method(R (T::*cbk)(Event &, J), T *base, J data, bool rc = false) { callback(signal, cbk, base, data, rc); }

    /// This type indicates the current state of a widget; the state determines how the widget is drawn. The StateType enumeration is also used to identify different colors in a GtkStyle for drawing, so states can be used for subparts of a widget as well as entire widgets.
    enum StateType {
        StateNormal = GTK_STATE_NORMAL /**< State during normal operation. */,
        StateActive = GTK_STATE_ACTIVE /**< State of a currently active widget, such as a depressed button. */,
        StatePrelight = GTK_STATE_PRELIGHT /**<	State indicating that the mouse pointer is over the widget and the widget will respond to mouse clicks. */,
        StateSelected = GTK_STATE_SELECTED /**<	State of a selected item, such the selected row in a list. */,
        StateInsensitive = GTK_STATE_INSENSITIVE /**<	State indicating that the widget is unresponsive to user actions. */
    };

    /// The TargetFlags enumeration is used to specify constraints on an entry in a GtkTargetTable.
    enum TargetFlags {
        TargetNotSpecified = 0 /**< Not specified */,
        TargetSameApp = GTK_TARGET_SAME_APP    /**< If this is set, the target will only be selected for drags within a single application. */,
        TargetSameWidget = GTK_TARGET_SAME_WIDGET /**< If this is set, the target will only be selected for drags within a single widget. */,
        TargetOtherApp = GTK_TARGET_OTHER_APP   /**< If this is set, the target will not be selected for drags within a single application. */,
        TargetOtherWidget = GTK_TARGET_OTHER_WIDGET /**< If this is set, the target will not be selected for drags withing a single widget. */
    };

    /// Totally incomplete, just enough to support text only D&D
    struct SelectionData : public GtkSelectionData {
        /// Sets a text selection
        void text(const std::string &txt) { gtk_selection_data_set_text(this, txt.c_str(), (int)txt.length()); }
        /// Gets a text selection
        std::string text() const {
            std::string result;
            if (char *txt = (char*)gtk_selection_data_get_text(const_cast<SelectionData *>(this))) {
                result = txt;
                g_free(txt);
            }
            return result;
        }
        std::vector<std::string> uris() const {
            std::vector<std::string> res;
            if (gchar **t = gtk_selection_data_get_uris(const_cast<SelectionData *>(this))) {
                for (int i = 0; t[i] != NULL; ++i)
                    res.push_back(t[i]);
                g_strfreev(t);
            }
            return res;
        }
    };

    /// The DestDefaults enumeration specifies the various types of action that will be taken on behalf of the user for a drag destination site.
    enum DestDefaults {
      DestDefaultMotion = GTK_DEST_DEFAULT_MOTION /**< 	 If set for a widget, GTK+, during a drag over this widget will check if the drag matches this widget's list of possible targets and actions. GTK+ will then call gdk_drag_status() as appropriate. */,
      DestDefaultHighlight = GTK_DEST_DEFAULT_HIGHLIGHT /**< If set for a widget, GTK+ will draw a highlight on this widget as long as a drag is over this widget and the widget drag format and action are acceptable. */,
      DestDefaultDrop = GTK_DEST_DEFAULT_DROP /**< 	 If set for a widget, when a drop occurs, GTK+ will will check if the drag matches this widget's list of possible targets and actions. If so, GTK+ will call gtk_drag_get_data() on behalf of the widget. Whether or not the drop is successful, GTK+ will call gtk_drag_finish(). If the action was a move, then if the drag was successful, then TRUE will be passed for the delete parameter to gtk_drag_finish(). */,
      DestDefaultAll = GTK_DEST_DEFAULT_ALL  /**< 	 If set, specifies that all default actions should be taken. */
    };

   /** A TargetEntry structure represents a single type of data than can be supplied for by a widget for a selection or for supplied or received during drag-and-drop. It contains a string representing the drag type, a flags field (used only for drag and drop - see GtkTargetFlags), and an application assigned integer ID. The integer ID will later be passed as a signal parameter for signals like "selection_get". It allows the application to identify the target type without extensive string compares.
     */
   struct TargetEntry : public GtkTargetEntry {
       TargetEntry(const char *t, TargetFlags f, int i) {
           target = (gchar*)t; flags = f; info = i;
       }
   };

   /// the size of the stock icon to request.
   enum IconSize
   {
        IconSizeInvalid = GTK_ICON_SIZE_INVALID,
        IconSizeMenu = GTK_ICON_SIZE_MENU /**< the smaller size, for menu items */,
        IconSizeSmallToolbar = GTK_ICON_SIZE_SMALL_TOOLBAR /**< icon size suited for small toolbars */,
        IconSizeLargeToolbar = GTK_ICON_SIZE_LARGE_TOOLBAR /**< icon size suited for large toolbars*/,
        IconSizeButton = GTK_ICON_SIZE_BUTTON /**< icon size suited for buttons */,
        IconSizeDnD = GTK_ICON_SIZE_DND /**< icon size suited for drag and drop */,
        IconSizeDialog = GTK_ICON_SIZE_DIALOG /**< the greatest size, used in dialogs */
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


  /// Tells about certain properties of the widget.
    enum WidgetFlags
    {
        TopLevel =  GTK_TOPLEVEL /**< widgets without a real parent, as there are Windows and Menus have this flag set throughout their lifetime. Toplevel widgets always contain their own GdkWindow. */,
        NoWindow =  GTK_NO_WINDOW /**< indicative for a widget that does not provide its own GdkWindow. Visible action (e.g. drawing) is performed on the parent's GdkWindow.  */,
        Realized =  GTK_REALIZED /**< Set by Widget::Realize(), unset by Widget::Unrealize(). A realized widget has an associated GdkWindow. */,
        Mapped =  GTK_MAPPED /**< Set by Widget::Map(), unset by Widget::Unmap(). Only realized widgets can be mapped. It means that gdk_window_show() has been called on the widgets window(s). */,
        Visible =  GTK_VISIBLE /**< Set by Widget:Show(), unset by Widget:Hide(). Implies that a widget will be mapped as soon as its parent is mapped. */,
        WidgetSensitive =  GTK_SENSITIVE /**< Set and unset by Widget::Sensitive(bool). The sensitivity of a widget determines whether it will receive certain events (e.g. button or key presses). One premise for the widget's sensitivity is to have this flag set. */,
        ParentSensitive =  GTK_PARENT_SENSITIVE /**< Set and unset by Widget::Sensitive(bool) operations on the parents of the widget. This is the second premise for the widget's sensitivity. Once it has Sensitive and ParentSensitive set, its state is effectively sensitive. This is expressed (and can be examined) by the Widget::IsSensitive() method. */,
        CanFocus =  GTK_CAN_FOCUS /**< Determines whether a widget is able to handle focus grabs. */,
        HasFocus =  GTK_HAS_FOCUS /**< Set by Widget::GrabFocus() for widgets that also have GTK_CAN_FOCUS set. The flag will be unset once another widget grabs the focus. */,
        CanDefault =  GTK_CAN_DEFAULT /**<	 The widget is allowed to receive the default action via Widget::GrabDefault().  */,
        HasDefault =  GTK_HAS_DEFAULT /**< 	 The widget currently is receiving the default action. */,
        HasGrab =  GTK_HAS_GRAB /**< Set by gtk_grab_add(), unset by gtk_grab_remove(). It means that the widget is in the grab_widgets stack, and will be the preferred one for receiving events other than ones of cosmetic value. */,
        WidgetRcStyle =  GTK_RC_STYLE /**< Indicates that the widget's style has been looked up through the rc mechanism. It does not imply that the widget actually had a style defined through the rc mechanism. */,
        CompositeChild =  GTK_COMPOSITE_CHILD /**< Indicates that the widget is a composite child of its parent; see gtk_widget_push_composite_child(), gtk_widget_pop_composite_child(). */,
        NoReparent =  GTK_NO_REPARENT /**< Unused since before GTK+ 1.2, will be removed in a future version. */,
        AppPaintable = GTK_APP_PAINTABLE /**< Set and unset by Widget::AppPaintable(bool). Must be set on widgets whose window the application directly draws on, in order to keep GTK+ from overwriting the drawn stuff.  */,
        ReceiveDefault =  GTK_RECEIVES_DEFAULT /**< The widget when focused will receive the default action and have GTK_HAS_DEFAULT set even if there is a different widget set as default. */,
        DoubleBuffered =  GTK_DOUBLE_BUFFERED /**< Set and unset by Widget::DoubleBuffered(bool). Indicates that exposes done on the widget should be double-buffered. */,
        NoShowAll =  GTK_NO_SHOW_ALL /**< */
    };

// only in GTK 2.12+
#if GTK_MINOR_VERSION > 12

/** Tooltip belongs to the new tooltips API that was introduced in GTK+ 2.12 and which deprecates the old tooltips API.

Basic tooltips can be realized simply by using Widget::TooltipText() or Widget::TooltipMarkup() without any explicit tooltip object.

When you need a tooltip with a little more fancy contents, like adding an image, or you want the tooltip to have different contents per TreeView row or cell, you will have to do a little more work:

Set the "has-tooltip" property to TRUE, this will make GTK+ monitor the widget for motion and related events which are needed to determine when and where to show a tooltip.

Connect to the "query-tooltip" signal. This signal will be emitted when a tooltip is supposed to be shown. One of the arguments passed to the signal handler is a Tooltip object. This is the object that we are about to display as a tooltip, and can be manipulated in your callback using functions like Tooltip::Icon(). There are functions for setting the tooltip's markup, setting an image from a stock icon, or even putting in a custom widget.

Return true from your query-tooltip handler. This causes the tooltip to be show. If you return FALSE, it will not be shown.

In the probably rare case where you want to have even more control over the tooltip that is about to be shown, you can set your own GtkWindow which will be used as tooltip window. This works as follows:

Set "has-tooltip" and connect to "query-tooltip" as before.

Use Widget::TooltipWindow(Window &) to set a Window created by you as tooltip window.

In the ::query-tooltip callback you can access your window using Widget::TooltipWindow() and manipulate as you wish. The semantics of the return value are exactly as before, return true to show the window, false to not show it.
*/

    class Tooltip : public Object
    {
        public:
/// DOXYS_OFF
            operator  GtkTooltip *() const { return GTK_TOOLTIP(obj_); }
/// DOXYS_ON

        /// Sets the text of the tooltip to be markup, which is marked up with the Pango text markup language. If markup is not specified, the label will be hidden.
        void Markup(const std::string &markup = "") {   gtk_tooltip_set_markup(*this, markup.empty() ? NULL : markup.c_str()); }
        /// Sets the text of the tooltip to be text. If text is not specified, the label will be hidden. See also Tooltip::Markup().
        void Text(const std::string &text = "") {   gtk_tooltip_set_text(*this, text.empty() ? NULL : text.c_str()); }
        /// Resets the icon of the tooltip (image hidden).
        void Icon() { gtk_tooltip_set_icon(*this, NULL); }
        /// Sets the icon of the tooltip (which is in front of the text) to be pixbuf.
        void Icon(const Pixbuf &pixbuf) { gtk_tooltip_set_icon(*this, pixbuf); }
        /// Sets the icon of the tooltip (which is in front of the text) to be the stock item indicated by stock_id with the size indicated by size. If stock_id is NULL, the image will be hidden.
        void StockIcon(const char *stock_id, IconSize size) { gtk_tooltip_set_icon_from_stock(*this, stock_id, (GtkIconSize) size); }
        /// Replaces the widget packed into the tooltip with custom_widget. custom_widget does not get destroyed when the tooltip goes away. By default a box with a Image and Label is embedded in the tooltip, which can be configured using Tooltip::Markup() and Tooltip::Icon().
        void Custom(const Widget &custom_widget);
        /// Sets the area of the widget, where the contents of this tooltip apply, to be rect (in widget coordinates). This is especially useful for properly setting tooltips on TreeView rows and cells, IconViews, etc.
        void Area(const Rect &rect) { gtk_tooltip_set_tip_area(*this, &rect); }
    };
#endif

/** Base class for all widgets.
Widget introduces to the hiearchy style properties - these are basically object properties that are stored not on the object, but in the style object associated to the widget. Style properties are set in resource files. This mechanism is used for configuring such things as the location of the scrollbar arrows through the theme, giving theme authors more control over the look of applications without the need to write a theme engine in C.

*/
    class Widget : public Object
    {
        public:
/// DOXYS_OFF
            operator  GtkWidget *() const { return GTK_WIDGET(obj_); }
#if GTK_MINOR_VERSION > 15
            operator  Drawable *() const { return GDK_DRAWABLE(gtk_widget_get_window(GTK_WIDGET(obj_))); }
#else
            operator  Drawable *() const { return GDK_DRAWABLE((GTK_WIDGET(obj_)->window)); }
#endif
/// DOXYS_ON
            Widget &Ref() { g_object_ref(obj_); return *this; }
            /// Sets text as the contents of the tooltip. This function will take care of setting GtkWidget:has-tooltip to TRUE and of the default handler for the GtkWidget::query-tooltip signal.
            void TooltipText(const std::string &text) { gtk_widget_set_tooltip_text(*this, text.c_str());}
            /** Sets markup as the contents of the tooltip, which is marked up with the Pango text markup language.

This function will take care of setting GtkWidget:has-tooltip to TRUE and of the default handler for the GtkWidget::query-tooltip signal.

\seealso GtkWidget tooltip-markup property and Tooltip::Markup().
            */
            void TooltipMarkup(const std::string &text) { gtk_widget_set_tooltip_markup(*this, text.c_str()); }

            /// Gets the contents of the tooltip for widget. An empty string is returned if no tooltip is available.
            std::string TooltipMarkup() const {
                std::string res;
                if (gchar *c = gtk_widget_get_tooltip_markup(*this)) {
                    res = c;
                    g_free(c);
                }
                return res;
            }
            /// Gets the contents of the tooltip for widget. An empty string is returned if no tooltip is available.
            std::string TooltipText() const {
                std::string res;
                if (gchar *c = gtk_widget_get_tooltip_text(*this)) {
                    res = c;
                    g_free(c);
                }
                return res;
            }
            /// Reverses the effects of Widget::Show(bool), causing the widget to be hidden (invisible to the user).
            void Hide() { gtk_widget_hide(*this); }
            /** Flags a widget to be displayed.
Any widget that isn't shown will not appear on the screen. If you want to show all the widgets in a container, it's easier to call Widget::ShowAll() on the container, instead of individually showing the widgets.

Remember that you have to show the containers containing a widget, in addition to the widget itself, before it will appear onscreen.

When a toplevel container is shown, it is immediately realized and mapped; other shown widgets are realized and mapped when their toplevel container is realized and mapped.
\sa Widget::ShowAll()
*/
            void Show(bool flag = true /**< if false it causes the widget to hide. */) {
                if (flag) gtk_widget_show(*this); else gtk_widget_hide(*this);
            }
            /// Recursively shows a widget, and any child widgets (if the widget is a container).
            /// \sa Widget::Show(bool)
            void ShowAll() { gtk_widget_show_all(*this); }
            /** Disable sensitivity of a widget. */
            void Insensitive() { Sensitive(false); }
            /** Sets the sensitivity of a widget.
 A widget is sensitive if the user can interact with it. Insensitive widgets are "grayed out" and the user can't interact with them. Insensitive widgets are known as "inactive", "disabled", or "ghosted" in some other toolkits.
 */
            void Sensitive(bool flag = true /**< true to make widget sensitive */) { gtk_widget_set_sensitive(*this, flag); }
            /** Sets markup as the contents of the tooltip, which is marked up with the Pango text markup language.
This function will take care of setting Widget property "has-tooltip" to true and of the default handler for the Widget "query-tooltip" signal.
            */
            void Tooltip(const std::string &tip) { gtk_widget_set_tooltip_markup(*this, tip.c_str()); }

/** Sets the minimum size of a widget.
The widget's size request will be width by height. You can use this function to force a widget to be either larger or smaller than it normally would be.

In most cases, Window::DefaultSize() is a better choice for toplevel windows than this function; setting the default size will still allow users to shrink the window. Setting the size request will force them to leave the window at least as large as the size request. When dealing with window sizes, Window::GeometryHints() can be a useful function as well.

Note the inherent danger of setting any fixed size - themes, translations into other languages, different fonts, and user action can all change the appropriate size for a given widget. So, it's basically impossible to hardcode a size that will always be correct.

The size request of a widget is the smallest size a widget can accept while still functioning well and drawing itself correctly. However in some strange cases a widget may be allocated less than its requested size, and in many cases a widget may be allocated more space than it requested.

If the size request in a given direction is -1 (unset), then the "natural" size request of the widget will be used instead.

Widgets can't actually be allocated a size less than 1 by 1, but you can pass 0,0 to this function to mean "as small as possible."
*/
            void SizeRequest(int width /**< width widget should request, or -1 to unset*/,
                             int height /**< height widget should request, or -1 to unset */) { gtk_widget_set_size_request(*this, width, height); }
/** Shortcut for Widget::SizeRequest(int,int) */
            void SizeRequest(const Point &size /**< a point with widget dimensions (x,y)*/) { gtk_widget_set_size_request(*this, size.x, size.y); }

            // returns a Point containing width and height for the CURRENT size of the widget
          //  Point Allocation() const { return Point(GTK_WIDGET(Obj())->allocation.width, GTK_WIDGET(Obj())->allocation.height); }
            Rect Allocation() const {
                if (const GtkWidget *w = GTK_WIDGET(Obj()))
                    return Rect(w->allocation.x, w->allocation.y, w->allocation.width,w->allocation.height);
                else
                    return Rect(0,0,0,0);
            }

#if GTK_MINOR_VERSION > 16
            void CanDefault(bool flag) { gtk_widget_set_can_default(*this, flag); }
            bool CanDefault() const { return gtk_widget_get_can_default(*this); }
            void CanFocus(bool flag) { gtk_widget_set_can_focus(*this, flag); }
            bool CanFocus() const { return gtk_widget_get_can_focus(*this); }
            bool HasDefault() const { return gtk_widget_has_default(*this); }
#endif

            /** Modifies style values on the widget.
Modifications made using this technique take precedence over style values set via an RC file, however, they will be overriden if a style is explicitely set on the widget using Widget::SetStyle(). The RcStyle structure is designed so each field can either be set or unset, so it is possible, using this function, to modify some style values and leave the others unchanged.

Note that modifications made with this function are not cumulative with previous calls to Widget::ModifyStyle() or with such functions as Widget::ModifyFg(). If you wish to retain previous values, you must first call Widget::ModifierStyle(), make your modifications to the returned style, then call Widget::ModifyStyle() with that style. On the other hand, if you first call Widget::ModifyStyle(), subsequent calls to such functions Widget::ModifyFg() will have a cumulative effect with the initial modifications.
*/
            void ModifyStyle(const RcStyle &style /**< the RcStyle holding the style modifications */) {
                gtk_widget_modify_style(*this, style);
            }

/**
    A convenience method that uses the theme settings for widget to look up stock_id and render it to a Pixbuf. stock_id should be a stock icon ID such as GTK_STOCK_OPEN or GTK_STOCK_OK. size should be a size in IconSize enum.
*/
            Pixbuf RenderIcon(const char *stock_id, IconSize size) {
                return Pixbuf((GObject*)gtk_widget_render_icon(*this, stock_id, (GtkIconSize)size, NULL));
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
                            StateType type = StateNormal /**< 	 the state for which to set the text color */) {
                gtk_widget_modify_base(*this, (GtkStateType)type, color);
            }
            /** Sets the text color for a widget in a particular state.
All other style values are left untouched. The text color is the foreground color used along with the base color (see Widget::ModifyBase()) for widgets such as Entry and TextView.
\sa  See also Widget::ModifyStyle().
             */
            void ModifyText(const Color &color, /**< the color to assign */
                            StateType type = StateNormal /**< 	 the state for which to set the text color */) {
                 gtk_widget_modify_text(*this, (GtkStateType)type, color);
            }
            /** Sets the font to use for a widget. All other style values are left untouched. */
            void ModifyFont(const FontDesc &font /**< the font description to use */) {
                gtk_widget_modify_font(*this, font);
            }

            /** Sets the foreground color for a widget in a particular state. All other style values are left untouched. */
            void ModifyFg(const Color &color, /**< the color to assign */
                          StateType type = StateNormal /**< the state for which to set the text color */
                    ) {
                gtk_widget_modify_fg(*this, (GtkStateType)type, color);
            }
            /** Sets the background color for a widget in a particular state.
All other style values are left untouched. See also Widget::ModifyStyle().

Note that "no window" widgets (which have the NoWindow flag set) draw on their parent container's window and thus may not draw any background themselves. This is the case for e.g. Label. To modify the background of such widgets, you have to set the background color on their parent; if you want to set the background of a rectangular area around a label, try placing the label in a EventBox widget and setting the background color on that.
*/
            void ModifyBg(const Color &color, /**< the color to assign */
                          StateType type = StateNormal /**< the state for which to set the text color */
                    ) {
                gtk_widget_modify_bg(*this, (GtkStateType)type, color);
            }
            /** Causes widget to have the keyboard focus for the Window it's inside.
Widget must be a focusable widget, such as a Entry; something like Frame won't work. (More precisely, it must have the CanFocus flag set.)
*/
            void GrabFocus() { gtk_widget_grab_focus(*this); }
            /** Causes widget to become the default widget.
Widget must have theCanDefault flag set; typically you have to set this flag yourself by calling Widget::Flags(uint32_t). The default widget is activated when the user presses Enter in a window. Default widgets must be activatable, that is, Widget::Activate() should affect them.
*/
            void GrabDefault() { gtk_widget_grab_default(*this); }

            /** Creates the GDK (windowing system) resources associated with a widget.
The GdkWindow/GdkDrawable associated with the widget will be created when a widget is realized. Normally realization happens implicitly; if you show a widget and all its parent containers, then the widget will be realized and mapped automatically.

Realizing a widget requires all the widget's parent widgets to be realized; calling Widget::Realize() realizes the widget's parents in addition to widget itself. If a widget is not yet inside a toplevel window when you realize it, bad things will happen.

This function is primarily used in widget implementations, and isn't very useful otherwise. Many times when you think you might need it, a better approach is to connect to a signal that will be called after the widget is realized automatically, such as "expose-event".
             */
            void Realize() { gtk_widget_realize(*this); }
            /** Unrealize a widget destroying associated GDK resources.
This function is only useful in widget implementations. Causes a widget to be unrealized (frees all GDK resources associated with the widget, such as the associated GdkWindow). */
            void Unrealize() { gtk_widget_unrealize(*this); }
            /** This function is only for use in widget implementations. Causes a widget to be mapped if it isn't already. */
            void Map() { gtk_widget_map(*this); }
            /** This function is only for use in widget implementations. Causes a widget to be unmapped if it's currently mapped. */
            void Unmap() { gtk_widget_unmap(*this); }

            /// remove the widget from his parent container
            void Unparent() { gtk_widget_unparent(*this); }

            /** Returns the sensitivity state of a widget
            \return true if the widget is sensitive.
            */
            bool IsSensitive() const { return GTK_WIDGET_IS_SENSITIVE(GTK_WIDGET(Obj())); }
            /** Returns the widget focus.
            \return true if the widget has focus
            */
            bool HasFocus() const { return GTK_WIDGET_HAS_FOCUS(GTK_WIDGET(Obj())); }

            /// Sets Pixbuf as background image for this widget
            void BackImage(const Pixbuf &pix) {
                GtkStyle *s = gtk_style_new();
                s->bg_pixmap[0] = pix.Pixmap();
                gtk_widget_set_style(*this, s);
            }
            /** Returns the active WidgetFlags for the widget.
            \return a bitmask of WidgetFlags
            */
            uint32_t Flags() const { return GTK_OBJECT_FLAGS(Obj()); }
            /// Sets one or more WidgetFlags for the widget.
            void SetFlags(uint32_t flags /**< flags to be set */) const { GTK_WIDGET_SET_FLAGS(GTK_WIDGET(Obj()), flags); }
            /// Unsets one or more WidgetFlags for the widget.
            void UnsetFlags(uint32_t flags /**< flags to be unset */) const { GTK_WIDGET_SET_FLAGS(GTK_WIDGET(Obj()), flags); }
            /** Activates a widget.
For widgets that can be "activated" (buttons, menu items, etc.) this function activates them.
Activation is what happens when you press Enter on a widget during key navigation. If widget isn't activatable, the function returns false.
\return false if the widget is not activable.
            */
            bool Activate() { return gtk_widget_activate(*this); }

/** Sets the event mask (see GdkEventMask) for a widget.

The event mask determines which events a widget will receive. Keep in mind that different widgets have different default event masks, and by changing the event mask you may disrupt a widget's functionality, so be careful. This function must be called while a widget is unrealized. Consider Widget::AddEvents() for widgets that are already realized, or if you want to preserve the existing event mask. This function can't be used with NoWindow widgets; to get events on those widgets, place them inside a EventBox and receive events on the event box.
*/
            void SetEvents(guint events /**< an event mask, see EventMask */) {
                gtk_widget_set_events(*this, events);
            }

/** Adds the events in the bitfield events to the event mask for widget.
\sa Widget::SetEvents()
*/
            void AddEvents(guint events /**< an event mask, see EventMask */) {
                gtk_widget_add_events(*this, events);
            }

/** Invalidates the rectangular area of widget defined by the input Rect.
The area is invalidated for the widget's window and all its child windows. Once the main loop becomes idle (after the current batch of events has been processed, roughly), the window will receive expose events for the union of all regions that have been invalidated.

Normally you would only use this function in widget implementations. You might also use it, or gdk_window_invalidate_rect() directly, to schedule a redraw of a GtkDrawingArea or some portion thereof.

The advantage of adding to the invalidated region compared to simply drawing immediately is efficiency; using an invalid region ensures that you only have to redraw one time.
*/
            void QueueDraw(const Rect &area = Rect() /**< The rectangle that has to be redrawn, if not specified queues the redraw of the entire widget */) {
                if (!area.width && !area.height)
                    gtk_widget_queue_draw(*this);
                else
                    gtk_widget_queue_draw_area(*this, area.x, area.y,
                                                      area.width, area.height);
            }


/**  Sets a widget as a potential drop destination, and adds default behaviors.

The default behaviors listed in flags have an effect similar to installing default handlers for the widget's drag-and-drop signals ("drag-motion", "drag-drop", ...). They all exist for convenience. When passing GTK_DEST_DEFAULT_ALL for instance it is sufficient to connect to the widget's "drag-data-received" signal to get primitive, but consistent drag-and-drop support.

Things become more complicated when you try to preview the dragged data, as described in the documentation for "drag-motion". The default behaviors described by flags make some assumptions, that can conflict with your own signal handlers. For instance GTK_DEST_DEFAULT_DROP causes invokations of gdk_drag_status() in the context of "drag-motion", and invokations of gtk_drag_finish() in "drag-data-received". Especially the later is dramatic, when your own "drag-motion" handler calls gtk_drag_get_data() to inspect the dragged data.
*/
            void EnableDragDest(int32_t dest_flags /**< Flags as specified by DestDefaults enumeration */,
                                const std::vector<TargetEntry> &entries /**<  a vector of TargetEntry indicating the drop types that this widget will accept, or an empty vector if none. */,
                                int32_t actions /**< a bitmask of possible actions for a drop onto this widget from the enumeation DragAction */) {
                gtk_drag_dest_set(*this, (GtkDestDefaults)dest_flags, &entries[0], entries.size(), (GdkDragAction)actions);
            }

            void DisableDragDest() { gtk_drag_dest_unset(*this); }

            /// Tells the widget to emit ::drag-motion and ::drag-leave events regardless of the targets and the DestDefaultMotion flag.
            void DragMotion(bool value) { gtk_drag_dest_set_track_motion(*this, value); }

/** Enable/disable double buffering for this object.

Widgets are double buffered by default; you can use this function to turn off the buffering. "Double buffered" simply means that gdk_window_begin_paint_region() and gdk_window_end_paint() are called automatically around expose events sent to the widget. gdk_window_begin_paint() diverts all drawing to a widget's window to an offscreen buffer, and gdk_window_end_paint() draws the buffer to the screen. The result is that users see the window update in one smooth step, and don't see individual graphics primitives being rendered.

In very simple terms, double buffered widgets don't flicker, so you would only use this function to turn off double buffering if you had special needs and really knew what you were doing.

Note: if you turn off double-buffering, you have to handle expose events, since even the clearing to the background color or pixmap will not happen automatically (as it is done in gdk_window_begin_paint()).
*/
            void DoubleBuffered(bool flag /**< true to double-buffer a widget */ ) {
                gtk_widget_set_double_buffered(*this, flag);
            }

            template <typename T, typename J>
            void OnDragReceive(void (T::*fnc)(Widget &, SelectionData &, J), T *obj, J arg) {
                AbstractDragCbk *cbk = new CbkDrag<T,J>(obj, fnc, arg);
                g_signal_connect(Obj(), "drag-data-received", (GCallback)AbstractDragCbk::real_callback_received, cbk);
            }
            BUILD_EVENT(OnDragMotion, "drag-motion");
            BUILD_EVENT(OnDragLeave, "drag-leave");

            template <typename T, typename J>
            void OnDragGet(void (T::*fnc)(Widget &, SelectionData &, J), T *obj, J arg) {
                AbstractDragCbk *cbk = new CbkDrag<T,J>(obj, fnc, arg);
                g_signal_connect(Obj(), "drag-data-get", (GCallback)AbstractDragCbk::real_callback_get, cbk);
            }
            template <typename T>
            void OnDragReceive(void (T::*fnc)(Widget &, SelectionData &), T *obj) {
                g_signal_connect(Obj(), "drag-data-received", (GCallback)AbstractDragCbk::real_callback_received,
                        new CbkDrag<T>(obj, fnc));
            }
            template <typename T>
            void OnDragGet(void (T::*fnc)(Widget &, SelectionData &), T *obj) {
                g_signal_connect(Obj(), "drag-data-get", (GCallback)AbstractDragCbk::real_callback_get,
                        new CbkDrag<T>(obj, fnc));
            }

            BUILD_EVENT(OnFocusIn,  "focus-in-event");
            BUILD_EVENT(OnFocusOut, "focus-out-event");
            BUILD_EVENT(OnLeave, "leave-notify-event");
            BUILD_EVENT(OnExpose,   "expose-event");
            BUILD_EVENT(OnRealize, "realize");

            BUILD_EVENTED_EVENT(OnMotion, "motion-notify-event");
            BUILD_EVENTED_EVENT(OnExpose,   "expose-event");
            BUILD_EVENTED_EVENT(OnConfigure,   "configure-event");

            BUILD_EVENTED_EVENT(OnButtonRelease, "button-release-event");
            BUILD_EVENTED_EVENT(OnButtonPress, "button-press-event");
            BUILD_EVENTED_EVENT(OnKeyRelease, "key-release-event");
            BUILD_EVENTED_EVENT(OnKeyPress, "key-press-event");
            BUILD_EVENTED_EVENT(OnScroll, "scroll-event");

    };

#if GTK_MINOR_VERSION > 12
    inline void Tooltip::Custom(const Widget &custom_widget) { gtk_tooltip_set_custom(*this, custom_widget); }
#endif

/** Base class for widgets with alignments and padding.
The Misc widget is an abstract widget which is not useful itself, but is used to derive subclasses which have alignment and padding attributes.

The horizontal and vertical padding attributes allows extra space to be added around the widget.

The horizontal and vertical alignment attributes enable the widget to be positioned within its allocated area. Note that if the widget is added to a container in such a way that it expands automatically to fill its allocated area, the alignment settings will not alter the widgets position.
*/
    class Misc : public Widget
    {
        public:
/// DOXYS_OFF
            operator  GtkMisc *() const { return GTK_MISC(Obj()); }
/// DOXYS_ON
            /// Sets the amount of space to add around the widget.
            void Padding(int xpad /**< 	the amount of space to add on the left and right of the widget, in pixels. */,
                         int ypad = 0 /**< 	the amount of space to add on the top and bottom of the widget, in pixels, defaults to 0. */) {
                gtk_misc_set_padding(*this, xpad, ypad);
            }
            /// Sets the amount of space to add around the widget.
            void Padding(const Point &coords /**< A Point containing the padding values. */) { Padding(coords.x, coords.y); }
            /// Gets the padding in the X and Y directions of the widget.
            /// \return a Point containing the padding value for X and Y coordinates.
            Point Padding() const {
                Point c;
                gtk_misc_get_padding(*this, &c.x, &c.y);
                return c;
            }

            /// Sets the alignment of the widget.
            void Alignment(float xalign /**< the horizontal alignment, from 0 (left) to 1 (right). */,
                           float yalign = 0.5f /**< the vertical alignment, from 0 (top) to 1 (bottom). */) {
                gtk_misc_set_alignment(*this, xalign, yalign);
            }
            /// Sets the alignment of the widget.
            void Alignment(Align &align /**< the horizontal and vertical alignment, from 0, 0 (top left) to 1, 1 (bottom right)
                    */) {
                Alignment(align.first, align.second);
            }
            /// Gets the X and Y alignment of the widget within its allocation.
            /// \return an Align containing X alignment in first and Y alignment in second (from 0 to 1).
            Align Alignment() const {
                Align a;
                gtk_misc_get_alignment(*this, &a.first, &a.second);
                return a;
            }
    };

    /** Describes the image data representation used by a Image. If you want to get the image from the widget, you can only get the currently-stored representation. e.g. if the Image::StorageType() returns ImageBixbuf, then you can call Image::Pixbuf() but not Image::Stock(). For empty images, you can request any storage type (call any of the "get" methods), but they will all return NULL values.
    */
    enum ImageType
    {
        ImageEmpty = GTK_IMAGE_EMPTY /**< there is no image displayed by the widget */,
        ImagePixmap = GTK_IMAGE_PIXMAP /**< the widget contains a GdkPixmap */,
        ImageImage = GTK_IMAGE_IMAGE /**< 	the widget contains a GdkImage */,
        ImagePixbuf = GTK_IMAGE_PIXBUF /**< 	the widget contains a Pixbuf */,
        ImageStock = GTK_IMAGE_STOCK /**< 	the widget contains a stock icon name */,
        ImageIconSet = GTK_IMAGE_ICON_SET /**< 	the widget contains a GtkIconSet */,
        ImageAnimation = GTK_IMAGE_ANIMATION /**< the widget contains a PixbufAnimation */,
        ImageIconName = GTK_IMAGE_ICON_NAME /**< 	the widget contains a named icon. This image type was added in GTK+ 2.6 */
    };

    /** A widget displaying an image
The Image widget displays an image. Various kinds of object can be displayed as an image; most typically, you would load a Pixbuf ("pixel buffer") from a file, and then display that. There's a convenience constructor to do this as follows:

\code
    gtk::Image image("myfile.png");
\endcode

If the file isn't loaded successfully, the image will contain a "broken image" icon similar to that used in many web browsers. If you want to handle errors in loading the file yourself, for example by displaying an error message, then load the image with Pixbuf methods, then create the Image with the Pixbuf constructor.

The image file may contain an animation, if so the Image will display an animation (PixbufAnimation) instead of a static image.

Image is a subclass of Misc, which implies that you can align it (center, left, right) and add padding to it, using Misc methods.

Image is a "no window" widget (has no GdkWindow of its own), so by default does not receive events. If you want to receive events on the image, such as button clicks, place the image inside a EventBox, then connect to the event signals on the event box.

!Handling button press events on a Image
\example
class MyWin : public gtk::Window
{
        bool onpress(gtk::Event &e) {
            if (gtk::EventButton *b = e)
                g_print("Event box clicked at coords: %f, %f\n");
        }
    public:
        MyWin() : gtk::Window("EventBox button test") {
            gtk::Image img("myfile.png");
            gtk::EventBox eb(img);
            eb.OnButtonPress();
            Child(eb);
            ShowAll();
        }
};
\endexample

When handling events on the event box, keep in mind that coordinates in the image may be different from event box coordinates due to the alignment and padding settings on the image (see GMisc). The simplest way to solve this is to set the alignment to 0.0 (left/top), and set the padding to zero. Then the origin of the image will be the same as the origin of the event box.

Sometimes an application will want to avoid depending on external data files, such as image files. GTK+ comes with a program to avoid this, called gdk-pixbuf-csource. This program allows you to convert an image into a C variable declaration, which can then be loaded into a Pixbuf.

*/
    class Image : public Misc
    {
        public:
/// DOXYS_OFF
            operator  GtkImage *() const { return GTK_IMAGE(Obj()); }

            Image(GObject *obj) { Init(obj); }
/// DOXYS_ON
/** Creates a new Image displaying the file filename.
If the file isn't found or can't be loaded, the resulting Image will display a "broken image" icon. The constructor never fail or throw exceptions, it always returns a valid Image widget.

If the file contains an animation, the image will contain an animation.

If you need to detect failures to load the file, use Pixbuf API to load the file yourself, then create the Image from the pixbuf.

The storage type (Image::StorageType()) of the returned image is not defined, it will be whatever is appropriate for displaying the file.
*/
            Image(const std::string &name /**< a filename */) {
                Init(gtk_image_new_from_file(name.c_str()));
                Internal(true);
            }
            /** Creates a new Image displaying pixbuf.
The Image does not assume a reference to the pixbuf; you still need to unref it if you own references. Image will add its own reference rather than adopting yours.

\example
// valid code since Image keeps a reference to the Pixbuf
gtk::Image *create_image(const std::string &name) {
    try {
        gtk::Pixbuf pix(name);

        return new Image(pix);
    }
    catch (std::runtime_error &e) {
        return NULL;
    }
}
\endexample

Note that this function just creates an Image from the pixbuf. The Image created will not react to state changes.
*/
            Image(const Pixbuf &pixbuf) {
                Init(gtk_image_new_from_pixbuf(pixbuf));
                Internal(true);
            }
/** Creates a Image displaying a stock icon.
Sample stock icon names are GTK_STOCK_OPEN, GTK_STOCK_QUIT. Sample stock sizes are IconSizeMenu, IconSizeSmallToolbar. If the stock icon name isn't known, the image will be empty. You can register your own stock icon names, see gtk_icon_factory_add_default() and gtk_icon_factory_add().
*/
            Image(IconSize size, const char *stockid) {
                Init(gtk_image_new_from_stock(stockid, (GtkIconSize)size));
                Internal(true);
            }
/** Creates an empty Image, you can set its contents with Image::Set() family.
 */
            Image() {
                Init(gtk_image_new());
                Internal(true);
            }


            /** Gets the type of representation being used by the Image to store image data.
If the Image has no image data, the return value will be ImageEmpty.
\return a ImageType value specific for the object.
*/
            ImageType StorageType() const { return (ImageType)gtk_image_get_storage_type(*this); }
/** Sets the Image with the image pointed by the file name.
Existing contents are unrefrenced and freed, if you don't own other references to them.
\sa Image::Image(const std::string &)
*/
            void Set(const std::string &name /**< a filename */) { gtk_image_set_from_file(*this, name.c_str()); }
/** Sets the Image with a stock item of a particular size.
Existing contents are unrefrenced and freed, if you don't own other references to them.
\sa Image::Image(IconSize, const char*)
*/
            void Set(IconSize size /* a valid IconSize value */,
                     const char *stockid /**< A valid stock icon name */) {
                gtk_image_set_from_stock(*this, stockid, (GtkIconSize)size);
            }
            /** Set the Image contents with a Pixbuf.
Existing contents are unrefrenced and freed, if you don't own other references to them.
\sa
*/
            void Set(Pixbuf &pixbuf /**< A pixbuf */) { gtk_image_set_from_pixbuf(*this, pixbuf); }
/// Resets the image to be empty.
            void Clear() { gtk_image_clear(*this); }
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
             \sa Label::Justify(Justification )
             \return a Justification or a GtkJustification valid enum value.
             */
            Justification Justify() const {
                return (Justification)gtk_label_get_justify(*this);
            }
            /** Sets the alignment of the lines in the text of the label relative to each other.
            JustifyLeft is the default value when the widget is first created with any constructor. If you instead want to set the alignment of the label as a whole, use Misc::Alignment() instead. Label::Justify() has no effect on labels containing only a single line.
            */
            void Justify(Justification mode) {
                gtk_label_set_justify(*this, (GtkJustification)mode);
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

    /** Base class for HSeparator and VSeparator
The Separator widget is an abstract class, used only for deriving the subclasses HSeparator and VSeparator.
*/
    struct Separator : public Widget
    {
    };
/** A horizontal separator.
The HSeparator widget is a horizontal separator, used to group the widgets within a window. It displays a horizontal line with a shadow to make it appear sunken into the interface.

!Note
The HSeparator widget is not used as a separator within menus. To create a separator in a menu create an empty SeparatorMenuItem widget and add it to the menu with MenuShell::Append().
\sa VSeparator widget.
*/
    class HSeparator : public Separator
    {
        public:
/// DOXYS_OFF
            HSeparator(GObject *obj) { Init(obj); }
/// DOXYS_ON
            /// Creates a new HSeparator.
            HSeparator() {
                Init(gtk_hseparator_new());
                Internal(true);
            }
    };
/** A vertical separator.
The VSeparator widget is a vertical separator, used to group the widgets within a window. It displays a vertical line with a shadow to make it appear sunken into the interface.
*/
    class VSeparator : public Separator
    {
        public:
/// DOXYS_OFF
            VSeparator(GObject *obj) { Init(obj); }
/// DOXYS_ON
            /// Creates a new VSeparator.
            VSeparator() {
                Init(gtk_vseparator_new());
                Internal(true);
            }
    };
/** Interface for text-editing widgets.
The Editable interface is an interface which should be implemented by text editing widgets, such as Entry. It contains functions for generically manipulating an editable widget, a large number of action signals used for key bindings, and several signals that an application can connect to to modify the behavior of a widget.
*/
    class Editable : public Widget
    {
/// DOXYS_OFF
            static void insert_text_handler (GtkEditable *editable,
                    const gchar *text, gint length, gint *position, gpointer data)
            {
                gchar *result = NULL;

                switch(GPOINTER_TO_INT(data)) {
                    case 1:
                        result = g_utf8_strup (text, length);
                        break;
                    case 2:
                        result = g_utf8_strdown (text, length);
                        break;
                }
                if (!result) return;

                g_signal_handlers_block_by_func (editable, (gpointer) insert_text_handler, data);
                gtk_editable_insert_text (editable, result, length, position);
                g_signal_handlers_unblock_by_func (editable, (gpointer) insert_text_handler, data);
                g_signal_stop_emission_by_name (editable, "insert_text");

                g_free (result);
            }
/// DOXYS_ON
        public:
            /** Inserts text at a given position.
            \return the position after the newly inserted text.
             */
            int InsertText(const std::string &text /**< the text to insert. */,
                           int position /**< The position where to insert the text */) {
                gtk_editable_insert_text(GTK_EDITABLE(Obj()), text.c_str(), text.length(), &position);
                return position;
            }

            /** Deletes a sequence of characters.
The characters that are deleted are those characters at positions from start_pos up to, but not including end_pos. If end_pos is negative or if it's not specified, then the the characters deleted will be those characters from start_pos to the end of the text.
*/
            void DeleteText(int start_pos /**< 	the starting position. */,
                            int end_pos = -1 /**< the optional end position. */) {
                gtk_editable_delete_text(GTK_EDITABLE(Obj()), start_pos, end_pos);
            }

            /** Retrieves a sequence of characters.
The characters that are retrieved are those characters at positions from start_pos up to, but not including end_pos. If end_pos is negative or if it's not specified, then the the characters retrieved will be those characters from start_pos to the end of the text.
*/
            std::string GetChars(int start_pos    /**< the starting position. */,
                                 int end_pos = -1 /**< the optional end position. */) const {
                std::string res;

                if (char *result = gtk_editable_get_chars(GTK_EDITABLE(Obj()), start_pos, end_pos)) {
                    res = result;
                    g_free(result);
                }

                return res;
            }
/** Retrieves the current cursor position.
\return the position of the cursor. The cursor is displayed before the character with the given (base 0) index in the widget. The value will be less than or equal to the number of characters in the widget. Note that this position is in characters, not in bytes.
*/
            int Position() const {
                return gtk_editable_get_position(GTK_EDITABLE(Obj()));
            }
            /** Sets the cursor position. */
            void Position(int pos = - 1 /**< the position of the cursor. The cursor is displayed before the character with the given (base 0) index in the widget. The value must be less than or equal to the number of characters in the widget. A value of -1 or no value indicates that the position should be set after the last character in the entry. Note that this position is in characters, not in bytes. */
                    ) {
                return gtk_editable_set_position(GTK_EDITABLE(Obj()), pos);
            }

            /** Filter Editable text input so that every character typed is uppercase */
            void ForceUppercase() {
                RemoveForce();
                g_signal_connect(Obj(), "insert_text", GCallback(insert_text_handler), GINT_TO_POINTER(1));
            }
            /** Filter Editable text input so that every character typed is lowercase */
            void ForceLowercase() {
                RemoveForce();
                g_signal_connect(Obj(), "insert_text", GCallback(insert_text_handler), GINT_TO_POINTER(2));
            }
            /** Remove filters. */
            void RemoveForce() {
                g_signal_handlers_disconnect_by_func(Obj(), (void*)insert_text_handler, GINT_TO_POINTER(1));
                g_signal_handlers_disconnect_by_func(Obj(), (void*)insert_text_handler, GINT_TO_POINTER(2));
            }
            /** Selects a region of text.
The characters that are selected are those characters at positions from start_pos up to, but not including end_pos. If end_pos is negative or if you don't specify it, then the the characters selected will be those characters from start_pos to the end of the text.
*/
            void Select(int start_pos /**< 	the starting position. */,
                        int end_pos = -1 /**< the end position, if not specified or negative selects till the end of text. */
                    ) {
                gtk_editable_select_region(GTK_EDITABLE(Obj()), start_pos, end_pos);
            }
            /** Unselect the text. */
            void Unselect() {
                gtk_editable_select_region(GTK_EDITABLE(Obj()), 0, 0);
            }
            /** Gets the current selection bounds, if there is a selection.
             \return true if there is a selection and the argument has been filled.
             */
            bool SelectionBounds(Point &bounds /**< Input parameters where to store the bounds of the selection */ ) {
                return gtk_editable_get_selection_bounds(GTK_EDITABLE(Obj()), &bounds.x, &bounds.y);
            }
            /// Indicates that the user has changed the contents of the widget.
            BUILD_EVENT(OnChanged, "changed");
    };

    /// Accepts only numbers
    const std::string EntryNumeric = "0123456789";
    /// Accepts numbers and dots, useful for IP addresses.
    const std::string EntryIpAddress = "0123456789.";
    /// Accepts only uppercase letters.
    const std::string EntryUpperCase = "ABCDEFGHIJKLMNOPQRSTUVXYWZ";
    /// Accepts only lowercase letters.
    const std::string EntryLowerCase = "abcdefghijklmnopqrstuvxywz";
    /// Accepts only letters.
    const std::string EntryLetters = EntryUpperCase + EntryLowerCase;
    /// Accepts only letters and numbers.
    const std::string EntryAlphaNumeric = EntryLetters + EntryNumeric;

    // forward declaration
    class EntryCompletion;

    /** A single line text entry field.
The Entry widget is a single line text entry widget. A fairly large set of key bindings are supported by default. If the entered text is longer than the allocation of the widget, the widget will scroll so that the cursor position is visible.

    */
    class Entry: public Editable
    {
/// DOXYS_OFF
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
/// DOXYS_ON
        public:
            /// Creates a new entry with an optional max length in characters.
            Entry(int maxlen = -1) {
                if (maxlen != -1) {
                    Init(gtk_entry_new_with_max_length(maxlen));
                }
                else
                    Init(gtk_entry_new());
                Internal(true);
            }
            /** Sets the text in the widget to the given value, every streamable type is accepted.
            The new value will replace the current contents.
            */
            template <typename T>
            void Set(const T &t) {
                std::ostringstream os;
                os << t;
                gtk_entry_set_text(*this, os.str().c_str());
            }
            /// Push an object inside the widget, concatenating it with the current contents.
            template<typename T>
            Entry &operator<<(const T &t) {
                std::ostringstream os(Get(), std::ios_base::app);
                os << t;
                Set(os.str());
                return *this;
            }
            /// Sets the text in the widget to the given string, replacing the current contents.
            void Set(const std::string &name) { gtk_entry_set_text(*this, name.c_str()); }

            /// clear the contents of the given widget, current contents are lost.
            void Clear() { gtk_entry_set_text(*this, ""); }

            /// Sets the text in the widget using printf like syntax, replacing the current contents.
            void SetF(const char *format, ...) {
                char *buffer;
                va_list va;
                va_start(va, format);
                g_vasprintf(&buffer, format, va);
                va_end(va);
                gtk_entry_set_text(*this, buffer);
                g_free(buffer);
            }
#if GTK_MINOR_VERSION > 15
            /// Add a progress bar behind the text, with a range from 0.0 to 1.0
            void Progress(double fraction) { gtk_entry_set_progress_fraction(*this, fraction); }
#endif
            /** Retrieves the contents of the entry widget.
            \return a string with the widget contents.
            \sa Editable::GetChars
            */
            std::string Get() const { return gtk_entry_get_text(*this); }

            /// Sets whether the entry has a beveled frame around it.
            void HasFrame(bool value /**< new value */) { gtk_entry_set_has_frame(*this, false); }
            // some get/set methods
            /** Sets the alignment for the contents of the entry.
This controls the horizontal positioning of the contents when the displayed text is shorter than the width of the entry. */
            void Alignment(float xalign /**< The horizontal alignment, from 0 (left) to 1 (right). Reversed for RTL layouts */) {
                gtk_entry_set_alignment(*this, xalign);
            }
            /// Sets completion to be the auxiliary completion object to use with entry. All further configuration of the completion mechanism is done on completion using the EntryCompletion API. Completion is disabled if completion is set to NULL.
            void Completion(const EntryCompletion *completion /**< An EntryCompletion object or NULL */);
            /// Returns the auxiliary completion object currently in use by entry.
            EntryCompletion *Completion() const;
/** Gets the value set by Entry::Alignment(float).
\return the alignment (from 0 to 1).
\sa Entry::Alignment(float)
*/
            float Alignment() const { return gtk_entry_get_alignment(*this); }
            /** Sets the maximum allowed length of the contents of the widget.
If the current contents are longer than the given length, then they will be truncated to fit.
             */
            void MaxLength(int chars /**< the maximum length of the entry, or 0 for no maximum. (other than the maximum length of entries.) The value passed in will be clamped to the range 0-65536. */
                    ) {
                gtk_entry_set_max_length(*this, chars);
            }
            /** Retrieves the maximum allowed length of the text in entry.
            \sa Entry::MaxLength(int)
            \return the maximum allowed number of characters in Entry, or 0 if there is no maximum.
            */
            int MaxLength() const { return gtk_entry_get_max_length(*this); }
            /** Sets whether the contents of the entry are visible or not.
When visibility is set to false, characters are displayed as the invisible char, and will also appear that way when the text in the entry widget is copied elsewhere.

The default invisible char is the asterisk '*', but it can be changed with Entry::InvisibleChar(uint32_t).
*/
            void Visibility(bool flag /**< true if the contents of the entry are displayed as plaintext */) {
                gtk_entry_set_visibility(*this, flag);
            }
            /** Retrieves whether the text in entry is visible.
            \sa Entry::Visibility(bool)
            \return true if the text is currently visible.
            */
            bool Visibility() const { return gtk_entry_get_visibility(*this); }
            /** Retrieves the character displayed in place of the real characters for entries with visibility set to false.
            \sa Entry::InvisibleChar(uint32_t).
            \return the current invisible char, or 0, if the entry does not show invisible text at all.
            */
            uint32_t InvisibleChar() const { return gtk_entry_get_invisible_char(*this); }
            /** Sets the character to use in place of the actual text when Entry::Visibility(bool) has been called to set text visibility to false. i.e. this is the character used in "password mode" to show the user how many characters have been typed. The default invisible char is an asterisk ('*'). If you set the invisible char to 0, then the user will get no feedback at all; there will be no text on the screen as they type.
             */
            void InvisibleChar(uint32_t unicode_char /**< a Unicode character */) { gtk_entry_set_invisible_char(*this, unicode_char); }
            /** Changes the size request of the entry to be about the right size for n_chars characters.
Note that it changes the size request, the size can still be affected by how you pack the widget into containers. If n_chars is -1, the size reverts to the default entry size. */
            void WidthChars(int chars /**< width in chars */) { gtk_entry_set_width_chars(*this, chars); }
            /** Gets the value set by Entry::WidthChars(int)
            \return number of chars to request space for, or negative if unset
            */
            int WidthChars() const { return gtk_entry_get_width_chars(*this); }

            /** Filter Entry input to accept only a certain range of characters.
This function is a facility of oogtk to accept only a certain range of characters in a Entry widget.
There are a few presets you can use:

||Preset||Effect
|EntryNumeric|Accepts only numbers
|EntryIPAddress|Accepts numbers and dots, useful for IP addresses.
|EntryUpperCase|Accepts only uppercase letters.
|EntryLowerCase|Accepts only lowercase letters.
|EntryLetters|Accepts only letters.
|EntryAlphaNumeric|Accepts only letters and numbers.

If you want to disable the filter simply call Entry::Accepts without arguments.
            */
            void Accepts(const std::string &chars = "" /**< a string containing characters that can be accepted by the Entry */,
                        int maxlength = -1 /**< Optional maximum length of the Entry */ ) {
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

            /** Activate the default widget if this entry is activated.
If setting is true, pressing Enter in the entry will activate the default widget for the window containing the entry. This usually means that the dialog box containing the entry will be closed, since the default widget is usually one of the dialog buttons.

(For experts: if setting is true, the entry calls Window::ActivateDefault() on the window containing the entry, in the default handler for the "activate" signal.)
             */
            void ActivatesDefault(bool flag /**< true to activate window's default widget on Enter keypress */) {
                gtk_entry_set_activates_default(*this, flag);
            }
            /** Retrieves the value set by Entry::ActivatesDefault(bool)
            \return true if the entry will activate the default widget of this window.
             */
            bool ActivatesDefault() const { return gtk_entry_get_activates_default(*this); }
            // callbacks
            BUILD_EVENT(OnActivate, "activate");
// not in editable because conflicts with constructor!
            /** Determines if the user can edit the text in the editable widget or not. */
            void Editable(bool flag /**< true if the user is allowed to edit the text in the widget. */ ) { gtk_editable_set_editable(GTK_EDITABLE(Obj()), flag); }
            /** Retrieves whether editable is editable.
            \return true if the Editable is editable.
            */
            bool Editable() const { return gtk_editable_get_editable(GTK_EDITABLE(Obj())); }

    };

/// An enumeration representing possible orientations and growth directions for the visible progress bar.
    enum ProgressBarOrientation
    {
      ProgressLeftToRight = GTK_PROGRESS_LEFT_TO_RIGHT /**< A horizontal progress bar growing from left to right. */,
      ProgressRightToLeft = GTK_PROGRESS_RIGHT_TO_LEFT /**< A horizontal progress bar growing from right to left.  */,
      ProgressBottomToTop = GTK_PROGRESS_BOTTOM_TO_TOP /**< A vertical progress bar growing from bottom to top. */,
      ProgressTopToBottom = GTK_PROGRESS_TOP_TO_BOTTOM /**< A vertical progress bar growing from top to bottom.  */
    };

    enum SocketCondition {
        SocketRead = G_IO_IN|G_IO_HUP 	/**< There is data to read. */,
        SocketWrite = G_IO_OUT 	/**< Data can be written (without blocking). */,
        SocketUrgent = G_IO_PRI /**< There is urgent data to read. */,
        SocketError = G_IO_ERR 	/**< Error condition. */,
        SocketHung =  G_IO_HUP 	/**< Hung up (the connection has been broken, usually for pipes and sockets). */,
        SocketInvalid = G_IO_NVAL /**< 	Invalid request. The file descriptor is not open. */
    };

/** A widget which indicates progress visually

The ProgressBar object is typically used to display the progress of a long running operation. It provides a visual clue that processing is underway. The ProgressBar object can be used in two different modes: percentage mode and activity mode.

When an application can determine how much work needs to take place (e.g. read a fixed number of bytes from a file) and can monitor its progress, it can use the ProgressBar in percentage mode and the user sees a growing bar indicating the percentage of the work that has been completed. In this mode, the application is required to call ProgressBar::Fraction(double) periodically to update the progress bar.

When an application has no accurate way of knowing the amount of work to do, it can use the ProgressBar in activity mode, which shows activity by a block moving back and forth within the progress area. In this mode, the application is required to call ProgressBar::Pulse() perodically to update the progress bar.

There is quite a bit of flexibility provided to control the appearance of the ProgressBar. Functions are provided to control the orientation of the bar, optional text can be displayed along with the bar, and the step size used in activity mode can be set.
*/
    class ProgressBar : public Widget {
        public:
/// DOXYS_OFF
            operator GtkProgressBar *() const { return GTK_PROGRESS_BAR(Obj()); }
            ProgressBar(GObject *obj) { Init(obj); }
/// DOXYS_ON
            /// Creates a new ProgressBar object
            ProgressBar() {
                Init(gtk_progress_bar_new());
                Internal(true);
            }
            /** Indicates that some progress is made, but you don't know how much.
Causes the progress bar to enter "activity mode," where a block bounces back and forth. Each call to ProgressBar::Pulse() causes the block to move by a little bit (the amount of movement per pulse is determined by ProgressBar::PulseStep(double) ).
              */
            void Pulse() { gtk_progress_bar_pulse(*this); }

            // get/set methods
            /// Causes the given text to appear superimposed on the progress bar.
            void Text(const std::string &text /**< string to be shown over the progress bar */ ) { gtk_progress_bar_set_text(*this, text.c_str()); }
            /// Causes the progress bar to "fill in" the given fraction of the bar. The fraction should be between 0.0 and 1.0, inclusive.
            void Fraction(double value /**< fraction of the task that's been completed */) {
                if (value >= 0.0 && value <= 1.0)
                    gtk_progress_bar_set_fraction(*this, value);
            }
            /// Sets the fraction of total progress bar length to move the bouncing block for each call to ProgressBar::Pulse().
            void PulseStep(double value /**< fraction between 0.0 and 1.0 */) { gtk_progress_bar_set_pulse_step(*this, value); }
            /// Causes the progress bar to switch to a different orientation (left-to-right, right-to-left, top-to-bottom, or bottom-to-top).
            void Orientation(ProgressBarOrientation mode /**< orientation of the progress bar */) {
                gtk_progress_bar_set_orientation(*this, (GtkProgressBarOrientation)mode);
            }
            /** Retrieves the text displayed superimposed on the progress bar.
            This method returns the text displayed superimposed on the progress bar, if any, otherwise it returns an empty string. The return value is a copy to the text, so will not become invalid (nor change) if you change the text in the progress bar.
            */
            std::string Text() const { if (const gchar *g =  gtk_progress_bar_get_text(*this)) return g; else return ""; }
            /** Returns the current fraction of the task that's been completed.
              \retval A fraction between 0.0 and 1.0
            */
            double Fraction() const { return gtk_progress_bar_get_fraction(*this); }
            /** Retrieves the pulse step set with ProgressBar::PulseStep(double)
              \retval A fraction between 0.0 and 1.0
            */
            double PulseStep() const { return gtk_progress_bar_get_pulse_step(*this); }
            /// Retrieves the current progress bar orientation.
            ProgressBarOrientation Orientation() const {
                return (ProgressBarOrientation)gtk_progress_bar_get_orientation(*this);
            }
    };

    /** System clipboard access object
     *
     * Use Clipboard::Get() to get the default clipboard and then handle it with Clipboard::SetText, Clipboard::GetText
     */
    class Clipboard : public Object { // 30% done
        public:
/// DOXYS_OFF
            operator GtkClipboard *() const { return GTK_CLIPBOARD(Obj()); }
            Clipboard(GObject *obj) { Init(obj); }
/// DOXYS_ON
            /// Returns the clipboard object for the given selection.
            static Clipboard &Get() {
                static Clipboard *clip = nullptr;

                if (!clip)
                    clip = new Clipboard((GObject*)gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));

                return *clip;
            }
            /** Clears the contents of the clipboard.
             *
             * Generally this should only be called between the time you call gtk_clipboard_set_with_owner() or gtk_clipboard_set_with_data(),
             * and when the clear_func you supplied is called. Otherwise, the clipboard may be owned by someone else.
             */
            void Clear() { gtk_clipboard_clear(*this); }
            /** Sets the contents of the clipboard to the given UTF-8 string.
             *
             *  GTK+ will make a copy of the text and take responsibility for responding for requests for the text, and for converting the text into the requested format.
             */
            void SetText(const std::string &txt /**< a UTF-8 string. */ ) { gtk_clipboard_set_text(*this, txt.c_str(), txt.length()); }
            /** Requests the contents of the clipboard as text and converts the result to UTF-8 if necessary.
             *
             * This function waits for the data to be received using the main loop, so events, timeouts, etc, may be dispatched during the wait.
             *
             * \retval a string with the clipboard contents converted or an empty string if the operation failed. (This could happen for various reasons, in particular if the clipboard was empty or if the contents of the clipboard could not be converted into text form.)
             */
            std::string GetText() const {
                if (gchar *t = gtk_clipboard_wait_for_text(*this)) {
                    std::string res = t;
                    g_free(t);
                    return res;
                }
                return std::string();
            }
    };

/** A widget for custom user interface elements.

The DrawingArea widget is used for creating custom user interface elements. It's essentially a blank widget; you can draw on its GdkDrawable. After creating a drawing area, the application may want to connect to:

- Mouse and button press signals to respond to input from the user. (Use Widget::AddEvents() to enable events you wish to receive.)

- The "realize" signal to take any necessary actions when the widget is instantiated on a particular display. (Create GDK resources in response to this signal.)

- The "configure_event" signal to take any necessary actions when the widget changes size.

- The "expose_event" signal to handle redrawing the contents of the widget.

Note that GDK automatically clears the exposed area to the background color before sending the expose event, and that drawing is implicitly clipped to the exposed area.

To receive mouse events on a drawing area, you will need to enable them with Widget::AddEvents(). To receive keyboard events, you will need to set the CanFocus flag on the drawing area, and should probably draw some user-visible indication that the drawing area is focused. Use the Widget::HasFocus() call in your expose event handler to decide whether to draw the focus indicator. See gtk_paint_focus() for one way to draw focus.

\sa Sometimes Image is a useful alternative to a drawing area. You can put a GdkPixmap in the Image and draw to the GdkPixmap, calling Widget::QueueDraw() on the Image when you want to refresh to the screen.
*/
    class DrawingArea : public Widget {
        public:
/// DOXYS_OFF
            operator GtkDrawingArea *() const { return GTK_DRAWING_AREA(Obj()); }
            DrawingArea(GObject *obj) { Init(obj); }
/// DOXYS_ON
            /// Creates a new drawing area.
            DrawingArea() {
                Init(gtk_drawing_area_new());
                Internal(true);
            }
    };

#if GTK_MINOR_VERSION > 19

/// A spinner is a widget to indicate progresss
class Spinner : public DrawingArea {
        public:
/// DOXYS_OFF
            operator GtkSpinner *() const { return GTK_SPINNER(Obj()); }
            Spinner(GObject *obj) { Init(obj); }
/// DOXYS_ON
            /// Creates a new spinner.
            Spinner() {
                Init(gtk_spinner_new());
                Internal(true);
            }
            /// Start the animation
            void Start() { gtk_spinner_start(*this); }
            /// Stop the animation
            void Stop() { gtk_spinner_stop(*this); }
    };
#endif

    typedef int CbkId;
/** Base OOGtk Application class.
This is the base OOGtk application class, ideally you should derive your application class from this one, all the examples in oogtk does it, but it's not mandatory, the members you will need for sure (Application::Run(), Application::Quit()...) are static so you can use them without instancing an Application object, the only function you will need to call if you choose to not derive from Application is Application::Init to initialize the GTK subsystem.

This class offers support for timers (Application::AddTimer() family) and idle functions (Application::AddIdle() family) and main loop handling.

Application::Run() is the OOGtk equivalent of gtk_main(), Application::Quit() is the OOGtk equivalent of gtk_main_quit(). You can nest main loops like you can do in plain gtk.

Example of a basic application using the Application class as base:

\example
#include "oogtk.h"

class MyApp : public gtk::Application {
    gtk::Window w_;
public:
    MyApp() : w_("OOGtk") {
        w_.Border(16);
        w_.Child(gtk::Label("Hello world!"));
        w_.OnDelete(&gtk::Application::QuitLoop, dynamic_cast<gtk::Application *>(this));
        w_.ShowAll();
    }
};

int main()
{
    MyApp app;
    app.Run();
}
\endexample

Here is the same example modified not to use the Application class:

\example
#include "oogtk.h"

int main() {
    gtk::Application::Init();
    gtk::Window win("OOGtk");
    win.Border(16);
    win.Child(gtk::Label("Hello world!"));
    win.OnDelete((GCallback)gtk::Application::Quit);
    gtk::Application::Run();
}
\endexample
 */
    class Application
    {
            typedef std::map<CbkId, AbstractCbk*> CbkMap;
            typedef CbkMap::iterator CbkIt;
            typedef std::map<CbkId, GIOChannel*> ChannelMap;
            typedef ChannelMap::iterator ChannelIt;
        public:
/** initialize an Application object passing the program parameters to it.

This function initialize GTK subsystem passing the parameters from the application to GTK. This can be useful because GTK can find some debug paramaters directed to it and remove them from the command line, so argc/argv MAY be modified after this function returns.
*/
            Application(int &a /**< argc as received from main() */,
                        char **&b /**< argv as received from main() */) { gtk_init(&a,&b); }

/// Initialize an application without passing parameters to GTK.
            Application() { gtk_init(NULL, NULL); }

/** Initialize an Application passing the program parameters to it.
\sa Application::Application(int&,char**&) for more informations.
*/
            static void Init(int &a, char **&b) { gtk_init(&a,&b); }
/** Initialize an Application without parameters for GTK.
\sa Application::Application(int&,char**&) for more informations.
*/
            static void Init() { gtk_init(NULL, NULL); }

            static void Run() { gtk_main(); }
            static void Iteration() { gtk_main_iteration(); }
            static void Flush() { while (gtk_events_pending()) gtk_main_iteration(); }
            static void Quit() { gtk_main_quit(); }

            static void ThreadInit() {
                // da glib 2.32 e`deprecata e non più necessaria
#if GLIB_MINOR_VERSION < 32
                g_thread_init(NULL);
#endif
            }
            // helpers for signals that need a fixed answer.
            void QuitLoop() { gtk_main_quit(); }
            bool True() { return true; }
            bool False() { return false; }


/** Add a socket to the input loop.

This call will start monitoring for the Condition "cond" in the oogtk
main loop. If the condition triggers the member class "fnc" of the object "obj" will be called.
The function will have as parameters both the socket file descriptor
and the data field you passed to this call.

Use Application::DelSource() to remove this socket from the main loop, this will only remove the socket from the GTK watch lists, you'll have to close the socket yourself.
*/
            template <typename T, typename R, typename J>
            CbkId AddSocket(SockFd fd, SocketCondition cond, R (T::*fnc)(SockFd, J),
                            T* obj, J data, int rc = true) {
                return AddSocket(new CbkEvent<T,R,J>(obj, fnc, data, rc), fd, cond);
            }
            template <typename T, typename R>
            CbkId AddSocket(SockFd fd, SocketCondition cond, R (T::*fnc)(SockFd),
                            T* obj, int rc = true) {
                return AddSocket(new CbkEvent<T,R>(obj, fnc, rc), fd, cond);
            }

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


/** Add a keyboard snooper to the application.

This method add a keyboard snooper to the application, the snooper will receive all the keyboard
events before routing them to GTK widgets. If your snooper does not returns void you can use the
return value to choice if to reroute the event to GTK (return false) or to remove it from the event
queue (return true). You can also pass a void method and choose the return value once for all
with the parameter 'rc'.

\example
class MyApp : public Application
    MyApp() {
        [...]
        AddKeySnooper(&MyApp::mysnooper, this, (int)123456);
    }
    void mysnooper(Event &e, int userdata) {
        if (EventKey *k = e) {
            std::cerr << "Received " << (k->Press() ? "PRESS" : "RELEASE")
                      << " event " << k->KeyVal() << " with userdata " << userdata << "\n";
        }
    }
}
\endexample
\return a CbkId that could be used to remove the snooper with Application::DelSource
*/
            template <typename T, typename R>
            CbkId AddKeySnooper(R (T::*fnc)(Event &) /**< The method to call every time the app receive a keyboard event */,
                                T *obj /**< The class the method belongs to */,
                                bool rc = false /**< The default return code of the snooper, it's meaningful only if your pass a void method */) {
                return AddKeySnooper(new CbkEvent<T,R>(obj, fnc, rc));
            }
/** Add a keyboard snooper with user defined data to the application.

This method differs from the other only because it let the user pass an additional user data
parameter of (almost) arbitrary type to the application.

The almost of the previous sentence is because you cannot pass as user data a reference (but you
can pass a pointer to an object or a simple object, also if not PoD).

\sa Application::AddKeySnooper(R (T::*fnc)(Event &), T *, bool)
\return a CbkId that could be used to remove the snooper with Application::DelSource
*/
            template <typename T, typename R, typename J>
            CbkId AddKeySnooper(R (T::*fnc)(Event &, J), T *obj, J data, bool rc = false) {
                return AddKeySnooper(new CbkEvent<T,R,J>(obj, fnc, rc));
            }

            void DelSource(CbkId id) {
                g_source_remove(id);

                std::unique_lock<std::mutex> lock(mtx());

                ChannelIt cit = Channels().find(id);

                if (cit != Channels().end()) {
                    g_io_channel_unref(cit->second);
                    Channels().erase(id);
                }
            }

            void DelTimer(CbkId id) { DelSource(id);  }
            void DelIdle(CbkId id) { DelSource(id);  }


        private:
            static std::mutex &mtx() { static std::mutex m; return m; }
            static ChannelMap &Channels() { static ChannelMap channels; return channels; }

            CbkId AddKeySnooper(AbstractCbk *cbk) {
                return gtk_key_snooper_install((gint (*)(GtkWidget*, GdkEventKey*, void*))
                                                 AbstractCbk::real_callback_2, cbk);
            }

            static void destroy_source(AbstractCbk * data) {
                delete data;
            }

            CbkId AddIdle(AbstractCbk *cbk) {
                return g_idle_add_full(G_PRIORITY_DEFAULT_IDLE, (gboolean (*)(void*))AbstractCbk::real_callback_0, cbk, GDestroyNotify(destroy_source));
            }
            CbkId AddTimer(AbstractCbk *cbk, int msec) {
                return g_timeout_add_full(G_PRIORITY_DEFAULT, msec, (gboolean (*)(void*))AbstractCbk::real_callback_0, cbk, GDestroyNotify(destroy_source));
            }

            CbkId AddSocket(AbstractCbk *cbk, SockFd fd, SocketCondition cond) {
                GIOChannel *ch = FindChannel(fd);

                if (ch)
                    g_io_channel_ref(ch);
                else {
#ifdef WIN32
                    ch = g_io_channel_win32_new_socket(fd);
#else
                    ch = g_io_channel_unix_new(fd);
#endif
                }

                int id = g_io_add_watch_full(ch, G_PRIORITY_DEFAULT, (GIOCondition)cond, (GIOFunc)AbstractCbk::real_callback_2, cbk, GDestroyNotify(destroy_source));

                std::unique_lock<std::mutex> lock(mtx());
                Channels().insert(ChannelMap::value_type(id, ch));

                return id;
            }
            GIOChannel *FindChannel(SockFd fd) {
                std::unique_lock<std::mutex> lock(mtx());

                for (ChannelIt it = Channels().begin(); it != Channels().end(); ++it)
                    if (fd == g_io_channel_unix_get_fd((it->second)))
                        return it->second;
                return NULL;
            }
    };
/** A Builder is an auxiliary object that reads textual descriptions of a user interface and instantiates the described objects.

To pass a description to a Builder, call Builder::Load() or directly pass the description to the class constructor. Builder::Load can be called multiple times; the builder merges the content of all descriptions.

A Builder holds a reference to all objects that it has constructed and drops these references when it is finalized. This finalization can cause the destruction of non-widget objects or widgets which are not contained in a toplevel window. For toplevel windows constructed by a builder, it is the responsibility of the user to delete the top level objects (Window, Dialog...) to get rid of them and all the widgets they contain.

The methods Builder::Get() and Builder:Get<>() can be used to access the widgets in the interface by the names assigned to them inside the UI description. Toplevel windows returned by these functions will stay around until the user explicitly destroys them. Other widgets will either be part of a larger hierarchy constructed by the builder (in which case you should not have to worry about their lifecycle), or without a parent, in which case they have to be added to some container to make use of them. Non-widget objects need to be reffed with Object::Ref() to keep them beyond the lifespan of the builder.

Builder UI Definitions

Builder parses textual descriptions of user interfaces which are specified in an XML format which can be roughly described by the DTD below. We refer to these descriptions as Builder UI definitions or just UI definitions if the context is clear. Do not confuse Builder UI Definitions with UIManager UI Definitions, which are more limited in scope.

\example
<!ELEMENT interface (requires|object)* >
<!ELEMENT object    (property|signal|child|ANY)* >
<!ELEMENT property  PCDATA >
<!ELEMENT signal    EMPTY >
<!ELEMENT requires  EMPTY >
<!ELEMENT child     (object|ANY*) >
<!ATTLIST interface  domain         	    #IMPLIED >
<!ATTLIST object     id             	    #REQUIRED
                     class          	    #REQUIRED
                     type-func      	    #IMPLIED
                     constructor    	    #IMPLIED >
<!ATTLIST requires   lib             	    #REQUIRED
                     version          	    #REQUIRED >
<!ATTLIST property   name           	    #REQUIRED
                     translatable   	    #IMPLIED
                     comments               #IMPLIED
                     context                #IMPLIED >
<!ATTLIST signal     name           	    #REQUIRED
                     handler        	    #REQUIRED
                     after          	    #IMPLIED
                     swapped        	    #IMPLIED
                     object         	    #IMPLIED
                     last_modification_time #IMPLIED >
<!ATTLIST child      type           	    #IMPLIED
                     internal-child 	    #IMPLIED >
\endexample

The toplevel element is <interface>. It optionally takes a "domain" attribute, which will make the builder look for translated strings using dgettext() in the domain specified. This can also be done by calling Builder::TranslationDomain() on the builder. Objects are described by <object> elements, which can contain <property> elements to set properties, <signal> elements which connect signals to handlers, and <child> elements, which describe child objects (most often widgets inside a container, but also e.g. actions in an action group, or columns in a tree model). A <child> element contains an <object> element which describes the child object. The target toolkit version(s) are described by <requires> elements, the "lib" attribute specifies the widget library in question (currently the only supported value is "gtk+") and the "version" attribute specifies the target version in the form "<major>.<minor>". The builder will error out if the version requirements are not met.

Typically, the specific kind of object represented by an <object> element is specified by the "class" attribute. If the type has not been loaded yet, GTK+ tries to find the _get_type() from the class name by applying heuristics. This works in most cases, but if necessary, it is possible to specify the name of the _get_type() explictly with the "type-func" attribute. As a special case, Builder allows to use an object that has been constructed by a UIManager in another part of the UI definition by specifying the id of the UIManager in the "constructor" attribute and the name of the object in the "id" attribute.

Objects must be given a name with the "id" attribute, which allows the application to retrieve them from the builder with Builder::Get(). An id is also necessary to use the object as property value in other parts of the UI definition.

Setting properties of objects is pretty straightforward with the <property> element: the "name" attribute specifies the name of the property, and the content of the element specifies the value. If the "translatable" attribute is set to a true value, GTK+ uses gettext() (or dgettext() if the builder has a translation domain set) to find a translation for the value. This happens before the value is parsed, so it can be used for properties of any type, but it is probably most useful for string properties. It is also possible to specify a context to disambiguate short strings, and comments which may help the translators.

Builder can parse textual representations for the most common property types: characters, strings, integers, floating-point numbers, booleans (strings like "TRUE", "t", "yes", "y", "1" are interpreted as TRUE, strings like "FALSE", "f", "no", "n", "0" are interpreted as FALSE), enumerations (can be specified by their name, nick or integer value), flags (can be specified by their name, nick, integer value, optionally combined with "|", e.g. "GTK_VISIBLE|GTK_REALIZED") and colors (in a format understood by gdk_color_parse()). Objects can be referred to by their name. Pixbufs can be specified as a filename of an image file to load. In general, Builder allows forward references to objects — an object doesn't have to constructed before it can be referred to. The exception to this rule is that an object has to be constructed before it can be used as the value of a construct-only property.

Sometimes it is necessary to refer to widgets which have implicitly been constructed by GTK+ as part of a composite widget, to set properties on them or to add further children (e.g. the vbox of a Dialog). This can be achieved by setting the "internal-child" propery of the <child> element to a true value. Note that Builder still requires an <object> element for the internal child, even if it has already been constructed.

A number of widgets have different places where a child can be added (e.g. tabs vs. page content in notebooks). This can be reflected in a UI definition by specifying the "type" attribute on a <child> The possible values for the "type" attribute are described in the sections describing the widget-specific portions of UI definitions.

\example
<interface>
  <object class="GtkDialog" id="dialog1">
    <child internal-child="vbox">
      <object class="GtkVBox" id="vbox1">
        <property name="border-width">10</property>
        <child internal-child="action_area">
          <object class="GtkHButtonBox" id="hbuttonbox1">
            <property name="border-width">20</property>
            <child>
              <object class="GtkButton" id="ok_button">
                <property name="label">gtk-ok</property>
                <property name="use-stock">TRUE</property>
                <signal name="clicked" handler="ok_button_clicked"/>
              </object>
            </child>
          </object>
        </child>
      </object>
    </child>
  </object>
</interface>
\endexample

Beyond this general structure, several object classes define their own XML DTD fragments for filling in the ANY placeholders in the DTD above. Note that a custom element in a <child> element gets parsed by the custom tag handler of the parent object, while a custom element in an <object> element gets parsed by the custom tag handler of the object.
*/
    class Builder
    {
        public:
            /// this type describes the source type for a Builder interface.
            enum BuildSource {File /**< The string is a file name */,
                              String /**< The string contains an XML definition for the Builder */ };

            /** Create a new Builder object.
Usually you should use this by deriving one of your classes from a Builder object.
\sa Builder::Get<>() for an example.
            */
            Builder(const std::string &ui = "" /**< the filename of the builder interface to load or a string containing an interface definition, as specified by the second parameter */,
                    BuildSource source = File /**< specify what kind of objects the first parameter points to, defaults to Builder::File*/) {
                builder_ = gtk_builder_new();

                if (!ui.empty()) {
                    if (!Load(ui, source)) {
                        g_object_unref(builder_);
                        throw std::runtime_error("Builder error: " + error_);
                    }
                }
            }

            /** Load an interface inside the Builder
If the builder already contains one or more interfaces the interfaces are merged.
            */
            bool Load(const std::string &ui/**< the filename of the builder interface to load or a string containing an interface definition, as specified by the second parameter */,
                    BuildSource source = File /**< specify what kind of objects the first parameter points to, defaults to Builder::File*/) {
                GError *err = NULL;

                int rc;

                if (source == File)
                    rc = gtk_builder_add_from_file(builder_, ui.c_str(), &err);
                else
                    rc = gtk_builder_add_from_string(builder_, ui.c_str(), ui.length(), &err);

                if (!rc) {
                    if (err) {
                        error_ = err->message;
                        g_error_free(err);
                    }
                    else
                        error_ = "generic error";

                    return false;
                }

#ifdef WIN32
                if (g_module_supported())
#endif
                    gtk_builder_connect_signals (builder_, this);

                return true;
            }

            /** Parses a string containing a Builder UI definition and merges it with the current contents of builder.
            \return false if the string contains an invalid interface
            */
            bool FromString(const std::string &ui /**< 	 the string to parse */) { return Load(ui, String); }
            /** Parses a file containing a Builder UI definition and merges it with the current contents of builder.
            \return false if the file doesn't exist or if the file contains an invalid interface
            */
            bool FromFile(const std::string &ui /**< the name of the file to parse */) { return Load(ui, File); }

            ~Builder() { g_object_unref(builder_); }

/** Gets the widget named "label".
Note that this function does not increment the reference count of the returned object.

You can dynamic_cast the result to whatever object it's the real type of this widget,
\return a pointer to a Widget or NULL if a object with that name doesn't exist or if the object is not a Widget (for instance a TreeStore).
*/
            Widget *
            Get(const char *label /**< name of object to get */) const {
                if (GObject *obj = gtk_builder_get_object(builder_, label)) {
                    return dynamic_cast<Widget *>(Object::Find(obj));
                }
                else
                    return NULL;
            }

/** Gets the object named "label", casting it to the appropriate type.
Note that this function does not increment the reference count of the returned object.

\example
    class MyApp : public gtk::Application, public gtk::Builder {
            gtk::Window win_;
        public:
            MyApp() : Builder("interface.xml") {
                if (gtk::TreeView *tv = Get<gtk::TreeView>("treeview_main")) {
                    tv->AddSortableTextColumn("Title", 0);
                    tv->AddSortableTextColumn("Author", 1);
                    tv->AddTextColumn("Price", 2);
                }
                if (win_ = Get<gtk::Window>("window_main"))
                    win_->Show();
            }
            ~MyApp() {
                delete win_;
            }
    }

    int main()
    {
        MyApp app;
        app.Run();
    }
\endexample
\return NULL if a object with that name doesn't exist or if the object is not dynamic_cast-able to the template type.
*/
            template<typename T>
            T *Get(const char *label /**< name of object to get */) const {
                if (GObject *obj = gtk_builder_get_object(builder_, label))
                    return dynamic_cast<T *>(Object::Find(obj));
                else
                    return NULL;
            }
            template<typename T>
            T *Get(const std::string &label /**< name of object to get */) const {
                return Get<T>(label.c_str());
            }
            /** Returns last error of Load() if any */
            const std::string &Error() const { return error_; }
        private:
            GtkBuilder *builder_;
            std::string error_;
    };
}
#include "oocont.h"
#include "oobutton.h"
#include "oowin.h"
#include "ootree.h"
#include "ootext.h"
#include "ooui.h"
#include "oomenu.h"

// this must be defined here for forward declaration issues

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
            } else if (GTK_IS_FILE_CHOOSER_BUTTON(o)) { // button handling
                return new FileChooserButton(o);
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
            } else if (GTK_IS_CELL_RENDERER_PROGRESS(o)) {
                return new CellRendererProgress(o);
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
            } else if (GTK_IS_ICON_VIEW(o)) {
                return new IconView(o);
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
            } else if (GTK_IS_HSCALE(o)) {
                return new HScale(o);
            } else if (GTK_IS_VSCALE(o)) {
                return new VScale(o);
            } else if (GTK_IS_SCALE(o)) {
                return new Scale(o);
            } else if (GTK_IS_RANGE(o)) {
                return new Range(o);
            } else if (GTK_IS_ENTRY_COMPLETION(o)) {
                return new EntryCompletion(o);
#if GTK_MINOR_VERSION > 23
            } else if (GTK_IS_COMBO_BOX_TEXT(o)) {
                return new ComboBoxText(o);
#endif
            } else if (GTK_IS_COMBO_BOX(o)) {
                return new ComboBox(o);
            } else if (GTK_IS_EVENT_BOX(o)) {
                return new EventBox(o);
            } else if (GTK_IS_ASPECT_FRAME(o)) {
                return new AspectFrame(o);
            } else if (GTK_IS_FRAME(o)) {
                return new Frame(o);
            } else if (GTK_IS_VBOX(o)) {
                return new VBox(o);
            } else if (GTK_IS_HBOX(o)) {
                return new HBox(o);
            } else if (GTK_IS_PROGRESS_BAR(o)) {
                return new ProgressBar(o);
            } else if (GTK_IS_NOTEBOOK(o)) {
                return new Notebook(o);
#if GTK_MINOR_VERSION > 19
            } else if (GTK_IS_SPINNER(o)) {
                return new Spinner(o);
#endif
            } else if (GTK_IS_DRAWING_AREA(o)) {
                return new DrawingArea(o);
            } else if (GTK_IS_TABLE(o)) {
                return new Table(o);
            } else if (GTK_IS_LABEL(o)) {
                return new Label(o);
            } else if (GTK_IS_FILE_FILTER(o)) { // file filter for FileChooser widget
                return new FileFilter(o);
            } else if (GTK_IS_UI_MANAGER(o)) { // UI Manager stuff
                return new UIManager(o);
            } else if (GTK_IS_ACCEL_GROUP(o)) {
                return new AccelGroup(o);
            } else if (GTK_IS_VPANED(o)) {
                return new VPaned(o);
            } else if (GTK_IS_HPANED(o)) {
                return new HPaned(o);
            } else if (GTK_IS_ALIGNMENT(o)) {
                return new Alignment(o);
            } else if (GTK_IS_ACTION_GROUP(o)) {
                return new ActionGroup(o);
            } else if (GTK_IS_ACTION(o)) {
                return new Action(o);
            } else if (GTK_IS_SIZE_GROUP(o)) {
                return new SizeGroup(o);
            } else if (GTK_IS_HSEPARATOR(o)) {
                return new HSeparator(o);
            } else if (GTK_IS_VSEPARATOR(o)) {
                return new VSeparator(o);
            } else if (GTK_IS_CLIPBOARD(o)) {
                return new Clipboard(o);
            } else
                std::cerr << "Undefined type " << g_type_name(GTK_OBJECT_TYPE(o)) << '\n';
        }

        return NULL;
    }

    template <typename T,typename R, typename J>
    inline bool CbkEvent<T,R,J>::
    notify(GtkWidget *w, GdkEvent *e) const
    {
        switch (type) {
            case NoParam:
                {
                   ReturnType<T, R, FakeTypeBase, J> rtype;

                   if (myFnc0)
                        return rtype.notify(myFnc0, myObj, rccode);
                   else
                        return rtype.notify(ma1, myFnc1, myObj, rccode);
                }
            case HasWidget:
                if (Widget *ww = dynamic_cast<Widget *>(Object::Find((GObject *)w))) {
                    ReturnType<T, R, Widget &, J> rtype;

                    if (mywFnc0)
                        return rtype.notify(*ww, mywFnc0, myObj, rccode);
                    else
                        return rtype.notify(*ww, ma1, mywFnc1, myObj, rccode);
                }
                else
                    throw std::runtime_error("Callback asking for a widget with widget NULL!");
            case HasSocket:
                {
                    ReturnType<T, R, SockFd, J> rtype;
                    SockFd fd = g_io_channel_unix_get_fd((GIOChannel*)w);

                    if (mysFnc0)
                        return rtype.notify(fd, mysFnc0, myObj, rccode);
                    else
                        return rtype.notify(fd, ma1, mysFnc1, myObj, rccode);
                }
            case HasEvent:
                if (Event *ee = (Event *)e) {
                    ReturnType<T, R, Event &, J> rtype;
                    if (myeFnc0)
                        return rtype.notify(*ee, myeFnc0, myObj, rccode);
                    else
                        return rtype.notify(*ee, ma1, myeFnc1, myObj, rccode);
                }
                else
                    throw std::runtime_error("Callback asking for an event with event NULL!");
            default:
                    throw std::runtime_error("Callback asking for an event with unknown type!");
        }
    }

    template <typename T, typename J>
    inline bool CbkDrag<T,J>::
    notify(GtkWidget *w, SelectionData *e) const
    {
        if (Widget *ww = dynamic_cast<Widget *>(Object::Find((GObject *)w))) {
            if (!e)
                throw std::runtime_error("Callback asking for a selectiondata with selectiondata NULL!");

            if (mywFnc1)
                (myObj->*mywFnc1)(*ww, *e, ma1);
            else
                (myObj->*mywFnc0)(*ww, *e);
        }
        else
            throw std::runtime_error("Callback asking for a widget with widget NULL!");

        return true;
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
