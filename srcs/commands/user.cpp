/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 22:49:59 by yxu               #+#    #+#             */
/*   Updated: 2025/09/05 23:06:15 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Client.hpp"
#include "../../includes/CommandHandler.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/CommandUtils.hpp"

// to be implemented
bool CommandHandler::userCmd(Server &server, Client &client,
                             const IrcMessage &command) {
  (void)command;
  if (client.getNickname() != "*") CommandUtils::sendWelcomeMessage(server, client);
  return true;
}
