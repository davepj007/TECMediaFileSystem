//
// Created by davepj007 on 25/11/18.
//

#ifndef TEC_MEDIA_FILE_SYSTEM_CLIENT_H
#define TEC_MEDIA_FILE_SYSTEM_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <arpa/inet.h>
#include <fstream>
#include <iomanip>
#include "nlohmann/json.hpp"

class Client{
public:
    Client();
    void error(const char *msg);
    void connectCall(int caso, std::string path);

};

#endif //TEC_MEDIA_FILE_SYSTEM_CLIENT_H
