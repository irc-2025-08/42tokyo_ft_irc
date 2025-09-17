/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 XX:XX:XX by yxu               #+#    #+#             */
/*   Updated: 2025/09/17 XX:XX:XX by yxu              ###   ########.fr       */
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

bool Command::modeCmd(Server &server, Client &client,
                     const IrcMessage &command) {
  // パラメータチェック (channel mode_string)
  if (command.params.size() < 2) {
    ServerHandler::queueMessage(server, client,
      ":myserver 461 " + client.getNickname() + " MODE :Not enough parameters\r\n");
    return false;
  }
  
  std::string channelName = command.params[0];
  std::string modeString = command.params[1];
  std::string clientNickname = client.getNickname();
  
  // チャンネルが存在するかチェック
  Channel* channel = server.findChannel(channelName);
  if (channel == NULL) {
    ServerHandler::queueMessage(server, client,
      ":myserver 403 " + clientNickname + " " + channelName + " :No such channel\r\n");
    return false;
  }
  
  // クライアントがチャンネルのメンバーかチェック
  if (!channel->hasMember(clientNickname)) {
    ServerHandler::queueMessage(server, client,
      ":myserver 442 " + clientNickname + " " + channelName + " :You're not on that channel\r\n");
    return false;
  }
  
  // クライアントがチャンネルオペレーターかチェック
  if (!channel->isOperator(clientNickname)) {
    ServerHandler::queueMessage(server, client,
      ":myserver 482 " + clientNickname + " " + channelName + " :You're not channel operator\r\n");
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
    ServerHandler::queueMessage(server, client,
      ":myserver 461 " + clientNickname + " MODE :Only one mode change allowed per command\r\n");
    return false;
  }
  
  if (modeCount == 0) {
    ServerHandler::queueMessage(server, client,
      ":myserver 461 " + clientNickname + " MODE :No valid mode specified\r\n");
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
          ServerHandler::queueMessage(server, client,
            ":myserver 461 " + clientNickname + " MODE :Not enough parameters for +l\r\n");
          continue;
        }
        
        std::string limitStr = command.params[paramIndex++];
        int limit = 0;
        
        // 数値変換チェック
        for (size_t j = 0; j < limitStr.length(); j++) {
          if (!std::isdigit(limitStr[j])) {
            ServerHandler::queueMessage(server, client,
              ":myserver 461 " + clientNickname + " MODE :Invalid limit value\r\n");
            continue;
          }
        }
        
        limit = std::atoi(limitStr.c_str());
        if (limit <= 0 || limit > 999) {
          ServerHandler::queueMessage(server, client,
            ":myserver 461 " + clientNickname + " MODE :Invalid limit value (1-999)\r\n");
          continue;
        }
        
        // 現在のメンバー数をチェック
        if (limit < static_cast<int>(channel->getMembers().size())) {
          ServerHandler::queueMessage(server, client,
            ":myserver 467 " + clientNickname + " " + channelName + " :Channel limit is lower than current users\r\n");
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
          ServerHandler::queueMessage(server, client,
            ":myserver 461 " + clientNickname + " MODE :Not enough parameters for +k\r\n");
          return false;
        }
        
        std::string channelKey = command.params[paramIndex++];
        
        // パスワードが空でないかチェック
        if (channelKey.empty()) {
          ServerHandler::queueMessage(server, client,
            ":myserver 461 " + clientNickname + " MODE :Channel key cannot be empty\r\n");
          return false;
        }
        
        // パスワードの長さチェック（1-50文字）
        if (channelKey.length() > 50) {
          ServerHandler::queueMessage(server, client,
            ":myserver 461 " + clientNickname + " MODE :Channel key too long (max 50 characters)\r\n");
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
        ServerHandler::queueMessage(server, client,
          ":myserver 461 " + clientNickname + " MODE :Not enough parameters for " + 
          (adding ? "+o" : "-o") + "\r\n");
        return false;
      }
      
      std::string targetNickname = command.params[paramIndex++];
      
      // 対象ユーザーがチャンネルメンバーかチェック
      if (!channel->hasMember(targetNickname)) {
        ServerHandler::queueMessage(server, client,
          ":myserver 441 " + clientNickname + " " + targetNickname + " " + channelName + " :They aren't on that channel\r\n");
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
      ServerHandler::queueMessage(server, client,
        ":myserver 472 " + clientNickname + " " + std::string(1, c) + " :is unknown mode char to me\r\n");
      continue;
    }
  }
  
  // モード変更の通知を送信（変更があった場合のみ）
  if (!responseMode.empty()) {
    // チャンネルの全メンバーにモード変更を通知
    std::vector<std::string> members = channel->getMembers();
    for (std::vector<std::string>::const_iterator it = members.begin();
         it != members.end(); ++it) {
      Client* memberClient = server.findClientByNickname(*it);
      if (memberClient) {
        ServerHandler::queueMessage(server, *memberClient,
          ":" + clientNickname + " MODE " + channelName + " " + responseMode + "\r\n");
      }
    }
  }
  
  return true;
}
