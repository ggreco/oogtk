#ifndef OOCAIRO_H
#define OOCAIRO_H

#include "oogtk.h"

namespace cairo
{

class Surface
{
private:
    cairo_surface_t *s_;
public:
    /// Format is used to identify the memory format of image data.
/// DOXYS_OFF
    operator  cairo_surface_t *() const { return s_; }
/// DOXYS_ON

    enum Format {        
            RGB32 = CAIRO_FORMAT_ARGB32 /**< each pixel is a 32-bit quantity, with alpha in the upper 8 bits, then red, then green, then blue. The 32-bit quantities are stored native-endian. Pre-multiplied alpha is used. (That is, 50% transparent red is 0x80800000, not 0x80ff0000.) */,
            RGB24 = CAIRO_FORMAT_RGB24 /**< each pixel is a 32-bit quantity, with the upper 8 bits unused. Red, Green, and Blue are stored in the remaining 24 bits in that order.*/,
            A8 = CAIRO_FORMAT_A8 /**< each pixel is a 8-bit quantity holding an alpha value.*/,
            A1 = CAIRO_FORMAT_A1 /**< each pixel is a 1-bit quantity holding an alpha value. Pixels are packed together into 32-bit quantities. The ordering of the bits matches the endianess of the platform. On a big-endian machine, the first pixel is in the uppermost bit, on a little-endian machine the first pixel is in the least-significant bit. */,
            RGB16 = CAIRO_FORMAT_RGB16_565 /**< each pixel is a 16-bit quantity with red in the upper 5 bits, then green in the middle 6 bits, and blue in the lower 5 bits. */
    };

    /// Creates an image surface of the specified format and dimensions. Initially the surface contents are all 0. (Specifically, within each pixel, each color or alpha channel belonging to format will be 0. The contents of bits within a pixel, but not belonging to the given format are undefined).
    Surface(Format fmt, int width, int height) {
        s_ = ::cairo_image_surface_create((cairo_format_t)fmt, width, height);
    }
    /// Create a surface with the same format of an existing surface.
    Surface(const Surface &s, int width, int height) {
        s_ = ::cairo_surface_create_similar(s.s_, CAIRO_CONTENT_COLOR_ALPHA, width, height);
    }

    /// Scales a surface to a new width, height.
    void scale(int new_width, int new_height);

    /// Creates a new surface from a PNG image.
    Surface(const std::string &name) : s_(NULL) { from_png(name);  }
    /** Creates a new empty surface.

The surface cannot be used until you insert something in it, for instance through Surface::from_png().
*/
    Surface() : s_(NULL) {}
    ~Surface() {
        if (s_)
            ::cairo_surface_destroy(s_);
    }
    /** Set the surface data and size to the png image pointed by name.

If the surface already contains something the previous contents are freed.
*/
    bool from_png(const std::string &name) {
        if (s_)
            ::cairo_surface_destroy(s_);

        s_ = ::cairo_image_surface_create_from_png(name.c_str());

        return s_ != NULL;
    }
    /// Returns the width of the surface in pixels
    int width() const { return ::cairo_image_surface_get_width(s_); }
    /// Returns the height of the surface in pixels
    int height() const { return ::cairo_image_surface_get_height(s_); }
};

class Context
{
/// DOXYS_OFF

private:
    cairo_t *c_;

public:
    operator  cairo_t *() const { return c_; }
/// DOXYS_ON

    typedef cairo_text_extents_t TextExtents;

    enum FontSlant {
        FontSlantNormal  = CAIRO_FONT_SLANT_NORMAL,
        FontSlantItalic  = CAIRO_FONT_SLANT_ITALIC,
        FontSlantOblique = CAIRO_FONT_SLANT_OBLIQUE
    };
    enum FontWeight {
        FontWeightNormal = CAIRO_FONT_WEIGHT_NORMAL,
        FontWeightBold   = CAIRO_FONT_WEIGHT_BOLD
    };

    /// Creates a new cairo context with a Surface as destination
    Context(const Surface &s) { c_ = cairo_create(s); }
    /// Creates a new cairo context width a widget as destination
    Context(const gtk::Widget &w) { c_ = gdk_cairo_create(w);  }
    ~Context() { cairo_destroy(c_); }

    // context handling
    void save() { cairo_save(c_); }
    void restore() { cairo_restore(c_); }

    // fonts
    void font_face(const std::string &name, FontSlant s, FontWeight w) { 
        cairo_select_font_face(c_, name.c_str(), (cairo_font_slant_t)s, (cairo_font_weight_t)w); 
    }
    TextExtents text_extents(const std::string &txt) { 
        TextExtents te;
        cairo_text_extents(c_, txt.c_str(), &te);
        return te;
    }
    void show_text(const std::string &txt) { cairo_show_text(c_, txt.c_str()); }
    void font_size(double size) { cairo_set_font_size(c_, size); }

    // transforms
    void translate(const gtk::Point &p) { cairo_translate(c_, p.x, p.y); }
    void translate(double x, double y) { cairo_translate(c_, x, y); }
    void scale(const gtk::Point &p) { cairo_scale(c_, p.x, p.y); }
    void scale(double w, double h) { cairo_scale(c_, w, h); }
    void rotate(double angle /**< angle in radians */) { cairo_rotate(c_, angle); }

    // setup 
    void line_width(double size) { cairo_set_line_width(c_, size); }
    void source_rgb(const char *src) {
        if (*src == '#') {
            int r, g, b;
            if (::sscanf(src + 1, "%02x%02x%02x", &r, &g, &b) == 3)
                cairo_set_source_rgb(c_, (double)r / 255.0f, (double)g / 255.0f, (double)b / 255.0f);
        }
    }
    void source_rgb(double r, double g, double b) { cairo_set_source_rgb(c_, r, g, b); }
    void source_rgba(double r, double g, double b, double a) { cairo_set_source_rgba(c_, r, g, b, a); }
    void move_to(double x, double y) { cairo_move_to(c_, x, y); }
    void move_to(const gtk::Point &p) { cairo_move_to(c_, p.x, p.y); }
    void dash(const std::vector<double> &dashes) { cairo_set_dash(c_, &dashes[0], dashes.size(), 0); }
    void dash(const double *dashes, int size) { cairo_set_dash(c_, dashes, size, 0); }
    void dash(double interval) { cairo_set_dash(c_, &interval, 1, 0); }
    void dash() { cairo_set_dash(c_, NULL, 0, 0); }


    // operations
    void line_to(double x, double y) { cairo_line_to(c_, x, y); }
    void line_to(const gtk::Point &p) { cairo_line_to(c_, p.x, p.y); }
    void rectangle(const gtk::Rect &r) { cairo_rectangle(c_, r.x, r.y, r.width, r.height); }
    void rectangle(double x, double y, double w, double h) { cairo_rectangle(c_, x, y, w, h); }
    void arc(double xc, double yc, double r, double a1, double a2) { cairo_arc(c_, xc, yc, r, a1, a2); }

    // surface blitting
    void source(Surface &s, double x = 0.0, double y = 0.0) { cairo_set_source_surface(c_, s, x, y); }

    // strokes
    void fill() { cairo_fill(c_); }
    void fill_preserve() { cairo_fill_preserve(c_); }
    void stroke() { cairo_stroke(c_); }
    void clip() { cairo_clip(c_); }
    void paint() { cairo_paint(c_); }
};

inline void Surface::scale(int new_width, int new_height) {
    Surface new_surface(*this, new_width, new_height);
    Context cr(new_surface);

    cr.scale((double)new_width / (double)width(), (double)new_height / (double)height());
    cr.source(*this);

    ::cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);

    cr.paint();

    ::cairo_surface_destroy(s_);
    s_ = new_surface.s_;
    new_surface.s_ = NULL;
}

}
#endif
