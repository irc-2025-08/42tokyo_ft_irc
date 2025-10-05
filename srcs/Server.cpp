/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 00:18:29 by yxu               #+#    #+#             */
/*   Updated: 2025/10/05 18:48:10 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/CommandHandler.hpp"
#include "../includes/CommandUtils.hpp"
#include "../includes/ServerHandler.hpp"
#include "../includes/ServerUtils.hpp"
#include "../includes/config.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

bool ctrl_c_shutdown = false;

Server::Server(int port, std::string password)
    : port_(port), password_(password), serverName_(config::serverName),
      serverStatus_(SERV_STOP) {
  std::cout << "[INFO] ircd: Initializing server..." << std::endl;

  // create a socket and bind it to the port
  socketFd_ = ServerUtils::createSocket(port_);
  if (socketFd_ < 0)
    throw std::runtime_error("Failed to create socket");

  // set the socket to non-blocking
  if (ServerUtils::setNonblock(socketFd_) < 0)
    throw std::runtime_error("Failed to set socket to non-blocking");

  // create an epoll instance
  epollFd_ = epoll_create1(0);
  if (epollFd_ < 0)
    throw std::runtime_error("Failed to create epoll instance");

  // add the socket to the epoll instance
  if (ServerUtils::addEpollEvent(epollFd_, socketFd_, EPOLLIN) < 0)
    throw std::runtime_error("Failed to add socket to epoll");

  // initialize irc commands map
  CommandHandler::initCommandMap();

  std::cout << "[INFO] ircd: Server initialized successfully" << std::endl;
}

Server::~Server() {
  // close the socket and all clients fds
  close(socketFd_);
  for (std::map<int, Client>::iterator it = clients_.begin();
       it != clients_.end(); ++it) {
    close(it->first);
  }
}

void Server::setStatus(ServerStatus status) {
  if (getStatus() == SERV_ERROR)
    return;
  serverStatus_ = status;
}

Server::ServerStatus Server::getStatus() const { return serverStatus_; }

std::string Server::getPassword() const { return password_; }

std::string Server::getServerName() const { return serverName_; }

std::map<int, Client> Server::getClientsMap() const { return clients_; }

std::vector<std::string> Server::getChannelNamesForMember(
    const std::string &nickname) {
  std::vector<std::string> channelNames;

  for (std::vector<Channel>::iterator it = channels_.begin();
       it != channels_.end(); ++it) {
    if (it->hasMember(nickname)) {
      channelNames.push_back(it->getName());
    }
  }

  return channelNames;
}

void Server::removeClientFromChannel(const std::string &channelName,
                                     const std::string &nickname) {
  Channel *channel = findChannel(channelName);
  if (channel == NULL) {
    return;
  }

  channel->removeMember(nickname);
  if (channel->isOperator(nickname)) {
    channel->removeOperator(nickname);
  }

  if (channel->isEmpty()) {
    removeChannel(channelName);
  }
}

static std::string buildQuitPrefix(const Server &server, const Client &client) {
  std::string prefix = client.getNickname();
  if (!client.getUsername().empty()) {
    prefix += "!" + client.getUsername();
  }
  prefix += "@" + server.getServerName();
  return prefix;
}

void Server::handleClientQuit(Client &client, const std::string &reason) {
  std::vector<std::string> channelNames =
      getChannelNamesForMember(client.getNickname());

  IrcMessage quitMessage = CommandUtils::createIrcMessage(
      buildQuitPrefix(*this, client), "QUIT", ":" + reason);

  for (std::vector<std::string>::iterator it = channelNames.begin();
       it != channelNames.end(); ++it) {
    const std::string &channelName = *it;
    Channel *channel = findChannel(channelName);
    if (channel == NULL) {
      continue;
    }

    std::vector<std::string> members = channel->getMembers();
    for (std::vector<std::string>::iterator memberIt = members.begin();
         memberIt != members.end(); ++memberIt) {
      Client *memberClient = findClientByNickname(*memberIt);
      if (memberClient != NULL && memberClient->getFd() != client.getFd()) {
        CommandUtils::reply(*this, *memberClient, quitMessage);
      }
    }

    removeClientFromChannel(channelName, client.getNickname());
  }

  CommandUtils::reply(*this, client, quitMessage);

  IrcMessage errorMessage = CommandUtils::createIrcMessage(
      getServerName(), "ERROR",
      ":Closing Link: " + client.getNickname() + " (" + reason + ")");
  CommandUtils::reply(*this, client, errorMessage);
}

void Server::run() {
  if (getStatus() == SERV_ERROR)
    return;

  setStatus(SERV_RUNNING);
  std::cout << "[INFO] ircd: IRC server running on port " << port_
            << " with password " << password_ << std::endl;

  eventLoop();
}

void Server::eventLoop() {
  epoll_event events[config::maxEvents];

  while (getStatus() == SERV_RUNNING) {
    // wait for events
    int nfds = epoll_wait(epollFd_, events, config::maxEvents, -1);

    if (ctrl_c_shutdown == true) {
      setStatus(SERV_STOP);
      break;
    }

    if (nfds < 0) {
      std::cerr << "[WARN] ircd: Failed to wait for events" << std::endl;
      continue;
    }

    // handle events
    for (int i = 0; i < nfds; i++) {
      // handle accept event if the ready fd is the socket fd
      if (events[i].data.fd == socketFd_) {
        ServerHandler::handleAccept(*this);
        continue;
      }

      // if the ready fd is not the socket fd, it must be a client fd
      // get the ready client
      Client &client = clients_[events[i].data.fd];

      // handle client events
      if (events[i].events & (EPOLLHUP | EPOLLERR | EPOLLRDHUP)) {
        ServerHandler::closeClientConnection(*this, client);
        continue;
      }
      if (events[i].events & EPOLLIN)
        ServerHandler::handleRecv(*this, client);
      if (events[i].events & EPOLLOUT)
        ServerHandler::handleSend(*this, client);
    }
  }
}

// Channel management methods
Channel* Server::findChannel(const std::string& channelName) {
  for (std::vector<Channel>::iterator it = channels_.begin();
       it != channels_.end(); ++it) {
    if (it->getName() == channelName) {
      return &(*it);
    }
  }
  return NULL;
}

void Server::addChannel(const std::string& channelName) {
  channels_.push_back(Channel(channelName));
}

void Server::removeChannel(const std::string& channelName) {
  for (std::vector<Channel>::iterator it = channels_.begin();
       it != channels_.end(); ++it) {
    if (it->getName() == channelName) {
      std::cout << "[INFO] Channel deleted: " << channelName << std::endl;
      channels_.erase(it);
      return;
    }
  }
}

// Client management methods
Client* Server::findClientByNickname(const std::string& nickname) {
  for (std::map<int, Client>::iterator it = clients_.begin();
       it != clients_.end(); ++it) {
    if (it->second.getNickname() == nickname) {
      return &(it->second);
    }
  }
  return NULL;
}

void Server::updateNicknameInAllChannels(const std::string& oldNickname, const std::string& newNickname) {
  // 全チャンネルでニックネームを更新
  for (std::vector<Channel>::iterator it = channels_.begin();
       it != channels_.end(); ++it) {
    if (it->hasMember(oldNickname)) {
      it->updateMemberNickname(oldNickname, newNickname);
    }
  }
}
