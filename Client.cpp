//
// Created by davepj007 on 16/11/18.
//
#include "Client.h"

#define portNo 9898

void error(const char *msg){
    perror(msg);
    exit(1);
}

void * Client::clientThread(void *arg){
    std::ifstream f;
    int clientSocket;
    struct sockaddr_in serv_Addr;
    socklen_t addr_size;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    serv_Addr.sin_family = AF_INET;
    serv_Addr.sin_port = htons(portNo);
    serv_Addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serv_Addr.sin_zero, '\0', sizeof(serv_Addr.sin_zero));

    std::ofstream jsonFile("/home/davepj007/CLionProjects/TEC Media File System/caso.json");
    json_object* jsonObj = json_object_new_object();
    json_object* jString = json_object_new_string("0");
    json_object_object_add(jsonObj,"Caso", jString);
    jsonFile << std::setw(4) << json_object_get_string(jsonObj) << std::endl;
    jsonFile.close();

    addr_size = sizeof(serv_Addr);
    if(connect(clientSocket, (struct sockaddr *) &serv_Addr, addr_size) < 0) {
        error("[-]Connection Failed");
    }
    printf("[+]Connected to server...\n");

    f.open("caso.json", std::ios::binary);
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
    printf("[+]The JSON File has been succesfully sent.\n");
    close(clientSocket);
    f.close();

    pthread_exit(NULL);
}

void Client::connectCall(){
    int i = 0;
    pthread_t tid[4];
    while(i < 1){
        if(pthread_create(&tid[i], NULL, clientThread, NULL) != 0){
            printf("Failed to create thread\n");
        }
        i++;
    }
    sleep(20);
    i = 0;
    while(i < 1){
        pthread_join(tid[i++], NULL);
    }

    /*int sockfd, portNo;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[255];
    if(argc < 3){
        fprintf(stderr, "[-]Usage %s hostname port\n", argv[0]);
        exit(1);
    }

    portNo = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
        error("[-]Error opening socket");

    server = gethostbyname(argv[1]);
    if(server == NULL){
        fprintf(stderr, "[-]Error, no such host");
    }

    memset(&serv_addr, '\0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portNo);

    if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        error("[-]Connection Failed");
    }
    printf("[+]Connected to server...\n");

    FILE *f;
    f = fopen("SG.mp4", "rb");

    while(1){
        ssize_t  bytesRead = fread(buffer, 1,sizeof(buffer), f);
        if(bytesRead <= 0) break;
        if(send(sockfd, buffer, bytesRead, 0) != bytesRead) {
            perror("[-]Error on sending");
            break;
        }
    }
    printf("[+]The video has been succesfully sent.\n");

    while(1){
        scanf("%s", &buffer[0]);
        if(strcmp(buffer, "exit") == 0){
            close(sockfd);
            printf("[+]Disconnected from server...\n");
            exit(1);
        }
    }

    close(sockfd);
    fclose(f);
    return 0;*/
}