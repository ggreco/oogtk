#ifndef OOGDK_H
#define OOGDK_H

#include "ooobj.h"

namespace gtk
{
    // convert an hex color in format #aabbcc (RR GG BB) to a double[3] color (0...1, 0...1, 0...1) suitable for cairo
    inline bool hex_color(const std::string &src, double *dest) {
        if (src[0] != '#' && src.length() != 7)
            return false;
        std::stringstream ss;
        ss << std::hex << src.substr(1);
        uint32_t l;
        ss >> l;
        dest[0] = ((double)(l >> 16)) / 255.0;
        dest[1] = ((double)((l >> 8)&0xff)) / 255.0;
        dest[2] = ((double)(l&0xff)) / 255.0;
        return true;
    }

    /// Used in DragContext to indicate what the destination should do with the dropped data.
    enum DragAction {
        ActionDefault = GDK_ACTION_DEFAULT /**< Means nothing, and should not be used. */,
        ActionCopy = GDK_ACTION_COPY /**< 	Copy the data. */,
        ActionMove = GDK_ACTION_MOVE /**< Move the data, i.e. first copy it, then delete it from the source using the DELETE target of the X selection protocol. */,
        ActionLink = GDK_ACTION_LINK /**< Add a link to the data. Note that this is only useful if source and destination agree on what it means. */,
        ActionPrivate = GDK_ACTION_PRIVATE /**< Special action which tells the source that the destination will do something that the source doesn't understand. */,
        ActionAsk = GDK_ACTION_ASK  /**< Ask the user what to do with the data. */
    };

    /** A set of bit-flags to indicate the state of modifier keys and mouse buttons in various event types. Typical modifier keys are Shift, Control, Meta, Super, Hyper, Alt, Compose, Apple, CapsLock or ShiftLock.

Like the X Window System, GDK supports 8 modifier keys and 5 mouse buttons.
    */
    enum ModifierType {
        ShiftMask = GDK_SHIFT_MASK,
        LockMask = GDK_LOCK_MASK,
        ControlMask = GDK_CONTROL_MASK,
        Mod1Mask = GDK_MOD1_MASK,
        Mod2Mask = GDK_MOD2_MASK,
        Mod3Mask = GDK_MOD3_MASK,
        Mod4Mask = GDK_MOD4_MASK,
        Mod5Mask = GDK_MOD5_MASK,
        Button1Mask = GDK_BUTTON1_MASK,
        Button2Mask = GDK_BUTTON2_MASK,
        Button3Mask = GDK_BUTTON3_MASK,
        Button4Mask = GDK_BUTTON4_MASK,
        Button5Mask = GDK_BUTTON5_MASK,
        SuperMask = GDK_SUPER_MASK,
        HyperMask = GDK_HYPER_MASK,
        MetaMask = GDK_META_MASK,
        ReleaseMask = GDK_RELEASE_MASK,
        ModifierMask = GDK_MODIFIER_MASK
    };

/** A simple object to represent a Point */
    struct Point
    {
        /** Creates a new point with (0,0) coordinates */
        Point() : x(0), y(0) {}
        /** Creates a new point with specified coordinates */
        Point(int xp /**< X coord for the new point */,
              int yp /**< Y coord for the new point */) : x(xp), y(yp) {}
        /** Returns point X coordinate */
        int X() const { return x; }
        /** Returns point Y coordinate */
        int Y() const { return y; }
/// DOXYS_OFF
        int x, y;
/// DOXYS_ON
    };

/** A simple object to represent a Rectangle */
    struct Rect : GdkRectangle
    {
        /** Create a new rectangle with all 0'ed coordinates */
        Rect() { x = y = width = height = 0; }
        /** Create a new rectangle from a GdkRectangle (pointer) */
        Rect(const GdkRectangle *r) { memcpy(this, r, sizeof(GdkRectangle)); }
        /** Create a new rectangle from a GdkRectangle (reference) */
        Rect(const GdkRectangle &r) { memcpy(this, &r, sizeof(GdkRectangle)); }
        /** Create a new rectangle with given coords and size */
        Rect(int leftedge, int topedge, int w, int h) {
            x = leftedge; y = topedge;
            width = w; height = h;
        }
        /** Returns point X coordinate */
        int X() const { return x; }
        /** Returns point Y coordinate */
        int Y() const { return y; }
        /** Returns the width of the rectangle */
        int Width() const { return width; }
        /** Returns the height of the rectangle */
        int Height() const { return height; }

        /** Set the position of the rectangle */
        void Position(const Point &p) { x = p.x; y = p.y; }
        /** Set the size of the rectangle */
        void Size(const Point &p) { width = p.x; height = p.y; }
        /** Returns a point rapresenting (width,height) of the rectangle */
        Point Size() const { return Point(width,height); }
        /** Returns a point rapresenting (x,y) position of left corner of the rectangle */
        Point Position() const { return Point(x,y); }
    };

    /** A pair of floats to specify X and Y alignments.
Align values goes from 0 to 1, where 0 is left, and 1 is right (and 0.5 is center, what a suprise!).

- Align.first is the X alignment.
- Align.second is the Y alignment.
*/
    typedef std::pair<float,float> Align;

/** A simple object to represent a color */
    struct Color : public GdkColor
    {
/// DOXYS_OFF
        operator const GdkColor *() const { return dynamic_cast<const GdkColor *>(this); }
/// DOXYS_ON
        /** Creates a new color from a color definition.

The definition may be a string descripting the color (eg. "white") or
a standard rgb24 or RGB15 color notation (eg. "#ff0000" for red in RGB24,
"#0f0" for green in RGB15 notation).
         */
        Color(const char *desc) {
            gdk_color_parse(desc, this);
        }
        /** Create a new color from RGB values.

The values for the color components must be in the range 0-255.
*/
        Color(unsigned char r, unsigned char g, unsigned char b) {
            red = convert(r);
            green = convert(g);
            blue = convert(b);
        }
        /** Convert a RGB24 value to RGB48.

This static member can be used to convert a standard RGB24 color component (8 bits wide, 0-255)
to a RGB48 color component (16 bits wide, 0-65535).

RGB48 values are used internally by GTK+.
        */
        static unsigned short convert(int comp) {
            comp *= 257;

            return (comp > 0xffff) ? 0xffff : comp;
        }
    };

    class FontDesc
    {
        public:
            operator  PangoFontDescription *() const { return desc_; }

            FontDesc(const std::string &name) {
                if (!(desc_ = pango_font_description_from_string(name.c_str())))
                    throw std::runtime_error("Font unavailable " + name);
            }
            ~FontDesc() {
                pango_font_description_free(desc_);
            }

        private:
            PangoFontDescription *desc_;
    };

    /** A set of bit-flags to indicate which events a window is to receive.

Most of these masks map onto one or more of the EventType event types above.

PointerMotionHintMask is a special mask which is used to reduce the number of MotionNotify events received. Normally a MotionNotify event is received each time the mouse moves. However, if the application spends a lot of time processing the event (updating the display, for example), it can lag behind the position of the mouse. When using PointerMotionHintMask, fewer MotionNotify events will be sent, some of which are marked as a hint (the is_hint member is true). To receive more motion events after a motion hint event, the application needs to asks for more, by calling gdk_event_request_motions().
*/
    enum EventMask {
        ExposureMask = GDK_EXPOSURE_MASK /**<	receive expose events */,
        PointerMotionMask = GDK_POINTER_MOTION_MASK /**<	receive all pointer motion events */,
        PointerMotionHintMask = GDK_POINTER_MOTION_HINT_MASK /**<	see the explanation above */,
        ButtonMotionMask = GDK_BUTTON_MOTION_MASK /**<	receive pointer motion events while any button is pressed */,
        Button1MotionMask = GDK_BUTTON1_MOTION_MASK /**<	receive pointer motion events while 1 button is pressed */,
        Button2MotionMask = GDK_BUTTON2_MOTION_MASK /**<	receive pointer motion events while 2 button is pressed */,
        Button3MotionMask = GDK_BUTTON3_MOTION_MASK /**<	receive pointer motion events while 3 button is pressed */,
        ButtonPressMask = GDK_BUTTON_PRESS_MASK /**<	receive button press events */,
        ButtonReleaseMask = GDK_BUTTON_RELEASE_MASK /**<	receive button release events */,
        KeyPressMask = GDK_KEY_PRESS_MASK /**<	receive key press events */,
        KeyReleaseMask = GDK_KEY_RELEASE_MASK /**<	receive key release events */,
        EnterNotifyMask = GDK_ENTER_NOTIFY_MASK /**<	receive window enter events */,
        LeaveNotifyMask = GDK_LEAVE_NOTIFY_MASK /**<	receive window leave events */,
        FocusChangeMask = GDK_FOCUS_CHANGE_MASK /**<	receive focus change events */,
        StructureMask = GDK_STRUCTURE_MASK /**<	receive events about window configuration change */,
        PropertyChangeMask = GDK_PROPERTY_CHANGE_MASK /**<	receive property change events */,
        VisibilityNotifyMask = GDK_VISIBILITY_NOTIFY_MASK /**<	receive visibility change events */,
        ProximityInMask = GDK_PROXIMITY_IN_MASK /**<	receive proximity in events */,
        ProximityOutMask = GDK_PROXIMITY_OUT_MASK /**<	receive proximity out events */,
        SubstructureMask = GDK_SUBSTRUCTURE_MASK /**<	receive events about window configuration changes of child windows */,
        ScrollMask = GDK_SCROLL_MASK /**<	receive scroll events */,
        AllEventsMask = GDK_ALL_EVENTS_MASK /**<	the combination of all the above event masks. */
    };

    ///  This enumeration describes the different interpolation modes that can be used with the scaling functions. InterpNearest is the fastest scaling method, but has horrible quality when scaling down. InterpBilinear is the best choice if you aren't sure what to choose, it has a good speed/quality balance.
    enum InterpType {
	    InterpNearest = GDK_INTERP_NEAREST /**< 	Nearest neighbor sampling; this is the fastest and lowest quality mode. Quality is normally unacceptable when scaling down, but may be OK when scaling up. */,
    	InterpTiles = GDK_INTERP_TILES /**< This is an accurate simulation of the PostScript image operator without any interpolation enabled. Each pixel is rendered as a tiny parallelogram of solid color, the edges of which are implemented with antialiasing. It resembles nearest neighbor for enlargement, and bilinear for reduction. */,
	    InterpBilinear = GDK_INTERP_BILINEAR /**< Best quality/speed balance; use this mode by default. Bilinear interpolation. For enlargement, it is equivalent to point-sampling the ideal bilinear-interpolated image. For reduction, it is equivalent to laying down small tiles and integrating over the coverage area.  */,
    	InterpHyper = GDK_INTERP_HYPER /**< This is the slowest and highest quality reconstruction function. It is derived from the hyperbolic filters in Wolberg's "Digital Image Warping", and is formally defined as the hyperbolic-filter sampling the ideal hyperbolic-filter interpolated image (the filter is designed to be idempotent for 1:1 pixel mapping). */
    };

    struct EventExpose : public GdkEventExpose {
        gtk::Rect  Rect() const { return gtk::Rect(area); }
    };
    struct EventScroll : public GdkEventScroll {
        bool Up() const { return direction == GDK_SCROLL_UP; }
        bool Down() const { return direction == GDK_SCROLL_DOWN; }
        bool Left() const { return direction == GDK_SCROLL_LEFT; }
        bool Right() const { return direction == GDK_SCROLL_RIGHT; }
    };

    struct EventButton : public GdkEventButton {
        bool IsDoubleClick() const {
            return (type == GDK_2BUTTON_PRESS ||
                    type == GDK_3BUTTON_PRESS);
        }
        bool IsLeft() const { return button == 1; }
        bool IsMiddle() const { return button == 2; }
        bool IsRight() const {
#ifdef __APPLE__
            if ((state & GDK_CONTROL_MASK) && button == 1)
                return true;
#endif
            return button == 3;
        }
        int Button() const { return button; }
        bool IsPress()   const { return type == GDK_BUTTON_PRESS; }
        bool IsRelease() const { return type == GDK_BUTTON_RELEASE; }
        Point Coords()   const { return Point((int)x,(int)y); }
    };
    struct EventMotion : public GdkEventMotion {
        Point Coords() const { return Point((int)x,(int)y); }
    };
    struct EventConfigure : public GdkEventConfigure {
        Point Coords() const { return Point(x, y); }
        Point Size() const { return Point(width, height); }
    };
    struct EventKey : public GdkEventKey {
        unsigned int KeyVal() const { return keyval; }
        unsigned int Modifiers() const { return state; }
        bool Press() { return type == GDK_KEY_PRESS; }
        bool Release() { return type == GDK_KEY_RELEASE; }
    };

    /// Generic event class, all event classes derived from this.
    struct Event : public GdkEventAny
    {
        /** checks if a generic event is a EventConfigure.

This cast method checks if a generic event is a EventConfigure and returns
a pointer to it or NULL otherwise.
*/
        operator EventConfigure *() const {
            if (type == GDK_CONFIGURE)
                return (EventConfigure *)this;
            else
                return NULL;
        }
        /** checks if a generic event is a EventExpose.

This cast method checks if a generic event is a EventExpose and returns
a pointer to it or NULL otherwise.
*/
        operator EventExpose *() const {
            if (type == GDK_EXPOSE)
                return (EventExpose *)this;
            else
                return NULL;
        }
        /** checks if a generic event is a EventScroll.

This cast method checks if a generic event is a EventScroll and returns
a pointer to it or NULL otherwise.
*/
        operator EventScroll *() const {
            if (type == GDK_SCROLL)
                return (EventScroll *)this;
            else
                return NULL;
        }
        /** checks if a generic event is a EventButton.

This cast method checks if a generic event is a EventButton and returns
a pointer to it or NULL otherwise.
*/
        operator EventButton *() const {
            if (type == GDK_BUTTON_PRESS ||
                type == GDK_2BUTTON_PRESS ||
                type == GDK_3BUTTON_PRESS ||
                type == GDK_BUTTON_RELEASE)
                return (EventButton *)this;
            else
                return NULL;
        }
/** checks if a generic event is a EventKey.

This cast method checks if a generic event is a EventKey and returns
a pointer to it or NULL otherwise.
*/
        operator EventKey *() const {
            if (type == GDK_KEY_PRESS ||
                type == GDK_KEY_RELEASE)
                return (EventKey *)this;
            else
                return NULL;
        }
/** checks if a generic event is a EventMotion.

This cast method checks if a generic event is a EventMotion and returns
a pointer to it or NULL otherwise.
*/
        operator EventMotion *() const {
            if (type == GDK_MOTION_NOTIFY)
                return (EventMotion *)this;
            else
                return NULL;
        }
/// Returns the GdkWindow that has generated this event
        GdkWindow *Window() const { return window; }
/// Returns the event type.
        unsigned int Type() const { return type; }
    };

    typedef GdkDrawable Drawable;

/** Class that describes an image.

The Pixbuf object contains information that describes an image in memory.

Image data in a pixbuf is stored in memory in uncompressed, packed format. Rows in the image are stored top to bottom, and in each row pixels are stored from left to right. There may be padding at the end of a row. The "rowstride" value of a pixbuf, as returned by Pixbuf::Rowstride(), indicates the number of bytes between rows.
*/

    class Pixbuf : public Object
    {
        public:
/// DOXYS_OFF
            operator  GdkPixbuf *() const { return GDK_PIXBUF(Obj()); }
            Pixbuf(GObject *obj) { Init(obj); }
/// DOXYS_ON
            /** Creates a new pixbuf from a drawable. */
            Pixbuf(const Drawable &drawable) {
                int w, h;
                gdk_drawable_get_size(const_cast<Drawable*>(&drawable), &w, &h);
                Init(gdk_pixbuf_get_from_drawable(NULL, const_cast<Drawable*>(&drawable), gdk_colormap_get_system(), 0, 0, 0, 0, w, h));
                Internal(true);
            }
            /** Creates a new pixbuf copying another pixbuf. */
            Pixbuf(const Pixbuf &rhs) {
                Init(gdk_pixbuf_copy(rhs));
                Internal(true);
            }
            /** Creates a new pixbuf by loading an image from a file.

The file format is detected automatically. If the file is not found or it is of an unknown format
an exception (std::runtime_error) is thrown. So it's a good policy to wrap your Pixbuf creation
in a try / catch block
*/
            Pixbuf(const std::string &name) {
                if (GdkPixbuf *b = gdk_pixbuf_new_from_file(name.c_str(), NULL))
                    Init(b);
                else
                    throw std::runtime_error("Unable to load file " + name);

                Internal(true);
            }
/** Creates a new pixbuf by loading an image from data already available in memory.

This call can thrown an exception (std::runtime_error) if the memory block contains
an unknown image format.

\note If you use this method in your code you should link your code with gio-2.0, so this method is not available if GTK version is below 2.16.
*/
#if GTK_MINOR_VERSION > 15
            Pixbuf(int len, const char *data) {
                GInputStream *s = g_memory_input_stream_new_from_data (data, len, NULL);
                if (!s)
                    throw std::runtime_error("Unable to allocate input stream");

                GdkPixbuf *b = gdk_pixbuf_new_from_stream(s, NULL, NULL);

                g_object_unref(s);

                if (!b)
                    throw std::runtime_error("Unable to allocate input stream");

                Init(b);

                Internal(true);
            }
#endif
/// Create a new empty pixbuf.
            Pixbuf(int width /**< The width of the image in pixels */,
                   int height /**< The height of the image in pixels */,
                   int depth = 32 /**< the depth of the pixmap in bits */,
                   int alpha = true /**< If the pixmap has or not an alpha channel, defaults to true*/) {
                Init(gdk_pixbuf_new(GDK_COLORSPACE_RGB, alpha, depth, width, height));
                Internal(true);
            }
            GdkPixmap *Pixmap() const {
                GdkPixmap *p = NULL;
                gdk_pixbuf_render_pixmap_and_mask(*this, &p, NULL, 0);
                return p;
            }
            /// Creates an exact copy of the Pixmap and returns a reference to it.
            Pixbuf &Copy() const { Pixbuf *b = new Pixbuf((GObject *)gdk_pixbuf_copy(*this)); b->Internal(true); return *b; }

            /// Creates a new pixmap from a Rect of the actual object and returns a reference to it.
            Pixbuf &Sub(const Rect &area) {
                Pixbuf *b = new Pixbuf((GObject *)
                        gdk_pixbuf_new_subpixbuf(*this, area.x, area.y,
                                                 area.width, area.height));
                b->Internal(true);
                return *b;
            }

            // Save the pixmap to a buffer in the specified format
            bool Save(std::string &dest /**< A string that will contain the BINARY image */,
                      const std::string &format = "png" /**< Destination image format, defaults to png, available formats: png, jpeg, bmp, ico */) {
                gboolean rc;
                gchar *buffer;
                gsize len;
                if (format == "png")
                    rc = gdk_pixbuf_save_to_buffer(*this, &buffer, &len, "png", NULL, "compression", "9", NULL);
                else if (format == "jpeg")
                    rc = gdk_pixbuf_save_to_buffer(*this, &buffer, &len, "jpeg", NULL, "quality", "50", NULL);
                else
                    rc = gdk_pixbuf_save_to_buffer(*this, &buffer, &len, format.c_str(), NULL, NULL);

                if (!rc)
                    return false;

                dest.assign(buffer, len);
                g_free(buffer);
                return true;
            }
            /// Scale the pixmap resizing its size to the specified parameters.
            void Scale(int width, int height, InterpType interpolation = InterpNearest)
            {
                GdkPixbuf *b = gdk_pixbuf_scale_simple(*this, width, height, (GdkInterpType)interpolation);
                if (type_ == InternalObj)
                    g_object_unref(obj_);
                Internal(true);
                obj_ = (GObject *)b;
            }

            /// Scale the pixmap to a destination Pixmap object.
            void Scale(Pixbuf &dest /**< the GdkPixbuf into which to render the results  */,
                       Rect &destarea /**< A rectangle specifying the destination region to render*/,
                       double scale_x /**< the scale factor in the X direction  */,
                       double scale_y /**< the scale factor in the Y direction */,
                       InterpType interpolation = InterpNearest /**< the interpolation type for the transformation. */,
                       double offset_x = 0.0  /**< the offset in the X direction, currently rounded to an integer */,
                       double offset_y = 0.0  /**< 	 the offset in the Y direction, currently rounded to an integer */) {
                gdk_pixbuf_scale(*this, dest, destarea.x, destarea.y,
                                              destarea.width, destarea.height,
                                 offset_x, offset_y, scale_x, scale_y,
                                 (GdkInterpType)interpolation);
            }

            /// Queries the width of a pixbuf.
            int width() const { return gdk_pixbuf_get_width(*this); }
            /// Queries the width of a pixbuf.
            int Width() const { return width(); }
            /// Queries the height of a pixbuf.
            int height() const { return gdk_pixbuf_get_height(*this); }
            /// Queries the height of a pixbuf.
            int Height() const { return height(); }
            /// Returns a point containing width and height of the pixmap
            Point Size() const { return Point(width(), height()); }

            /// Copy an area of the Pixmap to a destination object.
            void CopyArea(const Rect &area /**< A rectangle rapresenting the source Pixmap area to copy */,
                          Pixbuf &dest /**< The destination Pixmap for the operation */,
                          const Point &position /**< The position where to copy the source rectangle in the destination Pixmap */) {
                gdk_pixbuf_copy_area(*this, area.x, area.y,
                                            area.width, area.height,
                                      dest, position.x, position.y);
            }
            /// Fills a pixmap with a given color.
            void Fill(guint32 color = 0) { gdk_pixbuf_fill(*this, color); }

            /// Flips a pixmap.
            void Flip(bool horizontal = true) { gdk_pixbuf_flip(*this, horizontal); }

/** Saves pixbuf to a file in format type.

By default, "jpeg", "png", "ico" and "bmp" are possible file formats to save in, but more formats may be installed. If "type" is not specified the image is saved as PNG.

\retval true if the save was correctly written, false otherwise.
*/
            bool Save(const std::string &filename /**< Destination path for the file */,
                      const std::string &type = "png" /**< Image format, defaults to PNG. */) {
                if( !gdk_pixbuf_save(*this, filename.c_str(), type.c_str(), NULL, NULL)) {
                    return false;
                }
                else
                    return true;
            }

            /// Queries whether a pixbuf has an alpha channel (opacity information).
            bool HasAlpha() const { return gdk_pixbuf_get_has_alpha(*this); }
            /// Queries the rowstride of a pixbuf, which is the number of bytes between the start of a row and the start of the next row.
            int Rowstride() const { return gdk_pixbuf_get_rowstride(*this); }
            /// Queries a pointer to the pixel data of a pixbuf.
            unsigned char *Pixels() { return gdk_pixbuf_get_pixels(*this); }
            /// Queries the number of channels of a pixbuf.
            int NChannels() const { return gdk_pixbuf_get_n_channels(*this); }
    };
}
inline std::ostream &operator<<(std::ostream &dest, const gtk::Point &point) {
    dest << '(' << point.x << ',' << point.y << ')';
    return dest;
}
inline std::ostream &operator<<(std::ostream &dest, const gtk::Rect &rect) {
    dest << '(' << rect.x << ',' << rect.y << ' ' << rect.width << 'x' << rect.height << ')';
    return dest;
}
#endif
