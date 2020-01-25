
#include "player.hpp"

const std::string &Player::getNick() const {
    return nick;
}

void Player::setNick(const std::string &nick) {
    Player::nick = nick;
}

const std::string &Player::getPassword() const {
    return password;
}

void Player::setPassword(const std::string &password) {
    Player::password = password;
}
