/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   auth.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykhourba <ykhourba@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 17:50:54 by ykhourba          #+#    #+#             */
/*   Updated: 2024/03/01 19:54:24 by ykhourba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/server.hpp"

void Server::authenticate(std::string buffer,int fd)
{
    std::string cmd;
    std::string key;
    std::stringstream ss;
   int pos;

    
    this->clients[fd].msg.append(buffer);
    if(this->clients[fd].msg.find("\n") == std::string::npos)
        return;
    ss << this->clients[fd].msg;
    this->clients[fd].msg.clear();
    ss >> cmd;
    ss >> key;

    if (cmd == "PASS" && !this->clients[fd].is_pass)
    {
        pos  = buffer.find("\r\n") != std::string::npos ? 2 : 1;
        key = buffer.substr(buffer.find(key));
        key.erase(key.size() - pos, pos);
        if (key != this->password)
        {
            std::string msg = Client::messageGenerator(getHostName(),ERR_PASSWDMISMATCH,"PASS" ,"invalid password");
            send(fd, msg.c_str(), msg.size(), 0);
            return;
        }
        this->clients[fd].is_pass= true;
    }
    if (cmd == "NICK" && !this->clients[fd].isKeySet)
    {

        if (!this->clients[fd].checkRegNick(key))
        {
            std::string msg = Client::messageGenerator(getHostName(),ERR_NICKNAMEINUSE,"NICK" ,"invalid nickname");
            send(fd, msg.c_str(), msg.size(), 0);
            return;
        }
        std::map<int, Client>::iterator it = this->clients.begin();
        while (it != this->clients.end())
        {
            if (it->second.getNick() == key)
            {
                std::string msg = Client::messageGenerator(getHostName(),ERR_ERRONEUSNICKNAME,"NICK" ,"already in use");
                send(fd, msg.c_str(), msg.size(), 0);
                return;
            }
            it++;
        }
        this->clients[fd].setNick(key);
        this->clients[fd].isKeySet = true;
    }
    if (cmd == "USER" && !this->clients[fd].isUsrnameSet)
    {
        if (!this->clients[fd].checkRegNick(key))
        {
            std::string msg = Client::messageGenerator(getHostName(),ERR_INVALIDUSERNAME,"USER" ,"invalid username");
            send(fd, msg.c_str(), msg.size(), 0);
            return;
        }
        this->clients[fd].setUsername(key);
        this->clients[fd].isUsrnameSet = true;
    }
    if (this->clients[fd].isUsrnameSet && this->clients[fd].isKeySet && this->clients[fd].is_pass)
    {
        this->clients[fd].isconnected = true;
        std::string msg = Client::messageGenerator(getHostName(),RPL_WELCOME,this->clients[fd].getNick() ,"authenticated successfully");
        send(fd, msg.c_str(), msg.size(), 0);
        msg = Client::messageGenerator(getHostName(),RPL_WELCOME,this->clients[fd].getNick() ,"Welcome to the server " + this->clients[fd].getNick());
        send(fd, msg.c_str(), msg.size(), 0);
    }
    return;
}