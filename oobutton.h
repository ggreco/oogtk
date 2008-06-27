#ifndef OOBUTTON_H
#define OOBUTTON_H

#include "oocont.h"

namespace gtk {
    enum ReliefStyle
    {
        ReliefNormal = GTK_RELIEF_NORMAL,
        ReliefHalf = GTK_RELIEF_HALF,
        ReliefNone = GTK_RELIEF_NONE
    };

    class Button : public Bin
    {
        public:
            operator  GtkButton *() const { return GTK_BUTTON(Obj()); }

            Button(GObject *obj) { Init(obj); }
            Button(const DerivedType &) {} // empty constructor for derived classes

            Button(const std::string &label = "") {
                if (!label.empty())
                    Init(gtk_button_new_with_label(label.c_str()));
                else
                    Init(gtk_button_new());

                Internal(true);
            }

            void Relief(OneOf<GtkReliefStyle, ReliefStyle> relief) {
                gtk_button_set_relief(*this, relief);
            }
            OneOf<GtkReliefStyle, ReliefStyle> Relief() const { 
                return gtk_button_get_relief(*this);
            }

            // Added gtk prefix to avoid name conflict between class and method
            void Image(const gtk::Image &img) { gtk_button_set_image(*this, img); }
            Widget *Image() const {
                GtkWidget *w = gtk_button_get_image(*this);
                return dynamic_cast<Widget *>(Object::Find((GObject *)w));
            }

            void Label(const std::string &label) {
                gtk_button_set_label(*this, label.c_str());
            }
            std::string Label() const { return gtk_button_get_label(*this); }

            void Alignment(const Align &align) {
                gtk_button_set_alignment(*this, align.first, align.second);
            }
            Align Alignment() const {
                Align align;
                gtk_button_get_alignment(*this, &align.first, &align.second);
                return align;
            }

            void ImagePosition(OneOf<GtkPositionType, PositionType> pos) {
                gtk_button_set_image_position(*this, pos);
            }
            OneOf<GtkPositionType, PositionType> ImagePosition() const {
                return gtk_button_get_image_position(*this);
            }

            void Underline(bool flag) { gtk_button_set_use_underline(*this, flag); }
            bool Underline() const { return gtk_button_get_use_underline(*this); }

            void FocusOnClick(bool flag) { gtk_button_set_focus_on_click(*this, flag); }
            bool FocusOnClick() const { return gtk_button_get_focus_on_click(*this); }

            // button specific signals
            BUILD_VOID_EVENT(OnClick, "clicked");
            BUILD_VOID_EVENT(OnPress, "pressed");
            BUILD_VOID_EVENT(OnRelease, "released");
    };

    class StockButton : public Button
    {
        public:
            StockButton(const char *id) : Button(DerivedType()) { 
                Init(gtk_button_new_from_stock(id)); 
                Internal(true); 
            }
    };

    class ToggleButton : public Button
    {
        public:
            operator  GtkToggleButton *() const { return GTK_TOGGLE_BUTTON(Obj()); }

            ToggleButton(GObject *obj) : Button(DerivedType()) { Init(obj); }
            ToggleButton(const std::string &label = "") : Button(DerivedType()) {
                if (label.empty())
                    Init(gtk_toggle_button_new());
                else
                    Init(gtk_toggle_button_new_with_label(label.c_str()));
                Internal(true);
            }
            ToggleButton(const DerivedType &) {} // empty costructor for subclasses

            bool Active() const { return gtk_toggle_button_get_active(*this); }
            void Active(bool flag) { gtk_toggle_button_set_active(*this, flag); }
            bool Inconsistent() const { return gtk_toggle_button_get_inconsistent(*this); }
            void Inconsistent(bool flag) { gtk_toggle_button_set_inconsistent(*this, flag); }
            bool Mode() const { return gtk_toggle_button_get_mode(*this); }
            void Mode(bool flag) { gtk_toggle_button_set_mode(*this, flag); }

            BUILD_VOID_EVENT(OnToggle, "toggled");
    };

    class CheckButton : public ToggleButton
    {
        public:
            CheckButton(GObject *obj) : ToggleButton(DerivedType()) { Init(obj); }
            CheckButton(const DerivedType &) : ToggleButton(DerivedType()) {} // empty costructor for subclasses
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
