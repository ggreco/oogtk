#pragma once

#include <gtk/gtk.h>
#include <stdexcept>
#include <vector>
#include <string>
#include <string.h>
#include "inline_containers.h"

namespace gtk
{
    inline std::string escape(const std::string &src) {
        std::string res;
        if (gchar *s = g_uri_escape_string(src.c_str(), NULL, TRUE)) {
            res = s;
            g_free(s);
        }
        return res;
    }
    inline std::string escape_markup(const std::string &src) {
        std::string res;
        if (gchar *s = g_markup_escape_text(&src[0], src.length())) {
            res = s;
            g_free(s);
        }
        return res;
    }
    inline std::string unescape(const std::string &src) {
        std::string res;
        if (gchar *s = g_uri_unescape_string(src.c_str(), NULL)) {
            res = s;
            g_free(s);
        }
        return res;
    }
    /** Convert GTK standard enums to oogtk ones and back.
This is an helper class to allow oogtk functions to accept both C style GTK+ enums and
oogtk C++ enums.
    */
    template <typename A, typename B>
    class OneOf
    {
        public:
            OneOf(const A &aa) : a(aa) {}
            OneOf(const B &bb) : a(static_cast<A>(bb)) {}
            operator A() const { return a; }
            operator B() const { return static_cast<B>(a); }
        private:
            A a;
    };

    /// Empty class used for null constructor of intermediate, instantiable, classes.
    class DerivedType
    {
    };

/// DOXYS_OFF
    struct SelectionData;
    class Widget;
    struct FakeType {
    };


    struct AbstractDragCbk
    {
        virtual ~AbstractDragCbk() {}
        virtual bool notify(GtkWidget *w, SelectionData *e) const = 0;

        static void real_callback_received(GtkWidget *w, GdkDragContext *c, gint x, gint y, SelectionData *d, guint i, guint t, AbstractDragCbk *b) {
            if (b) b->notify(w, d);
            gtk_drag_finish(c, TRUE, FALSE, t);
        }
        static void real_callback_get(GtkWidget *w, void *u1, SelectionData *d, guint i, guint t, AbstractDragCbk *b) {
            if (b) b->notify(w, d);
        }
    };
    template <typename T, typename J = FakeType>
    class CbkDrag : public AbstractDragCbk
    {
        public:
            CbkDrag( T* obj, void (T::*fnc)(Widget &, SelectionData &))
                : myObj(obj), mywFnc0(fnc), mywFnc1(NULL) {}
            CbkDrag( T* obj, void (T::*fnc)(Widget &, SelectionData &, J), J a1)
                : myObj(obj), mywFnc1(fnc), ma1(a1) {}

        private:
            T*  myObj;
            void (T::*mywFnc0)(Widget &, SelectionData &);
            void (T::*mywFnc1)(Widget &, SelectionData &, J);
            J ma1;

            virtual bool notify(GtkWidget *w, SelectionData *e) const;
    };

    struct AbstractCbk
    {
        virtual ~AbstractCbk() {}

        virtual bool notify(GtkWidget *w = NULL, GdkEvent *e = NULL) const = 0;

        static gint real_callback_0(AbstractCbk *ce) {
            return ce->notify();
        }
        static gint real_callback_1(GtkWidget *w, AbstractCbk *ce) {
            return ce->notify(w);
        }
        static gint real_callback_2(GtkWidget *w, GdkEvent *e, AbstractCbk *ce) {
            return ce->notify(w, e);
        }
        static gint real_callback_3(GtkWidget *w, GdkEvent *e, void *u1, AbstractCbk *ce) {
            return ce->notify(w, e);
        }
        static gint real_callback_4(GtkWidget *w, GdkEvent *e, void *u1, void *u2, AbstractCbk *ce) {
            return ce->notify(w, e);
        }
        static gint real_callback_5(GtkWidget *w, GdkDragContext *c, void *u1, void *u2, void *u3, AbstractCbk *ce) {
            return ce->notify(w, (GdkEvent*)c);
        }
        static gint real_callback_7(GtkWidget *w, GdkDragContext *c, void *u1, void *u2, void *u3, void *u4, void *u5, AbstractCbk *ce) {
            return ce->notify(w, (GdkEvent*)c);
        }
    };

    struct Event;

    struct FakeTypeBase {
    };
    typedef int SockFd;

    template <typename T, typename R, typename A = FakeTypeBase, typename J = FakeType> struct ReturnType {
       bool notify(R (T::*fnc)(void), T *obj, bool rc) const {
            return (obj->*fnc)();
       }
       bool notify(J arg, R (T::*fnc)(J), T *obj, bool rc) const {
            return (obj->*fnc)(arg);
       }
       bool notify(A arg, R (T::*fnc)(A), T *obj, bool rc) const {
            return (obj->*fnc)(arg);
       }
       bool notify(A w, J arg, R (T::*fnc)(A, J), T *obj, bool rc) const {
            return (obj->*fnc)(w,arg);
       }
    };

    template <typename T, typename A, typename J> struct ReturnType<T, void, A, J> {
      bool notify(void (T::*fnc)(void), T *obj, bool rc) const {
            (obj->*fnc)();
            return rc;
       }
       bool notify(J arg, void (T::*fnc)(J), T *obj, bool rc) const {
            (obj->*fnc)(arg);
            return rc;
       }
       bool notify(A arg, void (T::*fnc)(A), T *obj, bool rc) const {
            (obj->*fnc)(arg);
            return rc;
       }
       bool notify(A w, J arg, void (T::*fnc)(A, J), T *obj, bool rc) const {
            (obj->*fnc)(w, arg);
            return rc;
       }
    };

    // I've had to add those two partial specialization to cover the cases where
    // the args of the method are of the same type.
    template <typename T, typename R, typename A> struct ReturnType<T, R, A, A> {
       bool notify(R (T::*fnc)(void), T *obj, bool rc) const {
            return (obj->*fnc)();
       }
       bool notify(A arg, R (T::*fnc)(A), T *obj, bool rc) const {
            return (obj->*fnc)(arg);
       }
       bool notify(A w, A arg, R (T::*fnc)(A, A), T *obj, bool rc) const {
            return (obj->*fnc)(w,arg);
       }
    };
    template <typename T, typename A> struct ReturnType<T, void, A, A> {
      bool notify(void (T::*fnc)(void), T *obj, bool rc) const {
            (obj->*fnc)();
            return rc;
       }
       bool notify(A arg, void (T::*fnc)(A), T *obj, bool rc) const {
            (obj->*fnc)(arg);
            return rc;
       }
       bool notify(A w, A arg, void (T::*fnc)(A, A), T *obj, bool rc) const {
            (obj->*fnc)(w, arg);
            return rc;
       }
    };


    template <typename T, typename R, typename J = FakeType>
        class CbkEvent : public AbstractCbk
    {
            enum CbkType {NoParam, HasWidget, HasEvent, HasSocket};
        public:
            // two constructs for widgetless returning callbacks
            CbkEvent( T* obj, R (T::*fnc)(void), bool rc = true)
                : myObj(obj), myFnc0(fnc), rccode(rc), type(NoParam) {}
            CbkEvent( T* obj, R (T::*fnc)(J), J a1, bool rc = true)
                : myObj(obj), myFnc0(NULL), myFnc1(fnc), ma1(a1), rccode(rc), type(NoParam) {}
            // two constructor with originating widget support
            CbkEvent( T* obj, R (T::*fnc)(Widget &), bool rc = true)
                : myObj(obj), mywFnc0(fnc), rccode(rc), type(HasWidget) {}
            CbkEvent( T* obj, R (T::*fnc)(Widget &, J), J a1, bool rc = true)
                : myObj(obj), mywFnc0(NULL), mywFnc1(fnc), ma1(a1), rccode(rc), type(HasWidget) {}
            CbkEvent( T* obj, R (T::*fnc)(Event &), bool rc = true)
                : myObj(obj), myeFnc0(fnc), rccode(rc), type(HasEvent) {}
            CbkEvent( T* obj, R (T::*fnc)(Event &, J), J a1, bool rc = true)
                : myObj(obj), myeFnc0(NULL), myeFnc1(fnc), ma1(a1), rccode(rc), type(HasEvent) {}
            CbkEvent( T* obj, R (T::*fnc)(SockFd), bool rc = true)
                : myObj(obj), mysFnc0(fnc), rccode(rc), type(HasSocket) {}
            CbkEvent( T* obj, R (T::*fnc)(SockFd, J), J a1, bool rc = true)
                : myObj(obj), mysFnc0(NULL), mysFnc1(fnc), ma1(a1), rccode(rc), type(HasSocket) {}


        private:
            T*  myObj ;
            R (T::*myFnc0)();
            R (T::*myFnc1)(J);
            R (T::*mywFnc0)(Widget &);
            R (T::*mywFnc1)(Widget &, J);
            R (T::*myeFnc0)(Event &);
            R (T::*myeFnc1)(Event &, J);
            R (T::*mysFnc0)(int);
            R (T::*mysFnc1)(int, J);
            J ma1;
            bool rccode;
            CbkType type;

            virtual bool notify(GtkWidget *w = NULL, GdkEvent *e = NULL) const;
    };

    class Object;
/// DOXYS_ON

    /// Base class for Object property handling.
    class PropertyBase
    {
        public:
            PropertyBase(const std::string &key) : keyword_(key) {}
            virtual void Set(Object *) const = 0;
            virtual ~PropertyBase() {}
        protected:
            std::string keyword_;
    };

    typedef std::vector<PropertyBase *> PropertyList;
    typedef PropertyList::const_iterator PropertyIt;

    /// Base class for every type of GTK object.
    class Object
    {
        private:
        public:
            enum ObjectType { InternalObj, ExternalObj, ReferenceObj};
            typedef std::list<AbstractCbk *> CbkList;

            Object() : obj_(NULL), type_(ExternalObj), id_(-1) {}
            /** Get the internal GObject pointer from any gtk::Object.
             Constant variant.
             \return a pointer to the GObject associated with this Object.
             */
            const GObject *Obj() const { return obj_; }
            /** Get the internal GObject pointer from any gtk::Object.
             Not constant variant.
             \return a pointer to the GObject associated with this Object.
             */
            GObject *Obj() { return obj_; }
            void Init(void *obj);
            Object &Ref() { g_object_ref(obj_); return *this; }
            Object(const Object &o) : type_(ReferenceObj) { obj_ = o.obj_; g_object_ref(obj_); add_destroy_cbk(); }
            virtual ~Object() {

                if (obj_) {
#ifdef OOGTK_DEBUG
                std::cerr << "Destructor for " << (void *)obj_ << " type:"
                          << g_type_name(G_OBJECT_TYPE(obj_))
                          << " references: " << obj_->ref_count << std::endl;
#endif

                    if (id_ >= 0 && g_signal_handler_is_connected (obj_, id_))
                        g_signal_handler_disconnect (obj_, id_);

                    GObject *obj = obj_;

                    if (type_ != ReferenceObj)
                        Dispose(); // dispose sets obj_ to null

                    if (type_ == InternalObj) {
                        g_object_unref(obj);
                    }
                }
            }
            static Object *Find(GObject *o);
            void Internal(bool val) { if (val) type_ = InternalObj; else type_ = ExternalObj; }
            ObjectType ObjType() const { return type_; }
            /** Connect a callback to a signal.
                This call is used internally from most signal handling calls, you should usually
                not need to call it directly.
              */
            void Connect(AbstractCbk *e /**< The callback */, const char *signal /**< the signal */) {
                CbkList *events = (CbkList *) g_object_get_data(obj_, "events");
                if (!events) {
                    events = new CbkList();
                    g_object_set_data(obj_, "events", events);
                }

                bool after = false;

                after |= !strcmp(signal, "switch-page");

                if (int id = g_signal_lookup(signal, G_OBJECT_TYPE(obj_))) {
                    GSignalQuery query;
                    GCallback cbk;

                    g_signal_query(id, &query);
                    switch(query.n_params) {
                        case 0:
                            cbk = GCallback(AbstractCbk::real_callback_1);
                            break;
                        case 1:
                            cbk = GCallback(AbstractCbk::real_callback_2);
                            break;
                        case 2:
                            cbk = GCallback(AbstractCbk::real_callback_3);
                            break;
                        case 3:
                            cbk = GCallback(AbstractCbk::real_callback_4);
                            break;
                        case 4:
                            cbk = GCallback(AbstractCbk::real_callback_5);
                            break;
                        case 6:
                            cbk = GCallback(AbstractCbk::real_callback_7);
                            break;
                        default:
                            throw std::runtime_error(std::string("Unhandled signal in Connect: ") + signal);
                    }

                    if (after)
                        g_signal_connect_after(obj_, signal, cbk, e);
                    else
                        g_signal_connect(obj_, signal, cbk, e);
                }
                else
                    throw std::runtime_error(std::string("Bad signal type for object: ") + signal);

                events->push_back(e);
            }

            // callbacks we don't need the widget

            template< typename T, typename R>
            void callback(const char *signal, R (T::*cbk)(), T *classbase, bool returncode = true)
            { Connect(new CbkEvent<T,R>(classbase, cbk, returncode), signal); }
            template< typename T, typename R, typename J>
            void callback(const char *signal, R (T::*cbk)(J), T *classbase, J data,
                          bool returncode = true)
            { Connect(new CbkEvent<T,R,J>(classbase, cbk, data, returncode), signal); }

            // widget callbacks...
            template< typename T, typename R>
            void callback(const char *signal, R (T::*cbk)(Widget &), T *classbase, bool returncode = true)
            { Connect(new CbkEvent<T,R>(classbase, cbk, returncode), signal); }
            template< typename T, typename R, typename J>
            void callback(const char *signal, R (T::*cbk)(Widget &, J), T *classbase, J data,
                          bool returncode = true)
            { Connect(new CbkEvent<T,R,J>(classbase, cbk, data, returncode), signal); }

            // event callbacks...
            template< typename T, typename R>
            void callback(const char *signal, R (T::*cbk)(Event &), T *classbase, bool returncode = true)
            { Connect(new CbkEvent<T,R>(classbase, cbk, returncode), signal); }
            template< typename T, typename R, typename J>
            void callback(const char *signal, R (T::*cbk)(Event &, J), T *classbase, J data,
                          bool returncode = true)
            { Connect(new CbkEvent<T,R,J>(classbase, cbk, data, returncode), signal); }

            void Dispose();

            void Set(const char *property, gfloat value) {
                g_object_set(obj_, property, value, NULL);
            }
            void Set(const char *property, bool value) {
                gboolean val = value;
                g_object_set(obj_, property, val, NULL);
            }
            void Set(const char *property, double value) {
                gfloat temp = value;
                g_object_set(obj_, property, temp, NULL);
            }
            void Set(const char *property, int value) {
                g_object_set(obj_, property, value, NULL);
            }
            void Set(const char *property, const std::string &value) {
                g_object_set(obj_, property, value.c_str(), NULL);
            }
            void Set(const char *property, void *value) {
                g_object_set(obj_, property, value, NULL);
            };
            void Get(const char *property, std::string &value) const {
                gchar *v = NULL;   g_object_get(obj_, property, &v, NULL);
                if (v) {
                    value = v;
                    g_free(v);
                }
            }
            void Get(const char *property, void *&value) const {
                g_object_get(obj_, property, &value, NULL);
            }
            void Get(const char *property, int &value) const {
                g_object_get(obj_, property, &value, NULL);
            }
            void Get(const char *property, bool &value) const {
                gboolean val;
                g_object_get(obj_, property, &val, NULL);
                value = val;
            }
            void Get(const char *property, double &value) const {
                gfloat temp;
                g_object_get(obj_, property, &temp, NULL);
                value = temp;
            }
            void Get(const char *property, gfloat &value) const {
                g_object_get(obj_, property, &value, NULL);
            }
            void Set(const PropertyList &props) {
                for (PropertyIt it = props.begin(); it != props.end(); ++it)
                    (*it)->Set(this);
            }
            void SetData(const char *key, int data) {
                g_object_set_data(obj_, key, GINT_TO_POINTER(data));
            }
            void SetData(const char *key, void *data) {
                g_object_set_data(obj_, key, data);
            }
            void GetData(const char *key, int &data) {
                data = GPOINTER_TO_INT(g_object_get_data(obj_, key));
            }
            void GetData(const char *key, void *&data) {
                data = g_object_get_data(obj_, key);
            }
        protected:
            void Set(GObject *obj) {
                obj_ = obj;
                g_object_set_data(obj_, "object", this);
            }
            static Object *ToObject(void *obj) { return (Object *)g_object_get_data(G_OBJECT(obj), "object"); }
            GObject *obj_;
            ObjectType type_;
            long id_;

            static void purge(GObject *obj, Object *d) {
#ifdef OOGTK_DEBUG
                std::cerr << "Purge called for " << (void *)obj << " type:"
                          << g_type_name(G_OBJECT_TYPE(obj)) << std::endl;
#endif
                d->Dispose();
            }
        private:
            void add_destroy_cbk() {
                if (G_IS_OBJECT(obj_))
                    id_ = g_signal_connect(obj_, "destroy", (void (*)())purge, this);
            }
    };

    /** Template class to handle arbitrary properties on Object instances.
        You can pass a PropertyList to an object through
        Object::Set(const PropertyList &).
       */
    template <typename T>
    class Property : public PropertyBase {
        public:
          Property(const std::string &key, const T &value) :
              PropertyBase(key), payload_(value) {}
          virtual void Set(Object *obj) const {
             obj->Set(keyword_.c_str(), payload_);
          }
        private:
          T payload_;
    };


    inline void Object::
    Init(void *obj) {
        if (obj) {
            obj_ = (GObject *)obj;
            // we want only to sink references, not to increment them
            if (g_object_is_floating(obj_))
                    g_object_ref_sink(obj_); // If an object has a floating reference it should be sinked!

            g_object_set_data(obj_, "object", this);
            add_destroy_cbk();
        }
        else
            throw std::runtime_error("Object allocation failed");
    }

    inline void Object::
    Dispose() {
        if (type_ != ReferenceObj) {
            if (CbkList *events = (CbkList *)
                    g_object_get_data(obj_, "events"))
                delete events;

            g_object_steal_data(obj_, "events");
            g_object_steal_data(obj_, "object");
        }
        obj_ = NULL;
    }
}
