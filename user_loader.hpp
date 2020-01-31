
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

    std::cout << "searchForData: "<< std::endl;
    std::cout << "Login: " << login << std::endl;
    std::cout << "Haslo: " << password << std::endl;

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
                    if (login == word){
                        found++;
                    }
                } else {
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


void addUser(std::string user, std::string password){
    std::fstream userFile;
    userFile.open("user_data.txt",  std::fstream::in | std::fstream::out | std::fstream::ate ); // append instead of overwrite
    if (userFile.is_open()){
        long pos = userFile.tellp();
        userFile.seekp(pos-1);
        userFile << ",{" + user + ":" + password + "};";
        userFile.close();
        std::cout<<"Successfully appended file!" <<std::endl;
    } else {
        std::cout<<"Couldn't open file!" <<std::endl;
    }
}



#endif //SIECI_WISIELEC_USER_LOADER_HPP
