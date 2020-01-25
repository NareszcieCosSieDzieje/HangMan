
#ifndef WISIELEC_PLAYER_HPP
#define WISIELEC_PLAYER_HPP

#include "account.hpp"

class Player {
private:
    std::string nick;
    std::string password;

public:
    const std::string &getNick() const;

    void setNick(const std::string &nick);

    const std::string &getPassword() const;

    void setPassword(const std::string &password);

};


#endif //WISIELEC_PLAYER_HPP
