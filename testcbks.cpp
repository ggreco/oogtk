#include "oogtk.h"

class CbkApp : public gtk::Application
{
    public:
       CbkApp() {
           AddTimer(100, &CbkApp::TimerA, this);
           AddTimer(200, &CbkApp::TimerB, this, (void *)0xdeadbeef);
           AddTimer(2000, &CbkApp::TimerC, this);
           AddIdle(&CbkApp::IdleA, this);

           // if you don't comment this you'll get a lot of spam
           AddIdle(&CbkApp::IdleB, this, (void *)0x12345678);
       }

       bool IdleA() { std::cerr << "."; return false; }
       void IdleB(void *data) {
           std::cerr << "<" << data << " >";
       }
       void TimerA() { std::cerr << "<Ta>";}
       void TimerB(void *data) {
           std::cerr << "<Tb- " << data << " >";
       }
       bool TimerC() {
           std::cerr << "\n\nTimedC -> QUIT!\n";
           Quit();
           return false;
       };
};

int main()
{
    CbkApp app;

    app.Run();
}
