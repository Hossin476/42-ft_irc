/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykhourba <ykhourba@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 15:00:45 by ykhourba          #+#    #+#             */
/*   Updated: 2024/02/27 16:00:04 by ykhourba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Client.hpp"

void Client::priveMessage(std::string buffer,std::map<std::string, Channel> &channels, std::map<int, Client> &clients)
 {

	std::vector<std::string> recievers;
	std::string message;
	std::stringstream ss;
	std::stringstream token;
	std::string recStr;
	

	buffer.erase(0, 7);
	buffer.find("\r\n") == std::string::npos 
	? buffer.erase(buffer.find("\n"))
	: buffer.erase(buffer.find("\r\n"));
	ss << buffer;
	ss >> recStr;
	ss >> message;

	if(recStr.empty() || message.empty())
	{
		this->msg = messageGenerator(this->client_ip, ERR_NORECIPIENT, std::string("PRIVMSG"), "no recipient given (PRIVMSG)");
		send(this->client_fd, this->msg.c_str(), this->msg.size(), 0);
		return;
	}
	message = buffer.substr(buffer.find(recStr) + recStr.size() + 1);
	if(message[0] == ':')
		message.erase(0, 1);
	token << recStr;
	while (std::getline(token, recStr, ','))
	{
		if(recStr.find("#")!= std::string::npos)
			recievers.push_back(toLower(recStr));
		else
			recievers.push_back(recStr);
	}
	std::vector<std::string>::iterator it = recievers.begin();
	while (it != recievers.end())
	{
		if(it->find("#")!= std::string::npos)
		{
			if(channels.find(*it) == channels.end())
			{
				this->msg = messageGenerator(Server::getHostName(), ERR_NOSUCHCHANNEL,this->nickname + " " + *it , "no such channel");
				send(this->client_fd, this->msg.c_str(), this->msg.size(), 0);
			}
			else
			{
				if(channels[*it].__getClients().find(this->client_fd) == channels[*it].__getClients().end())
				{
					this->msg = messageGenerator(Server::getHostName(), ERR_CANNOTSENDTOCHAN, this->nickname + " " + *it, "you are not in  the channel");
					send(this->client_fd, this->msg.c_str(), this->msg.size(), 0);
				}
				else
				{
					std::map<int, Client>::iterator clientList = channels[*it].__getClients().begin();
					while (clientList != channels[*it].__getClients().end())
					{
						this->msg = messageGenerator(this->nickname + "!~"+this->username +"@"+ this->client_ip, "PRIVMSG ", *it, message);
						if(this->client_fd != clientList->first)
							send(clientList->first, this->msg.c_str(), this->msg.size(), 0);
						clientList++;
					}
				}
			}
		}
		else
		{
			std::map<int , Client>::iterator cl = clients.begin();
			while (cl != clients.end())
			{
				if(cl->second.nickname == *it)
				{
					this->msg = messageGenerator(this->nickname + "!~" + this->username  + "@"+ this->client_ip, "PRIVMSG ", *it, message);
					send(cl->second.client_fd, this->msg.c_str(), this->msg.size(), 0);
					break;
				}
				cl++;
			}
			if(cl == clients.end())
			{
				this->msg = messageGenerator(Server::getHostName(), ERR_NOSUCHNICK,this->nickname + " " + *it, "no such nick");
				send(this->client_fd, this->msg.c_str(), this->msg.size(), 0);
			}
		}
		it++;
	}
 }