#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

bool send_and_respond(Connection &connection, Message &client_message, Message &server_response);

int main(int argc, char **argv) {
  if (argc != 5) {
    std::cerr << "Usage: ./receiver [server_address] [port] [username] [room]\n";
    return 1;
  }

  std::string server_hostname = argv[1];
  int server_port = std::stoi(argv[2]);
  std::string username = argv[3];
  std::string room_name = argv[4];

  Connection conn;

  // TODO: connect to server
  Connection connection;
  connection.connect(server_hostname, server_port);
  if(!connection.is_open()) {
    std::cerr << "Failed to open connection" << "\n";
    return 2;
  }
  // TODO: send rlogin and join messages (expect a response from
  //       the server for each one)
  Message server_response_rlogin;
  Message server_response_join;
  Message client_message_rlogin(TAG_RLOGIN, username);
  Message client_message_join(TAG_JOIN, room_name);
  if(!send_and_respond(connection, client_message_rlogin, server_response_rlogin)) {
    return 3;
  }
  if(!send_and_respond(connection, client_message_join, server_response_join)) {
    return 3;
  }
  if(server_response_rlogin.tag == TAG_ERR) {
    std::cerr << server_response_rlogin.data << "\n";
    return 4;
  } 
  if(server_response_join.tag == TAG_ERR) {
    std::cerr << server_response_join.data << "\n";
    return 4;
  } 
  // TODO: loop waiting for messages from server
  //       (which should be tagged with TAG_DELIVERY)
  std::string server_data;
  bool quit = false;
  bool message_received;
  bool in_room = false;
  while (!quit) {
    Message server_response;
    message_received = connection.receive(server_response);
    if(message_received != 0) {
      std::cerr << "Server Transmission Error\n";
      return 3;
    }
    if (server_response.data.length() == 0) {
      quit = true;
      continue;
    }
    server_data = server_response.data;
    std::string delim = ":";
    std::string room_name = server_data.substr(0, server_data.find(delim));
    server_data.erase(0,server_data.find(delim) + 1);
    std::string name = server_data.substr(0,server_data.find(delim));
    server_data.erase(0,server_data.find(delim) + 1);
    std::string message = server_data;
    server_response.tag = TAG_DELIVERY;
    std::cout << name << ": " << message ;
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
