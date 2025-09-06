/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 22:49:59 by yxu               #+#    #+#             */
/*   Updated: 2025/09/06 21:37:40 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Command.hpp"
#include "../../includes/CommandUtils.hpp"

// to be implemented
bool Command::user(Server &server, Client &client, const IrcMessage &command) {
  (void)command;
  if (client.getNickname() != "*")
    CommandUtils::sendWelcomeMessage(server, client);
  return true;
}
