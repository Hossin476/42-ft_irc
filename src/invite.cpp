/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asabri <asabri@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 16:31:31 by asabri            #+#    #+#             */
/*   Updated: 2024/02/27 10:48:34 by asabri           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/server.hpp"

void Server::_INVITE(std::vector<std::string> cmd, int fd)
{
    std::string Channels;
    std::string client;
    Client cl;

    cl = this->clients[fd];
    if (cmd.size() < 2){
        std::string msg = messageGenerator(Server::getHostName(),ERR_NEEDMOREPARAMS,cl.getNick() + " " + "INVITE","not enough parameters");
        send (fd,msg.c_str(),strlen(msg.c_str()),0);// not enough parameters
        throw std::invalid_argument("not enough parameters");
    }
    client = cmd[0];
    Channels = cmd[1];
    Client::toLower(Channels);
    if (getfdInServ(client) == -1){
        std::string msg = messageGenerator(Server::getHostName(),ERR_NOSUCHNICK,cl.getNick() + " " + client,"this client does not exist");
        send (fd,msg.c_str(),strlen(msg.c_str()),0);// this client does not exist
        throw std::invalid_argument("this client does not exist");
    }
    else if (this->channels.find(Channels) == this->channels.end() || Channels[0] != '#'){
        std::string msg = messageGenerator(Server::getHostName(),ERR_NOSUCHCHANNEL,cl.getNick() + " " + Channels,"this channel does not exist");
        send (fd,msg.c_str(),strlen(msg.c_str()),0);
        throw std::invalid_argument("this channel does not exist");
    }
    else if (this->channels[Channels].__getClients().find(fd) == this->channels[Channels].__getClients().end()){
        std::string msg = messageGenerator(Server::getHostName(),ERR_NOTONCHANNEL,cl.getNick() + " " + Channels,"you are not on this channel");
        send (fd,msg.c_str(),strlen(msg.c_str()),0);// you are not on this channel
        throw std::invalid_argument("you are not on this channel");
    }
    else if (getfd(client,Channels) != -1){
        std::string msg = messageGenerator(Server::getHostName(),ERR_NOTONCHANNEL,cl.getNick() + " " + Channels,"you are already on this channel");
        send (fd,msg.c_str(),strlen(msg.c_str()),0);// this client does not exist
        throw std::invalid_argument("this client does not exist");
    }
    else if (std::find(this->channels[Channels].operators.begin(), this->channels[Channels].operators.end(), fd) == this->channels[Channels].operators.end()){
        std::string msg = messageGenerator(Server::getHostName(),ERR_CHANOPRIVSNEEDED,cl.getNick() + " " + Channels,"you are not an operator");
        send(fd,msg.c_str(),strlen(msg.c_str()),0);// the client is not an operator
        throw std::invalid_argument("the client is not an operator");
    }
    else 
    {
        cl = this->channels[Channels].__getClients()[fd];
        std::vector<std::string> inviteList = this->channels[Channels].__getInviteList();
        if (std::find(inviteList.begin(), inviteList.end(), client) == inviteList.end()){
            this->channels[Channels].__getInviteList().push_back(client);
            
            std::string ip = this->channels[Channels].__getClients()[fd].client_ip;
            std::string msg = messageGenerator(ip,RPL_INVITING,cl.getNick() + " " + client + " " + Channels,"");
            send(fd,msg.c_str(),strlen(msg.c_str()),0);
            
            msg = messageGenerator(cl.getNick() + "!~@" + cl.client_ip,"INVITE",client + " " + Channels,"");
            send(getfdInServ(client),msg.c_str(),strlen(msg.c_str()),0);
            throw std::invalid_argument("the client has been invited");
        }
    }
}