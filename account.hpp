//
// Created by pawel on 22.01.2020.
//

#ifndef WISIELEC_ACCOUNT_HPP
#define WISIELEC_ACCOUNT_HPP


#include <stdio.h>
#include <string>

class Account {
private:
    std::string nick;
    std::string password;
public:
    Account();
    ~Account();

    const std::string &getNick() const;

    void setNick(const std::string &nick);

    const std::string &getPassword() const;

    void setPassword(const std::string &password);

    Account(const std::string &nick, const std::string &password);
};


#endif //WISIELEC_ACCOUNT_HPP
