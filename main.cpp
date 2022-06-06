#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <thread>
#include <mutex>
#include <termios.h>

using namespace std;

struct termios orig_termios;
char msg[1024];
string send_msg;
string dmsg;


void disableRawMode(){
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
void enableRawMode(){
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    raw.c_lflag&=~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}



void recvmg(int socket){
    int len;
    while((len = recv(socket, msg, sizeof(msg), 0)) > 0){
        msg[len] = '\n';
        string data = dmsg + send_msg;
        write(STDOUT_FILENO, "\r\x1b[0J", 6);
        write(STDOUT_FILENO, msg, len + 1);
        write(STDOUT_FILENO, data.c_str(), data.size());
    }
}

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "Russian");
    int len;
    int sock;

    sockaddr_in address;
    string client_name = argv[1];
    dmsg = client_name + ":";

    //ENABLE RAW MODE
    enableRawMode();

    sock = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_port = htons(8082);
    address.sin_family = AF_INET;
    inet_pton(AF_INET, "31.172.66.211", &address.sin_addr);
    write(STDOUT_FILENO, "Connected\n", 10);

    if((connect(sock, (sockaddr*)&address, sizeof(address))) < 0)
        write(STDOUT_FILENO, "Connect failure", 15);

    write(STDOUT_FILENO, dmsg.c_str(), dmsg.size());

    thread t(recvmg, sock);


    while(true){
        char c;
        read(STDIN_FILENO, &c, 1);
        if(c == 127){
            int size = send_msg.size();
            if(size == 0) continue;

            write(STDOUT_FILENO, "\b\x1b[0J", 5);

            if(size >= 2){
                string cp;
                cp.push_back(send_msg[size-2]);
                cp.push_back(send_msg[size-1]);
                if((cp >= "а" && cp <= "я") || (cp >= "А" && cp <= "Я"))
                    send_msg.pop_back();
            }
            send_msg.pop_back();
            continue;
        }
        else if(c == '\n' && send_msg.size() > 0){
            string data = dmsg + send_msg;
            if((len = send(sock, data.c_str(), data.size(), 0)) < 0)
                write(STDOUT_FILENO, "Send failure", 12);
            write(STDOUT_FILENO, &c, 1);
            write(STDOUT_FILENO, dmsg.c_str(), dmsg.size());
            send_msg.clear();
            continue;
        }

        send_msg += c;
        write(STDOUT_FILENO, &c, 1);
    }

    t.join();
    close(sock);
    return 0;
}
