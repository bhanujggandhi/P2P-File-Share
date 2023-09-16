#include "utils.h"

// Globals
pair<string, int> connectioninfo;
pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;
queue<int*> thread_queue;
unordered_map<string, User*> usertomap;
unordered_map<string, Group*> grouptomap;
unordered_map<string, bool> loggedin_map;
unordered_map<string, FileStr*> filetomap;

int main(int argc, char* argv[]) {

    if (argc != 3) {
        cout << "USAGE: ./tracker <path-to-trackerinfo.txt> <tracker-number>"
             << endl;
        exit(1);
    }

    getipport(argv[1], stoi(argv[2]));
    cout << "\x1B[2J\x1B[H";

    printf("Connected to: %s on port %d\n", connectioninfo.first.c_str(),
           connectioninfo.second);

    pthread_t server_thread;
    pthread_create(&server_thread, NULL, server_function, NULL);

    while (1) {
        char request[255];
        memset(request, 0, 255);
        fflush(stdin);
        fgets(request, 255, stdin);
        vector<string> parsedReq;
        string req(request);
        if (req == "quit\n") {
            exit(0);
        }
        splitutility(req, ':', parsedReq);
        int CLIENTPORT = atoi(parsedReq[0].c_str());
        if (CLIENTPORT == 0) continue;
        client_function(parsedReq[1].c_str(), CLIENTPORT);
    }
}
