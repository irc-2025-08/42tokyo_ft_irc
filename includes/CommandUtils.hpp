#pragma once

class Server;
class Client;

class CommandUtils {
public:
  static int sendWelcomeMessage(Server &server, Client &client);
};
