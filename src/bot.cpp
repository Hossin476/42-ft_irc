/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lshail <lshail@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/02 15:38:49 by lshail            #+#    #+#             */
/*   Updated: 2024/03/02 15:39:33 by lshail           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/server.hpp"

void Server::run_bot(int fd)
{
    std::vector<std::string> facts;
    srand(time(NULL));

    facts.push_back( "Fact 1: The universe is around 13.8 billion years old.");
    facts.push_back( "Fact 2: The Andromeda galaxy is our closest spiral galaxy neighbor at 2.537 million light years away.");
    facts.push_back( "Fact 3: The Milky Way galaxy will collide with the Andromeda galaxy in about 5 billion years.");
    facts.push_back( "Fact 4: The water we have on earth is older than the sun.");
    facts.push_back( "Fact 5: 95% of the universe is made up of dark matter and dark energy which are invisible.");
    facts.push_back( "Fact 6: The sun is 99.8% of the total mass of the solar system.");
    facts.push_back( "Fact 7: There are more stars in the universe than grains of sand on all the beaches on earth.");
    facts.push_back( "Fact 8: There is a supermassive black hole at the center of the Milky Way galaxy called Sagittarius A*.");
    facts.push_back( "Fact 9: The largest known star is UY Scuti, a hypergiant with a radius around 1708 times larger than the sun.");
    facts.push_back( "Fact 10: Ganyemede is the largest moon in the solar system and is even larger than the planet Mercury.");
    facts.push_back( "Fact 11: The hottest planet in the solar system is Venus with an average surface temperature of 450°C.");
    facts.push_back( "Fact 12: Leonor de Todos los Santos de Borbón y Ortiz belongs to the GOAT which is me :) .");
    int index = rand() % facts.size();
    std::string msg = Client::messageGenerator("BOT", RPL_WELCOME, "BOT", facts[index]);
    send(fd, msg.c_str(), msg.size(), 0);
}