#include "utils.h"

// Concurrency Control
pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;
queue<int*> thread_queue;

pthread_mutex_t mutexDownQueue;
pthread_cond_t condDownQueue;
queue<DownloadData*> threadDownQueue;

// User Data
pair<string, int> connection_info;
User currUser;
unordered_map<string, FileStr*> filetomap;

int main(int argc, char* argv[]) {
    cout << "\x1B[2J\x1B[H";
    signal(SIGPIPE, SIG_IGN);
    if (argc != 3) {
        cout << "USAGE: ./client IP:PORT <path-to-trackerinfo.txt>" << endl;
        exit(1);
    }

    connecttotracker(argv[2]);
    string str(argv[1]);
    currUser.address = str;
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, server_function, (void*)str.c_str());

    cout << "========================================= " << endl;
    cout << "Available Commands:- " << endl;
    cout << "1. create_user <user_id> <password>" << endl;
    cout << "2. login <user_id> <password>" << endl;
    cout << "3. create_group <group_id>" << endl;
    cout << "4. join_group <group_id>" << endl;
    cout << "5. leave_group <group_id>" << endl;
    cout << "6. list_requests <group_id>" << endl;
    cout << "7. accept_request <group_id> <user_id>" << endl;
    cout << "8. list_groups" << endl;
    cout << "9. list_files <group_id>" << endl;
    cout << "10. upload_file <file_path> <group_id>" << endl;
    cout << "11. download_file <group_id> <file_name> <destination_path>"
         << endl;
    cout << "12. logout" << endl;
    cout << "13. show_downloads" << endl;
    cout << "14. stop_share <group_id> <file_name>" << endl;
    cout << "========================================= " << endl;

    while (1) {
        char request[BUFSIZ];
        fflush(stdin);
        memset(request, 0, BUFSIZ);
        fgets(request, BUFSIZ, stdin);
        vector<string> parsedReq;
        string req(request);
        if (req.empty()) continue;
        int CLIENTPORT = connection_info.second;
        if (CLIENTPORT == 0) continue;
        client_function(req.c_str(), CLIENTPORT);
    }
}
