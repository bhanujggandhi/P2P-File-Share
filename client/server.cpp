#include "utils.h"

/// @brief Function to initialise the server
/// @param arg
/// @return
void* server_function(void* arg) {
    // Parse IP:PORT
    char* ipport = (char*)arg;
    vector<string> ipportsplit;
    string ipportstr(ipport);
    splitutility(ipportstr, ':', ipportsplit);

    int portno = stoi(ipportsplit[1]);

    int server_socket;
    check(server_socket = socket(AF_INET, SOCK_STREAM, 0),
          "ERROR: Socket Cannot be Opened!");

    struct sockaddr_in server_addr;
    bzero((char*)&server_addr, sizeof(server_addr));

    // int portno = SERVERPORT;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(portno);

    check(bind(server_socket, (struct sockaddr*)&server_addr,
               sizeof(server_addr)),
          "ERROR: on binding");

    check(listen(server_socket, 100), "ERROR: Listen Failed");

    struct sockaddr_in client_addr;
    socklen_t clilen = sizeof(client_addr);

    pthread_t th[THREAD_POOL_SIZE];
    pthread_mutex_init(&mutexQueue, NULL);
    pthread_cond_init(&condQueue, NULL);
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        check(pthread_create(&th[i], NULL, start_thread, NULL),
              "Failed to create the thread");
    }

    while (1) {
        printf("Waiting for connections...\n");

        int client_socket;
        check(client_socket = accept(server_socket,
                                     (struct sockaddr*)&client_addr, &clilen),
              "ERROR: on Accept");

        printf("Server: Got connection from %s:%d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        int* pclient = (int*)malloc(sizeof(int));
        *pclient = client_socket;

        pthread_mutex_lock(&mutexQueue);
        thread_queue.push(pclient);
        pthread_cond_signal(&condQueue);
        pthread_mutex_unlock(&mutexQueue);
    }

    shutdown(server_socket, SHUT_RDWR);
    pthread_mutex_destroy(&mutexQueue);
    pthread_cond_destroy(&condQueue);
}

/// @brief Util to connect a client to the tracker
/// @param filepath
void connecttotracker(const char* filepath) {
    char resolvedpath[PATH_MAX];
    if (realpath(filepath, resolvedpath) == NULL) {
        printf("ERROR: bad path %s\n", resolvedpath);
        return;
    }

    std::ifstream file(resolvedpath);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            vector<string> ipport;
            splitutility(line, ':', ipport);

            int portno = stoi(ipport[1]);
            int server_socket;
            if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                printf("Error: Opening socket\n");
                exit(1);
            }

            struct sockaddr_in server_address;
            bzero((char*)&server_address, sizeof(server_address));

            server_address.sin_family = AF_INET;

            server_address.sin_addr.s_addr = INADDR_ANY;

            server_address.sin_port = htons(portno);

            if (connect(server_socket, (struct sockaddr*)&server_address,
                        sizeof(server_address)) < 0) {
                printf("Could not connect\n");
                close(server_socket);
                continue;
            }
            connection_info.first = ipport[0];
            connection_info.second = stoi(ipport[1]);
            printf("Connected successfully\n");
            file.close();
            close(server_socket);

            return;
        }

        file.close();
        printf("No tracker online\n");
        exit(1);
    }
}
