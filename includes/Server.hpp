/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tac <tac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 00:18:48 by yxu               #+#    #+#             */
/*   Updated: 2025/10/05 18:02:56 by tac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"
#include "Channel.hpp"
#include <map>
#include <string>
#include <vector>

extern bool ctrl_c_shutdown;

class ServerHandler;

class Server {
public:
  friend class ServerHandler;
  enum ServerStatus { SERV_STOP, SERV_RUNNING, SERV_ERROR };

  Server(int port, std::string password);
  ~Server();

  /**
   * @brief run the server
   */
  void run();
  ServerStatus getStatus() const;
  void setStatus(ServerStatus status);
  std::string getPassword() const;
  std::string getServerName() const;
  std::map<int, Client> getClientsMap() const;
  
  // Channel management methods
  Channel* findChannel(const std::string& channelName);
  void addChannel(const std::string& channelName);
  void removeChannel(const std::string& channelName);
  
  // Client management methods
  Client* findClientByNickname(const std::string& nickname);
  void updateNicknameInAllChannels(const std::string& oldNickname, const std::string& newNickname);
  std::vector<std::string> getChannelNamesForMember(const std::string& nickname);
  void removeClientFromChannel(const std::string &channelName,
                               const std::string &nickname);
  void handleClientQuit(Client &client, const std::string &reason);

private:
  int port_;
  std::string password_;
  std::string serverName_;
  ServerStatus serverStatus_;
  int socketFd_;
  int epollFd_;
  std::map<int, Client> clients_;
  std::vector<Channel> channels_;

  /**
   * @brief event loop
   */
  void eventLoop();
};
