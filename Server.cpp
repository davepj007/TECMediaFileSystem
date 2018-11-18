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

FILE *fp;
char buffer[255];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void *socketThread(void *arg){
    int newServerSocket = *((int *) arg);
    fp = fopen("/home/davepj007/Desktop/DiskNode1/vid_received.mp4", "wb");

    pthread_mutex_lock(&lock);
    while(1) {
        ssize_t bytesReceived = recv(newServerSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived < 0) perror("Error on receiving");
        if (bytesReceived == 0) break;

        size_t write = fwrite(buffer, 1,(size_t) bytesReceived, fp);
        if (write != (size_t) bytesReceived) {
            perror("Error on writing");
            break;
        }
    }
    pthread_mutex_unlock(&lock);
    sleep(1);
    printf("The video has been received succesfully. It is saved by the name: vid_received.mp4\n");
    close(newServerSocket);
    fclose(fp);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    /// Verifica que se ingrese el numero de puerto como parametro
    if(argc < 2){
        fprintf(stderr, "Port no provided, Program terminated\n");
        exit(1);
    }
    /// Declara los socketdel server, el numero de puerto y el buffer
    int ServerSocket, newServerSocket, portNo;
    /// Declara la estructura que contiene la direccion del servidor y del cliente
    struct sockaddr_in serv_addr;
    struct sockaddr_storage serverStorage;
    socklen_t  addr_size;

    /// Inicializa el socket file descriptor
    ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(ServerSocket < 0){
        error("Error opening socket.");
    }
    portNo = atoi(argv[1]);

    /// Define los valores de la estructura del servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portNo);

    /// Establece todos los bits del limite a cero
    memset(serv_addr.sin_zero, '\0', sizeof(serv_addr.sin_zero));

    /// Ejecuta el enlace del servidor
    if(bind(ServerSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        error("Binding Failed.");
    }

    /// Define cuantos clientes puede escuchar simultaneamente el servidor
    if(listen(ServerSocket, 4) == 0) printf("[+]Listening...\n");
    else printf("Error\n");

    pthread_t tid[5];
    int i = 0;
    while (1) {
        /// Llamada de aceptacion, crea un nuevo socket para la conexion entrante
        addr_size = sizeof(serverStorage);
        newServerSocket = accept(ServerSocket, (struct sockaddr *) &serverStorage, &addr_size);
        int pid_c = 0;

        if (pthread_create(&tid[i], NULL, socketThread, &newServerSocket) != 0) {
            printf("[-] Failed to create thread\n");
        }
        if (i >= 3) {
            i = 0;
            while (i < 4) {
                pthread_join(tid[i++], NULL);
            }
            i = 0;
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
