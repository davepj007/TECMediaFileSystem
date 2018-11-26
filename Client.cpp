//
// Created by davepj007 on 16/11/18.
//
#include "Client.h"

#define portNo 9898

using json = nlohmann::json;

void Client::error(const char *msg){
    perror(msg);
    exit(1);
}

void Client::connectCall(int caso, std::string path){
    std::ifstream f;
    int clientSocket;
    struct sockaddr_in serv_Addr;
    socklen_t addr_size;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    serv_Addr.sin_family = AF_INET;
    serv_Addr.sin_port = htons(portNo);
    serv_Addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serv_Addr.sin_zero, '\0', sizeof(serv_Addr.sin_zero));

    addr_size = sizeof(serv_Addr);
    if(connect(clientSocket, (struct sockaddr *) &serv_Addr, addr_size) < 0) {
        error("[-]Connection Failed");
    }
    printf("[+]Connected to server...\n");

    send(clientSocket, &caso, sizeof(caso),0);

    char* buff = new char[255];
    switch(caso){
        case 0: {
            std::ofstream fp("/home/davepj007/CLionProjects/TEC Media File System/videos.json");
            while (1) {
                ssize_t bytesReceived = read(clientSocket, buff, 255);
                std::cout << "Bytes received: " << bytesReceived << std::endl;
                if (bytesReceived < 0) perror("[-]Error on receiving");
                if (bytesReceived == 0) break;

                fp.write(buff, bytesReceived);
            }
            fp.close();
        }break;
        case 1:{
            f.open(path, std::ios::binary);
            f.seekg(0, std::ifstream::beg);
            while(!f.eof()) {
                char *buffer = new char[255];
                int BytesSent = 0;
                int BytesIndex = 0;
                f.read(buffer, 255);
                int BytesLeft = f.gcount();
                while(BytesLeft != 0){
                    BytesSent = send(clientSocket, &buffer[BytesIndex], BytesLeft, 0);
                    BytesLeft -= BytesSent;
                    BytesIndex +=BytesSent;
                }
            }
            printf("[+]The video has been succesfully sent.\n");
            f.close();
        }break;
        case 2: {
            send(clientSocket, path.data(), path.size(), 0);
        }break;
        case 3: break;
        case 4: break;
        case 5: break;
    }
    std::cout << "[+] JSON File has been received succesfully" << std::endl;
    close(clientSocket);
}