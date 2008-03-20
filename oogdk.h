#ifndef OOGDK_H
#define OOGDK_H

#include "ooobj.h"

namespace gtk
{
    struct Point
    {
        Point() : x(0), y(0) {}
        Point(int xp, int yp) : x(xp), y(yp) {}
        int X() const { return x; }
        int Y() const { return y; }
        int x, y;
    };

    struct Rect
    {
        Rect(const GdkRectangle &r) : x(r.x), y(r.y), w(r.width), h(r.height) {}
        Rect(int leftedge, int topedge, int width, int height) :
            x(leftedge), y(topedge), w(width), h(height) {}
        Point Size() const { return Point(w,h); }
        Point Position() const { return Point(x,y); }
        Rect &operator=(const GdkRectangle &r) { x = r.x; y = r.y; w = r.width; h = r.height; }
        int x,y,w,h;
    };

    typedef std::pair<float,float> Align;

    struct Color : public GdkColor
    {
        operator const GdkColor *() const { return dynamic_cast<const GdkColor *>(this); }

        Color(unsigned char r, unsigned char g, unsigned char b) {
            red = convert(r);
            green = convert(g);
            blue = convert(b);
        }
        static unsigned short convert(int comp) {
            comp *= 257;

            return (comp > 0xffff) ? 0xffff : comp;
        }
    };

    class FontDesc 
    {
        public:
            operator  PangoFontDescription *() const { return desc_; }

            FontDesc(std::string &name) {
                if (!(desc_ = pango_font_description_from_string(name.c_str())))
                    throw std::runtime_error("Font unavailable " + name);
            }
            ~FontDesc() {
                pango_font_description_free(desc_);
            }

        private:
            PangoFontDescription *desc_;
    };

    enum InterpType {
	    InterpNearest = GDK_INTERP_NEAREST,
    	InterpTiles = GDK_INTERP_TILES,
	    InterpBilinear = GDK_INTERP_BILINEAR,
    	InterpHyper = GDK_INTERP_HYPER
    };

//    InterpType operator|(InterpType &a, InterpType &b) { return (InterpType)((int)a|(int)b); }

    class Pixbuf : public Object
    {
        public:
            operator  GdkPixbuf *() const { return GDK_PIXBUF(Obj()); }

            Pixbuf(GObject *obj) { Init(obj); }
            Pixbuf(const std::string &name) {
                if (GdkPixbuf *b = gdk_pixbuf_new_from_file(name.c_str(), NULL))
                    Init(b);
                else
                    throw std::runtime_error("Unable to load file " + name);

                Internal(true);
            }
            Pixbuf(int width, int height, int depth = 32, int alpha = true) {
                Init(gdk_pixbuf_new(GDK_COLORSPACE_RGB, alpha, depth, width, height));
                Internal(true);
            }

            Pixbuf &Copy() const { Pixbuf *b = new Pixbuf((GObject *)gdk_pixbuf_copy(*this)); b->Internal(true); return *b; }

            Pixbuf &Sub(const Rect &area) {
                Pixbuf *b = new Pixbuf((GObject *)
                        gdk_pixbuf_new_subpixbuf(*this, area.x, area.y, area.w, area.h)); 
                b->Internal(true); 
                return *b;
            }

            void Scale(int width, int height, OneOf<GdkInterpType, InterpType> interpolation = InterpNearest)
            {
                gdk_pixbuf_scale_simple(*this, width, height, interpolation);
            }

            void Scale(Pixbuf &dest, Rect &destarea, double scale_x, double scale_y, 
                       OneOf<GdkInterpType, InterpType> interpolation = InterpNearest,
                       double offset_x = 0.0, double offset_y = 0.0) {
                gdk_pixbuf_scale(*this, dest, destarea.x, destarea.y, destarea.w, destarea.h,
                                 offset_x, offset_y, scale_x, scale_y, interpolation);
            }

            int width() const { return gdk_pixbuf_get_width(*this); }
            int Width() const { return width(); }
            int height() const { return gdk_pixbuf_get_height(*this); }
            int Height() const { return height(); }
            Point Size() const { return Point(width(), height()); }

            void CopyArea(const Rect &area, Pixbuf &dest, const Point &position) {
                gdk_pixbuf_copy_area(*this, area.x, area.y, area.w, area.h,
                                      dest, position.x, position.y);
            }
            void Fill(guint32 color = 0) { gdk_pixbuf_fill(*this, color); }
            void Flip(bool horizontal = true) { gdk_pixbuf_flip(*this, horizontal); }
    };
}
#endif
