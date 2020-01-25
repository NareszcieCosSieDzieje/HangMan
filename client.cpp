
#include <netinet/in.h>
#include <netdb.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/epoll.h>
#include <cstring>
#include <csignal>
#include <string>

#include "statuses.hpp"

//=====================================GLOBALS============================================\\

int epollFd{};
int clientFd{};
int serverFd{};
const unsigned int localPort{59998};
const unsigned int remotePort{59999};
sockaddr_in bindAddr {
        .sin_family = AF_INET,
        .sin_port = htons(localPort),
        .sin_addr = htonl(INADDR_ANY)};
addrinfo hints {
        .ai_flags= 0,
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
        .ai_protocol = IPPROTO_TCP};
addrinfo *resolved;

char* login;
char* password;
//std::string login;
//std::string password;


//========================================FUNC PROTO========================================\\


void startClient(void);
void closeClient(void);
void startConnection(void);
ssize_t readData(int fd, char * buffer, ssize_t buffsize);
void writeData(int fd, char * buffer, ssize_t count);
void sigHandler(int signal);


//TODO: obsługa zerwania połączenia, obsługa rozłączenia się tj: wysłanie sygnału itp

//=======================================MAIN=============================================\\

int main(int argc, char* argv[]){

    signal(SIGINT, sigHandler);

    //TODO: Dorobic wysylanie HASLA i LOGINU, ładowanie z plku, jesli nie ma trzeba podać zarejestrować się?
    // na razie wysylane to samo z kazdego klienta byle cos wyslac. Ustawiane w ------> startClient
    startClient();

    startConnection();

    char msg[100];
    strcpy(msg, login);
    strcpy(msg, "-");
    strcat(msg, password);

    writeData(serverFd, msg, sizeof(msg));
    memset(msg, 0, sizeof(msg));
    auto x = readData(serverFd, msg, sizeof(msg) );
    if (x != 3){
        perror("Error loggin in.\n");
    }

    //TODO: POŁĄCZ SIE Z DANĄ SESJA

    bool joinedSession = false;
    while(!joinedSession){
        //TODO:
        //read dane sesji jakie sa dostepne
        //DWIE OPCJE STWORZ I DOŁĄCZ
        // przetwórz i wyświetl dane sesji roznych one maja ID
        //na podstawie tego co klikniesz daj znac ktora wybierasz
        //wyslij ID SESJI do ktorej chcesz dołączyć
        //
        //czekaj na odpowiedz do ktorej sesji dołaczyłes, i czy, jesli sukces to break



        joinedSession = true;
    }

    //TODO:JAK OK TO WJEDZ W PETLE GRY

    while(true){
        epoll_event event{};
        event.events = EPOLLIN;
        int set = epoll_wait(epollFd, &event, 1, -1); // -1 == nieskończoność
        if (set == -1){
            perror("Epoll failed to wait.\n");
            exit(EPOLL_WAIT); //TODO: dodaj obsluge zamkniecia
        }
        if( event.events & EPOLLIN && event.data.fd == clientFd ){

            //send jesli nie wyslal ile mial to od nopwa
            //read clientFd
            //write serverFd
        }

        if (0){
          break;
        }
    }

    closeClient();

    return 0;
}


//=======================================FUNC DEC=========================================\\




void startClient(void){
    if((clientFd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
        perror("Failed to create socket.\n");
        exit(SOCKET_CREATE);
    }
    if( bind(clientFd, (sockaddr*)&bindAddr, sizeof(bindAddr)) < 0 ){
        perror("Failed to bind the socket.\n");
        exit(SOCKET_BIND);
    }
    epollFd =  epoll_create1(0);
    epoll_event event;
    event.events = EPOLLIN; //TODO: CZY MA BYC IN??????
    event.data.fd = clientFd; //TODO: CZY TAK MA BYĆ?
    //FIXME: CZY TO POTRZEBNE DO KLIENTA WGL
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &event) < 0) {
        perror("Failed to add fd to epoll.\n");
        exit(EPOLL_ADD);
    }
    //TODO: ustaw login hasło, tj załaduj itp
    login = "test_user";
    password = "test_pass";
}

void closeClient(void){
    shutdown(clientFd, SHUT_RDWR);
    close(clientFd);
}


void startConnection(void){ //TODO: MOZE POLACZ ZE STARETEM ALE ZOBACZYMY
    int err = getaddrinfo("127.0.0.1", "59999", &hints, &resolved);
    if (err || !resolved){
        perror("Resolving address failed!\n");
        exit(GETADDRINFO_ERROR);
    }
    serverFd = connect(clientFd, resolved->ai_addr, resolved->ai_addrlen);
    if (serverFd < 0){
        perror("Failed to connect.\n");
        exit(SOCKET_CONNECT);
    }
    freeaddrinfo(resolved);

}


/*
int recv_all(int sockfd, void *buf, size_t len, int flags)
{
    size_t toread = len;
    char  *bufptr = (char*) buf;

    while (toread > 0)
    {
        ssize_t rsz = recv(sockfd, bufptr, toread, flags);
        if (rsz <= 0)
            return rsz;  // Errr or other end closed cnnection

        toread -= rsz;  // Read less next time
        bufptr += rsz;  // Next buffer position to read into
    }

    return len;
}

*/


void sigHandler(int signal){
    closeClient();
}


ssize_t readData(int fd, char * buffer, ssize_t buffsize){
    auto ret = read(fd, buffer, buffsize);
    if(ret==-1) perror("read failed on descriptor %d\n");
    return ret;
}

void writeData(int fd, char * buffer, ssize_t count){
    auto ret = write(fd, buffer, count);
    if(ret==-1) perror("write failed on descriptor %d\n");
    if(ret!=count) perror("wrote less than requested to descriptor %d (%ld/%ld)\n");
}


