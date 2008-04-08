#ifndef OOOBJ_H
#define OOOBJ_H

#include <stdexcept>
#include <vector>
#include "inline_containers.h"

namespace gtk
{
    // questa classe serve x la conversione tra enum interscambiabili
    // ad esempio GtkInterpType e InterpType
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

    // empty class used for null constructor of intermediate, instantiable, classes.
    class DerivedType
    {
    };

    class AbstractCbk
    {
        public:
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
    };

    class Widget;
    struct Event;

    template <typename T, typename R, typename J = int> struct ReturnType {
       bool notify(R (T::*fnc)(void), T *obj, bool rc) const { 
            return (obj->*fnc)();
       }
       bool notify(J arg, R (T::*fnc)(J), T *obj, bool rc) const { 
            return (obj->*fnc)(arg);
       }
       bool notify(Widget &w, R (T::*fnc)(Widget &), T *obj, bool rc) const { 
            return (obj->*fnc)(w);
       }
       bool notify(Widget &w, J arg, R (T::*fnc)(Widget &, J), T *obj, bool rc) const { 
            return (obj->*fnc)(w,arg);
       }
       bool notify(Event &w, R (T::*fnc)(Event &), T *obj, bool rc) const { 
            return (obj->*fnc)(w);
       }
       bool notify(Event &w, J arg, R (T::*fnc)(Event &, J), T *obj, bool rc) const { 
            return (obj->*fnc)(w,arg);
       }
    };

    template <typename T, typename J> struct ReturnType<T, void, J> {
      bool notify(void (T::*fnc)(void), T *obj, bool rc) const { 
            (obj->*fnc)();
            return rc;
       }
       bool notify(J arg, void (T::*fnc)(J), T *obj, bool rc) const { 
            (obj->*fnc)(arg);
            return rc;
       }
       bool notify(Widget &w, void (T::*fnc)(Widget &), T *obj, bool rc) const { 
            (obj->*fnc)(w);
            return rc;
       }
       bool notify(Widget &w, J arg, void (T::*fnc)(Widget &, J), T *obj, bool rc) const { 
            (obj->*fnc)(w,arg);
            return rc;
       }
       bool notify(Event &w, void (T::*fnc)(Event &), T *obj, bool rc) const { 
            (obj->*fnc)(w);
            return rc;
       }
       bool notify(Event &w, J arg, void (T::*fnc)(Event &, J), T *obj, bool rc) const { 
            (obj->*fnc)(w,arg);
            return rc;
       }
    };

    template <typename T, typename R, typename J = int>
        class CbkEvent : public AbstractCbk
    {
            enum CbkType {NoParam, HasWidget, HasEvent};
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

        private:    
            T*  myObj ;
            R (T::*myFnc0)();
            R (T::*myFnc1)(J);
            R (T::*mywFnc0)(Widget &);
            R (T::*mywFnc1)(Widget &, J);
            R (T::*myeFnc0)(Event &);
            R (T::*myeFnc1)(Event &, J);
            J ma1;
            bool rccode;
            CbkType type;

            virtual bool notify(GtkWidget *w = NULL, GdkEvent *e = NULL) const;
    };

    class Object;

    class PropertyBase 
    {
        public:
            PropertyBase(const std::string &key) : keyword_(key) {}
            virtual void Set(Object *) const = 0;
        protected:
            std::string keyword_;
    };

    typedef std::vector<PropertyBase *> PropertyList;
    typedef PropertyList::const_iterator PropertyIt;

    class Object
    {
        private:
        public:
            enum ObjectType { InternalObj, ExternalObj, ReferenceObj};
            typedef std::list<AbstractCbk *> CbkList;

            Object() : obj_(NULL), type_(ExternalObj), id_(-1) {}
            const GObject *Obj() const { return obj_; }
            GObject *Obj() { return obj_; }
            void Init(void *obj);
            Object &Ref() { g_object_ref(obj_); return *this; }
            Object(const Object &o) : type_(ReferenceObj) { obj_ = o.obj_; g_object_ref(obj_); add_destroy_cbk(); }
            virtual ~Object() {

                if (obj_) {
#ifdef OOGTK_DEBUG
                std::cerr << "Destructor for " << (void *)obj_ << " type:"
                          << g_type_name(GTK_OBJECT_TYPE(obj_))  
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
            void Connect(AbstractCbk *e, const char *signal) {
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
            
            void Set(const char *property, int value) {
                g_object_set(obj_, property, value, NULL);
            }
            void Set(const char *property, const std::string &value) {
                g_object_set(obj_, property, value.c_str(), NULL);
            }
            void Set(const char *property, void *value) {
                g_object_set(obj_, property, value, NULL);
            };
            void Get(const char *property, std::string &value) {
                gchar *v = NULL;   g_object_get(obj_, property, &v, NULL);
                if (v) { 
                    value = v;
                    g_free(v);
                }
            }
            void Get(const char *property, void *&value) {
                g_object_get(obj_, property, &value, NULL);
            }
            void Get(const char *property, int &value) {
                g_object_get(obj_, property, &value, NULL);
            }
            void Set(const PropertyList &props) {
                for (PropertyIt it = props.begin(); it != props.end(); ++it)
                    (*it)->Set(this);
            }
        protected:
            void Set(GObject *obj) {
                obj_ = obj; 
                g_object_set_data(obj_, "object", this);
            }
            static Object *ToObject(void *obj) { return (Object *)g_object_get_data(G_OBJECT(obj), "object"); }
            GObject *obj_;
            ObjectType type_;
            int id_;

            static void purge(GObject *obj, Object *d) {
#ifdef OOGTK_DEBUG
                std::cerr << "Purge called for " << (void *)obj << " type:"
                          << g_type_name(GTK_OBJECT_TYPE(obj)) << std::endl;
#endif
                d->Dispose();
            }
        private:
            void add_destroy_cbk() {
                if (GTK_IS_OBJECT(obj_))
                    id_ = g_signal_connect(obj_, "destroy", (void (*)())purge, this);
            }
    };

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

    template <typename T,typename R, typename J>
    inline bool CbkEvent<T,R,J>::
    notify(GtkWidget *w, GdkEvent *e) const
    { 
        ReturnType<T, R, J> rtype;

        switch (type) {
            case NoParam:
                if (myFnc0)
                    return rtype.notify(myFnc0, myObj, rccode);
                else
                    return rtype.notify(ma1, myFnc1, myObj, rccode);
        

            case HasWidget:
                if (Widget *ww = dynamic_cast<Widget *>(Object::Find((GObject *)w))) {
                    if (mywFnc0)
                        return rtype.notify(*ww, mywFnc0, myObj, rccode);
                    else
                        return rtype.notify(*ww, ma1, mywFnc1, myObj, rccode);
                }
                else
                    throw std::runtime_error("Callback asking for a widget with widget NULL!");
            case HasEvent:
                if (Event *ee = (Event *)e) {
                    if (myeFnc0)
                        return rtype.notify(*ee, myeFnc0, myObj, rccode);
                    else
                        return rtype.notify(*ee, ma1, myeFnc1, myObj, rccode);
                }
                else
                    throw std::runtime_error("Callback asking for an event with event NULL!");
        }
    }

}
#endif
