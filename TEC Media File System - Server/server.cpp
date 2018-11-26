//
// Created by davepj007 on 16/11/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <wait.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <json-c/json.h>

#define portNo 9898

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void error(const char *msg) {
    perror(msg);
    exit(1);
}

char* readParts(std::ifstream& part){
    char* buffer = new char[255];
    int pointer = 0;
    std::ifstream::pos_type nSize = part.tellg();
    if(part.is_open()) {
        part.seekg(0, std::ios::beg);
        while (part.tellg() < nSize) {
            part.seekg(pointer, std::ios::beg);
            part.read(buffer, 255);
            pointer += 1;
        }
    }
    return buffer;
}

void parityBit(const char *fileName){
    std::stringstream ss;
    std::string sName;
    ss << fileName;
    ss >> sName;

    std::string path = "/home/davepj007/Desktop/";
    std::ofstream parity((path+"DiskNode4/"+sName+".parity").c_str());

    std::ifstream part1((path+"DiskNode1/"+sName+".part1").c_str(), std::ios_base::ate|std::ios::binary|std::ios::in);
    char* buff1 = readParts(part1);
    part1.close();
    std::ifstream part2((path+"DiskNode2/"+sName+".part2").c_str(), std::ios_base::ate|std::ios::binary|std::ios::in);
    char* buff2 = readParts(part2);
    part2.close();
    std::ifstream part3((path+"DiskNode3/"+sName+".part3").c_str(), std::ios_base::ate|std::ios::binary|std::ios::in);
    char* buff3 = readParts(part3);
    part3.close();
    for (int i = 0; i < sizeof(buff1); i++) {
        char c = (buff1[i]^buff2[i]^buff3[i]);
        printf("%u / ", c);
        parity.write(&c, sizeof(char));
    }
    parity.close();
}

void splitFile(std::ifstream& fSource, const char *fileName){
    //Variables
    std::ostringstream sStringer;    /// Contiene el nombre de los archivos en los que se dividira
    char * buffer;
    std::ifstream::pos_type nSize = fSource.tellg();   /// Tamano del archivo a dividir
    std::string sExtension = strstr(fileName,".");
    int nGetPointer = 0;
    int nChunkSize = nSize/3 + 1;
    int nLastChunkSize = nChunkSize;
    int nPartNumber = 1;
    std::string sDestinationFile;

    if (fSource.is_open()) {

        fSource.seekg(0, std::ios::beg);

        while (fSource.tellg() < nSize) {
            fSource.seekg(nGetPointer, std::ios::beg);

            if (nGetPointer + nChunkSize > nSize) {
                while (nGetPointer + nLastChunkSize > nSize) {
                    nLastChunkSize--;
                }
                buffer = new char[nLastChunkSize];
                fSource.read(buffer, nLastChunkSize);
            }
            else {
                buffer = new char[nChunkSize];
                fSource.read(buffer, nChunkSize);
            }

            sDestinationFile = fileName;
            sDestinationFile.append(".part");
            sStringer.str("");
            sStringer << nPartNumber;
            sDestinationFile.append(sStringer.str());

            std::string path = "/home/davepj007/Desktop/";
            switch (nPartNumber){
                case 1: path += "DiskNode1/"; break;
                case 2: path += "DiskNode2/"; break;
                case 3: path += "DiskNode3/"; break;
            }
            std::ofstream fDestination((path + sDestinationFile).c_str());
            fDestination.write(buffer, nLastChunkSize);
            fDestination.close();
            nGetPointer += nChunkSize;
            nPartNumber += 1;
        }
    }
    parityBit(fileName);
}

void validateJson(std::ifstream& jFile){
    char* buffer;
    jFile >> buffer;
    json_object* jsonObj;
    json_tokener_parse(buffer);
    json_object_object_foreach(jsonObj, key, value){
        if(key == "Caso"){
            if(json_object_get_string(value) == "0"){

            }
        }
    }
}

void *socketThread(void *arg){
    int newServerSocket = *((int *) arg);
    std::ofstream fp("json_received.json", std::ios::out | std::ios::binary);
    char* buffer = new char[256];

    pthread_mutex_lock(&lock);
    while(1) {
        ssize_t bytesReceived = read(newServerSocket, buffer, sizeof(buffer));
        if (bytesReceived < 0) perror("[-]Error on receiving");
        if (bytesReceived == 0) break;

        fp.write(buffer, bytesReceived);
    }
    fp.close();


    std::ifstream fJson("json_received.json");

    /*std::ifstream fSource;
    fSource.open("vid_received.mp4", std::ios_base::ate|std::ios::binary|std::ios::in);
    std::string fileName = "vid_received.mp4";
    if(fSource.is_open()) splitFile(fSource, fileName.c_str());
    fSource.close();
    std::remove("vid_received.mp4");*/

    pthread_mutex_unlock(&lock);
    sleep(1);
    printf("[+]The JSON File has been received succesfully.\n");

    close(newServerSocket);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    /// Declara los socketdel server, el numero de puerto y el buffer
    int ServerSocket, newServerSocket;
    /// Declara la estructura que contiene la direccion del servidor y del cliente
    struct sockaddr_in serv_addr;
    struct sockaddr_storage serverStorage;
    socklen_t  addr_size;

    /// Inicializa el socket file descriptor
    ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(ServerSocket < 0){
        error("[-]Error opening socket.");
    }

    /// Define los valores de la estructura del servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portNo);

    /// Establece todos los bits del limite a cero
    memset(serv_addr.sin_zero, '\0', sizeof(serv_addr.sin_zero));

    /// Ejecuta el enlace del servidor
    if(bind(ServerSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        error("[-]Binding Failed.");
    }

    /// Define cuantos clientes puede escuchar simultaneamente el servidor
    if(listen(ServerSocket, 4) == 0) printf("[+]Listening...\n");
    else printf("[-]Error on listening.\n");

    pthread_t tid[4];
    int i = 0;
    int pid_c = 0;
    while (1) {
        /// Llamada de aceptacion, crea un nuevo socket para la conexion entrante
        addr_size = sizeof(serverStorage);
        newServerSocket = accept(ServerSocket, (struct sockaddr *) &serverStorage, &addr_size);
        if((pid_c < 4) && (newServerSocket > 0)){
            pid_c++;
            printf("[+]Client #%i accepted. \n", pid_c);

            if (pthread_create(&tid[i], NULL, socketThread, &newServerSocket) != 0) {
                printf("[-]Failed to create thread\n");
            }
            if (i >= 3) {
                i = 0;
                while (i < 4) {
                    pthread_join(tid[i++], NULL);
                }
                i = 0;
            }
        }else{
            pid_c = 5;
            printf("[-]Client #%i rejected.\n", pid_c);
            close(newServerSocket);
        }
    }
    return 0;

    /*FILE *fp;
    pid_t pid;
    while(1){
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if(newsockfd < 0){
            exit(1);
        }

        pid = fork();
        if(pid == 0){
            close(sockfd);
            fp = fopen("/home/davepj007/Desktop/DiskNode1/vid_received.mp4", "wb");

            while(1) {
                ssize_t bytesReceived = recv(newsockfd, buffer, sizeof(buffer), 0);
                if (bytesReceived < 0) perror("Error on receiving");
                if (bytesReceived == 0) break;

                size_t write = fwrite(buffer, 1,(size_t) bytesReceived, fp);
                if (write != (size_t) bytesReceived) {
                    perror("Error on writing");
                    break;
                }
            }
            printf("The video has been received succesfully. It is saved by the name: vid_received.mp4\n");
        }
    }

    fclose(fp);
    close(newsockfd);
    close(sockfd);*/
}
