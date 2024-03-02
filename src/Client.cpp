/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lshail <lshail@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/06 14:08:53 by asabri            #+#    #+#             */
/*   Updated: 2024/03/02 15:33:45 by lshail           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Client.hpp"

Client::Client()
{
	this->isconnected = false;
	this->is_pass = false;
	this->isKeySet = false;
	this->isUsrnameSet = false;
}

Client::~Client()
{
}
Client::Client(const Client &client)
{
    this->nickname = client.nickname;
    this->username = client.username;
    this->isOperator = client.isOperator;
    this->isconnected = client.isconnected;
    this->is_pass = client.is_pass;
    this->isKeySet = client.isKeySet;
    this->isUsrnameSet = client.isUsrnameSet;
	this->client_fd = client.client_fd;
	this->client_ip = client.client_ip;
}
std::string Client::getNick()
{
	return this->nickname;
}

void Client::setNick(std::string nickname)
{
	this->nickname = nickname;
}

void Client::setUsername(std::string username)
{
	this->username = username;
}

std::string Client::getUserName()
{
	return this->username;
}

bool Client::checkRegNick(std::string nickname)
{
	size_t i = 0;
	std::string allowed = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";
	if (nickname.size() > 9 || nickname.size() < 1)
		return false;
	if (nickname.at(0) >= '0' && nickname.at(0) <= '9')
		return false;
	while (i < nickname.size())
	{
		if (allowed.find(nickname.at(i)) == std::string::npos)
			return false;
		i++;
	}
	return true;
}

std::string Client::messageGenerator(std::string prefix, std::string command, std::string params, std::string trail)
{
	std::string message;
	message = ":" + prefix + " " + command + " " + params + " " + ":" + trail + "\r\n";
	return message;
}

bool Client::checkChanReg(std::string channel)
{
	std::string allowed = "#abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";

	if(channel.find("#") != 0)
	{
		return false;
	}
	for (size_t i = 0; i < channel.size(); i++)
	{
		if (allowed.find(channel.at(i)) == std::string::npos)
		{
			return false;
		}
	}
	return true;
}

std::string &Client::toLower(std::string &str)
{
	size_t i;

	i = -1;
	while (++i < str.size())
		str[i] = std::tolower(str[i]);
	return str;
}

 Client &Client::operator=(const Client &client)
 {
	 this->nickname = client.nickname;
	 this->username = client.username;
	 this->isconnected = client.isconnected;
	 this->is_pass = client.is_pass;
	 this->isKeySet = client.isKeySet;
	 this->isUsrnameSet = client.isUsrnameSet;
	 this->client_fd = client.client_fd;
	 this->client_ip = client.client_ip;
	 return *this;
}