#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

bool send_and_respond(Connection &connection, Message &client_message, Message &server_response);

int main(int argc, char **argv) {
  if (argc != 4) {
    std::cerr << "Usage: ./sender [server_address] [port] [username]\n";
    return 1;
  }

  std::string server_hostname;
  int server_port;
  std::string username;

  server_hostname = argv[1];
  server_port = std::stoi(argv[2]);
  username = argv[3];
  // TODO: connect to server
  Connection connection;
  connection.connect(server_hostname, server_port);
  if(!connection.is_open()) {
    std::cerr << "Failed to open connection" << "\n";
    return 2;
  }
  // TODO: send slogin message
  Message server_response;
  Message client_message(TAG_SLOGIN, username);
  if(!send_and_respond(connection, client_message, server_response)) {
    return 3;
  }
  if(server_response.tag == TAG_ERR) {
    std::cerr << server_response.data << "\n";
    return 4;
  } 
  // TODO: loop reading commands from user, sending messages to
  //       server as appropriate
  std::string user_input;
  bool quit = false;
  bool in_room = false;
  while(!quit) {
    std::cout << "> ";
    std::getline(std::cin, user_input);
    if(user_input.substr(0,6) == "/join " || user_input == "/join") {
      if(user_input == "/join") {
        std::cerr << "/join command needs room name\n";
        continue;
      }
      client_message.tag = TAG_JOIN;
      client_message.data = user_input.substr(6);
      if(client_message.data == "") {
        std::cerr << "/join command needs room name\n";
        continue;
      } else {
        if(!send_and_respond(connection, client_message, server_response)) {
          return 3;
        }
        if(server_response.tag == TAG_ERR) {
          std::cerr << server_response.data << "\n";
        }
        in_room = true;
      } 
    } else if(user_input == "/leave") {
      client_message.tag = TAG_LEAVE;
      client_message.data = "";
      if(!in_room) {
        std::cerr << "not in a room\n";
        continue;
      }
      if(!send_and_respond(connection, client_message, server_response)) {
        return 3;
      }
      if(server_response.tag == TAG_ERR) {
        std::cerr << server_response.data << "\n";
      }
      in_room = false;
    } else if(user_input == "/quit") {
      client_message.tag = TAG_QUIT;
      client_message.data = "";
      quit = true;
      if(!send_and_respond(connection, client_message, server_response)) {
        return 3;
      }
      if(server_response.tag == TAG_ERR) {
        std::cerr << server_response.data << "\n";
      }
    } else if(user_input.at(0) == '/') {
      std::cerr << "Unknown command " << user_input << "\n";
      continue;
    } else {
      client_message.tag = TAG_SENDALL;
      client_message.data = user_input;
      if(!in_room) {
        std::cerr << "not in a room\n";
        continue;
      }
      if(!send_and_respond(connection, client_message, server_response)) {
        if(connection.get_last_result() != Connection::INVALID_MSG) {
          return 3;
        }
      }
      if(server_response.tag == TAG_ERR) {
        std::cerr << server_response.data << "\n";
      }
    }
  }

  return 0;
}

bool send_and_respond(Connection &connection, Message &client_message, Message &server_response) {
  if(!connection.send(client_message)) {
    std::cerr << "Client Transmission Error\n";
    return false;
  }
  if(!connection.receive(server_response)) {
    std::cerr << "Server Transmission Error\n";
    return false;
  }
  return true;
}