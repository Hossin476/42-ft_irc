/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asabri <asabri@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 19:43:43 by asabri            #+#    #+#             */
/*   Updated: 2024/03/01 23:46:27 by asabri           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/server.hpp"
#include <stdlib.h>

std::string  Server::messageGenerator(std::string prefix, std::string command, std::string params,std::string trail)
{
	std::string message;
	message = ":" + prefix + " " + command + " " + params + " " + ":" + trail + "\r\n";
	return message;
}

void Server::_Error(std::vector<std::string> cmd, int fd)
{
    (void)cmd;
    Client client;
    client = this->clients[fd];
    std::string msg = messageGenerator(getHostName(),ERR_UNKNOWNCOMMAND,client.getNick() + " " + cmd[0],"Unknown command");
    send (fd,msg.c_str(),strlen(msg.c_str()),0);
}



int Server::getfdInServ(std::string nick){
    
    std::map<int,Client>::iterator it = this->clients.begin();
    while (it != this->clients.end())
    {
        if (it->second.getNick() == nick)
            return it->first;
        it++;
    }
    return -1;
}
int Server::getfd(std::string nick,std::string channel){
    
    std::map<int,Client>::iterator it = this->channels[channel].__getClients().begin();
    while (it != this->channels[channel].__getClients().end())
    {
        if (it->second.getNick() == nick)
            return it->first;
        it++;
    }
    return -1;
}

void Server::broadcast(std::string msg, std::map<int,Client>& cl)
{
    std::map<int,Client>::iterator it = cl.begin();
    while (it != cl.end())
    {
        send(it->first,msg.c_str(),strlen(msg.c_str()),0);
        it++;
    }
}

void Server::_PART(std::vector<std::string> cmd, int fd)
{
    std::string Channels;
    std::string Channel;
    Client client;
    client = this->clients[fd];
    if (cmd.size() < 2){
        std::string msg = messageGenerator(getHostName(),ERR_NEEDMOREPARAMS,client.getNick() + " " + "PART","not enough parameters");
        send (fd,msg.c_str(),strlen(msg.c_str()),0);// not enough parameters
        throw std::invalid_argument("not enough parameters");
    }
    Channel = cmd[0];
    Client::toLower(Channel);
    if (this->channels.find(Channel) == this->channels.end() || Channel[0] != '#'){
        std::string msg = messageGenerator(getHostName(),ERR_NOSUCHCHANNEL,client.getNick() + " " + Channel,"this channel does not exist");
        send (fd,msg.c_str(),strlen(msg.c_str()),0);
        throw std::invalid_argument("this channel does not exist");
    }
    else if (this->channels[Channel].__getClients().find(fd) == this->channels[Channel].__getClients().end()){
        std::string msg = messageGenerator(getHostName(),ERR_NOTONCHANNEL,client.getNick() + " " + Channel,"you are not on this channel");
        send (fd,msg.c_str(),strlen(msg.c_str()),0);// you are not on this channel
        throw std::invalid_argument("you are not on this channel");
    }
    else {
        client = this->channels[Channel].__getClients()[fd];
        std::string ip = this->channels[Channel].__getClients()[fd].client_ip;
        cmd[1].erase(remove(cmd[1].begin(), cmd[1].end(), ':'), cmd[1].end());
        std::string msg = messageGenerator(client.getNick() + "!~@" + client.client_ip,"PART",Channel,cmd[1]);
        broadcast(msg,this->channels[Channel].__getClients());
        this->channels[Channel].__getClients().erase(fd);
        if (this->channels[Channel].__getClients().size() == 0)
            this->channels.erase(Channel);
        throw std::invalid_argument("you are exited from this channel");
    }
}
void tokenize(std::string const &str, std::vector<std::string> &out)
{
    std::stringstream ss(str);
    std::string s;
    std::string tmp;
    while (ss >> s){
        if (s[0] == ':'){
            tmp = s + " ";
            while (ss >> s){
                tmp += s + " ";
            }
            out.push_back(tmp);
            break;
        }
        out.push_back(s);
    }
}

void Server::execute(std::string cmd, int fd)
{
    if (cmd.find("\n") == std::string::npos)
        return;
    std::vector<std::string> tokens;
    void (Server::*func[])(std::vector<std::string>, int) = {&Server::_Error, &Server::_KICK,
            &Server::_INVITE, &Server::_TOPIC, &Server::_MODE,&Server::_PART,&Server::_QUIT};
    tokenize(cmd, tokens);
    if (tokens.empty())
        return;
    int i = (tokens[0] == "KICK") * 1  +
     (tokens[0] == "INVITE") * 2 + 
    (tokens[0] == "TOPIC") * 3 + 
    (tokens[0] == "MODE") * 4 + 
    (tokens[0] == "PART") * 5 + 
    (tokens[0] == "PONG") * 8 + 
    (tokens[0] == "QUIT") * 6 ;
    if (i != 0 && i != 8)
        tokens.erase(tokens.begin());
    if (i != 8)
        (void)(this->*func[i])(tokens, fd);
    this->clients[fd].msg.clear();
}


