/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lshail <lshail@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 16:31:02 by asabri            #+#    #+#             */
/*   Updated: 2024/03/02 15:31:03 by lshail           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/server.hpp"
void Server::_KICK(std::vector<std::string> cmd, int fd)
{
    std::string Channels;
    std::string Clients;
    Client client;

    client = this->clients[fd];
    if (cmd.size() < 2)
    {
        std::string msg = messageGenerator(Server::getHostName(), ERR_NEEDMOREPARAMS, client.getNick() + " " + "KICK", "not enough parameters");
        send(fd, msg.c_str(), strlen(msg.c_str()), 0);
        throw std::invalid_argument("not enough parameters");
    }
    Channels = cmd[0];
    Client::toLower(Channels);
    cmd.erase(cmd.begin());
    if (this->channels.find(Channels) == this->channels.end() || Channels[0] != '#')
    {
        std::string msg = messageGenerator(Server::getHostName(), ERR_NOSUCHCHANNEL, client.getNick() + " " + Channels, "this channel does not exist");
        send(fd, msg.c_str(), strlen(msg.c_str()), 0);
        throw std::invalid_argument("this channel does not exist");
    }
    if (std::find(this->channels[Channels].operators.begin(), this->channels[Channels].operators.end(), fd) == this->channels[Channels].operators.end())
    {
        std::string msg = messageGenerator(Server::getHostName(), ERR_CHANOPRIVSNEEDED, client.getNick() + " " + Channels, "you are not an operator");
        send(fd, msg.c_str(), strlen(msg.c_str()), 0);
        throw std::invalid_argument("the client is not an operator");
    }
    Clients = cmd[0];
    cmd.erase(cmd.begin());
    client = this->channels[Channels].__getClients()[fd];
    std::map<int, Client> cls = this->channels[Channels].__getClients();

    std::string msg;
    for (std::map<int, Client>::iterator it = cls.begin(); it != cls.end(); it++)
    {
        if (cls[it->first].getNick() == Clients)
        {
            std::string ip = cls[fd].client_ip;
            if (!cmd.empty())
            {
                cmd[0].erase(remove(cmd[0].begin(), cmd[0].end(), ':'), cmd[0].end());
                msg = messageGenerator(client.getNick() + "!~@" + client.client_ip, "KICK", Channels + " " + Clients + " " + client.getNick(), cmd[0]);
            }
            else
                msg = messageGenerator(client.getNick() + "!~@" + client.client_ip, "KICK", Channels + " " + Clients, client.getNick());
            broadcast(msg, cls);
            if (std::find(this->channels[Channels].operators.begin(), this->channels[Channels].operators.end(), getfd(Clients, Channels)) != this->channels[Channels].operators.end())
                this->channels[Channels].operators.erase(std::find(this->channels[Channels].operators.begin(), this->channels[Channels].operators.end(), getfd(Clients, Channels)));
            this->channels[Channels].__getClients().erase(it);
            throw std::invalid_argument("the client has been kicked");
        }
    }
    msg = messageGenerator(Server::getHostName(), ERR_USERNOTINCHANNEL, client.getNick() + " " + Clients + " " + Channels, "They aren't on that channel");
    send(fd, msg.c_str(), strlen(msg.c_str()), 0);
    throw std::invalid_argument("the client does not exist");
}