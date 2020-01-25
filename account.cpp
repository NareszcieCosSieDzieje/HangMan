//
// Created by pawel on 22.01.2020.
//

#include "account.hpp"

const std::string &Account::getNick() const {
    return nick;
}

void Account::setNick(const std::string &nick) {
    Account::nick = nick;
}

const std::string &Account::getPassword() const {
    return password;
}

void Account::setPassword(const std::string &password) {
    Account::password = password;
}

Account::Account(const std::string &nick, const std::string &password) : nick(nick), password(password) {}
