#include<iostream>
#include<unistd.h>
#include<arpa/inet.h>
#include<cstring>

#define PORT 8080

int main(){
  int server_fd = socket(AF_INET,SOCK_STREAM,,0);
  if(server_fd == -1){
    std::cerr<<"Socket creation failed\n";
    return -1;
  }
  sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANT;
  address.sin_port = htons(PORT);

  bind(server_fd,(sockaddr*)&address,sizeof(address));
  listen(server_fd,5);

  std::cout<<"Server Running on port"<< PORT<<std::endl;
  
  while(true){
    int client_socket = accept(server_fd,nullptr,nullptr);
    std::cout<<"Client connected\n";
    
    char buffer[1024] = {0};
    read(client_server,buffer,1024);

    std::cout<<"Request:\n"<<buffer<<std::endl;

    std::string response = "Hello World\n";
    send(client_socket,response.c_str(),response.size(),0);

    close(client_socket);
  }
}
