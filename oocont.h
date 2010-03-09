#ifndef OOCONT_H
#define OOCONT_H

#include "oogtk.h"
#include <string>
#include <vector>

namespace gtk {
    /** Base class for widgets which contain other widgets
A GTK+ user interface is constructed by nesting widgets inside widgets. Container widgets are the inner nodes in the resulting tree of widgets: they contain other widgets. So, for example, you might have a Window containing a Frame containing a Label. If you wanted an image instead of a textual label inside the frame, you might replace the Label widget with a Image widget.

There are two major kinds of container widgets in GTK+. Both are subclasses of the abstract Container base class.

The first type of container widget has a single child widget and derives from Bin. These containers are decorators, which add some kind of functionality to the child. For example, a Button makes its child into a clickable button; a Frame draws a frame around its child and a Window places its child widget inside a top-level window.

The second type of container can have more than one child; its purpose is to manage layout. This means that these containers assign sizes and positions to their children. For example, a HBox arranges its children in a horizontal row, and a Table arranges the widgets it contains in a two-dimensional grid.

To fulfill its task, a layout container must negotiate the size requirements with its parent and its children. This negotiation is carried out in two phases, size requisition and size allocation.

!Size Requisition

The size requisition of a widget is it's desired width and height. This is represented by a Requisition.

How a widget determines its desired size depends on the widget. A Label, for example, requests enough space to display all its text. Container widgets generally base their size request on the requisitions of their children.

The size requisition phase of the widget layout process operates top-down. It starts at a top-level widget, typically a Window. The top-level widget asks its child for its size requisition by calling Widget::SizeRequest(). To determine its requisition, the child asks its own children for their requisitions and so on. Finally, the top-level widget will get a requisition back from its child.

!Size Allocation

When the top-level widget has determined how much space its child would like to have, the second phase of the size negotiation, size allocation, begins. Depending on its configuration (Window::Resizable()), the top-level widget may be able to expand in order to satisfy the size request or it may have to ignore the size request and keep its fixed size. It then tells its child widget how much space it gets by calling Widget::SizeAllocate(). The child widget divides the space among its children and tells each child how much space it got, and so on. Under normal circumstances, a Window will always give its child the amount of space the child requested.

A child's size allocation is represented by a Allocation. This struct contains not only a width and height, but also a position (i.e. X and Y coordinates), so that containers can tell their children not only how much space they have gotten, but also where they are positioned inside the space available to the container.

Widgets are required to honor the size allocation they receive; a size request is only a request, and widgets must be able to cope with any size.
     */
    class Container : public Widget
    {
        public:
/// DOXYS_OFF
            operator  GtkContainer *() const { return GTK_CONTAINER(Obj()); }
/// DOXYS_ON

            /** Returns the container's non-internal children.
This method will fill the list you pass with the pointers to all the widgets contained in the container.
            */
            void Children(WidgetList &list /**< An empty list of pointers to Widget to be populated by the call */) {
                GList *lst, *l = gtk_container_get_children(*this);
                lst = l;

                while (l) {
                    if (Widget *w = dynamic_cast<Widget *>(Object::Find((GObject *)l->data)))
                        list.push_back(w);

                    l = l->next;
                }

                g_list_free(lst);
            }
            /** Adds widget to container. 
Typically used for simple containers such as Window, Frame, or Button; for more complicated layout containers such as Box or Table, this function will pick default packing parameters that may not be correct. So consider functions such as Box::PackStart() and Table::Attach() as an alternative to Container::Add() in those cases. A widget may be added to only one container at a time; you can't place the same widget inside two different containers. 
*/
            void Add(const Widget &w /**< a widget to be placed inside container */) { gtk_container_add(*this, w); }
            /** Sets the border width of the container.
The border width of a container is the amount of space to leave around the outside of the container. The only exception to this is Window; because toplevel windows can't leave space outside, they leave the space inside. The border is added on all sides of the container. To add space to only one side, one approach is to create a Alignment widget, call Widget::SizeRequest() to give it a size, and place it on the side of the container as a spacer.
*/
            void Border(int size /**< the size in pixel of the border width for the container */) { 
                gtk_container_set_border_width(*this, size); 
            }
            /** Retrieves the border width of the container.
             \sa Container::Border(int)
             \return the size in pixel of the border width of the container.
             */
            int Border() const { return gtk_container_get_border_width(*this); }
    };
/** A container with just one child
The Bin widget is a container with just one child. It is not very useful itself, but it is useful for deriving subclasses, since it provides common code needed for handling a single child widget.

Many GTK+ widgets are subclasses of Bin, including Window, Button, Frame, HandleBox, and ScrolledWindow. 
*/
    class Bin : public Container // COMPLETE API
    {
        public:
/// DOXYS_OFF
            operator  GtkBin *() const { return GTK_BIN(Obj()); }
/// DOXYS_ON
/** Gets the child of the Bin, or NULL if the bin contains no child widget.
If an OOGTK wrapper for the widget doesn't exists it will be created on the fly, see also Bin::Child(const Widget &)
to see an example of this mechanism.
\return pointer to child of the Bin container
*/
            Widget *Child() {
                return dynamic_cast<Widget *>(Object::Find((GObject *)gtk_bin_get_child(*this))); 
            }
/** Set the child of the Bin container.
A reference to the object will be created so you can also use a temporary wrapper for it, see the following example for details.
\example
#include "oogtk.h"

class MyApp : public gtk::Application {
    gtk::Window window_;
public:
    MyApp() : window_("Wrapper test!") { 
        window_.Child(gtk::Label("My window body")); // label wrapper is local but the underlying GTK object survive since it's referenced.
        window_.ShowAll();
    }
    gtk::Label &Label() {
        if (gtk::Label *l = dynamic_cast<gtk::Label *>(window_.Child()))
            return *l;
        else
            throw "AAAARGH, error!";
    }
};
int main() {
    MyApp app;
    std::cerr << "Label text: " << app.Label() << "\n";
}
\endexample
*/
            void Child(const Widget &w ) { Add(w); }
    };

/// Abstract base class for MenuItem.
    class Item : public Bin // COMPLETE API
    {
        public:
/// DOXYS_OFF
            operator  GtkItem *() const { return GTK_ITEM(Obj()); }
/// DOXYS_ON
            /// Emits the "select" signal on the item.
            void Select() { gtk_item_select(*this); }
            /// Emits the "deselect" signal on the item.
            void Deselect() { gtk_item_deselect(*this); }
            /// Emits the "toggle" signal on the item.
            void Toggle() { gtk_item_toggle(*this); }
    };

/** A widget used to catch events for widgets which do not have their own window.
The EventBox widget is a subclass of Bin which also has its own window. It is useful since it allows you to catch events for widgets which do not have their own window. You can use it also to change properties like background colors for the child NoWindow widget.
*/
    class EventBox : public Bin // COMPLETE API
    {
        public:
/// DOXYS_OFF
            operator  GtkEventBox *() const { return GTK_EVENT_BOX(Obj()); }
/// DOXYS_ON
/// Creates a new event box without child widget.
            EventBox() {
                Init(gtk_event_box_new());
                Internal(true);
            }
/// Creates a new event box with a child widget.
            EventBox(const Widget &w) {
                Init(gtk_event_box_new());
                Child(w);
                Internal(true);
            }
            /** Set whether the event box window is positioned above the windows of its child, as opposed to below it. 

If the window is above, all events inside the event box will go to the event box. If the window is below, events in windows of child widgets will first got to that widget, and then to its parents.

The default is to keep the window below the child.
            */
            void AboveChild(bool flag /**< true if the event box window is above the windows of its child */) { 
                gtk_event_box_set_above_child(*this, flag); 
            }
            /** Returns whether the event box window is above or below the windows of its child.
\sa EventBox::AboveChild(bool) for details.
\return true if the event box window is above the window of its child. 
*/
            bool AboveChild() const { 
                return gtk_event_box_get_above_child(*this);
            }
/** Set whether the event box uses a visible or invisible child window. 

The default is to use visible windows.

In an invisible window event box, the window that the event box creates is a GDK_INPUT_ONLY window, which means that it is invisible and only serves to receive events.

A visible window event box creates a visible (GDK_INPUT_OUTPUT) window that acts as the parent window for all the widgets contained in the event box.

You should generally make your event box invisible if you just want to trap events. Creating a visible window may cause artifacts that are visible to the user, especially if the user is using a theme with gradients or pixmaps.

The main reason to create a non input-only event box is if you want to set the background to a different color or draw on it.

!Note

There is one unexpected issue for an invisible event box that has its window below the child. (See EventBox::AboveChild(bool).) Since the input-only window is not an ancestor window of any windows that descendent widgets of the event box create, events on these windows aren't propagated up by the windowing system, but only by GTK+. The practical effect of this is if an event isn't in the event mask for the descendant window (see gtk_widget_add_events()), it won't be received by the event box.

This problem doesn't occur for visible event boxes, because in that case, the event box window is actually the ancestor of the descendant windows, not just at the same place on the screen.

*/
            void VisibleWindow(bool flag /**< select if the EventBox is visible or not*/) {
                gtk_event_box_set_visible_window(*this, flag);
            }
/** Returns whether the event box has a visible window.
\sa EventBox::VisibleWindow(bool) for details.
\return true if the event box window is visible.
 */
            bool VisibleWindow() const {
                return gtk_event_box_get_visible_window(*this);
            }
    };

    /// Determines when a scroll bar will be visible.
    enum PolicyType
    {
      PolicyAlways = GTK_POLICY_ALWAYS /**< The scrollbar is always visible. */,
      PolicyAutomatic = GTK_POLICY_AUTOMATIC /**< The scrollbar will appear and disappear as necessary. For example, when all of a TreeView can not be seen. */,
      PolicyNever =GTK_POLICY_NEVER /**< The scrollbar will never appear. */
    };
    /// A pair of PolicyType to represent both horizontal and vertical adjustments.
    typedef std::pair<PolicyType, PolicyType> PolicyTypes;

/** Adds scrollbars to its child widget.
ScrolledWindow is a Bin subclass: it's a container the accepts a single child widget. ScrolledWindow adds scrollbars to the child widget and optionally draws a beveled frame around the child widget.

The scrolled window can work in two ways. Some widgets have native scrolling support; these widgets have "slots" for Adjustment objects. [5] Widgets with native scroll support include TreeView, TextView, and Layout.

For widgets that lack native scrolling support, the Viewport widget acts as an adaptor class, implementing scrollability for child widgets that lack their own scrolling capabilities. Use Viewport to scroll child widgets such as Table, Box, and so on.

If a widget has native scrolling abilities, it can be added to the ScrolledWindow with Container::Add(). If a widget does not, you must first add the widget to a Viewport, then add the Viewport to the scrolled window. 

The position of the scrollbars is controlled by the scroll adjustments. See Adjustment for the fields in an adjustment - for Scrollbar, used by ScrolledWindow, the "value" field represents the position of the scrollbar, which must be between the "lower" field and "upper - page_size." The "page_size" field represents the size of the visible scrollable area. The "step_increment" and "page_increment" fields are used when the user asks to step down (using the small stepper arrows) or page down (using for example the PageDown key).

If a ScrolledWindow doesn't behave quite as you would like, or doesn't have exactly the right layout, it's very possible to set up your own scrolling with Scrollbar and for example a Table.

 */
    class ScrolledWindow : public Bin
    {
        public:
/// DOXYS_OFF            
            operator GtkScrolledWindow *() const { return GTK_SCROLLED_WINDOW(Obj()); }
            ScrolledWindow(GObject *obj) { Init(obj); }
/// DOXYS_ON

            /** Create a new scrolled window object without children. */
            ScrolledWindow() {
                Init(gtk_scrolled_window_new(NULL, NULL));
                Internal(true);
            }

            /** Create a new scrolled window object.
The constructor argument is a child that will be added to the scrolled window. 
*/
            ScrolledWindow(const Widget &child /**< Child of the ScrolledWindow */) {
                Init(gtk_scrolled_window_new(NULL, NULL));
                Internal(true);
                Child(child);
            }
            /** Sets the scrollbar policy for the horizontal and vertical scrollbars. 
The policy determines when the scrollbar should appear; it is a pair of values from the PolicyType enumeration. If PolicyAlways, the scrollbar is always present; if PolicyNever, the scrollbar is never present; if PolicyAutomatic, the scrollbar is present only if needed (that is, if the slider part of the bar would be smaller than the trough - the display is larger than the page size).
\example
// Shows only and in any case the vertical scrollbar.
   gtk::ScrolledWindow sw;
   sw.Policy(PolicyTypes(gtk::PolicyNever, gtk::PolicyAlways));
\endexample
*/
            void Policy(const PolicyTypes &policy /**< A locally defined policy object*/) {
                gtk_scrolled_window_set_policy(*this, 
                        (GtkPolicyType)policy.first, (GtkPolicyType)policy.second);
            }
            /** Retrieves the current policy values for the horizontal and vertical scrollbars. 
             \sa ScrolledWindow::Policy(const PolicyTypes &)
             \return A PolicyTypes object containing both horizontal (retval.first) and vertical (retval.second) policy values.
             */
            PolicyTypes Policy() const {
                GtkPolicyType w, h;
                gtk_scrolled_window_get_policy(*this, &w, &h);
                return PolicyTypes((PolicyType)w, (PolicyType)h);
            }
    };
/** Base class for box containers
Box is an abstract widget which encapsulates functionality for a particular kind of container, one that organizes a variable number of widgets into a rectangular area. Box currently has two derived classes, HBox and VBox.

The rectangular area of a Box is organized into either a single row or a single column of child widgets depending upon whether the box is of type HBox or VBox, respectively. Thus, all children of a Box are allocated one dimension in common, which is the height of a row, or the width of a column.

Box uses a notion of packing. Packing refers to adding widgets with reference to a particular position in a Container. For a Box, there are two reference positions: the start and the end of the box. For a VBox, the start is defined as the top of the box and the end is defined as the bottom. For a HBox the start is defined as the left side and the end is defined as the right side.

Use repeated calls to Box::PackStart() to pack widgets into a Box from start to end. Use Box::PackEnd() to add widgets from end to start. You may intersperse these calls and add widgets from both ends of the same Box.

Because Box is a Container, you may also use Container::Add() to insert widgets into the box, and they will be packed as if with Box::PackStart() without specifying optional arguments. Use Container::Remove() to remove widgets from the Box.

Use Box::Homogeneous(bool) to specify whether or not all children of the Box are forced to get the same amount of space.

Use Box::Spacing(int) to determine how much space will be minimally placed between all children in the Box.

Use Box::ReorderChild() to move a Box child to a different place in the box.

*/
    class Box : public Container { // COMPLETE API
        public:
/// DOXYS_OFF
            operator GtkBox *() const { return GTK_BOX(Obj()); }
/// DOXYS_ON
            /** Adds child to box, packed with reference to the start of box. 
The child is packed after any other child packed with reference to the start of box.
*/
            void PackStart(const Widget &child /**< the Widget to be added to box.*/, 
                    bool expand = true /**<  flag that indicates whether extra space should be given to this child. Any extra space given to the parent Box is divided up among all children with this attribute set to true; set when packed, true by default. */, 
                    bool fill = true /**<  flag that indicates whether any extra space given to this child due to its expand attribute being set is actually allocated to the child, rather than being used as padding around the widget; set when packed, true by default. */, 
                    int padding = 0 /**< the number of extra pixels to put between this child and its neighbors, set when packed, zero by default. */) {  
                gtk_box_pack_start(*this, child, expand, fill, padding); 
            }
            /** Adds child to box, packed with reference to the end of box. 
The child is packed after (away from end of) any other child packed with reference to the end of box.
*/
            void PackEnd(const Widget &w /**< the Widget to be added to box */, 
                         bool expand = true /**< true if the new child is to be given extra space allocated to box. The extra space will be divided evenly between all children of box that use this option, defaults to true. */, 
                         bool fill = true /**< true if space given to child by the expand option is actually allocated to child, rather than just padding it. This parameter has no effect if expand is set to false. A child is always allocated the full height of a HBox and the full width of a VBox. This option affects the other dimension, defaults to true. */, 
                         int padding = 0 /**< extra space in pixels to put between this child and its neighbors, over and above the global amount specified by "spacing" property. If child is a widget at one of the reference ends of box, then padding pixels are also put between child and the reference edge of box, defaults to 0 */) { 
                gtk_box_pack_end(*this, w, expand, fill, padding); 
            }

            /// Sets the "spacing" property of box, which is the number of pixels to place between children of box.
            void Spacing(int space /**< the number of pixels to put between children */) { 
                gtk_box_set_spacing(*this, space); 
            }
            /// Gets the value set by Box::Spacing(int).
            /// \return spacing between children, in pixels.
            int Spacing(void) const { return gtk_box_get_spacing(*this); }

            /// Sets the "homogeneous" property of box, controlling whether or not all children of box are given equal space in the box.
            void Homogeneous(bool flag /**< a boolean value, true to create equal allotments, false for variable allotments */) { 
                gtk_box_set_homogeneous(*this, flag); 
            }
            /// Returns whether the box is homogeneous (all children are the same size).
            /// \return true if the box is homogeneous.
            bool Homogeneous(void) const { return gtk_box_get_homogeneous(*this); }

            /** Moves child to a new position in the list of box children. 
The list is the children field of Box, and contains both widgets packed PACK_START as well as widgets packed PACK_END, in the order that these widgets were added to box.

A widget's position in the box children list determines where the widget is packed into box. A child widget at some position in the list will be packed just after all other widgets of the same packing type that appear earlier in the list.
*/
            void ReorderChild(Widget &child /**< the Widget to move */ , 
                              int position = -1 /**< the new position for child in the list of children of box, starting from 0. If negative, indicates the end of the list, defaults to -1, so if you don't specify the parameter you'll move the child widget to the end of the list. */) {
                gtk_box_reorder_child(*this, child, position);
            }

             /** Adds childs to box, packed with reference to the start of box. 
The child is packed after any other child packed with reference to the start of box with default attributes (expand = true, fill = true, padding = 0).
*/
            void Pack(const Widget &c1, const Widget &c2) {
                PackStart(c1); PackStart(c2);
            }
            void Pack(const Widget &c1, const Widget &c2, const Widget &c3) {
                PackStart(c1); PackStart(c2); PackStart(c3);
            }
            void Pack(const Widget &c1, const Widget &c2, const Widget &c3, const Widget &c4) {
                PackStart(c1); PackStart(c2); PackStart(c3); PackStart(c4);
            }
            void Pack(const Widget &c1, const Widget &c2, const Widget &c3, const Widget &c4, const Widget &c5) {
                PackStart(c1); PackStart(c2); PackStart(c3); PackStart(c4); PackStart(c5);
            }
    };
/** A vertical container box
VBox is a container that organizes child widgets into a single column.

Use the Box packing interface to determine the arrangement, spacing, height, and alignment of VBox children.

All children are allocated the same width.

*/
    class VBox : public Box { // COMPLETE API
        public:
/// DOXYS_OFF
            VBox(GObject *obj) { Init(obj); }
/// DOXYS_ON
            /** Creates a new VBox. */
            VBox(bool homogenous = true /**< true if all children are to be given equal space allotments, defaults to true. */, 
                 int spacing = 0 /**< the number of pixels to place by default between children, defaults to 0. */) { 
                Init(gtk_vbox_new(homogenous, spacing)); Internal(true); 
            }

            /** Creates a new homogeneous VBox, with no spacing and one child. */
            VBox(const Widget &w1) {
                Init(gtk_vbox_new(true, 0)); Internal(true);
                PackStart(w1);
            }
            /** Creates a new homogeneous VBox, with no spacing and two children. */
            VBox(const Widget &w1, const Widget &w2) {
                Init(gtk_vbox_new(false, 0)); Internal(true);
                PackStart(w1);
                PackStart(w2);
            }
            /** Creates a new homogeneous VBox, with no spacing and three children. */
            VBox(const Widget &w1, const Widget &w2, const Widget &w3) {
                Init(gtk_vbox_new(false, 0)); Internal(true);
                PackStart(w1);
                PackStart(w2);
                PackStart(w3);
            }
            /** Creates a new homogeneous VBox, with no spacing and four children. */
            VBox(const Widget &w1, const Widget &w2, const Widget &w3, const Widget &w4) {
                Init(gtk_vbox_new(false, 0)); Internal(true);
                PackStart(w1);
                PackStart(w2);
                PackStart(w3);
                PackStart(w4);
            }
            /** Creates a new homogeneous VBox, with no spacing and five children. */
            VBox(const Widget &w1, const Widget &w2, const Widget &w3, const Widget &w4, const Widget &w5) {
                Init(gtk_vbox_new(false, 0)); Internal(true);
                PackStart(w1);
                PackStart(w2);
                PackStart(w3);
                PackStart(w4);
                PackStart(w5);
            }
    };
/** A horizontal container box
HBox is a container that organizes child widgets into a single row.

Use the Box packing interface to determine the arrangement, spacing, width, and alignment of HBox children.

All children are allocated the same height. 
*/
    class HBox : public Box { // COMPLETE API
        public:
/// DOXYS_OFF
            HBox(const DerivedType &) {} // do nothing
            HBox(GObject *obj) { Init(obj); }
/// DOXYS_ON
            /** Creates a new VBox. */
            HBox(bool homogenous = true /**< true if all children are to be given equal space allotments, defaults to true.  */, 
                 int spacing = 0 /**< the number of pixels to place by default between children, defaults to 0. */) { 
                Init(gtk_hbox_new(homogenous, spacing)); Internal(true); 
            }
            
            /** Creates a new homogeneous HBox, with no spacing and a single child. */
            HBox(const Widget &w1) {
                Init(gtk_hbox_new(true, 0)); Internal(true);
                PackStart(w1);
            }
            /** Creates a new homogeneous HBox, with no spacing and two children. */
            HBox(const Widget &w1, const Widget &w2) {
                Init(gtk_hbox_new(false, 0)); Internal(true);
                PackStart(w1);
                PackStart(w2);
            }
            /** Creates a new homogeneous HBox, with no spacing and three children. */
            HBox(const Widget &w1, const Widget &w2, const Widget &w3) {
                Init(gtk_hbox_new(false, 0)); Internal(true);
                PackStart(w1);
                PackStart(w2);
                PackStart(w3);
            }
            /** Creates a new homogeneous HBox, with no spacing and four children. */
            HBox(const Widget &w1, const Widget &w2, const Widget &w3, const Widget &w4) {
                Init(gtk_hbox_new(false, 0)); Internal(true);
                PackStart(w1);
                PackStart(w2);
                PackStart(w3);
                PackStart(w4);
            }
            /** Creates a new homogeneous HBox, with no spacing and five children. */
            HBox(const Widget &w1, const Widget &w2, const Widget &w3, const Widget &w4, const Widget &w5) {
                Init(gtk_hbox_new(false, 0)); Internal(true);
                PackStart(w1);
                PackStart(w2);
                PackStart(w3);
                PackStart(w4);
                PackStart(w5);
            }
    };

    /// Used to dictate the style that a ButtonBox uses to layout the buttons it contains. (See also: VButtonBox and HButtonBox).
    enum ButtonBoxStyle
    {
        ButtonBoxDefaultStyle = GTK_BUTTONBOX_DEFAULT_STYLE /**< Default packing. */,
        ButtonBoxSpread = GTK_BUTTONBOX_SPREAD /**< Buttons are evenly spread across the box. */,
        ButtonBoxEdge   = GTK_BUTTONBOX_EDGE /**< Buttons are placed at the edges of the box. */,
        ButtonBoxStart  = GTK_BUTTONBOX_START /**< Buttons are grouped towards the start of the box, (on the left for a HBox, or the top for a VBox). */,
        ButtonBoxEnd    = GTK_BUTTONBOX_END /**< Buttons are grouped towards the end of the box, (on the right for a HBox, or the bottom for a VBox). */,
        ButtonBoxCenter = GTK_BUTTONBOX_CENTER /**< Buttons are centered in the box. Since 2.12 */
    };

/** Base class for HButtonBox and VButtonBox.
The primary purpose of this class is to keep track of the various properties of HButtonBox and VButtonBox widgets.

ButtonBox::ChildSize() retrieves the minimum width and height for widgets in a given button box. ButtonBox::ChildSize(const Point &) allows those properties to be changed.

The internal padding of buttons can be retrieved and changed per button box using ButtonBox::ChildPadding().

ButtonBox::Spacing() retrieve and change default number of pixels between buttons, respectively.

ButtonBox::Layout() retrieve and alter the method used to spread the buttons in a button box across the container, respectively.

The main purpose of ButtonBox is to make sure the children have all the same size. Therefore it ignores the homogeneous property which it inherited from Box, and always behaves as if homogeneous was true.

*/
    class ButtonBox : public Box { // COMPLETE API
        public:
/// DOXYS_OFF
            operator GtkButtonBox *() const { return GTK_BUTTON_BOX(Obj()); }
/// DOXYS_ON
            /** Sets whether child should appear in a secondary group of children, typical use of a secondary child is the help button in a dialog.

This group appears after the other children if the style is ButtonBoxStart, ButtonBoxSpread or ButtonBoxEdge, and before the other children if the style is ButtonBoxEnd. For horizontal button boxes, the definition of before/after depends on direction of the widget (see Widget::Direction()). If the style is ButtonBoxStart or ButtonBoxEnd, then the secondary children are aligned at the other end of the button box from the main children. For the other styles, they appear immediately next to the main children.
*/
            void Secondary(Widget &child /**< a child of the ButtonBox */,
                          bool flag /**< if true, the child appears in a secondary group of the button box.*/) {
                gtk_button_box_set_child_secondary(*this, child, flag);
            }
            /** Returns whether child should appear in a secondary group of children. */
            bool Secondary(Widget &child /**< a child of the ButtonBox */) const {
                return gtk_button_box_get_child_secondary(*this, child);
            }

            //// Changes the way buttons are arranged in their container.
            void Layout(ButtonBoxStyle style /**< the new layout style. */) {
                gtk_button_box_set_layout(*this, (GtkButtonBoxStyle)style);
            }
            /// Retrieves the way buttons are arranged in their container.
            /// \return the actual layout style
            ButtonBoxStyle Layout() const {
                return (ButtonBoxStyle)gtk_button_box_get_layout(*this);
            }

            /// Changes the amount of internal padding used by all buttons in a given button box.
            void ChildPadding(int x /**< the horizontal padding that should be used by each button in widget. */, 
                              int y /**< 	the vertical padding that should be used by each button in widget. */ ) {
                gtk_button_box_set_child_ipadding(*this, x, y);
            }
            /// Changes the amount of internal padding used by all buttons in a given button box.
            void ChildPadding(const Point &pad /**< a Point containing the padding values.*/) {
                gtk_button_box_set_child_ipadding(*this, pad.x, pad.y);
            }
            /// Gets the default number of pixels that pad the buttons in a given button box.
            /// \return a Point containing the x and y values of the padding.
            Point ChildPadding() const {
                Point pad;
                gtk_button_box_get_child_ipadding(*this, &pad.x, &pad.y);
                return pad;
            }

            /// Sets a new default size for the children of a given button box.
            void ChildSize(int min_width /**< a default width for buttons in widget. */, 
                           int min_height /**< a default height for buttons in widget. */) {
                gtk_button_box_set_child_size(*this, min_width, min_height);
            }
            /// Sets a new default size for the children of a given button box.
            void ChildSize(const Point &minsize /**< a Point structure containing the width and height for the default child */) {
                gtk_button_box_set_child_size(*this, minsize.x, minsize.y);
            }
            /// Retrieves the current width and height of all child widgets in a button box.
            /// \return a Point containing the width and the height of the default size.
            Point ChildSize() const {
                Point minsize;
                gtk_button_box_get_child_size(*this, &minsize.x, &minsize.y);
                return minsize;
            }
    };

/** A container for arranging buttons horizontally
A button box should be used to provide a consistent layout of buttons throughout your application. The layout/spacing can be altered by the programmer, or if desired, by the user to alter the 'feel' of a program to a small degree.

Buttons are packed into a button box the same way widgets are added to any other container, using Container::Add(). You can also use Box::PackStart() or Box::PackEnd(), but for button boxes both these functions work just like Container::Add(), ie., they pack the button in a way that depends on the current layout style and on whether the button has had ButtonBox::Secondary() called on it.

The spacing between buttons can be set with Box::Spacing(). The arrangement and layout of the buttons can be changed with Box::Layout(). 
*/
class HButtonBox : public ButtonBox { // COMPLETE API
        public:
/// DOXYS_OFF
            HButtonBox(GObject *obj) { Init(obj); }
/// DOXYS_ON

/// Create a new HORIZONTAL button box.
            HButtonBox() { 
                Init(gtk_hbutton_box_new());
                Internal(true);
            }
    };

/** A container for arranging buttons vertically
	
A button box should be used to provide a consistent layout of buttons throughout your application. The layout/spacing can be altered by the programmer, or if desired, by the user to alter the 'feel' of a program to a small degree.

Buttons are packed into a button box the same way widgets are added to any other container, using Container::Add(). You can also use Box::PackStart() or Box::PackEnd(), but for button boxes both these functions work just like Container::Add(), ie., they pack the button in a way that depends on the current layout style and on whether the button has had ButtonBox::Secondary() called on it.

The spacing between buttons can be set with Box::Spacing(). The arrangement and layout of the buttons can be changed with Box::Layout(). 
*/
    class VButtonBox : public ButtonBox { // COMPLETE API
        public:
/// DOXYS_OFF
            VButtonBox(GObject *obj) { Init(obj); }
/// DOXYS_ON

/// Create a new VERTICAL button box.
            VButtonBox() { 
                Init(gtk_vbutton_box_new());
                Internal(true);
            }
    };

/** A tabbed notebook container
The Notebook widget is a Container whose children are pages that can be switched between using tab labels along one edge.

There are many configuration options for Notebook. Among other things, you can choose on which edge the tabs appear (see Notebook::TabPos()), whether, if there are too many tabs to fit the noteobook should be made bigger or scrolling arrows added (see Notebook::Scrollable), and whether there will be a popup menu allowing the users to switch pages. (see Notebook::Popup())

*/
    class Notebook : public Container
    {
         public:
/// DOXYS_OFF             
            operator  GtkNotebook *() const { return GTK_NOTEBOOK(Obj()); }
            Notebook(GObject *obj) { Init(obj); }
/// DOXYS_ON

/// Creates a new GtkNotebook widget with no pages.
            Notebook() {
                Init(gtk_notebook_new());
                Internal(true);
            }


            /// Returns the page number of the current page.
            /// \return the index (starting from 0) of the current page in the notebook. If the notebook has no pages, then -1 will be returned.
            int Current() const { return gtk_notebook_get_current_page(*this); }
            ///
            void Current(int page) { gtk_notebook_set_current_page(*this, page); }

            /// Appends a page and a label to notebook.
            int Append(const Widget &contents /**< the Widget to use as the contents of the page.*/, 
                       const Widget &label /**< the Widget to use as the label of the page.*/) {
                return gtk_notebook_append_page(*this, contents, label);
            }
            /// Appends a page to notebook, use the default label.
            int Append(const Widget &contents /**< the Widget to use as the contents of the page.*/) {
                return gtk_notebook_append_page(*this, contents, NULL);
            }
            /// Prepends a page to notebook with a particular label.
            int Prepend(const Widget &contents  /**< the Widget to use as the contents of the page.*/,
                        const Widget &label /**< the Widget to use as the label of the page.*/) {
                return gtk_notebook_prepend_page(*this, contents, label);
            }
            /// Prepends a page to notebook, use the default label.
            int Prepend(const Widget &contents  /**< the Widget to use as the contents of the page.*/) {
                return gtk_notebook_prepend_page(*this, contents, NULL);
            }
            /// Insert a page into notebook at the given position, with a particular label.
            int Insert(int position /**< the index (starting at 0) at which to insert the page, or -1 to append the page after all other pages. */, 
                       const Widget &contents /**< the Widget to use as the contents of the page.*/, 
                       const Widget &label /**< the Widget to use as the label of the page.*/) {
                return gtk_notebook_insert_page(*this, contents, label, position);
            }                
            /// Insert a page into notebook at the given position, with the default label.
            int Insert(int position /**< the index (starting at 0) at which to insert the page, or -1 to append the page after all other pages. */, 
                       const Widget &contents /**< the Widget to use as the contents of the page.*/) {
                    return gtk_notebook_insert_page(*this, contents, NULL, position);
            }
            /// Removes a page from the notebook given its index in the notebook.
            void Remove(int pos = -1 /**< he index of a notebook page, starting from 0. Defaults to -1, that means that the last page will be removed.*/) {
                gtk_notebook_remove_page(*this, pos);
            }
            /// Finds the index of the page which contains the given child widget. 
            int Page(const Widget &contents) const { return gtk_notebook_page_num(*this, contents); }

            /// Returns the child widget contained in page number page_num.
            /// \return the child widget, or NULL if page_num is out of bounds, you can dynamic cast this value to the correct type of widget.
            Widget *GetNth(int page /**< the index of a page in the notebook, or -1 to get the last page. */ ) { 
                return dynamic_cast<Widget *>(
                        Object::Find((GObject *)gtk_notebook_get_nth_page(*this, page)));
            }

            template <typename T>
            void OnPageSwitch(void (T::*cbk)(), T *base ) {
                 callback("switch-page", cbk, base);
            }
            template <typename T, typename J>
            void OnPageSwitch(void (T::*cbk)(J), T *base, J user_data) {
                 callback("switch-page", cbk, base, user_data);
            }

            /** Return a reference to the current page widget.
This member returns a reference to the Widget of the active page of the Notebook, if the notebook has no pages it throws a runtime exception.
            \return Reference to the active page Widget, throws a std::runtime_error if the Notebook has no pages.
            */
            Widget &CurrentPage() {
                if (Widget *w = GetNth(Current()))
                    return *w;
                else
                    throw std::runtime_error("Asked CurrentPage for a notebook without pages?");
            }


            // get/set methods
            /// Sets whether to show the tabs for the notebook or not.
            void ShowTabs(bool flag) { gtk_notebook_set_show_tabs(*this, flag); }
            /// Sets whether a bevel will be drawn around the notebook pages. This only has a visual effect when the tabs are not shown. See Notebook::ShowTabs().
            void ShowBorder(bool flag) {gtk_notebook_set_show_border(*this, flag); }
            /// Sets whether the tab label area will have arrows for scrolling if there are too many tabs to fit in the area.
            void Scrollable(bool flag) {gtk_notebook_set_scrollable(*this, flag); }
            /// Returns whether the tabs of the notebook are shown.
            /// \return true if the tabs are shown
            /// \sa Notebook::ShowTabs(bool)
            bool ShowTabs() const { return gtk_notebook_get_show_tabs(*this); }
            /// Returns whether a bevel will be drawn around the notebook pages.
            /// \return true if the bevel is drawn
            /// \sa Notebook::ShowBorder(bool)
            bool ShowBorder() const { return gtk_notebook_get_show_border(*this); } 
            /// Returns whether the tab label area has arrows for scrolling. 
            /// \return true if arrows for scrolling are present
            bool Scrollable() const { return gtk_notebook_get_scrollable(*this); }
            
            /// Enables/disable the popup menu: if the popup is enabled and the user clicks with the right mouse button on the bookmarks, a menu with all the pages will be popped up.
            void Popup(bool enable /**< flag to enable/disable the popup menu of the Notebook. */) { 
                if (enable) 
                    gtk_notebook_popup_enable(*this); 
                else
                    gtk_notebook_popup_disable(*this); 
            }
    };

    /// Denotes the expansion properties that a widget will have when it (or its parent) is resized. 
    enum AttachOptions
    {
        Expand = GTK_EXPAND /**< the widget should expand to take up any extra space in its container that has been allocated. */,
        Shrink = GTK_SHRINK /**< the widget should shrink as and when possible. */,
        Fill   = GTK_FILL /**< 	the widget should fill the space allocated to it. */
    };

    inline AttachOptions operator|(const AttachOptions &a, const AttachOptions &b) {
        return (AttachOptions)(((int)a)|((int)b));
    }

/** The Table object allow the programmer to arrange widgets in rows and columns, making it easy to align many widgets next to each other, horizontally and vertically.

The size of a table can later be changed with Table::Resize().

Widgets can be added to a table using Table::Attach().

To alter the space next to a specific row, use Table::RowSpacing(int, int), and for a column, Table::ColSpacing(int, int).

The gaps between all rows or columns can be changed by calling Table::RowSpacing(int, int) or Table::ColSpacing(int, int) respectively without specifying the row parameter, you can also change row and columns spacing at once with a call to Table::Spacing(int).

Table::Homogeneous(bool), can be used to set whether all cells in the table will resize themselves to the size of the largest widget in the table.
*/
    class Table : public Container { // COMPLETE API
        public:
/// DOXYS_OFF             
            operator  GtkTable *() const { return GTK_TABLE(Obj()); }

            Table(GObject *obj) { Init(obj); }
/// DOXYS_ON
/** Create a new table widget. 

An initial size must be given by specifying how many rows and columns the table should have, although this can be changed later with Table::Resize(). rows and columns must both be in the range 0 .. 65535.  
*/
            Table(int rows /**< The number of rows the new table should have. */,
                  int columns /**< The number of columns the new table should have. */,
                  bool homogeneous = false /**< If set to TRUE, all table cells are resized to the size of the cell containing the largest widget, defaults to false. */) {
                Init(gtk_table_new(rows, columns, homogeneous));
                Internal(true);
            }

/** Change the size of a Table.

If you need to change a table's size after it has been created, this method allows you to do so. 
*/
            void Resize(int rows /**< The new number of rows. */, 
                        int columns /**< The new number of columns. */
                        ) { gtk_table_resize(*this, rows, columns); }

            void Spacing(int space) {
                RowSpacing(space);
                ColSpacing(space);
            }
            void RowSpacing(int space, int row = -1) {
                if (row > -1)
                    gtk_table_set_row_spacing(*this, row, space);
                else
                    gtk_table_set_row_spacings(*this, space);
            }
            int RowSpacing(int row = -1) const {
                if (row == -1)
                    return gtk_table_get_default_row_spacing(*this);
                else
                    return gtk_table_get_row_spacing(*this, row);
            }
/** Adds a widget to a table. 

The number of 'cells' that a widget will occupy is specified by left_attach, right_attach, top_attach and bottom_attach. These each represent the leftmost, rightmost, uppermost and lowest column and row numbers of the table. (Columns and rows are indexed from zero). 
*/
            void Attach(const Widget &child /**< The widget to add. */, 
                        int left /**< the column number to attach the left side of a child widget to.  */, 
                        int top /**< the row number to attach the top of a child widget to. */, 
                        int right = -1 /**< the column number to attach the right side of a child widget to, if not specified or -1 defaults to left + 1. */, 
                        int bottom = -1 /**< 	the row number to attach the bottom of a child widget to, if not specified or -1 defaults to top + 1 */,
                        AttachOptions xoptions = (Expand|Fill) /**< Used to specify the properties of the child widget when the table is resized, defaults to AttachOptions::Expand and AttachOptions::Fill. */,
                        AttachOptions yoptions = (Expand|Fill) /**< The same as xoptions, except this field determines behaviour of vertical resizing, defaults to AttachOptions::Expand and AttachOptions::Fill.*/,
                        int xpadding = 0 /**< An integer value specifying the padding on the left and right of the widget being added to the table. */, 
                        int ypadding = 0 /**< The amount of padding above and below the child widget. */
                        ) {
                gtk_table_attach(*this, child, 
                        left, right != -1 ? right : left + 1, top, bottom != -1 ? bottom : top + 1,
                        (GtkAttachOptions)xoptions, (GtkAttachOptions)yoptions, xpadding, ypadding);
            }

            void ColSpacing(int space, int column = -1) {
                if (column > -1)
                    gtk_table_set_col_spacing(*this, column, space);
                else
                    gtk_table_set_col_spacings(*this, space);
            }
            int ColSpacing(int column = -1) const {
                if (column == -1)
                    return gtk_table_get_default_col_spacing(*this);
                else
                    return gtk_table_get_col_spacing(*this, column);
            }

            /// Changes the homogenous property of table cells, ie. whether all cells are an equal size or not. 
            void Homogeneous(bool flag /**< Set to true to ensure all table cells are the same size. Set to false if this is not your desired behaviour. */
                    ) { gtk_table_set_homogeneous(*this, flag); }
            /// Returns whether the table cells are all constrained to the same width and height.
            /// \sa Table::Homogeneous(bool)
            /// \return true if the cells are all constrained to the same size 
            bool Homogeneous() const { return gtk_table_get_homogeneous(*this); }
    };

    enum ShadowType
    {
        ShadowNone = GTK_SHADOW_NONE,
        ShadowIn = GTK_SHADOW_IN,
        ShadowOut = GTK_SHADOW_OUT,
        ShadowEtchedIn = GTK_SHADOW_ETCHED_IN,
        ShadowEtchedOut = GTK_SHADOW_ETCHED_OUT
    };

    class Frame : public Bin { // COMPLETE API
        public:
/// DOXYS_OFF             
            operator  GtkFrame *() const { return GTK_FRAME(Obj()); }
            Frame(const DerivedType &) {}
            Frame(GObject *obj) { Init(obj); }
/// DOXYS_ON

            Frame(const std::string &label = "") {
                Init(gtk_frame_new(label.empty() ? NULL : label.c_str()));
                Internal(true);
            }
            Frame(const Widget &child, const std::string &label) {
                Init(gtk_frame_new(label.empty() ? NULL : label.c_str()));
                Child(child);
                Internal(true);
            }

            void Label(const std::string &label) { gtk_frame_set_label(*this, label.c_str()); }
            void Label(const Widget &widget) { LabelWidget(widget); }
            void LabelWidget(const Widget &label) { gtk_frame_set_label_widget(*this, label); }
            std::string Label() const { return gtk_frame_get_label(*this); }
            Widget *LabelWidget() const { 
                return dynamic_cast<Widget *>(
                        Object::Find((GObject *)gtk_frame_get_label_widget(*this))); 
            }

            void LabelAlign(const Align &align) { gtk_frame_set_label_align(*this, align.first, align.second); }
            Align LabelAlign() const {
                Align align;
                gtk_frame_get_label_align(*this, &align.first, &align.second);
                return align;
            }

            void Shadow(ShadowType shadow) { gtk_frame_set_shadow_type(*this, (GtkShadowType)shadow); }
            ShadowType Shadow() const { return (ShadowType)gtk_frame_get_shadow_type(*this); }
    };
/** A frame that constrains its child to a particular aspect ratio.

The AspectFrame is useful when you want pack a widget so that it can resize but always retains the same aspect ratio. For instance, one might be drawing a small preview of a larger image. AspectFrame derives from Frame, so it can draw a label and a frame around the child. The frame will be "shrink-wrapped" to the size of the child. 
*/
    class AspectFrame : public Frame {  // COMPLETE API
        public:
/// DOXYS_OFF                         
            operator  GtkAspectFrame *() const { return GTK_ASPECT_FRAME(Obj()); }
            AspectFrame(GObject *obj) : Frame(DerivedType()) { Init(obj); }
/// DOXYS_ON
            /// Create a new AspectFrame. 
            AspectFrame(float ratio /**< The desired aspect ratio. */, 
                        const std::string &label = "" /**< 	Label text, default to empty. */,
                        const Align &align = Align(0.5, 0.5) /**< 	Alignment of the child within the allocation of the AspectFrame. This ranges both for horizontal and vertical from 0.0 (left/top aligned) to 1.0 (right/top aligned), defaults to (0.5, 0.5)  */, 
                        bool obey_child = false /**< If true, ratio is ignored, and the aspect ratio is taken from the requistion of the child, defaults to false */) : Frame(DerivedType()) {
                Init(gtk_aspect_frame_new(label.empty() ? NULL : label.c_str(),
                                     align.first, align.second, ratio, obey_child));
                Internal(true);
            }

            /// Set parameters for an existing AspectFrame. 
            void Set(float ratio /**< The new aspect ratio. */, 
                     const Align &align = Align(0.5, 0.5) /**< 	Alignment of the child within the allocation of the AspectFrame. This ranges both for horizontal and vertical from 0.0 (left/top aligned) to 1.0 (right/top aligned), defaults to (0.5, 0.5)  */, 
                     bool obey_child = false /**< If true, ratio is ignored, and the aspect ratio is taken from the requistion of the child, defaults to false */) {
                gtk_aspect_frame_set(*this, align.first, align.second, ratio, obey_child);
            }
    };

/** Used to customize the appearance of a Toolbar. 
Note that setting the toolbar style overrides the user's preferences for the default toolbar style. Note that if the button has only a label set and ToolbarIcons is used, the label will be visible, and vice versa. 
*/
    enum ToolbarStyle
    {
        ToolbarIcons = GTK_TOOLBAR_ICONS /**< Buttons display only icons in the toolbar.  */,
        ToolbarText = GTK_TOOLBAR_TEXT /**< Buttons display only text labels in the toolbar.  */,
        ToolbarBoth = GTK_TOOLBAR_BOTH /**< 	Buttons display text and icons in the toolbar. */,
        ToolbarBothHoriz = GTK_TOOLBAR_BOTH_HORIZ /**< Buttons display icons and text alongside each other, rather than vertically stacked  */
    };

    class ToolItem : public Bin {
        public:
/// DOXYS_OFF             
            operator  GtkToolItem *() const { return GTK_TOOL_ITEM(Obj()); }
            ToolItem(GObject *obj) { Init(obj); }
/// DOXYS_ON             
            ToolItem(const DerivedType &) {} // do nothing
            ToolItem() { Init(gtk_tool_item_new()); Internal(true); }
            ToolItem(const Widget &child) { 
                Init(gtk_tool_item_new()); Internal(true); 
                Child(child);
            }

            bool Expand() const { return gtk_tool_item_get_expand(*this); }
            void Expand(bool flag) { return gtk_tool_item_set_expand(*this, flag); }
    };

/** A ToolItem subclass that displays buttons

ToolButtons are ToolItems containing buttons.

You can create a new ToolButton both empty or from a stock item.

The label of a ToolButton is determined by the properties "label-widget", "label", and "stock-id". If "label-widget" is non-NULL, then that widget is used as the label. Otherwise, if "label" is non-NULL, that string is used as the label. Otherwise, if "stock-id" is non-NULL, the label is determined by the stock item. Otherwise, the button does not have a label.

The icon of a ToolButton is determined by the properties "icon-widget" and "stock-id". If "icon-widget" is non-NULL, then that widget is used as the icon. Otherwise, if "stock-id" is non-NULL, the icon is determined by the stock item. Otherwise, the button does not have a icon. 
*/
    class ToolButton : public ToolItem // COMPLETE API
    {
        public:
/// DOXYS_OFF             
            operator  GtkToolButton *() const { return GTK_TOOL_BUTTON(Obj()); }
            ToolButton(GObject *obj) : ToolItem(DerivedType()) { Init(obj); }
/// DOXYS_ON
            /// Creates a new empty ToolButton
            ToolButton() :
                ToolItem(DerivedType()) {
                Init(gtk_tool_button_new(NULL, NULL)); 
                Internal(true); 
            }
            /// Creates a new empty ToolButton with an icon and an (optional) label
            ToolButton(const Widget &icon /**< Widget to be used as icon for the button */, 
                       const char *label = NULL /**< Optional label for the ToolButton */) :
                ToolItem(DerivedType()) {
                Init(gtk_tool_button_new(icon, label)); 
                Internal(true); 
            }
            /** Creates a new ToolButton containing the image and text from a stock item. 
            Some stock ids have preprocessor macros like GTK_STOCK_OK and GTK_STOCK_APPLY. 
            */
            ToolButton(const char *stock_id /**< stock-id for this toolbutton */) : 
                ToolItem(DerivedType()) {
                Init(gtk_tool_button_new_from_stock(stock_id));
                Internal(true); 
            }
/** Sets label as the label used for the tool button.

The "label" property only has an effect if not overridden by a non-NULL "label_widget" property. If both the "label_widget" and "label" properties are NULL, the label is determined by the "stock_id" property. If the "stock_id" property is also NULL, button will not have a label.
*/
            void Label(const std::string &label /**< a string that will be used as label */) {
                gtk_tool_button_set_label(*this, label.c_str());
            }
/** Returns the label used by the tool button.

This method will return a string containing the label of this ToolButton or an empty string if the tool button doesn't have a label or uses a the label from a stock item. 

\return A string containing the label of this ToolButton
*/
            std::string Label() const { 
                if (const gchar *label = gtk_tool_button_get_label(*this)) 
                    return label;
                else
                    return "";
            }
            void StockId(const std::string &label) {
                gtk_tool_button_set_stock_id(*this, label.c_str());
            }
            std::string StockId() const {
                if (const gchar *id = gtk_tool_button_get_stock_id(*this)) 
                    return id; 
                else
                    return "";
            }

            void Icon(const Widget &icon) { gtk_tool_button_set_icon_widget(*this, icon); }
            Widget *Icon() const { 
                return dynamic_cast<Widget *>(
                        Object::Find((GObject *)gtk_tool_button_get_icon_widget(*this))); 
            }
/** Set the underline property for the ToolButton label.

If set, an underline in the label property indicates that the next character should be used for the mnemonic accelerator key in the overflow menu. For example, if the label property is "_Open" and use_underline is TRUE, the label on the tool button will be "Open" and the item on the overflow menu will have an underlined 'O'.

Labels shown on tool buttons never have mnemonics on them; this property only affects the menu item on the overflow menu.            
*/
            void Underline(bool flag) { gtk_tool_button_set_use_underline(*this, flag); }
/** Return the state of the underline property for this ToolButton.

Returns whether underscores in the label property are used as mnemonics on menu items on the overflow menu.

\sa ToolButton::Underline(bool)
*/    
            bool Underline() const { return gtk_tool_button_get_use_underline(*this); }

            /// Call a method if the ToolButton is clicked             
            template <typename T>
            void OnClick(void (T::*cbk)() /**< the method to call */, 
                         T *base /**< the base of the class containing the method above */) {
                callback("clicked", cbk, base);
            }

    };

    class SeparatorToolItem : public ToolItem // COMPLETE API
    {
        public:
            operator  GtkSeparatorToolItem *() const { return GTK_SEPARATOR_TOOL_ITEM(Obj()); }
            SeparatorToolItem(GObject *obj) : ToolItem(DerivedType()) { Init(obj); }
            SeparatorToolItem() : ToolItem(DerivedType()) {
                Init(gtk_separator_tool_item_new()); 
                Internal(true); 
            }

            bool Draw() const { return gtk_separator_tool_item_get_draw(*this); }
            void Draw(bool flag) { return gtk_separator_tool_item_set_draw(*this, flag); }
    };

/** Create bars of buttons and other widgets

A toolbar can contain instances of a subclass of ToolItem. To add a ToolItem to the a toolbar, use Toolbar::Insert(), Toolbar::Append() or one of the constructors. To remove an item from the toolbar use Container::Remove(). To add a button to the toolbar, add an instance of ToolButton.

Toolbar items can be visually grouped by adding instances of SeparatorToolItem to the toolbar. If a SeparatorToolItem has the "expand" property set to TRUE and the "draw" property set to FALSE the effect is to force all following items to the end of the toolbar.

Creating a context menu for the toolbar can be done by connecting to the "popup-context-menu" signal. 
*/
    class Toolbar : public Container {
/// DOXYS_OFF
        private:
            void newitem() { Init(gtk_toolbar_new()); Internal(true); }
        public:
            operator  GtkToolbar *() const { return GTK_TOOLBAR(Obj()); }
            Toolbar(GObject *obj) { Init(obj); }
/// DOXYS_ON            
            /// Creates a new empty Toolbar
            Toolbar() { newitem(); }

            /// Creates a new Toolbar with a ToolItem
            Toolbar(const ToolItem &i1) { 
                newitem(); Append(i1);
            }
            /// Creates a new Toolbar with two ToolItem
            Toolbar(const ToolItem &i1, const ToolItem &i2) { 
                newitem(); Append(i1); Append(i2);
            }
            /// Creates a new Toolbar with three ToolItem
            Toolbar(const ToolItem &i1, const ToolItem &i2, const ToolItem &i3) { 
                newitem(); Append(i1); Append(i2); Append(i3);
            }
            /// Creates a new Toolbar with four ToolItem
            Toolbar(const ToolItem &i1, const ToolItem &i2, const ToolItem &i3, const ToolItem &i4) { 
                newitem(); Append(i1); Append(i2); Append(i3); Append(i4);
            }
            /// Creates a new Toolbar with five ToolItem
            Toolbar(const ToolItem &i1, const ToolItem &i2, const ToolItem &i3, const ToolItem &i4, const ToolItem &i5) { 
                newitem(); Append(i1); Append(i2); Append(i3); Append(i4); Append(i5);
            }
            /// Creates a new Toolbar with six ToolItem
            Toolbar(const ToolItem &i1, const ToolItem &i2, const ToolItem &i3, const ToolItem &i4, const ToolItem &i5, const ToolItem &i6) { 
                newitem(); Append(i1); Append(i2); Append(i3); Append(i4); Append(i5); Append(i6);
            }
            /// Creates a new Toolbar with seven ToolItem
            Toolbar(const ToolItem &i1, const ToolItem &i2, const ToolItem &i3, const ToolItem &i4, const ToolItem &i5, const ToolItem &i6, const ToolItem &i7) { 
                newitem(); Append(i1); Append(i2); Append(i3); Append(i4); Append(i5); Append(i6); Append(i7);
            }

            void IconSize(gtk::IconSize size) {
                gtk_toolbar_set_icon_size(*this, (GtkIconSize)size);
            }
            gtk::IconSize IconSize() const {
                return (gtk::IconSize)gtk_toolbar_get_icon_size(*this);
            }
            void Style(ToolbarStyle style) {
                gtk_toolbar_set_style(*this, (GtkToolbarStyle)style);
            }
            ToolbarStyle Style() const {
                return (ToolbarStyle)gtk_toolbar_get_style(*this);
            }
            ToolItem *GetNth(int idx) { 
                return dynamic_cast<ToolItem *>(
                        Object::Find((GObject *)gtk_toolbar_get_nth_item(*this, idx)));
            }
            /** Add a ToolItem at a particular position in the Toolbar

Insert a ToolItem into the toolbar at position pos. If pos is 0 the item is prepended to the start of the toolbar. If pos is negative, the item is appended to the end of the toolbar.
*/
            void Insert(const ToolItem &item /**< A ToolItem. */, 
                        int pos /**< the position of the new item. */) {
                gtk_toolbar_insert(*this, item, pos);
            }
            /// Add a ToolItem at the end of the Toolbar
            void Append(const ToolItem &item /**< A ToolItem. */) { Insert(item, -1); }
    };

    class Statusbar : public HBox // complete API
    {
        public:
            operator  GtkStatusbar *() const { return GTK_STATUSBAR(Obj()); }
            Statusbar(GObject *obj) : HBox(DerivedType()) { Init(obj); }
            Statusbar() : HBox(DerivedType()) { 
                Init(gtk_statusbar_new());
                Internal(true);
            }

            int ContextId(const std::string &desc) {
                return gtk_statusbar_get_context_id(*this, desc.c_str());
            }

            int Push(int context_id, const std::string &text) {
                return gtk_statusbar_push(*this, context_id, text.c_str());
            }
            void Pop(int context_id) { gtk_statusbar_pop(*this, context_id); }
            void Remove(int context_id, int message_id) {
                gtk_statusbar_remove(*this, context_id, message_id);
            }

            void ResizeGrip(bool flag) { gtk_statusbar_set_has_resize_grip(*this, flag); }
            bool ResizeGrip() const { return gtk_statusbar_get_has_resize_grip(*this); }
    };

    enum SizeGroupMode {
        SizeGroupNone = GTK_SIZE_GROUP_NONE,
        SizeGroupHorizontal = GTK_SIZE_GROUP_HORIZONTAL,
        SizeGroupVertical = GTK_SIZE_GROUP_VERTICAL,
        SizeGroupBoth = GTK_SIZE_GROUP_BOTH
    };

    class SizeGroup : public Object {
        public:
            operator  GtkSizeGroup *() const { return GTK_SIZE_GROUP(Obj()); }
            SizeGroup(GObject *obj) { Init(obj); }
            SizeGroup(SizeGroupMode mode = SizeGroupHorizontal) { 
                Init(gtk_size_group_new((GtkSizeGroupMode)mode));
                Internal(true);
            }
            void Mode(SizeGroupMode mode) { gtk_size_group_set_mode(*this, (GtkSizeGroupMode)mode); }
            SizeGroupMode Mode() const { return (SizeGroupMode)gtk_size_group_get_mode(*this); }

            void IgnoreHidden(bool flag) { gtk_size_group_set_ignore_hidden(*this, flag); }
            bool IgnoreHidden() const { return gtk_size_group_get_ignore_hidden(*this); }
            void Add(Widget &w) { gtk_size_group_add_widget(*this, w); }
            void Remove(Widget &w) { gtk_size_group_remove_widget(*this, w); }
            void Widgets(WidgetList &list) {
                GSList *l = gtk_size_group_get_widgets(*this);

                while (l) {
                    if (Widget *w = dynamic_cast<Widget *>(Object::Find((GObject *)l->data)))
                        list.push_back(w);
                    l = l->next;
                }
            }
    };
}

#endif
