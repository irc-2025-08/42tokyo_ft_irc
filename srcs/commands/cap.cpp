/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cap.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 22:42:11 by yxu               #+#    #+#             */
/*   Updated: 2025/09/05 23:28:32 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Client.hpp"
#include "../../includes/CommandHandler.hpp"
#include "../../includes/Server.hpp"

bool CommandHandler::capCmd(Server &server, Client &client,
                            const IrcMessage &command) {
  (void)command;
  IrcMessage msg = createIrcMessage(server.getServerName(), "CAP",
                                    client.getNickname() + " LS :");
  reply(server, client, msg);
  return true;
}
