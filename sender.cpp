#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

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
  if(!connection.send(client_message)) {
    // Handle when message was unable to be sent;
  }
  if(!connection.receive(server_response)) {
    // Handle when message was unable to be recieved;
  }
  if(server_response.tag == TAG_ERR) {
    std::cerr << server_response.data << "\n";
    return 3;
  } 

  // TODO: loop reading commands from user, sending messages to
  //       server as appropriate
  std::string user_input;
  bool quit = false;
  while(!quit) {
    std::getline(std::cin, user_input);
    if(user_input.substr(0,6) == "/join ") {
      client_message.tag = TAG_JOIN;
      client_message.data = user_input.substr(6);
      if(!connection.send(client_message)) {
        // Handle when message was unable to be sent;
      }
      if(!connection.receive(server_response)) {
        // Handle when message was unable to be recieved;
      }
      if(server_response.tag == TAG_ERR) {
        std::cerr << server_response.data << "\n";
      } 
    } else if(user_input == "/leave") {
      client_message.tag = TAG_LEAVE;
      client_message.data = "";
      if(!connection.send(client_message)) {
        // Handle when message was unable to be sent;
      }
      if(!connection.receive(server_response)) {
        // Handle when message was unable to be recieved;
      }
      if(server_response.tag == TAG_ERR) {
        std::cerr << server_response.data << "\n";
      } 
    } else if(user_input == "/quit") {
      client_message.tag = TAG_QUIT;
      client_message.data = "";
      if(!connection.send(client_message)) {
        // Handle when message was unable to be sent;
      }
      if(!connection.receive(server_response)) {
        // Handle when message was unable to be recieved;
      }
      if(server_response.tag == TAG_ERR) {
        std::cerr << server_response.data << "\n";
      }
      quit = true;
    } else if(user_input.at(0) == '/') {
      std::cerr << "Invalid command\n";
    } else {
      client_message.tag = TAG_SENDALL;
      client_message.data = user_input;
      if(!connection.send(client_message)) {
        // Handle when message was unable to be sent;
      }
      if(!connection.receive(server_response)) {
        // Handle when message was unable to be recieved;
      }
      if(server_response.tag == TAG_ERR) {
        std::cerr << server_response.data << "\n";
      }
    }
  }

  return 0;
}
