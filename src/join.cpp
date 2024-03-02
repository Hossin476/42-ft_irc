/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asabri <asabri@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 14:59:23 by ykhourba          #+#    #+#             */
/*   Updated: 2024/03/01 23:54:06 by asabri           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Client.hpp"

void Client::join(std::string buffer,std::map<std::string,Channel> &channels)
{

	std::vector<std::string> chans;
	std::vector<std::string> keys;
	std::stringstream ss;
	std::stringstream token;
	std::string chkey;
	std::string userList;
	std::string msg;

	buffer.erase(0, 5);
	ss << buffer;
	ss >> chkey;
	token << chkey;
	
	if(chkey.empty() || chkey == "#")
	{
		this->msg = messageGenerator(Server::getHostName(), ERR_NEEDMOREPARAMS, this->nickname + " " + "JOIN", "not enough parameters");
		send(this->client_fd, this->msg.c_str(), this->msg.size(), 0);
		return;
	}
	while (std::getline(token, chkey, ','))
		chans.push_back(toLower(chkey));

	token.clear();
	ss >> chkey;
	token << chkey;
	while (std::getline(token, chkey, ','))
		keys.push_back(chkey);

	std::vector<std::string>::iterator it = chans.begin();
	std::vector<std::string>::iterator it2;
	if(keys.size() > 0)
	  it2 = keys.begin();

	while (it != chans.end())
	{
		 
		if (!checkChanReg(*it))
		{
			this->msg = messageGenerator(Server::getHostName(), ERR_NOSUCHCHANNEL, std::string("JOIN") + " " + *it, "invalid channel name " + *it);
			send(this->client_fd, this->msg.c_str(), this->msg.size(), 0);
			it++;
			if (keys.size() > 0)
				it2++;
			continue;
		}
		if (channels.find((*it)) == channels.end() || channels.size()  == 0)
		{
			channels[*it].setName(*it);
			channels[*it].operators.push_back(this->client_fd);
			channels[*it].iskeySet = false;
			channels[*it].addClient(*this);
			msg = ":" + this->nickname + "!~" + this->username + "@" + this->client_ip + " " + "JOIN" + " " + *it + "\r\n";
			msg += messageGenerator(this->client_ip, RPL_NAMREPLY, this->nickname + " = " + *it, "@" +this->nickname);
			msg += messageGenerator(this->client_ip, RPL_ENDOFNAMES, this->nickname + " = " + *it, "end of /NAMES list");
			send(this->client_fd, msg.c_str(), msg.size(), 0);
			
		}
		else
		{
			if(channels[*it].if_it_client(this->client_fd))
			{	
				this->msg = messageGenerator(Server::getHostName(), ERR_ALREADYREGISTRED, std::string("JOIN") + " " + this->nickname, "you are already in " + *it);
				send(this->client_fd, this->msg.c_str(), this->msg.size(), 0);
			}
			else
			{
				if(channels[*it].__getClients().size() >= (size_t)channels[*it].__getLimit())
				{
					this->msg = messageGenerator(this->client_ip, ERR_CHANNELISFULL, std::string("JOIN") + " " + *it, "channel " + *it + " is full");
					send(this->client_fd, this->msg.c_str(), this->msg.size(), 0);
					it++;
					if (keys.size() > 0)
						it2++;
					continue;
				}
				if(channels[*it].__getInviteOnly())
				{
					if(std::find(channels[*it].__getInviteList().begin(), channels[*it].__getInviteList().end(), this->nickname) == channels[*it].__getInviteList().end())
					{
						this->msg = messageGenerator(Server::getHostName(), ERR_INVITEONLYCHAN, std::string("JOIN") + " " + this->nickname, "you are not invited to " + *it);
						send(this->client_fd, this->msg.c_str(), this->msg.size(), 0);
						it++;
						if (keys.size() > 0)
							it2++;
						continue;					
					}
				}
				if(channels[*it].iskeySet)
				{
					if(channels[*it].getPassword() != *it2)
					{
						this->msg = messageGenerator(Server::getHostName(), ERR_BADCHANNELKEY,std::string("JOIN") , "invalid channel key for " + *it);
						send(this->client_fd, this->msg.c_str(), this->msg.size(), 0);
						it++;
						if (keys.size() > 0)
							it2++;
						continue;
					}
				}

				channels[*it].addClient(*this);
				std::map<int, Client>::iterator clientList =  channels[*it].__getClients().begin();
				userList = "";
				while (clientList !=  channels[*it].__getClients().end())
				{
					std::string op;
				    std::find(channels[*it].operators.begin(),channels[*it].operators.end(),clientList->first) != channels[*it].operators.end() ? op = "@" : op = "";
					userList += op + clientList->second.nickname + " ";
					clientList++;
				}
				clientList = channels[*it].__getClients().begin();
				while (clientList != channels[*it].__getClients().end())
				{
					msg = ":" + this->nickname + "!~" + this->username + "@" + this->client_ip + " " + "JOIN" + " " +   *it + "\r\n";
					send(clientList->first, msg.c_str(), msg.size(), 0);
					clientList++;
				}
				msg = messageGenerator(this->client_ip, RPL_NAMREPLY, this->nickname + "  = " + *it, userList);
				msg += messageGenerator(this->client_ip, RPL_ENDOFNAMES, this->nickname + " = " + *it, "end of /NAMES list");
				send(this->client_fd, msg.c_str(), msg.size(), 0);
			}
		}
		it++;
		if (keys.size() > 0)
			it2++;
	}
}