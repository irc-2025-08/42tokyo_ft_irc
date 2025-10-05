/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tac <tac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 00:00:00 by yxu               #+#    #+#             */
/*   Updated: 2025/10/05 12:51:37 by tac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Command.hpp"
#include "../../includes/CommandHandler.hpp"
#include "../../includes/ServerHandler.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Channel.hpp"
#include <cctype>
#include <cstdlib>
#include <iostream>
#include "../../includes/CommandUtils.hpp"

bool Command::mode(Server &server, Client &client,
                     const IrcMessage &command) {
  
  // パラメータチェック (channel mode_string)
  if (command.params.empty()) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "461",
        client.getNickname() + " MODE :Not enough parameters");
    CommandUtils::reply(server, client, msg);
    return false;
  }

  std::string target = command.params[0];
  std::string clientNickname = client.getNickname();

  if (!target.empty() && target[0] == '#') {
    if (command.params.size() < 2) {
      IrcMessage msg = CommandUtils::createIrcMessage(
          server.getServerName(), "461",
          clientNickname + " MODE :Not enough parameters");
      CommandUtils::reply(server, client, msg);
      return false;
    }

    std::string channelName = target;
    std::string modeString = command.params[1];

    Channel *channel = server.findChannel(channelName);
    if (channel == NULL) {
      IrcMessage msg = CommandUtils::createIrcMessage(
          server.getServerName(), "403",
          clientNickname + " " + channelName + " :No such channel");
      CommandUtils::reply(server, client, msg);
      return false;
    }

    if (!channel->hasMember(clientNickname)) {
      IrcMessage msg = CommandUtils::createIrcMessage(
          server.getServerName(), "442",
          clientNickname + " " + channelName + " :You're not on that channel");
      CommandUtils::reply(server, client, msg);
      return false;
    }

    if (!channel->isOperator(clientNickname)) {
      IrcMessage msg = CommandUtils::createIrcMessage(
          server.getServerName(), "482",
          clientNickname + " " + channelName +
              " :You're not channel operator");
      CommandUtils::reply(server, client, msg);
      return false;
    }
  
  // 複数のモードオプションがある場合はエラー
  size_t modeCount = 0;
  for (size_t i = 0; i < modeString.length(); i++) {
    char c = modeString[i];
    if (c != '+' && c != '-' && (c == 'i' || c == 't' || c == 'l' || c == 'o' || c == 'k')) {
      modeCount++;
    }
  }
  
  if (modeCount > 1) {
    IrcMessage msg = CommandUtils::createIrcMessage(
      server.getServerName(), "461", 
      clientNickname + " MODE :Only one mode change allowed per command");
    CommandUtils::reply(server, client, msg);
    return false;
  }
  
  if (modeCount == 0) {
    IrcMessage msg = CommandUtils::createIrcMessage(
      server.getServerName(), "461", 
      clientNickname + " MODE :No valid mode specified");
    CommandUtils::reply(server, client, msg);
    return false;
  }

  // モード文字列を解析
  bool adding = true;  // デフォルトは追加モード
  std::string responseMode = "";
  size_t paramIndex = 2;  // 追加パラメータのインデックス（channel, mode_stringの次）
  
  for (size_t i = 0; i < modeString.length(); i++) {
    char c = modeString[i];
    
    if (c == '+') {
      adding = true;
      continue;
    } else if (c == '-') {
      adding = false;
      continue;
    }
    
    // -i オプション（invitation only）の処理
    if (c == 'i') {
      if (adding) {
        // +i: invitation only にする
        if (!channel->isInvitationOnly()) {
          channel->setInvitationOnly(true);
          responseMode += "+i";
        }
      } else {
        // -i: invitation only を解除する
        if (channel->isInvitationOnly()) {
          channel->setInvitationOnly(false);
          responseMode += "-i";
        }
      }
      // 1つのモードのみ処理して終了
      break;
    }
    // -t オプション（topic restriction）の処理
    else if (c == 't') {
      if (adding) {
        // +t: topic をオペレーターのみに制限する
        if (!channel->isTopicRestricted()) {
          channel->setTopicRestricted(true);
          responseMode += "+t";
          std::cout << "channel->isTopicRestricted() " << channel->isTopicRestricted() << std::endl;
        }
      } else {
        // -t: topic 制限を解除する（全メンバーが変更可能）
        if (channel->isTopicRestricted()) {
          channel->setTopicRestricted(false);
          responseMode += "-t";
        }
      }
      // 1つのモードのみ処理して終了
      break;
    }
    // -l オプション（user limit）の処理
    else if (c == 'l') {
      if (adding) {
        // +l: ユーザー数制限を設定（引数が必要）
        if (paramIndex >= command.params.size()) {
          IrcMessage msg = CommandUtils::createIrcMessage(
            server.getServerName(), "461", 
            clientNickname + " MODE :Not enough parameters for +l");
          CommandUtils::reply(server, client, msg);
          continue;
        }
        
        std::string limitStr = command.params[paramIndex++];
        int limit = 0;
        
        // 数値変換チェック
        for (size_t j = 0; j < limitStr.length(); j++) {
          if (!std::isdigit(limitStr[j])) {
            IrcMessage msg = CommandUtils::createIrcMessage(
              server.getServerName(), "461", 
              clientNickname + " MODE :Invalid limit value");
            CommandUtils::reply(server, client, msg);
            continue;
          }
        }
        
        limit = std::atoi(limitStr.c_str());
        if (limit <= 0 || limit > 999) {
          IrcMessage msg = CommandUtils::createIrcMessage(
            server.getServerName(), "461", 
            clientNickname + " MODE :Invalid limit value (1-999)");
          CommandUtils::reply(server, client, msg);
          continue;
        }
        
        // 現在のメンバー数をチェック
        if (limit < static_cast<int>(channel->getMembers().size())) {
          IrcMessage msg = CommandUtils::createIrcMessage(
            server.getServerName(), "467", 
            clientNickname + " " + channelName + " :Channel limit is lower than current users");
          CommandUtils::reply(server, client, msg);
          continue;
        }
        
        channel->setUserLimit(limit);
        responseMode += "+l " + limitStr;
      } else {
        // -l: ユーザー数制限を解除
        if (channel->hasUserLimit()) {
          channel->setUserLimit(0);
          responseMode += "-l";
        }
      }
      // 1つのモードのみ処理して終了
      break;
    }
    // -k オプション（channel key）の処理
    else if (c == 'k') {
      if (adding) {
        // +k: チャンネルキー（パスワード）を設定（引数が必要）
        if (paramIndex >= command.params.size()) {
          IrcMessage msg = CommandUtils::createIrcMessage(
            server.getServerName(), "461", 
            clientNickname + " MODE :Not enough parameters for +k");
          CommandUtils::reply(server, client, msg);
          return false;
        }
        
        std::string channelKey = command.params[paramIndex++];
        
        // パスワードが空でないかチェック
        if (channelKey.empty()) {
          IrcMessage msg = CommandUtils::createIrcMessage(
            server.getServerName(), "461", 
            clientNickname + " MODE :Channel key cannot be empty");
          CommandUtils::reply(server, client, msg);
          return false;
        }
        
        // パスワードの長さチェック（1-50文字）
        if (channelKey.length() > 50) {
          IrcMessage msg = CommandUtils::createIrcMessage(
            server.getServerName(), "461", 
            clientNickname + " MODE :Channel key too long (max 50 characters)");
          CommandUtils::reply(server, client, msg);
          return false;
        }
        
        channel->setChannelKey(channelKey);
        responseMode += "+k " + channelKey;
      } else {
        // -k: チャンネルキーを削除
        if (channel->hasChannelKey()) {
          channel->removeChannelKey();
          responseMode += "-k";
        }
      }
      // 1つのモードのみ処理して終了
      break;
    }
    // -o オプション（operator privilege）の処理
    else if (c == 'o') {
      // +o/-o 両方とも引数が必要
      if (paramIndex >= command.params.size()) {
        IrcMessage msg = CommandUtils::createIrcMessage(
          server.getServerName(), "461", 
          clientNickname + " MODE :Not enough parameters for " + 
          (adding ? "+o" : "-o"));
        CommandUtils::reply(server, client, msg);
        return false;
      }
      
      std::string targetNickname = command.params[paramIndex++];
      
      // 対象ユーザーがチャンネルメンバーかチェック
      if (!channel->hasMember(targetNickname)) {
        IrcMessage msg = CommandUtils::createIrcMessage(
          server.getServerName(), "441", 
          clientNickname + " " + targetNickname + " " + channelName + " :They aren't on that channel");
        CommandUtils::reply(server, client, msg);
        return false;
      }
      
      if (adding) {
        // +o: オペレーター権限を付与
        if (!channel->isOperator(targetNickname)) {
          channel->addOperator(targetNickname);
          responseMode += "+o " + targetNickname;
        }
      } else {
        // -o: オペレーター権限を剥奪
        if (channel->isOperator(targetNickname)) {
          channel->removeOperator(targetNickname);
          responseMode += "-o " + targetNickname;
        }
      }
      
      // 1つのモードのみ処理して終了
      break;
    } else {
      // 未知のモード
      IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "472", 
        clientNickname + " " + std::string(1, c) + " :is unknown mode char to me");
      CommandUtils::reply(server, client, msg);
      continue;
    }
  }
  
  // モード変更の通知を送信（変更があった場合のみ）
  if (!responseMode.empty()) {
    // チャンネルの全メンバーにモード変更を通知
    std::vector<std::string> members = channel->getMembers();
    for (std::vector<std::string>::const_iterator it = members.begin();
         it != members.end(); ++it) {
      Client *memberClient = server.findClientByNickname(*it);
      if (memberClient) {
        IrcMessage modeMsg = CommandUtils::createIrcMessage(
            clientNickname, "MODE", channelName + " " + responseMode);
        CommandUtils::reply(server, *memberClient, modeMsg);
      }
    }
  }

  return true;
  }

  // ユーザーモードの処理
  if (command.params.size() < 2) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "221",
        clientNickname + " +" + (client.isInvisible() ? "i" : ""));
    CommandUtils::reply(server, client, msg);
    return true;
  }

  if (target != clientNickname) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "502",
        clientNickname + " :Can't change mode for other users");
    CommandUtils::reply(server, client, msg);
    return false;
  }

  std::string modeString = command.params[1];
  if (modeString.empty()) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "221",
        clientNickname + " +" + (client.isInvisible() ? "i" : ""));
    CommandUtils::reply(server, client, msg);
    return true;
  }

  bool adding = true;
  bool changed = false;
  for (size_t i = 0; i < modeString.length(); ++i) {
    char c = modeString[i];
    if (c == '+') {
      adding = true;
    } else if (c == '-') {
      adding = false;
    } else if (c == 'i') {
      bool newValue = adding;
      if (client.isInvisible() != newValue) {
        client.setInvisible(newValue);
        changed = true;
      }
    } else {
      IrcMessage msg = CommandUtils::createIrcMessage(
          server.getServerName(), "501",
          clientNickname + " :Unknown MODE flag");
      CommandUtils::reply(server, client, msg);
      return false;
    }
  }

  if (changed) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        clientNickname, "MODE",
        clientNickname + " " + (client.isInvisible() ? "+i" : "-i"));
    CommandUtils::reply(server, client, msg);
  }

  return true;
}
