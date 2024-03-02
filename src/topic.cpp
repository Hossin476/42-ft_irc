/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asabri <asabri@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 16:33:41 by asabri            #+#    #+#             */
/*   Updated: 2024/02/27 10:49:11 by asabri           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/server.hpp"

void Server::_TOPIC(std::vector<std::string> cmd, int fd)
{
    std::string Channel;
    std::string ip;
    std::string msg;
    Client client;

    client = this->clients[fd];
    if (cmd.size() < 1){
        std::string msg = messageGenerator(Server::getHostName(),ERR_NEEDMOREPARAMS,client.getNick() + " " + "TOPIC","Not enough parameters");
        send (fd,msg.c_str(),strlen(msg.c_str()),0);// not enough parameters
        throw std::invalid_argument("not enough parameters");
    }
    Channel = cmd[0];
    Client::toLower(Channel);
    if (this->channels.find(Channel) == this->channels.end() || Channel[0] != '#'){
        std::string msg = messageGenerator(Server::getHostName(),ERR_NOSUCHCHANNEL,client.getNick() + " " + Channel,"this channel does not exist");
        send (fd,msg.c_str(),strlen(msg.c_str()),0);// this channel does not exist
        throw std::invalid_argument("this channel does not exist");
    }
    else if (this->channels[Channel].__getClients().find(fd) == this->channels[Channel].__getClients().end()){
        std::string msg = messageGenerator(Server::getHostName(),ERR_NOTONCHANNEL,client.getNick() + " " + Channel,"you are not on this channel");
        send (fd,msg.c_str(),strlen(msg.c_str()),0);// you are not on this channel
        throw std::invalid_argument("you are not on this channel");
    }
    else if (cmd.size() == 1 && this->channels[Channel].__getTopic().empty()){
        client = this->channels[Channel].__getClients()[fd];
        std::string msg = messageGenerator(Server::getHostName(),RPL_NOTOPIC,client.getNick() + " " + Channel,"No topic is set");
        send(fd,msg.c_str(),strlen(msg.c_str()),0);// the topic is
        throw std::invalid_argument("No topic is set");
    }
    else if (cmd.size() == 1 && !this->channels[Channel].__getTopic().empty()){
        ip = this->channels[Channel].__getClients()[fd].client_ip;
        client = this->channels[Channel].__getClients()[fd];
        std::string msg = messageGenerator(Server::getHostName(),RPL_TOPIC,client.getNick() + " " + Channel,this->channels[Channel].__getTopic());
        send(fd,msg.c_str(),strlen(msg.c_str()),0);// the topic is
        throw std::invalid_argument("the topic is");
    }
    else if (this->channels[Channel].isTopicSet == true
            && std::find(this->channels[Channel].operators.begin(), this->channels[Channel].operators.end(), fd)
            == this->channels[Channel].operators.end()){
        client = this->channels[Channel].__getClients()[fd];
        std::string msg = messageGenerator(Server::getHostName(),ERR_CHANOPRIVSNEEDED,client.getNick() +" " +Channel,"You do not have access to change the topic on this channel");
        send(fd,msg.c_str(),strlen(msg.c_str()),0);// the client is not an operator
        throw std::invalid_argument("the client is not an operator");
    }
    else if (cmd.size() == 2){
        this->channels[Channel].__setTopic(cmd[1]);
        client = this->channels[Channel].__getClients()[fd];
        ip = this->channels[Channel].__getClients()[fd].client_ip;
        msg = messageGenerator(client.getNick() + "!~@" + client.client_ip,"TOPIC",Channel ,cmd[1]);
        broadcast(msg,this->channels[Channel].__getClients());//boadcast the topic
        //boadcast the topic
        throw std::invalid_argument("the topic has been set");
    }
}