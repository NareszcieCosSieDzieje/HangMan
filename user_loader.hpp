
#ifndef SIECI_WISIELEC_USER_LOADER_HPP
#define SIECI_WISIELEC_USER_LOADER_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


//TODO: zczytywanie tego zapisywanie,
// Zapis -> przejedz az znajdziesz srednik i appenduj przed
// max login i max password length


const std::string delims("{,;:}\n"); //\n\t
inline bool isDelim(char c) {
    for (int i = 0; i < delims.size(); ++i)
        if (delims[i] == c)
            return true;
    return false;
}


bool searchForUserData(std::string login, std::string password) {

// wczytaj po loginie i hasle, jesli istnieja to przechowaj w pamieci jesli znajdz to koniec
//czy mozna dodawac nowych uzytkownikow nie [rzez serwer???

    std::ifstream userFile;
    userFile.open ("user_data.txt", std::ios::in);
    if (userFile.is_open()) {

        //TODO: if empty error??
        std::string line;
        while (std::getline(userFile, line)) {
            std::stringstream stringStream(line);
            int i = 0;
            int found = 0;
            stringStream.seekg(1);
            while (stringStream) {
                std::string word;
                char c;
                //word.clear();
                // Read word
                while (!isDelim((c = stringStream.get()))) {
                    word.push_back(c);
                }
                if (c != EOF) {
                    stringStream.unget();
                }
                if (i%2 == 0) {
                    std::cout << "iteracja: " << i << std::endl;
                    std::cout << "Login: " << login << std::endl;
                    std::cout << "Login word: " << word << std::endl;
                    if (login == word){
                        found++;
                    }
                } else {
                    std::cout << "Password: " << password << std::endl;
                    std::cout << "Password word: " << word << std::endl;
                    if(password == word){
                        found++;
                    }
                    if (found < 2){
                        found = 0;
                    } else {
                        userFile.close();
                        return true;
                    }
                }
                // wordVector.push_back(word);
                // Read delims
                while (isDelim((c = stringStream.get())));
                if (c != EOF) {
                    stringStream.unget();
                }
                i++;
            }
        }
        userFile.close();
    } else {
        std::cout << "Nie mozna otworzyc pliku!" << std::endl;
    }
    return false;
}




#endif //SIECI_WISIELEC_USER_LOADER_HPP
