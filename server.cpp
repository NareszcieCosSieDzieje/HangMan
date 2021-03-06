
#include <thread>
#include <vector>
#include <netinet/in.h>
#include <unistd.h>
#include <csignal>
#include <sys/epoll.h>
#include <cstring>
#include <map>
#include <mutex>
#include <errno.h>
#include <iostream>

#include "statuses.hpp"
#include "player.hpp"
#include "player.cpp"
#include "user_loader.hpp"


//=====================================GLOBALS============================================\\

std::vector<std::thread> threadVector;
std::vector<int> clientSockets;


std::map<int, Player> clientMap;
std::mutex clientMapMutex;

std::map<int, std::vector<Player>> playerSessions;
//std::mutex playerSessionsMutex;

int epollFd{};
int serverFd{};
const unsigned int localPort{55555};
sockaddr_in bindAddr {
        .sin_family = AF_INET,
        .sin_port = htons(localPort),
        //.sin_addr = htonl(INADDR_ANY)
};

int maxSessions = 2; //TODO: ile sesji?
int playersPerSession = 4;

//========================================FUNC PROTO========================================\\


ssize_t readData(int fd, char * buffer, ssize_t buffsize);
void writeData(int fd, char * buffer, ssize_t count);
void sigHandler(int signal);
void startServer(void);
void listenLoop(void);
void sendAvailableSessions(void);
void stopConnection(int ClientFd);
void clientValidation(int newClientFd);
void sendSessionData(int clientSocket);
std::string* loadUserData(char* filePath); //TODO: zwróć array stringów


/* TODO: ZADANIA SERWERA:
 * nasłuchuj połączeń;
 * pokazuj sesje dostępne i ile graczy;
 * obsługuj tworzenie sesji, zamykanie, dołączanie,
 * obsługuj przebieg rozgrywki sesji;
*/


//=======================================MAIN=============================================\\

int main(int argc, char* argv[]){


    //TODO: SERVER MUSI WCZYTAC LISTE LOGINOW I HASEL I SPRAWDZAC KTORE SA ZUZYTE

    signal(SIGINT, sigHandler);
    signal(SIGTSTP, sigHandler);

    bindAddr.sin_addr.s_addr = htonl(INADDR_ANY); //TODO: ZMIEN NA ADRESOKRESLONY

    startServer();

    std::cout<< "MAIN PRZED LOOP" << std::endl;
    while(true){

    }
    std::cout<< "MAIN PO LOOP" << std::endl;


    //obsluguj deskryptory graczy

    /*
    struct epoll_event event, events[MAX_EVENTS];

    event.events = EPOLLIN;
    event.data.fd = 0;

    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &event)){
        fprintf(stderr, "Failed to add file descriptor to epoll\n");
        close(epoll_fd);
        exit(EPOLL_ADD);
    }

    while(running)
    {
        printf("\nPolling for input...\n");
        event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, 30000);
        printf("%d ready events\n", event_count);
        for(i = 0; i < event_count; i++)
        {
            printf("Reading file descriptor '%d' -- ", events[i].data.fd);
            bytes_read = read(events[i].data.fd, read_buffer, READ_SIZE);
            printf("%zd bytes read.\n", bytes_read);
            read_buffer[bytes_read] = '\0';
            printf("Read '%s'\n", read_buffer);

            if(!strncmp(read_buffer, "stop\n", 5))
                running = 0;


        }


          //FIXME: xd
    if (close(epoll_fd) < 0){
        perror("Server epoll close error\n");
        exit(EPOLL_CLOSE);
    }
    */

            //TODO: gdzieś indziej! ALE OGARNIJ CZEKANIE NA WĄTKI
    while (!threadVector[0].joinable()){

    }
    threadVector[0].join();


    return 0;
}


//=======================================FUNC-DEC=========================================\\

void startServer(void){
    if( (serverFd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
        perror("Failed to create socket.\n");
        exit(SOCKET_CREATE);
    }
    int enable = 1;
    if( setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0 ){
        perror("Setsockopt(SO_REUSEADDR) failed.\n");
        exit(SOCKET_REEUSE);
    }
    if( bind(serverFd, (sockaddr*)&bindAddr, sizeof(bindAddr)) < 0 ){
        perror("Failed to bind the socket.\n");
        exit(SOCKET_BIND);
    }
    if( listen(serverFd, 5) < 0 ){
        perror("Failed to listen.\n");
        exit(SOCKET_LISTEN);
    }
    printf("1\n");
    epollFd = epoll_create1(0);
    if (epollFd < 0){
        perror("Server epoll error!\n");
        exit(EPOLL_ERROR);
    }
    std::thread loopThread(listenLoop); //Uruchom wątek nasłuchujący nowych połączeń.
    threadVector.push_back(std::move(loopThread)); //Wątek nie może być skopiowany.
}


//TODO: czy wątek/funkcja ma cały czas w pętli nawalać accepty? czy lepsze rozwiązanie jest?
void listenLoop(void){
    while(true){
        sockaddr_in clientAddr{};
        socklen_t cliLen = sizeof(clientAddr);
        printf("przed\n");
        int newClient = accept(serverFd, (struct sockaddr *)&clientAddr, &cliLen); //Nawiąż nowe połączenie z klientem.
        printf("po\n");
        if (newClient < 0) {
            perror("ERROR on accept.\n");
            exit(SOCKET_ACCEPT);
        }
        std::thread validationThread(clientValidation, newClient); //Nowe połączenie przeslij do zweryfikowania
        validationThread.detach();
    }
    //TODO: jeśli jakis condition_variable to zakoncz prace?
}


void clientValidation(int newClientFd){


    std::cout << "WERYFIKACJA KLIENTA\n" << std::endl;
    //TODO: sprawdz login haslo jesli rip to wywal, jak ok to dodaj, mozliwe jeszcze sprawdzanie portu ale jak jest haslo to raczej bez sensu?
    char msg[100];
    auto x = readData(newClientFd, msg, sizeof(msg) );
    //TODO: delete later
    if(x != 100){
        perror("User data sending error.\n");
    }
    //for (int index = 0; msg[index] != '/0'; index++) {
    //for (int index = 0; msg[index] != '/0'; index++) {
    //    cout << msg[index];
    //}
    //char* userData = "test_user-test_pass";
    char * pch;
    pch = strtok(msg, "-");
    char* login;
    char* pass;
    if(pch != NULL ){
        login = pch;
        printf("%s\n", login);
        pch = strtok(NULL, "-");
    }
    if(pch != NULL ){
        pass = pch;
        printf("%s\n", pass);
    }
    bool userExists = false;
    std::string loginS(login);
    std::string passwordS(pass);
    userExists = searchForUserData(loginS, passwordS);
    std::cout<<userExists<<std::endl;

    /* bool userExists = false;
     * if (  (strcmp(login, "test_user") == 0) && (strcmp(pass, "test_pass") == 0) ){
        printf("Pass sie zgadza.\n");
        printf("Log sie zgadza.\n");
        userExists = true;
    }
    */
    if (userExists) {
        //TODO: odhacz zużyte haslo login? z jakiejs maoy hasel loginow na starcie wcztytanej
        Player newPlayer(login, pass);
        //Dodaj do mapy klientow -graczy
        clientMapMutex.lock();
        clientMap.insert(std::pair<int, Player>(newClientFd, newPlayer));
        clientMapMutex.unlock();
        //Wyslij ack ze sie zalogował
        writeData(newClientFd, "AUTH-OK", sizeof("AUTH-OK"));
        //TODO: OBSŁUŻ GO TERAZ?
        bool joinedSession = false;
        while(!joinedSession){
            sendSessionData(newClientFd);
            //get chosen session

            //sleep?
        }
    } else {
        writeData(newClientFd, "AUTH-FAIL", sizeof("AUTH-FAIL"));
        //sleep(0.2)
        stopConnection(newClientFd);
    }

}

//MUTEX?
void sendSessionData(int clientSocket){
    std::string sessionData("");
    if (playerSessions.size() > 0){
        std::string sessionData(std::to_string(sessionData.size()));
        sessionData.append(":");
    }
    for( auto const& [key, val] : playerSessions )
    {
        int sessionID = key;
        sessionData.append(std::to_string(sessionID));
        sessionData.append("-");
        std::vector<Player> players = val;
        sessionData.append(std::to_string(players.size()));
        for (auto & element : players) {
            sessionData.append(",");
            sessionData.append(element.getNick());
        }
        sessionData.append(";");
        //CZY WYSYLAC POJEDYNCZE INFO O SESJI CZY WLASNIE NA KONCY FUNCJI CALOSC?
    }
    std::vector<char> chars(sessionData.c_str(), sessionData.c_str() + sessionData.size() + 1u); //TODO zobacz czy dziala
    writeData(clientSocket, &chars[0], sizeof(chars));
    //i1d-nick1-nick2-nick3:id2-nick4-nick5-nick6
    /* opcjonalnie zamiast wektora
     * char *cstr = &str[0];
     *
     * std::string str = "string";
        char *cstr = new char[str.length() + 1];
        strcpy(cstr, str.c_str());
        // do stuff
        delete [] cstr;
     *
     * */
}


//TODO: jakiś send że zrywamy połączenie?? to raczej w instacji danego problemu dac
void stopConnection(int ClientFd){
    if (shutdown(ClientFd, SHUT_RDWR) < 0 ){
        perror("Failed to disconnect with the client.\n");
        //FIXME: exit?
    }
    close(ClientFd);
}


void sigHandler(int signal){
    printf("CTRL + C\n");
    if (threadVector[0].joinable()) {
        threadVector[0].join();
    }
    //TODO: closeServer();
}

void sessionLoop() { //TODO: jak to rozwiązać
    while(true){

        //conduct session
    }

}



ssize_t readData(int fd, char * buffer, ssize_t buffsize){
    auto ret = read(fd, buffer, buffsize);
    std::cout << "Read ret: " << ret << std::endl;
    if(ret==-1) {
       // printf("Oh dear, something went wrong with read()! %s\n", strerror(errno));
        perror("read failed on descriptor %d\n");
    }
    //perror("read failed on descriptor %d\n");
    return ret;
}

void writeData(int fd, char * buffer, ssize_t count){
    auto ret = write(fd, buffer, count);
    std::cout << "Read ret: " << ret << std::endl;
    if(ret==-1) perror("write failed on descriptor %d\n");
    if(ret!=count) perror("wrote less than requested to descriptor %d (%ld/%ld)\n");
}


/*
void joinAllThreads(){
    for (std::thread & th : sessionThreads)
    {
        // If thread Object is Joinable then Join that thread.
        if (th.joinable()){
            th.join();
        }
    }
}
*/

