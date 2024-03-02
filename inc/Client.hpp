/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lshail <lshail@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/06 14:09:04 by asabri            #+#    #+#             */
/*   Updated: 2024/03/02 15:38:21 by lshail           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// ****************** //
/* Class of Client    */
/*      CLIENT        */
// ****************** //
#pragma once

#include <iostream>
#include <sys/socket.h>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <unistd.h>
#include "Channel.hpp"
#include "IrcReplies.hpp"
#include "server.hpp"
class Server;
class Channel;
class Client
{
    private:
        std::string nickname;
        std::string username;
        bool isOperator;
        std::string ipAddr;
    public:
        std::string client_ip;
        int client_fd;
        std::string msg;
        bool is_pass;
        bool isKeySet;
        bool isUsrnameSet;
        bool isconnected;
        static std::string&  toLower(std::string &str);
        std::string getNick();
        void setNick(std::string nickname);
        void setUsername(std::string username);
        std::string getUserName();
        bool checkRegNick(std::string nickname);
        void join(std::string buffer,std::map<std::string,Channel> &channels);
        void priveMessage(std::string buffer,std::map<std::string, Channel> &channels, std::map<int, Client> &clients);
        static std::string  messageGenerator(std::string prefix, std::string command, std::string params,std::string trail);
        bool checkChanReg(std::string channel);
        Client();
        Client(const Client &client);
        ~Client();
         Client &operator=(const Client &client);
};