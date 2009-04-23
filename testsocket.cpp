#include "oogtk.h"
#include "tcpcommon.h"
#include <stdlib.h>

// use for instance "nc -vlp 4000" to talk with this test

class MyApp : public gtk::Application
{
    gtk::Window w;
    gtk::Label l;
public:
    MyApp(int fd, const std::string &title) : w(title) {
        w.Border(16);
        l.Set("");
        w.Child(l);
        w.ShowAll();

        // attach for different callbacks to the same event to see if all of the syntaxes
        // available of AddSocket() work as expected.
        AddSocket(fd, gtk::SocketRead, &MyApp::recv1, this, 123456);
        AddSocket(fd, gtk::SocketRead, &MyApp::recv2, this);
        AddSocket(fd, gtk::SocketRead, &MyApp::recv3, this);
        AddSocket(fd, gtk::SocketRead, &MyApp::recv4, this, (short)3333);
    }

    void recv1(int fd, int userdata) {
        std::cerr << "Entering recv for fd: " << fd << " and userdata: " << userdata << '\n';
        char buffer[256];

        int rc = ::recv(fd, buffer, sizeof(buffer), 0); 

        if (rc == 0) {
            std::cerr << "Connection closed.\n";
            Quit();
        }
        else if (rc < 0 && ERRNO != EWOULDBLOCK) {
            std::cerr << "Connection error.\n";
            Quit();
        }
        else if (rc > 0) {
            l.SetF("%d bytes received: [%s]", rc, std::string(buffer, rc).c_str());
            std::cerr << l << '\n';
        }
    }
    void recv2(int fd) {
        std::cerr << "Entering FAKE recv for fd: " << fd << "\n";
    }
    bool recv3(int fd) {
        std::cerr << "Entering FAKE bool recv for fd: " << fd << "\n";
        return true;
    }
    bool recv4(int fd, short data) {
        std::cerr << "Entering FAKE bool recv for fd: " << fd << " data: " << data << "\n";
        return true;
    }
};

int main(int argc, char *argv[]) 
{
    if(argc != 3) {
        std::cerr << "Usage: testsocket host port\n";
        exit(0);
    }

    initialize_tcpip();

    struct sockaddr_in addr_out; 
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&addr_out, 0, sizeof(addr_out));
    addr_out.sin_family = AF_INET;
    addr_out.sin_port = htons(atoi(argv[2]));
    addr_out.sin_addr.s_addr = inet_addr(argv[1]);

    std::ostringstream title;
    title << argv[1] << ':' << argv[2];

    std::cerr << "Connecting to " << title.str() << "...";
    
    if (connect(fd, (struct sockaddr *)&addr_out, sizeof(addr_out)) == -1) {
        std::cerr << "Unable to connect to " << argv[1] << ':' << argv[2] << '\n';
        return -1;
    }

    std::cerr << "Ok.\n";

    MyApp a(fd, title.str());
    a.Run();
}
