#ifndef OOBUTTON_H
#define OOBUTTON_H

#include "oocont.h"

namespace gtk {
    /// Indicated the relief to be drawn around a Button.
    enum ReliefStyle
    {
        ReliefNormal = GTK_RELIEF_NORMAL /**< Draw a normal relief. */,
        ReliefHalf = GTK_RELIEF_HALF /**< A half relief. */,
        ReliefNone = GTK_RELIEF_NONE /**< No relief. */
    };

    /// Describes which edge of a widget a certain feature is positioned at, e.g. the tabs of a gtk::Notebook, the handle of a gtk::HandleBox or the label of a gtk::Scale.
    enum PositionType
    {
      PosLeft = GTK_POS_LEFT /**< The feature is at the left edge. */, 
      PosRight = GTK_POS_RIGHT /**< The feature is at the right edge. */, 
      PosTop = GTK_POS_TOP /**< The feature is at the top edge.*/,
      PosBottom = GTK_POS_BOTTOM /**< The feature is at the bottom edge. */
    };

/** Range is the common base class for widgets which visualize an adjustment, e.g scales or scrollbars.

Apart from signals for monitoring the parameters of the adjustment, Range provides properties and methods for influencing the sensitivity of the "steppers". It also provides properties and methods for setting a "fill level" on range widgets. See Range::FillLevel(double).   
*/
    class Range : public Widget
    {
        public: 
/// DOXYS_OFF
            operator  GtkRange *() const { return GTK_RANGE(Obj()); }

            Range(GObject *obj) { Init(obj); }
            Range(const DerivedType &) {} // empty constructor for derived classes
/// DOXYS_ON
            /// Gets the current position of the fill level indicator.
            double FillLevel() const { return gtk_range_get_fill_level(*this); }
            /** Set the new position of the fill level indicator.

The "fill level" is probably best described by its most prominent use case, which is an indicator for the amount of pre-buffering in a streaming media player. In that use case, the value of the range would indicate the current play position, and the fill level would be the position up to which the file/stream has been downloaded.

This amount of prebuffering can be displayed on the range's trough and is themeable separately from the trough. To enable fill level display, use Range::ShowFillLevel(bool). The range defaults to not showing the fill level.

Additionally, it's possible to restrict the range's slider position to values which are smaller than the fill level. This is controller by Range::RestrictToFillLevel(bool) and is by default enabled.
*/
            void FillLevel(double level) { gtk_range_set_fill_level(*this, level); }
            /// Gets whether the range is restricted to the fill level.
            bool RestrictToFillLevel() const { return gtk_range_get_restrict_to_fill_level(*this); }
            /// Sets whether the slider is restricted to the fill level. See Range::FillLevel(double) for a general description of the fill level concept.
            void RestrictToFillLevel(bool flag) { gtk_range_set_restrict_to_fill_level(*this, flag); }
            /// Gets whether the range displays the fill level graphically.
            bool ShowFillLevel() const { return gtk_range_get_show_fill_level(*this); }
            /// Sets whether a graphical fill level is show on the trough. 
            void ShowFillLevel(bool flag) { gtk_range_set_show_fill_level(*this, flag); }
            /// Gets the value set by Range::Inverted(bool).
            bool Inverted() const { return gtk_range_get_inverted(*this); }
            /// Ranges normally move from lower to higher values as the slider moves from top to bottom or left to right. Inverted ranges have higher values at the top or on the right rather than on the bottom or left.
            void Inverted(bool flag) { gtk_range_set_inverted(*this, flag); }
            /** Sets the step and page sizes for the range. 
              
The step size is used when the user clicks the Scrollbar arrows or moves Scale via arrow keys. The page size is used for example when moving via Page Up or Page Down keys.
*/
            void Increments(double step, double page) { gtk_range_set_increments(*this, step, page); }
            /// Sets the allowable values in the Range, and clamps the range value to be between min and max. (If the range has a non-zero page size, it is clamped between min and max - page-size.)
            void SetRange(double vmin, double vmax) { gtk_range_set_range(*this, vmin, vmax); }
            /// Sets the current value of the range; if the value is outside the minimum or maximum range values, it will be clamped to fit inside them. The range emits the "value-changed" signal if the value changes.
            void Value(double val) { gtk_range_set_value(*this, val); }
            /// Gets the current value of the range.
            double Value() const { return gtk_range_get_value(*this); }

#if GTK_MINOR_VERSION > 16

            /// This function returns sliders range along the long dimension, in widget->window coordinates.
            void SliderRange(int &slider_start,
                             int &slider_end) {
                gtk_range_get_slider_range(*this, &slider_start, &slider_end);
            }
            /// Sets the minimum size of the range's slider.
            void MinSliderSize(int min_size /**< The slider minimum size in pixels */) { gtk_range_set_min_slider_size(*this, min_size); }
            /// This function is useful mainly for Range subclasses. See Range::MinSliderSize(int) for more info.
            int MinSliderSize() const { return gtk_range_get_min_slider_size (*this); }
            /// Gets the value set by Range::Flippable(bool).
            bool Flippable() const { return gtk_range_get_flippable(*this); }
            /// If a range is flippable, it will switch its direction if it is horizontal and its direction is GTK_TEXT_DIR_RTL.
            void Flippable(bool flag) { gtk_range_set_flippable(*this, flag); }
            /// The callback you pass to this method is fired when the range value changes.
#endif
            BUILD_VOID_EVENT(OnChanged, "value-changed");
    };

/** A Scale is a slider control used to select a numeric value. 
  
To use it, you'll probably want to investigate the methods on its base class, Range, in addition to the methods for Scale itself. To set the value of a scale, you would normally use Range::Value(double). To detect changes to the value, you would normally use the "value_changed" signal.

The Scale widget is an abstract class, used only for deriving the subclasses HScale and VScale. To create a scale widget, call HScale::HScale() or VScale::VScale().
*/
    class Scale : public Range
    {
        public: 
/// DOXYS_OFF
            operator  GtkScale *() const { return GTK_SCALE(Obj()); }

            Scale(GObject *obj) : Range(DerivedType()) { Init(obj); }
            Scale(const DerivedType &d) : Range(d) {} // empty constructor for derived classes
/// DOXYS_ON
            /// Sets the number of decimal places that are displayed in the value. Also causes the value of the adjustment to be rounded off to this number of digits, so the retrieved value matches the value the user saw.
            void Digits(unsigned int d) { gtk_scale_set_digits(*this, d); }
            /// Gets the number of decimal places that are displayed in the value.
            int Digits() const { return gtk_scale_get_digits(*this); }
            /// Specifies whether the current value is displayed as a string next to the slider.
            void DrawValue(bool flag) { gtk_scale_set_draw_value(*this, flag); }
            /// Returns whether the current value is displayed as a string next to the slider.
            bool DrawValue() const { return gtk_scale_get_draw_value(*this); }
    };

/** The HScale widget is used to allow the user to select a value using a horizontal slider. 

The position to show the current value, and the number of decimal places shown can be set using the parent Scale and Range class's functions.
*/
    class HScale : public Scale
    {
        public:
/// DOXYS_OFF
            operator  GtkHScale *() const { return GTK_HSCALE(Obj()); }

            HScale(GObject *obj) : Scale(DerivedType()) { Init(obj); }
/// DOXYS_ON        
            /** Creates a new horizontal scale widget that lets the user input a number between min and max (including min and max) with the increment step. step must be nonzero; it's the distance the slider moves when using the arrow keys to adjust the scale value.

Note that the way in which the precision is derived works best if step is a power of ten. If the resulting precision is not suitable for your needs, use Scale::Digits(unsigned int) to correct it.
*/
            HScale(double vmin /**< minimum value */,
                   double vmax /**< maximum value */,
                   double step /**< step increment (tick size) used with keyboard shortcuts */) : Scale(DerivedType()) { 
                Init(gtk_hscale_new_with_range(vmin, vmax, step)); 
                Internal(true); 
            }
    };

/** The VScale widget is used to allow the user to select a value using a vertical slider. 

The position to show the current value, and the number of decimal places shown can be set using the parent Scale and Range class's functions.
*/
    class VScale : public Scale
    {
        public:
/// DOXYS_OFF
            operator  GtkVScale *() const { return GTK_VSCALE(Obj()); }

            VScale(GObject *obj) : Scale(DerivedType()) { Init(obj); }
/// DOXYS_ON        
            /** Creates a new vertical scale widget that lets the user input a number between min and max (including min and max) with the increment step. step must be nonzero; it's the distance the slider moves when using the arrow keys to adjust the scale value.

Note that the way in which the precision is derived works best if step is a power of ten. If the resulting precision is not suitable for your needs, use Scale::Digits(unsigned int) to correct it.
*/
            VScale(double vmin /**< minimum value */,
                   double vmax /**< maximum value */,
                   double step /**< step increment (tick size) used with keyboard shortcuts */) : Scale(DerivedType()) { 
                Init(gtk_vscale_new_with_range(vmin, vmax, step)); 
                Internal(true); 
            }
    };

    /** A widget that creates a signal when clicked on
The Button widget is generally used to attach a function to that is called when the button is pressed. The various signals and how to use them are outlined below.

The Button widget can hold any valid child widget. That is it can hold most any other standard Widget. The most commonly used child is the Label. 
*/
    class Button : public Bin
    {
        public: 
/// DOXYS_OFF
            operator  GtkButton *() const { return GTK_BUTTON(Obj()); }

            Button(GObject *obj) { Init(obj); }
            Button(const DerivedType &) {} // empty constructor for derived classes
/// DOXYS_ON

            /** Creates a new gtk::Button widget. If you don't specify a label to add a child widget to the button, use Bin::Child(const Widget &).
             */
            Button(const std::string &label = "") {
                if (!label.empty())
                    Init(gtk_button_new_with_label(label.c_str()));
                else
                    Init(gtk_button_new());

                Internal(true);
            }

            /** Sets the relief style of the edges of the given gtk::Button widget. Three styles exist, gtk::ReliefNormal, gtk::ReliefHalf, gtk::ReliefNone. The default style is, as one can guess, gtk::ReliefNormal.
            */
            void Relief(ReliefStyle relief /**< A valid gtk::ReliefStyle value as described above. */ ) {
                gtk_button_set_relief(*this, (GtkReliefStyle)relief);
            }
            /** Returns the current relief style of the given gtk::Button.
             \return The current gtk::ReliefStyle used by the button.
             */
            ReliefStyle Relief() const { 
                return (ReliefStyle)gtk_button_get_relief(*this);
            }

            /** Set the image of button to the given widget. Note that it depends on the "gtk-button-images" setting whether the image will be displayed or not, you don't have to call gtk::Widget::Show() on image yourself.
             */
            void Image(const gtk::Widget &img /** a widget to set as the image for the button */) { gtk_button_set_image(*this, img); }

            /** Gets the widget that is currenty set as the image of button. This may have been explicitly set by gtk::Button::Image(const gtk::Image &) or constructed by the gtk::StockButton constructor.
             \return a pointer to the widget specified as image, NULL if there is no image.
             */
            Widget *Image() const {
                GtkWidget *w = gtk_button_get_image(*this);
                return dynamic_cast<Widget *>(Object::Find((GObject *)w));
            }

            /** Sets the text of the label of the button.
                This will also clear any previously set labels.
              */
            void Label(const std::string &label /** New text for the button. */) {
                gtk_button_set_label(*this, label.c_str());
            }
            /** Fetches the text from the label of the button, as set by gtk_button_set_label().
                If the label text has not been set the return value will be an empty string. This will be the case if you create an empty button with the default constructor gtk::Button() to use as a container.
                \return a label containing the text of the button.
               */
            std::string Label() const { return gtk_button_get_label(*this); }

            /** Sets the alignment of the child. This property has no effect unless the child is a gtk::Misc or a gtk::Aligment.
             */
            void Alignment(const Align &align /** the position of the child (0.0,0.0 is topleft)*/ ) {
                gtk_button_set_alignment(*this, align.first, align.second);
            }
            /** Gets the alignment of the child in the button.
             \return a gtk::Align where (0.0, 0.0) is topleft and (1.0, 1.0) is bottom right.
             */
            Align Alignment() const {
                Align align;
                gtk_button_get_alignment(*this, &align.first, &align.second);
                return align;
            }
            /** Sets the position of the image relative to the text inside the button.
             */
            void ImagePosition(PositionType pos /** the position */ ) {
                gtk_button_set_image_position(*this, (GtkPositionType)pos);
            }
            /** Gets the position of the image relative to the text inside the button.
             \return the position
             */
            PositionType ImagePosition() const {
                return (PositionType)gtk_button_get_image_position(*this);
            }

            /** if true, an underline in the text of the button label indicates the next character should be used for the mnemonic accelerator key. 
             */
            void Underline(bool flag /** true if underlines in the text indicate mnemonics*/) { 
                gtk_button_set_use_underline(*this, flag); 
            }
            /** Returns whether an embedded underline in the button label indicates a mnemonic.
             \return true if underlines in the text indicate mnemonics
             */
            bool Underline() const { return gtk_button_get_use_underline(*this); }

            /** Sets whether the button will grab focus when it is clicked with the mouse. Making mouse clicks not grab focus is useful in places like toolbars where you don't want the keyboard focus removed from the main area of the application.
             */
            void FocusOnClick(bool flag /** whether the button grabs focus when clicked with the mouse */) { gtk_button_set_focus_on_click(*this, flag); }
            /** Returns whether the button grabs focus when it is clicked with the mouse.
             \return true if the button grabs focus when it is clicked with the mouse, false otherwise.
             */
            bool FocusOnClick() const { return gtk_button_get_focus_on_click(*this); }

            // button specific signals
            /** Attach a callback to the signal emitted when the button has been activated (pressed and released). */
            BUILD_VOID_EVENT(OnClick, "clicked");
            /** Attach a callback to the signal emitted when the button has been pressed. */
            BUILD_VOID_EVENT(OnPress, "pressed");
            /** Attach a callback to the signal emitted when the button has been released. */
            BUILD_VOID_EVENT(OnRelease, "released");
    };

    /** Create a button with from a stock item.
     This is a simple class wrapper to create a button from a stock item, the object you
     create is a button, do not try to dynamic cast it to a StockButton cause that operation may fail.
     */
    class StockButton : public Button
    {
        public:
            StockButton(const char *id ///< stock id for the button.
                                         ) : Button(DerivedType()) { 
                Init(gtk_button_new_from_stock(id)); 
                Internal(true); 
            }
    };

    /** Create buttons which retain their state
A ToggleButton is a Button which will remain 'pressed-in' when clicked. Clicking again will cause the toggle button to return to its normal state.

A toggle button is created by calling one of his constructors. If you don't specify a label, it is advisable to pack a widget, (such as a Label and/or a Pixmap), into the toggle button's container. (See Button for more information).

The state of a ToggleButton can be set specifically using ToggleButton::Active(bool), and retrieved using ToggleButton::Active().
    */
    class ToggleButton : public Button
    {
        public:
/// DOXYS_OFF
            operator  GtkToggleButton *() const { return GTK_TOGGLE_BUTTON(Obj()); }

            ToggleButton(GObject *obj) : Button(DerivedType()) { Init(obj); }

            ToggleButton(const DerivedType &) {} // empty costructor for subclasses
/// DOXYS_ON
            /** Creates a new toggle button. A widget should be packed into the button, as in Button::Button(), if you specify a label text a Label widget will be created and packed in the widget for you.
             */
            ToggleButton(const std::string &label = "" /**< Optional label for the Togglebutton */
                         ) : Button(DerivedType()) {
                if (label.empty())
                    Init(gtk_toggle_button_new());
                else
                    Init(gtk_toggle_button_new_with_label(label.c_str()));
                Internal(true);
            }

            /** Queries a ToggleButton and returns its current state. Returns true if the toggle button is pressed in and false if it is raised.
             \return a boolean value. 
             */
            bool Active() const { return gtk_toggle_button_get_active(*this); }
            /** Sets the status of the toggle button. Set to true if you want the ToggleButton to be 'pressed in', and false to raise it. This action causes the toggled signal to be emitted.
            */
            void Active(bool flag /**< 	true or false. */) { gtk_toggle_button_set_active(*this, flag); }
            /** Gets the value set by ToggleButton::Inconsistent(bool).
             \return true if the button is displayed as inconsistent, false otherwise
             */
            bool Inconsistent() const { return gtk_toggle_button_get_inconsistent(*this); }
            /**
If the user has selected a range of elements (such as some text or spreadsheet cells) that are affected by a toggle button, and the current values in that range are inconsistent, you may want to display the toggle in an "in between" state. This function turns on "in between" display. Normally you would turn off the inconsistent state again if the user toggles the toggle button. This has to be done manually, gtk_toggle_button_set_inconsistent() only affects visual appearance, it doesn't affect the semantics of the button.
            */
            void Inconsistent(bool flag /**< true if state is inconsistent, false otherwise */) { 
                gtk_toggle_button_set_inconsistent(*this, flag); 
            }
            /** Retrieves whether the button is displayed as a separate indicator and label.
             \return true if the ToggleButton is drawn as a separate indicator and label. 
             */
            bool Mode() const { return gtk_toggle_button_get_mode(*this); }
            /** Sets whether the button is displayed as a separate indicator and label. You can call this function on a checkbutton or a radiobutton with draw_indicator = FALSE to make the button look like a normal button

This function only affects instances of classes like GtkCheckButton and GtkRadioButton that derive from GtkToggleButton, not instances of GtkToggleButton itself.
            */
            void Mode(bool flag /**< true if the togglebutton is drawn as a separate indicator and label. */) { 
                gtk_toggle_button_set_mode(*this, flag); 
            }

            /// Connect a callback to this signal if you wish to perform an action whenever the ToggleButton's state is changed.
            BUILD_VOID_EVENT(OnToggle, "toggled");
    };

    /** Create widgets with a discrete toggle button
A CheckButton places a discrete ToggleButton next to a widget, (usually a Label). See the section on ToggleButton widgets for more information about toggle/check buttons.

The important signal ('toggled', connected through the member function ToggleButton::OnToggle()) is also inherited from ToggleButton. 
    */
    class CheckButton : public ToggleButton
    {
        public:
/// DOXYS_OFF
            CheckButton(GObject *obj) : ToggleButton(DerivedType()) { Init(obj); }
            CheckButton(const DerivedType &) : ToggleButton(DerivedType()) {} // empty costructor for subclasses
/// DOXYS_ON            
            CheckButton(const std::string &label = "") : ToggleButton(DerivedType()) {
                if (!label.empty())
                    Init(gtk_check_button_new_with_label(label.c_str()));
                else
                    Init(gtk_check_button_new());
                Internal(true);
            }
    };

    class LinkButton : public Button
    {
        public:
            operator  GtkLinkButton *() const { return GTK_LINK_BUTTON(Obj()); }
            LinkButton(GObject *obj) : Button(DerivedType()) { Init(obj); }
            LinkButton(const std::string &uri, const std::string &label = "") : 
                Button(DerivedType()) {
                if (!label.empty())
                    Init(gtk_link_button_new_with_label(uri.c_str(), label.c_str()));
                else
                    Init(gtk_link_button_new(uri.c_str()));
                Internal(true);
            }

            void Uri(const std::string &uri) { gtk_link_button_set_uri(*this, uri.c_str()); }
            std::string Uri() const { return gtk_link_button_get_uri(*this); }
    };

    class RadioGroup // simple wrapper for GSList, may be expanded with a Get method if needed
    {
        public:
            operator  GSList *() { return list_; }
            RadioGroup() {
                list_ = NULL;
            }
            RadioGroup(GSList *l) : list_(l) {}
        private:
            GSList *list_;
    };

    class RadioButton : public CheckButton // complete API
    {
        public:
            operator  GtkRadioButton *() const { return GTK_RADIO_BUTTON(Obj()); }
            RadioButton(GObject *obj) : CheckButton(DerivedType()) { Init(obj); }
            RadioButton(const std::string &label = "") :
                CheckButton(DerivedType()) {
                if (!label.empty())
                    Init(gtk_radio_button_new_with_label(NULL, label.c_str()));
                else
                    Init(gtk_radio_button_new(NULL));
                Internal(true);
            }
            RadioButton(const RadioGroup &group, const std::string &label = "") : 
                CheckButton(DerivedType()) {
                RadioGroup &gr = const_cast<RadioGroup &>(group);

                if (!label.empty())
                    Init(gtk_radio_button_new_with_label(gr, label.c_str()));
                else
                    Init(gtk_radio_button_new(gr));

                gr = Group();
                Internal(true);
            }
            RadioButton(const RadioButton &groupwith, const std::string &label = "") : 
                CheckButton(DerivedType()) {
                if (!label.empty())
                    Init(gtk_radio_button_new_with_label_from_widget(groupwith, label.c_str()));
                else
                    Init(gtk_radio_button_new_from_widget(groupwith));
                Internal(true);
            }
            void Group(RadioGroup &group) { gtk_radio_button_set_group(*this, group); }
            RadioGroup Group() { return RadioGroup(gtk_radio_button_get_group(*this)); }
    };

}

#endif
