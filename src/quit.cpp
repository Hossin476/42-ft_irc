/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asabri <asabri@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 10:50:57 by asabri            #+#    #+#             */
/*   Updated: 2024/02/27 17:49:42 by asabri           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/server.hpp"

void Server::removePollFd(int fd)
{
    std::vector<struct pollfd>::iterator it = this->pollfds.begin();
    while (it != this->pollfds.end())
    {
        if (it->fd == fd)
        {
            this->pollfds.erase(it);
            break;
        }
        it++;
    }
}  

void Server::_QUIT(std::vector<std::string> cmd, int fd)
{
    (void)cmd;
    Client client;
    std::vector<std::string> channelsthatleft;
    client = this->clients[fd];
    this->clients.erase(fd);
    std::map<std::string,Channel>::iterator it = this->channels.begin();
    while (it != this->channels.end())
    {
        if (it->second.__getClients().find(fd) != it->second.__getClients().end())
        {
            if (std::find(it->second.operators.begin(), it->second.operators.end(), fd) != it->second.operators.end())
                it->second.operators.erase(std::find(it->second.operators.begin(), it->second.operators.end(), fd));          
            if (std::find(it->second.__getInviteList().begin(), it->second.__getInviteList().end(), client.getNick()) 
                    != it->second.__getInviteList().end())
                it->second.__getInviteList().erase(std::find(it->second.__getInviteList().begin(), it->second.__getInviteList().end(), client.getNick()));
            it->second.__getClients().erase(fd);
            std::string msg = messageGenerator(client.getNick()+"!~" + client.getUserName() + "@" + client.client_ip, "QUIT", "", "Quit");
            broadcast(msg, it->second.__getClients());     
            channelsthatleft.push_back(it->first);
        }
        it++;
    }
    removePollFd(fd);
    for (size_t i = 0; i < channelsthatleft.size(); i++)
    {
        if (this->channels[channelsthatleft[i]].__getClients().size() == 0)
            this->channels.erase(channelsthatleft[i]);
    }
    close(fd);
    
}