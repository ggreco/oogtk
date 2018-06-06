#ifndef OOCRIT_H__
#define OOCRIT_H__

/**
 * GG
 * Small package around gtk Mutex 
 */

#include <glib.h> 

namespace gtk {

class Mutex
{
#if GLIB_MINOR_VERSION < 32
    private:
        GStaticMutex mutex;
    public:
        Mutex() {	
            g_static_mutex_init(&mutex);
        }
        ~Mutex() { 
            g_static_mutex_free(&mutex);
        }
        void Lock() { 
            g_static_mutex_lock(&mutex);
        }
        void Unlock() { 
            g_static_mutex_unlock(&mutex);
        }
#else
    private:
        GMutex mutex;
    public:
        Mutex() {	
            g_mutex_init(&mutex);
        }
        ~Mutex() { 
            g_mutex_clear(&mutex);
        }
        void Lock() { 
            g_mutex_lock(&mutex);
        }
        void Unlock() { 
            g_mutex_unlock(&mutex);
        }        
#endif
        // old CriticalSection api emulation
        void Enter() { Lock(); }
        void Leave() { Unlock(); }
};

class AutoMutex
{
    public:
        AutoMutex(Mutex & cs) : cs_(cs) { cs_.Lock(); };
        ~AutoMutex() { cs_.Unlock(); };
    private:
        Mutex & cs_;
};

class Sync
{
#if GLIB_MINOR_VERSION < 32
 private:
	GCond  *cv;
	GMutex *mutex;
 public:
    Sync() { mutex = g_mutex_new(); cv = g_cond_new(); }
    ~Sync() { g_cond_free(cv); g_mutex_free(mutex); }
	void Wait(void) { g_cond_wait(cv, mutex); };
	void Lock(void) { g_mutex_lock(mutex); };
	void Unlock(void) { g_mutex_unlock(mutex); };
    int  Wait(long msecs) {
        GTimeVal now;
        g_get_current_time(&now);
        g_time_val_add(&now, msecs * 1000L);
        return g_cond_timed_wait(cv, mutex, &now);
    }
	void Signal(void) { g_cond_signal(cv); };
	void SignalAll(void) { g_cond_broadcast(cv); };    
#else
 private:
	GCond  cv;
	GMutex mutex;
 public:
    Sync() { g_mutex_init(&mutex); g_cond_init(&cv); }
    ~Sync() { g_cond_clear(&cv); g_mutex_clear(&mutex); }
	void Wait(void) { g_cond_wait(&cv, &mutex); };
	void Lock(void) { g_mutex_lock(&mutex); };
	void Unlock(void) { g_mutex_unlock(&mutex); };
    int  Wait(long msecs) {
        gint64 end_time = g_get_monotonic_time() + msecs * G_TIME_SPAN_MILLISECOND;
        return g_cond_wait_until(&cv, &mutex, end_time);
    }
	void Signal(void) { g_cond_signal(&cv); };
	void SignalAll(void) { g_cond_broadcast(&cv); };    
#endif
};

}

#ifndef NO_CRITSECTION_EMUL
typedef gtk::Mutex CritSection;
typedef gtk::AutoMutex AutoCS;
#endif

#endif

