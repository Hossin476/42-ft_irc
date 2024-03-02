/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lshail <lshail@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/02 15:27:47 by lshail            #+#    #+#             */
/*   Updated: 2024/03/02 15:33:07 by lshail           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/server.hpp"
#include "../inc/IrcReplies.hpp"

bool check_port(std::string port)
{
    for (size_t i = 0; i < port.size(); i++)
    {
        if (!std::isdigit(port[i]))
            return false;
    }
    std::stringstream ss(port);
    int portNum;
    ss >> portNum;
    if (portNum < 1024 || portNum > 65535)
        return false;
    return true;
}

Server::Server() : port(5000), password("pass"),is_pass(false)
{
}

Server::Server(int port, std::string password) : port(port), password(password)
{
}

Server::~Server()
{
}

std::string& Server::getHostName()
{
    std::string host;
    char hostname[1024];
    hostname[1023] = '\0';
    gethostname(hostname, 1023);
    return host = hostname;
}

void Server::create_server()
{
    struct sockaddr_in server_addr;
    struct pollfd fd;
    int opt = 1;
    std::memset(&fd, 0, sizeof(fd));
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
        throw std::runtime_error("socket error");
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
        throw std::runtime_error("setsockopt error");
    if (fcntl(server_fd, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("fcntl error");
    if (bind(server_fd, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr)) < 0)
        throw std::runtime_error("bind error");
    if (listen(server_fd, SOMAXCONN) < 0)
        throw std::runtime_error("listen error");
    fd.fd = server_fd;
    fd.events = POLLIN;
    pollfds.push_back(fd);
}


void Server::accept_connection()
{
    struct sockaddr_in client_addr;
    struct pollfd fd;
    char *ip;
    std::memset(&client_addr, 0, sizeof(client_addr));
    socklen_t client_addr_len = sizeof(client_addr);
    client_fd = accept(server_fd, reinterpret_cast<struct sockaddr *>(&client_addr), &client_addr_len);
    if (client_fd < 0)
        throw std::runtime_error("accept error");
    ip = inet_ntoa(client_addr.sin_addr);
    Client client;
    fd.fd = client_fd;
    fd.events = POLLIN;
    this->clients[client_fd] = client;
    this->clients[client_fd].client_fd = client_fd;
    this->clients[client_fd].client_ip = ip;
    pollfds.push_back(fd);
}

void signal_handler(int signo)
{
    if (signo == SIGINT)
        exit(signo);
}

void Server::main_loop()
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, signal_handler);
    try
    {
        std::cout << "\033[1;32mWaiting for connection...\033[0m"<<std::endl;
        create_server();
        while (1)
        {
            is_pass = false;
            if (poll(&pollfds[0], pollfds.size(), 0) < 0)
            {
                std::cerr << "Error: poll failed" << std::endl;
                clear_clients();
                exit(1);
            }
            for (size_t i = 0; i < pollfds.size();)
            {
                if (pollfds[i].revents & POLLIN)
                {
                    if (pollfds[i].fd == server_fd)
                    {
                        std::cout << "\033[1;32mConnection started...\033[0m"<<std::endl;
                        accept_connection();
                        break;
                    }
                    else
                    {
                        char buffer[BUFFER_SIZE];
                        std::memset(buffer, 0, sizeof(buffer));
                        int bytes_read = recv(pollfds[i].fd, buffer, sizeof(buffer), 0);
                        if (bytes_read < 0)
                        {
                            std::cerr << "\033[1;31mConnection closed\033[0m" << std::endl;
                            erase_client(pollfds[i].fd);
                            break;
                        }
                        else if (bytes_read == 0)
                        {
                            std::cerr << "\033[1;31mConnection closed\033[0m" << std::endl;
                            erase_client(pollfds[i].fd);
                            break;
                        }
                        else
                        {
                            try
                            {
                                if (!this->clients[pollfds[i].fd].isconnected)
                                    this->authenticate(buffer, pollfds[i].fd);
                                else
                                {
                                    parseComand(buffer, pollfds[i].fd);
                                    if(is_pass)
                                        this->execute(buffer, pollfds[i].fd);
                                    is_pass = false;
                                }
                            }
                            catch (const std::exception &e)
                            {
                                is_pass = false;
                                this->clients[ pollfds[i].fd].msg.clear();
                                std::cerr << e.what() << '\n';
                            }
                            break;
                        }
                    }
                }
                i++;
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

void Server::clear_clients()
{
    std::map<int, Client>::iterator it = this->clients.begin();
    while (it != this->clients.end())
    {
        close(it->first);
        it++;
    }
    this->clients.clear();
    this->pollfds.clear();
}

void Server::erase_client(int fd)
{
    std::vector<std::string> cmd;
    _QUIT(cmd, fd);
}



void Server::parseComand(std::string buffer, int fd)
{
    std::string cmds[] = {"JOIN", "PRIVMSG","BOT"};
    std::string cmd;
    std::stringstream ss;
    size_t i = 0;   
    
    this->clients[fd].msg.append(buffer);
    if (this->clients[fd].msg.find("\n") == std::string::npos)
        return;
    is_pass = false;
    ss << this->clients[fd].msg;
    ss >> cmd;
    ss.clear();
    
    while (i < 3)
    {
        if(cmd == cmds[i])
            break;
        i++;
    }
    switch (i)
    {
    case 0:
        this->clients[fd].join(this->clients[fd].msg, this->channels);
        this->clients[fd].msg.clear();
        break;
    case 1:
        this->clients[fd].priveMessage(this->clients[fd].msg, this->channels, this->clients);
        this->clients[fd].msg.clear();
        break;
    case 2:
        run_bot(fd);
        this->clients[fd].msg.clear();
        break;
    case 9:
        run_bot(fd);
        break;
    default:
        is_pass = true;
        break;
    }
    return;
}

