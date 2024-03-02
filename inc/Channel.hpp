/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asabri <asabri@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/06 14:10:41 by asabri            #+#    #+#             */
/*   Updated: 2024/02/27 11:53:20 by asabri           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// ****************** //
/* Class of channel   */
/*      CHANNEL       */
// ****************** //
#pragma once

#include "Client.hpp"
#include <iostream>
#include <map>
#include <vector>

class Client;
class Channel
{
    private:
        std::string name;
        std::string password;
        std::map<int,Client> clients;
        bool inviteOnly;
        std::vector<std::string> inviteList;
        std::string topic;
        int limit;
    public:
        Channel();
        ~Channel();
        std::vector<int> operators;
        bool iskeySet;
        bool isTopicSet;
        std::map<int,Client>& __getClients();
        std::vector<std::string>& __getInviteList();
        std::string __getTopic();
        void __setTopic(std::string topic);
        void setName(std::string name);
        void setPassword(std::string password);
        std::string getName();
        std::string getPassword();
        bool if_it_client(int fd);
        void addClient(Client &client);
        void __setLimit(int limit);
        int __getLimit();
        void __setInviteOnly(bool inviteOnly);
        bool __getInviteOnly();
        void __setPassword(std::string password);
        Channel& operator=(const Channel &channel);
};
