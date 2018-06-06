#ifndef OOTHREAD_H

#define OOTHREAD_H

#include <string>
#include <stdexcept>
#include <glib.h>
#include <string.h>
#include <iostream>

namespace gtk {

/** A simple Thread class

This class implement the thread paradigm in an object oriented clean way.

Your objects may derive from Thread and implement a Thread::worker_thread function that will be executed in the new thread context. Your thread will have access to all the class data and it will be your responsability to sync the data access from the father and the child, for instance with a gtk::Mutex .

If you include oothread  you should remember to add gthread-2.0 to the package list in the linker flags, something like should be enough:

LDFLAGS += `pkg-config --libs gtk+-2.0 gmodule-2.0 gthread-2.0`

\example
#include "oogtk.h"
#include "oothread.h"

class MyThread : public gtk::Thread {
    int idx_;
    void worker_thread() {
        while (Running()) { // check if the thread is "running" is important since the Terminate() call depend from this
            USleep(100000);
            std::cerr << "Hi, I'm thread " << idx_ << "!\n";
        }
    }
public:
    MyThread(int i) : idx_(i + 1) { Start(); } // we start the thread in the costructor, this is optional
};

#define THREADS 5

int main()
{
    gtk::Thread *th[THREADS];
    gtk::Application::ThreadInit();

    for (int i = 0; i < THREADS; ++i)
        th[i] = new MyThread(i);
  
    gtk::Thread::Sleep(10);
    
    for (int i = 0; i < THREADS; ++i)
        delete th[i];
}
\endexample
*/
class Thread
{
    static /*__thread*/ thread_local char thread_name[40];
    
/// DOXYS_OFF    
    static GPrivate *PrivateKey() {
#if GLIB_MINOR_VERSION < 32
        static GPrivate *key = NULL;

        if (!key)
            key = g_private_new(NULL);
        return key;
#else
        static GPrivate key = G_PRIVATE_INIT(NULL);
        return &key;
#endif
    }

    std::string name_;
    bool running_;
    bool done_;
    bool detached_;
    GThread *th_;
/// DOXYS_ON
protected:
    /// The thread entry point function, your class should derive from thread and redefine this.
    virtual void worker_thread() = 0;
public:
    // set the name of the thread
    void Name(const std::string &n /**< a string that will be used as thread name */) { name_ = n; }
    /// get the name of the thread (as specified in his constructor)
    const std::string &Name() const { return name_; }
    /// set the current thread name
    static void SetCurrentThreadName(const std::string &n) {
         if (Thread *thr = 
                static_cast < Thread * >(g_private_get(PrivateKey()))) 
            thr->name_ = n;
         else
            strncpy(thread_name, n.c_str(), sizeof(thread_name) - 1);
    }
    /// get the current running thread name
    static std::string CurrentThreadName() {
        if (Thread *thr = 
                static_cast < Thread * >(g_private_get(PrivateKey()))) 
            return (*thr).name_;
        else if (*thread_name)
            return thread_name;
        else
            return "Main";
    }
    /// sleep for a certain amount of seconds
    static void Sleep(int secs /**< Number of seconds to wait */) {
        g_usleep(secs * 1000000);
    }
    /// sleep for a certain amount of usecs
    static void USleep(int usecs /**< Amount of micro seconds to wait */) {
        g_usleep(usecs);
    }
    /// create a new thread with an optional name
    Thread(const std::string &name = "child thread") : 
        name_(name), running_(false), done_(true), detached_(false), th_(0) {
    }

    virtual ~Thread() {};
    /** Terminate a thread

      You don't need to call this if your thread already completed (you can call Thread::join if the thread is not detached or nothing if the thread has been detached with Thread::detach), but it's not an error if you do so.

      This function will try to close a running thread first in a clean way changing Thread::running() value to false, if the thread doesn't terminate in 1000 ms then an error message is dumped, and the object is considered "hung". 

      \note On thread 
     */
    virtual void Terminate() {
        int die_wait = 0;

        if (g_thread_self() == th_)
            return;

        if (done_) {
            if (th_ != 0) {

                if (!detached_)
                    g_thread_join(th_);

                th_ = 0;
            }
            return;
        }

        running_ = false;

        while ((!done_) && (die_wait++ < 100))
            g_usleep(10000);

        if (die_wait >= 100) {
            std::cerr << "thread " << Name() << " refused to die - and cancelling not implemented!" << std::endl;
        }
        else if (!detached_)
            g_thread_join(th_);

        th_ = 0;
    }
    /** Detach a thread.

      This call will detach the thread, this will be no more joinable, but the thread will release all of his memory (except the gtk::Thread object itself) once it quits.

      \note You cannot call this on a running thread, if you do it a std::runtime_error exception will be raised.
     */
    void Detach() { 
        if (running_)
            throw std::runtime_error("Unable to detach a running thread.");

        detached_ = true;
    }

    /// Check if the thread is running
    bool Running() const { return running_; }

    /// Starts a thread    
    /// \retval false if the thread is already running or it cannot be created.
    bool Start() {
        if (running_)
            return false;

        running_ = true;
        done_ = false;

        if (!(th_ = 
#if GLIB_MINOR_VERSION < 32
                    g_thread_create(threadfunc, this, detached_ ? FALSE : TRUE, NULL)
#else
                    g_thread_new(name_.c_str(), threadfunc, this)
#endif
                )) {
            running_ = false;
            done_ = true;

            return false;
        }
#if GLIB_MINOR_VERSION > 31
        if (detached_)
            g_thread_unref(th_);
#endif

        return true;
    }

    /// Wait for a not detached thread to complete    
    void Join() { if (!detached_ && th_) g_thread_join(th_); }

    /// DOXYS_OFF   
private:    
    static void *threadfunc(void *param) {
        Thread *pThread = static_cast < Thread * >(param);

        GPrivate *p = PrivateKey();
        g_private_set(p, pThread);

        try {
            pThread->worker_thread();
        }
        catch(std::runtime_error &e) {
            std::cerr << "Exception in threadfunc: " << e.what() << std::endl;
            //ELOG << "Exception in threadfunc: " << e;
        }
        catch(...) {
            std::cerr <<
                "(thread) unhandled exception caught: (fatal)";
            //ELOG << "(thread) unhandled exception caught: (fatal)";
        }

        pThread->done_ = true;
        pThread->running_ = false;

        return NULL;
    }
    /// DOXYS_ON    
};

}
#endif
