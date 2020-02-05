
#include <netinet/in.h>
#include <netdb.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <csignal>
#include <string>
#include <map>
#include <vector>
#include <errno.h>
#include <iostream>
#include <chrono>
#include <thread>

#include "statuses.hpp"
#include "player.hpp"

//=====================================GLOBALS============================================\\

int clientFd{};
const unsigned int localPort{59998};
const unsigned int remotePort{55555};
sockaddr_in bindAddr {
        .sin_family = AF_INET,
        .sin_port = htons(localPort)
};
addrinfo hints {
        .ai_flags= 0,
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
        .ai_protocol = IPPROTO_TCP};
addrinfo *resolved;

char* login;
char* password;

int joinedSessionID{};
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
    signal(SIGTSTP, sigHandler);

    bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    //startClient();

    //TODO: OBSLUZ BUTTONY! i reset wybierania register or login jak nie wyjdzie


    unsigned int connectionType = 0;
    const unsigned int signin = 1;
    const unsigned int signup = 2;

    int petla = 0;
    bool connectionValidated = false;
    while(!connectionValidated) {

        petla++;
        std::cout << "Connection number: " << petla << std::endl;

        // setLogin setPassword set type of Connection
        // TODO: GUI!!!!!!!!1
        //  while(register or signup not clicked){
        //      wait;
        //  }
        //  typeOfConnection = daj wartość signin albo signup
        //TODO: ustaw login hasło, tj załaduj itp
        // MA BYC DODANE SPRAWDZANIE ZNAKOW W LOGINIE I HASLE I ZEBY BYLY CALOSCIOWO DO 100 ZNAKOW

        login = "test_user";
        password = "test_pass\0";
        connectionType = 1; //TODO: zmien to na wybor

        do
        {
            std::cout << '\n' << "Press a key to continue..."; //TODO: na razie zamiast gui, później usuń
        } while (std::cin.get() != '\n');

        startClient();
        startConnection();

        char buffer [10];
        int ret = snprintf(buffer, sizeof(buffer), "%d", connectionType);
        char * num_string = buffer;
        std::cout <<"num string: "<< num_string << "size:" << sizeof(num_string) << std::endl;

        writeData(clientFd, num_string, sizeof(buffer)); //wysyła czy rejestracja czy co login

        char msg[100];
        memset(msg, 0, sizeof(msg));
        strcpy(msg, login);
        strcat(msg, "-");
        strcat(msg, password); //Konkatenacja log haslo
        std::cout << "Write no: " << petla << std::endl;
        writeData(clientFd, msg, sizeof(msg)); //wyslij dane użytkownika
        memset(msg, 0, sizeof(msg)); //odczytaj czy autoryzacja się powiodła
        std::cout << "Read no: " << petla << std::endl;
        auto x = readData(clientFd, msg, sizeof(msg));
        //Jeśli nie powiodła się autoryzacja spróbuj połączyć sie od nowa.
        if (strncmp(msg, "AUTH-FAIL", 9) == 0) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            closeClient();
            if (connectionType == signin){
                printf("Blad logowania sprobuj ponownie!\n");
            } else if (connectionType == signup){
                printf("Blad rejestracji sprobuj ponownie!\n");
            }
        } else if (strncmp(msg, "AUTH-OK", 7) == 0) {
            if (connectionType == signin){
                printf("LOGIN SUKCES!\n");
            } else if (connectionType == signup){
                printf("REJESTRACJA SUKCES!\n");
            }
            connectionValidated = true;
        }
    }


    //TODO: POŁĄCZ SIE Z DANĄ SESJA
    std::map<int, std::vector<std::string>> playerSessions;
    std::vector<std::string> players;

    bool chosenSession = false;
    while(!chosenSession){

        //TODO: w jakiej pętli ma działać poniższy kod i odczytywanie/wybór sesji

        char msg[1024]; //TODO: WIEKSZY ROZMIAR BUFORA???
        readData(clientFd, msg, sizeof(msg)); //FIXME: MUTTTEX MOZE BO BLOKADA PRZY WATKU

        if(msg[0] == '\0'){
            printf("No sessions available.\n");
        }
        else {
            printf("Sessions found.\n");
            playerSessions.clear();
            players.clear();
            char* s;
            s = strtok(msg,":"); // TODO: jak nie zadziała to daj ze delimiter ma wsystkie znaki
            if (s == nullptr){ //TODO: czy to sprawdzać wogóle
                //error;
                printf("strtok error.\n");
            }
            long int numSessions = strtol(s, nullptr, 10);
            for( int i =0; i < numSessions; i++ ) {
                s = strtok(msg, "-");
                long int sessionID = strtol(s, nullptr, 10);
                s = strtok(msg, ",");
                long int numPlayers = strtol(s, nullptr, 10);
                for (int j = 0; j < numPlayers; j++) {
                    s = strtok(msg, ",;");
                    players.push_back(std::string(s));
                }
                playerSessions.insert(std::pair<int, std::vector<std::string>>(sessionID, players));
            }
        }
        // raczej nie potrzebne dalem w serwerze ze co 1 sekunde wysyla to ten sie poprostu zablokuje az cos przeczyta std::this_thread::sleep_for(std::chrono::seconds(1));

        //TODO: STWORZYC PRZYCISK DOŁĄCZ DO SESJI; jak nie wybrano z listy to nowa, a jak wybrano to przechowaj ID i wyślij
        // chosenSession = sessionButton.Clicked(); COS TEGO TYPU, COS CO ZMIENI TA ZMIENNA I ZAKONCZY PETLE
        chosenSession = true;
    }


    //FIXME: BRAK TWORZONEJ NAZWY SERWERA

    //TODO:
    // DWIE OPCJE STWORZ I DOŁĄCZ
    // przetwórz i wyświetl dane sesji roznych one maja ID
    // na podstawie tego co klikniesz daj znac ktora wybierasz
    // wyslij ID SESJI do ktorej chcesz dołączyć
    // czekaj na odpowiedz do ktorej sesji dołaczyłes, i czy, jesli sukces to break


    int sessionStatus = 1;      //TODO: PRZEKAZ WYBRANE ID PRZEZ GUI jak stworz to 0 jak jakas sesja to id wyciagnij jej
    char* sessionResponse;
    char buffer [128];
    int ret = snprintf(buffer, sizeof(buffer), "%d", sessionStatus);
    sessionResponse = buffer;
    writeData(clientFd, sessionResponse, sizeof(buffer));  //WYSYŁA DANE KTÓRA SESJA WYBRANA
    readData(clientFd, sessionResponse , sizeof(sessionResponse)); //ODCZYTAJ ID NOWEJ SESJI, LUB POTWIERDZENIE ID ISTNIEJACEJ

    joinedSessionID =  (int) strtol(sessionResponse, NULL, 10);


    // PO DOŁĄCZENIU DO SESJI JESZCZE MA WIDZIEĆ PANEL I MÓC WYBRAC ROZPOCZECIE GRY


    //TODO: SPRAWDZANIE CZY SESJA ZYJE??!!!!
    //TODO:JAK OK TO WJEDZ W PETLE GRY

    while(true){


            //send jesli nie wyslal ile mial to od nopwa
            //read clientFd
            //write serverFd
       break;
    }

    //TODO: HANDLING
    closeClient();

    return 0;
}


//=======================================FUNC DEC=========================================\\


void startClient(void){
    if((clientFd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
        perror("Failed to create socket.\n");
        exit(SOCKET_CREATE);
    }
    struct linger sl;
    sl.l_onoff = 1;		/* non-zero value enables linger option in kernel */
    sl.l_linger = 0;	/* timeout interval in seconds */
    setsockopt(clientFd, SOL_SOCKET, SO_LINGER, &sl, sizeof(sl));  //TODO: NOWE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if( bind(clientFd, (sockaddr*)&bindAddr, sizeof(bindAddr)) < 0 ){
        perror("Failed to bind the socket.\n");
        exit(SOCKET_BIND);
    }
}

void closeClient(void){
    shutdown(clientFd, SHUT_RDWR);
    close(clientFd);
}


void startConnection(void){ //TODO: MOZE POLACZ ZE STARETEM ALE ZOBACZYMY
    int err = getaddrinfo("127.0.0.1", "55555", &hints, &resolved);
    if (err || !resolved){
        perror("Resolving address failed!\n");
        exit(GETADDRINFO_ERROR);
    }
    if ( connect(clientFd, resolved->ai_addr, resolved->ai_addrlen) < 0){
        printf("TU\n");
        perror("Failed to connect.\n");
        exit(SOCKET_CONNECT);
    }
    freeaddrinfo(resolved);
}


void sigHandler(int signal){
    closeClient();
}


ssize_t readData(int fd, char * buffer, ssize_t buffsize){
    auto ret = read(fd, buffer, buffsize);
    std::cout << "Read ret: " << ret << std::endl;
    if(ret==-1) {
        // printf("Oh dear, something went wrong with read()! %s\n", strerror(errno));
        perror("read failed on descriptor %d\n");
    }
    return ret;
}

void writeData(int fd, char * buffer, ssize_t count){
    auto ret = write(fd, buffer, count);
    std::cout << "Write ret: " << ret << std::endl;
    if(ret==-1) perror("write failed on descriptor %d\n");
    if(ret!=count) perror("wrote less than requested to descriptor %d (%ld/%ld)\n");
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