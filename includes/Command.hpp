/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/06 21:50:37 by yxu               #+#    #+#             */
/*   Updated: 2025/09/06 21:50:38 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"
#include "IrcMessage.hpp"
#include "Server.hpp"

class Command {
public:
  static bool ping(Server &server, Client &client, const IrcMessage &command);
  static bool nick(Server &server, Client &client, const IrcMessage &command);
  static bool cap(Server &server, Client &client, const IrcMessage &command);
  static bool user(Server &server, Client &client, const IrcMessage &command);
};
