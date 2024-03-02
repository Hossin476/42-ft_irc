/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asabri <asabri@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/06 14:09:34 by asabri            #+#    #+#             */
/*   Updated: 2024/03/01 22:45:32 by asabri           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Channel.hpp"


Channel::Channel()
{
    inviteOnly = false;
    iskeySet = false;
    isTopicSet = false;
    limit = INT_MAX;
}

void Channel::setName(std::string name){
    this->name = name;
}
std::string Channel::getName(){
    return this->name;
}
void Channel::setPassword(std::string password){
    this->password = password;
}
std::string Channel::getPassword(){
    return this->password;
}

bool Channel::__getInviteOnly(){
    return this->inviteOnly;
}

std::map<int,Client>& Channel::__getClients(){
    return this->clients;    
}
std::vector<std::string>& Channel::__getInviteList(){
    return this->inviteList;
}

std::string Channel::__getTopic()
{
    return this->topic;
}



bool Channel::if_it_client(int fd)
{
    if (this->clients.find(fd) != this->clients.end())
        return true;
    return false;
}

void Channel::addClient(Client &client)
{
    this->clients[client.client_fd] = client;
}

Channel& Channel::operator=(const Channel &channel)
{
    this->name = channel.name;
    this->password = channel.password;
    this->clients = channel.clients;
    this->inviteOnly = channel.inviteOnly;
    this->inviteList = channel.inviteList;
    this->topic = channel.topic;
    return *this;
}




void Channel::__setTopic(std::string topic)
{
    this->topic = topic;
}
Channel::~Channel()
{
}

void Channel::__setInviteOnly(bool inviteOnly)
{
    this->inviteOnly = inviteOnly;
}
void Channel::__setPassword(std::string password)
{
    this->password = password;
}

int Channel::__getLimit()
{
    return this->limit;
}
void Channel::__setLimit(int limit)
{
    this->limit = limit;
}