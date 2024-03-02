/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asabri <asabri@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 19:51:16 by asabri            #+#    #+#             */
/*   Updated: 2024/03/01 19:58:35 by asabri           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/server.hpp"

int main(int ac, char **av)
{
    std::stringstream ss;
    int port;
    if (ac != 3)
    {
        std::cerr << "\033[31;1m Usage: ./IRC <port> <password> \033[0m" << std::endl;
       return 1; 
    }
    if (check_port(av[1]) == false)
    {
        std::cerr << "\033[31;1m Invalid Port Number \033[0m" << std::endl;
        return 1;
    }
    ss << av[1];
    ss >> port;
    std::string password = av[2];
    if (password.empty())
    {
        std::cerr << "\033[31;1m Invalid Password \033[0m" << std::endl;
        return 1;
    }
    Server server(port, av[2]);
    server.main_loop();
    return 0;
}