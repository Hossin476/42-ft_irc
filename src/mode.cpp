/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asabri <asabri@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 16:34:04 by asabri            #+#    #+#             */
/*   Updated: 2024/03/01 21:41:52 by asabri           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/server.hpp"
#include <cstring>

void Server::_K(std::string Channel, int fd,std::vector<std::string>& param,bool sign)
{
    Client client;
    std::string nick;
    std::string ip;

    client = this->channels[Channel].__getClients()[fd];
    nick = client.getNick();
    ip = this->channels[Channel].__getClients()[fd].client_ip;
    if (param.empty())
    {
        std::string msg = messageGenerator(Server::getHostName(),ERR_NEEDMOREPARAMS,client.getNick() + " " + "MODE","k Not enough parameters");
        send (fd,msg.c_str(),strlen(msg.c_str()),0);
        return;
    }
    if (sign == true){
        this->channels[Channel].__setPassword(param[0]);
        this->channels[Channel].iskeySet = true;
        msg = messageGenerator(client.getNick() + "!~@" + client.client_ip,"MODE",Channel + " +k",param[0]);
        broadcast(msg,this->channels[Channel].__getClients());
        param.erase(param.begin());
    }
    else if (sign == false){
        if (this->channels[Channel].getPassword() != param[0]){
            msg = messageGenerator(Server::getHostName(),ERR_KEYSET,client.getNick() + " " + Channel,"Channel key already set");
            send(fd,msg.c_str(),strlen(msg.c_str()),0);
            throw std::invalid_argument("wrong password");
        }
        this->channels[Channel].__setPassword("");
        this->channels[Channel].iskeySet = false;
        msg = messageGenerator(client.getNick() + "!~@" + client.client_ip,"MODE",Channel + " -k",param[0]);
        broadcast(msg,this->channels[Channel].__getClients());
        param.erase(param.begin());
    }
}

void Server::_O(std::string Channel, int fd,std::vector<std::string>& param,bool sign)
{
    Client client;
    std::string msg;

    client = this->channels[Channel].__getClients()[fd];
    if (param.empty())
    {
        std::string msg = messageGenerator(Server::getHostName(),ERR_NEEDMOREPARAMS,client.getNick() + " " + "MODE","o Not enough parameters");
        send (fd,msg.c_str(),strlen(msg.c_str()),0);
        return ;
    }
    if (getfd(param[0],Channel) == -1){
        msg = messageGenerator(Server::getHostName(),ERR_NOSUCHNICK,client.getNick() + " "+ param[0],"this client does not exist in this channel");
        send(fd,msg.c_str(),strlen(msg.c_str()),0);
        throw std::invalid_argument("the client does not exist");
    }
    if (sign == true
            && std::find(this->channels[Channel].operators.begin()
                ,this->channels[Channel].operators.end(), getfd(param[0],Channel))
                    == this->channels[Channel].operators.end()){
        
        this->channels[Channel].operators.push_back(getfd(param[0],Channel));
        msg = messageGenerator(client.getNick() + "!~@" + client.client_ip,"MODE",Channel + " +o",param[0]);
        broadcast(msg,this->channels[Channel].__getClients());
        param.erase(param.begin());
    }
    else if (sign == false 
                        && std::find(this->channels[Channel].operators.begin()
                            ,this->channels[Channel].operators.end(), getfd(param[0],Channel))
                                    != this->channels[Channel].operators.end()){
        this->channels[Channel].operators.erase(std::find(this->channels[Channel].operators.begin(), this->channels[Channel].operators.end(), getfd(param[0],Channel)));
        msg = messageGenerator(client.getNick() + "!~@" + client.client_ip,"MODE",Channel + " -o",param[0]);
        broadcast(msg,this->channels[Channel].__getClients());
        param.erase(param.begin());
    }
}

void Server::_L(std::string Channel,int fd,std::vector<std::string>& param,bool sign)
{
    Client client;
    std::string msg;
    std::string ip;

    ip = this->channels[Channel].__getClients()[fd].client_ip;
    client = this->channels[Channel].__getClients()[fd];
    if (sign == true){
        if (param.empty())
        {
            std::string msg = messageGenerator(Server::getHostName(),ERR_NEEDMOREPARAMS,client.getNick() + " " + "MODE","l Not enough parameters");
            send (fd,msg.c_str(),strlen(msg.c_str()),0);
            return;
        }
        if (atoi(param[0].c_str()) <= 0 || atoi(param[0].c_str()) > INT_MAX)
            return;
        this->channels[Channel].__setLimit(atoi(param[0].c_str()));
        msg = messageGenerator(client.getNick() + "!~@" + client.client_ip,"MODE",Channel + " +l",param[0]);
        broadcast(msg,this->channels[Channel].__getClients()); 
        param.erase(param.begin());
    }
    else if (sign == false){
        this->channels[Channel].__setLimit(INT_MAX);
        msg = messageGenerator(client.getNick() + "!~@" + client.client_ip,"MODE",Channel + " -l","");
        broadcast(msg,this->channels[Channel].__getClients());
    }
}

void Server::_I(std::string Channel, int fd, std::vector<std::string>& param,bool sign)
{
    (void)param;
    Client client;
    std::string msg;
    std::string ip;

    client = this->channels[Channel].__getClients()[fd];
    ip = this->channels[Channel].__getClients()[fd].client_ip;
    if (sign == true && this->channels[Channel].__getInviteOnly() == false){
        this->channels[Channel].__setInviteOnly(true);
        msg = messageGenerator(client.getNick() + "!~@" + client.client_ip,"MODE",Channel,"+i");
         broadcast(msg,this->channels[Channel].__getClients());
    }
    else if (sign == false && this->channels[Channel].__getInviteOnly() == true){
        this->channels[Channel].__setInviteOnly(false);
        msg = messageGenerator(client.getNick() + "!~@" + client.client_ip,"MODE",Channel,"-i");
        broadcast(msg,this->channels[Channel].__getClients());
    }
}

void Server::_T(std::string Channel, int fd, std::vector<std::string>& param,bool sign)
{
    std::string msg;
    Client client;

    (void)param;
    client = this->channels[Channel].__getClients()[fd];
    std::string ip = this->channels[Channel].__getClients()[fd].client_ip;
    if (sign == true && this->channels[Channel].isTopicSet == false){
        this->channels[Channel].isTopicSet = true;
        msg = messageGenerator(client.getNick() + "!~@" + client.client_ip,"MODE",Channel + "","+t");
        broadcast(msg,this->channels[Channel].__getClients());
    }
    else if (sign == false && this->channels[Channel].isTopicSet == true){
        this->channels[Channel].isTopicSet = false;
        msg = messageGenerator(client.getNick() + "!~@" + client.client_ip,"MODE",Channel + "","-t");
        broadcast(msg,this->channels[Channel].__getClients());
    }
}
void Server::checkMode(std::vector<std::string> cmd, std::string channel, int fd)
{
    std::string modeKeys = cmd[0];
    Client client;
    bool sign = true;
    cmd.erase(cmd.begin());
    client = this->clients[fd];
    for (size_t k = 0;k < modeKeys.length(); k++)
    {
        void (Server::*func[])(std::string, int, std::vector<std::string>&,bool) = 
            {&Server::_I, &Server::_T, &Server::_K, &Server::_O, &Server::_L};
        if (strchr("-+",modeKeys[k])){
            sign = (modeKeys[k] == '+') ? true : false;
            continue;
        }
        int j = (modeKeys[k] == 'i') * 1
            + (modeKeys[k] == 't') * 2
            + (modeKeys[k] == 'k') * 3
            + (modeKeys[k] == 'o') * 4
            + (modeKeys[k] == 'l') * 5;
        if (j != 0)
            (void)(this->*func[j - 1])(channel,fd,cmd,sign);
        else
        {
            std::string msg = messageGenerator(Server::getHostName(),ERR_UNKNOWNMODE,client.getNick() + " " + modeKeys[k],"is unknown mode char to me");
            send(fd,msg.c_str(),strlen(msg.c_str()),0);
        }
    }
}
void Server::_MODE(std::vector<std::string> cmd, int fd)
{
    std::string Channel;
    Client client;
    if (cmd.size() < 1){
        client = this->clients[fd];
        std::string msg = messageGenerator(Server::getHostName(),ERR_NEEDMOREPARAMS,client.getNick() + " " + "MODE","Not enough parameters");
        send (fd,msg.c_str(),strlen(msg.c_str()),0);
        throw std::invalid_argument("not enough parameters");
    }
    Channel = cmd[0];
    Client::toLower(Channel);
    if (this->channels.find(Channel) == this->channels.end() || Channel[0] != '#'){
        std::string msg = messageGenerator(Server::getHostName(),ERR_NOSUCHCHANNEL,client.getNick() + " " + Channel,"this channel does not exist");
        send (fd,msg.c_str(),strlen(msg.c_str()),0);
        throw std::invalid_argument("this channel does not exist");
    }
    else if (this->channels[Channel].__getClients().find(fd) == this->channels[Channel].__getClients().end()){
        std::string msg = messageGenerator(Server::getHostName(),ERR_NOTONCHANNEL,client.getNick() + " " + Channel,"you are not on this channel");
        send (fd,msg.c_str(),strlen(msg.c_str()),0);
        throw std::invalid_argument("you are not on this channel");
    }
    else if (std::find(this->channels[Channel].operators.begin(), this->channels[Channel].operators.end(), fd) 
        == this->channels[Channel].operators.end()){
        std::string msg = messageGenerator(Server::getHostName(),ERR_CHANOPRIVSNEEDED,client.getNick() + " " + Channel,"you are not an operator");
        send(fd,msg.c_str(),strlen(msg.c_str()),0);
        throw std::invalid_argument("the client is not an operator");
    }
    cmd.erase(cmd.begin());
    if (!cmd.empty())
        checkMode(cmd,Channel,fd);
}