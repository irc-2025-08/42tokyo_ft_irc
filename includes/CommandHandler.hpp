class Server;
class Client;

// TODO
class CommandHandler {
public:
  /**
   * @brief parse the command and process it
   * @param server the server
   * @param client the client
   */
  static void parseAndProcessCommand(Server &server, Client &client);
};