#pragma once

#include <map>
#include <string>
#include <vector>

class Server;
class Client;

struct IrcCommand {
  std::string prefix;
  std::string command;
  std::vector<std::string> params;
  bool lastParamIsTrailing;
};

// TODO
class CommandHandler {
public:
  /**
   * @brief parse the command and process it
   * @param server the server
   * @param client the client
   */
  static void parseAndProcessCommand(Server &server, Client &client);

  static void initCommandMap();

private:
  static std::map<std::string, bool (*)(Server &, Client &, const IrcCommand &)> commandMap_;

  static IrcCommand parseCommandLine(const std::string &cmdLine);

  static void processCommand(Server &server, Client &client,
                             const IrcCommand &command);

  static bool pingCmd(Server &server, Client &client,
                                 const IrcCommand &command);

  static bool passCmd(Server &server, Client &client,
                                 const IrcCommand &command);
};