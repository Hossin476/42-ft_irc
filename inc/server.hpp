/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lshail <lshail@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/02 15:26:11 by lshail            #+#    #+#             */
/*   Updated: 2024/03/02 15:33:33 by lshail           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <signal.h>
#include <dirent.h>
#include "../inc/Client.hpp"
#include "../inc/Channel.hpp"

#define BUFFER_SIZE 512
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define CL "\033[0m"


class Channel;
class Client;
class Server 
{
    private:
        void    _KICK(std::vector<std::string> cmd,int fd);
        void    _JOIN(std::vector<std::string> cmd,int fd);
        void    _INVITE(std::vector<std::string> cmd,int fd);
        void    _TOPIC(std::vector<std::string> cmd,int fd);
        void    _MODE(std::vector<std::string> cmd,int fd);
        void    _PART(std::vector<std::string> cmd,int fd);
        void    _QUIT(std::vector<std::string> cmd, int fd);
        void    _Error(std::vector<std::string> cmd,int fd);
        int port;
        std::string password;
        int server_fd;
        int client_fd;
        std::string msg;
    public:
        bool is_pass;
        std::vector<struct pollfd> pollfds;
        std::map<int, Client> clients;
        std::map<std::string,Channel> channels;
        Server();
        Server(int port, std::string password);
        ~Server();
        void main_loop();
        void create_server();
        void accept_connection();
        void authenticate(std::string buffer, int fd);
        void parseComand(std::string buffer, int fd);
        void execute(std::string cmd,int fd);
        void broadcast(std::string msg, std::map<int,Client>& cl);
        void _I(std::string Channel,int fd,std::vector<std::string>& param,bool sign);
        void _T(std::string Channel,int fd,std::vector<std::string>& param,bool sign);
        void _K(std::string Channel,int fd,std::vector<std::string>& param,bool sign);
        void _O(std::string Channel,int fd,std::vector<std::string>& param,bool sign);
        void _L(std::string Channel,int fd,std::vector<std::string>& param,bool sign);
        int getfd(std::string nick,std::string channel);
        int getfdInServ(std::string nick);
        void checkMode(std::vector<std::string> cmd, std::string channel, int fd);
        void erase_client(int fd);
        void clear_clients();
        void run_bot(int fd);
        static std::string& getHostName();
        std::string  messageGenerator(std::string prefix, std::string command, 
        std::string params,std::string trail);
        void removePollFd(int fd);
};

bool check_port(std::string port);

#endif