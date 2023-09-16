#include "utils.h"

/// @brief Utils to log error message
/// @param msg
void err(const char* msg) { printf("%s\n", msg); }

/// @brief Util to check if the status if message is error
/// @param status
/// @param msg
void check(int status, string msg) {
    if (status < 0) {
        err(msg.c_str());
    }
}

/// @brief Utils to compare size of the peers
/// @param p1
/// @param p2
/// @return
bool cmp(DownloadData* p1, DownloadData* p2) {
    return p1->peers.size() < p2->peers.size();
}

/// @brief Util to destructure the message received
/// @param str
/// @param del
/// @param arr
void splitutility(string str, char del, vector<string>& arr) {
    string temp = "";

    for (int i = 0; i < str.size(); i++) {
        if (str[i] != del)
            temp += str[i];
        else {
            arr.push_back(temp);
            temp = "";
        }
    }

    arr.push_back(temp);
}

/// @brief Client function to handle all the functionalities
/// @param request
/// @param CLIENTPORT
void client_function(const char* request, int CLIENTPORT) {
    int portno = CLIENTPORT;
    int server_socket;
    check(server_socket = socket(AF_INET, SOCK_STREAM, 0),
          "ERROR: Opening PORT");

    struct sockaddr_in server_address;
    bzero((char*)&server_address, sizeof(server_address));

    server_address.sin_family = AF_INET;

    server_address.sin_addr.s_addr = INADDR_ANY;

    server_address.sin_port = htons(portno);

    if (connect(server_socket, (struct sockaddr*)&server_address,
                sizeof(server_address)) < 0) {
        err("Error Connecting");
        return;
    }

    vector<string> reqarr;
    string req(request);
    if (req.length() > 1) {
        req.pop_back();
    }
    splitutility(req, ' ', reqarr);

    char buffer[BUFSIZ];
    memset(buffer, 0, BUFSIZ);
    if (reqarr[0] == "create_user") {
        if (reqarr.size() != 3) {
            string msg =
                "1:Invalid Number of arguments for Login\nUSAGE - create_user "
                "<user_id> <password>\n";
            printf("%s", msg.c_str());
            return;
        }

        req += " " + currUser.address + "\n";
        int n = write(server_socket, req.c_str(), req.size());
        if (n < 0) err("ERROR: writing to socket");

        size_t size;
        if (read(server_socket, buffer, BUFSIZ) < 0) {
            printf("Couldn't get response from the tracker\n");
            return;
        }

        vector<string> resarr;
        string res(buffer);
        splitutility(res, ':', resarr);
        if (resarr[0] == "2") {
            currUser.userid = resarr[1];
            currUser.loggedin = true;
            printf("%s\n", resarr[2].c_str());
        } else if (resarr[0] == "1") {
            printf("%s\n", resarr[1].c_str());
        }
    } else if (reqarr[0] == "login") {
        if (reqarr.size() != 3) {
            string msg =
                "1:Invalid Number of arguments for Login\nUSAGE - login "
                "<user_id> <password>\n";
            printf("%s", msg.c_str());
            return;
        }
        if (currUser.loggedin) {
            printf("You are already logged in as %s\nPlease logout first!\n",
                   currUser.userid.c_str());
            return;
        }
        req += " " + currUser.address + "\n";
        int n = write(server_socket, req.c_str(), req.size());
        if (n < 0) err("ERROR: writing to socket");

        size_t size;
        if (read(server_socket, buffer, BUFSIZ) < 0) {
            printf("Couldn't get response from the tracker\n");
            return;
        }

        vector<string> resarr;
        string res(buffer);
        splitutility(res, ':', resarr);

        if (resarr[0] == "2") {
            currUser.userid = resarr[1];
            currUser.loggedin = true;
            printf("%s\n", resarr[2].c_str());
        } else if (resarr[0] == "1") {
            printf("%s\n", resarr[1].c_str());
        }
    } else if (reqarr[0] == "create_group") {
        if (reqarr.size() < 2) {
            printf("Invalid number of arguments\n");
            printf("USAGE: create_group <group_id>\n");
            return;
        }

        if (!currUser.loggedin) {
            printf("You are not logged! Please login\n");
            return;
        }
        string preq = req + " " + currUser.userid + "\n";

        int n = write(server_socket, preq.c_str(), preq.size());
        if (n < 0) err("ERROR: writing to socket");

        size_t size;
        if (read(server_socket, buffer, BUFSIZ) < 0) {
            printf("Couldn't get response from the tracker\n");
            return;
        }

        vector<string> resarr;
        string res(buffer);
        splitutility(res, ':', resarr);

        if (resarr[0] == "2") {
            printf("%s\n", resarr[2].c_str());
        } else if (resarr[0] == "1") {
            printf("%s\n", resarr[1].c_str());
        }
    } else if (reqarr[0] == "join_group") {
        if (reqarr.size() < 2) {
            printf("Invalid number of arguments\n");
            printf("USAGE: join_group <group_id>\n");
            return;
        }

        if (!currUser.loggedin) {
            printf("You are not logged! Please login\n");
            return;
        }

        string preq = req + " " + currUser.userid + "\n";

        int n = write(server_socket, preq.c_str(), preq.size());
        if (n < 0) err("ERROR: writing to socket");

        size_t size;
        if (read(server_socket, buffer, BUFSIZ) < 0) {
            printf("Couldn't get response from the tracker\n");
            return;
        }

        vector<string> resarr;
        string res(buffer);
        splitutility(res, ':', resarr);

        if (resarr[0] == "2") {
            printf("[%s]: %s\n", resarr[1].c_str(), resarr[2].c_str());
        } else if (resarr[0] == "1") {
            printf("%s\n", resarr[1].c_str());
        }
    } else if (reqarr[0] == "leave_group") {
        if (reqarr.size() < 2) {
            printf("Invalid number of arguments\n");
            printf("USAGE: join_group <group_id>\n");
            return;
        }

        if (!currUser.loggedin) {
            printf("You are not logged! Please login\n");
            return;
        }

        string preq = req + " " + currUser.userid + "\n";

        int n = write(server_socket, preq.c_str(), preq.size());
        if (n < 0) err("ERROR: writing to socket");

        size_t size;
        if (read(server_socket, buffer, BUFSIZ) < 0) {
            printf("Couldn't get response from the tracker\n");
            return;
        }

        vector<string> resarr;
        string res(buffer);
        splitutility(res, ':', resarr);

        if (resarr[0] == "2") {
            printf("[%s]: %s\n", resarr[1].c_str(), resarr[2].c_str());
        } else if (resarr[0] == "1") {
            printf("%s\n", resarr[1].c_str());
        }
    } else if (reqarr[0] == "list_requests") {
        if (reqarr.size() < 2) {
            printf("Invalid number of arguments\n");
            printf("USAGE: list_requests <group_id>\n");
            return;
        }

        if (!currUser.loggedin) {
            printf("You are not logged! Please login\n");
            return;
        }

        string preq = req + " " + currUser.userid + "\n";

        int n = write(server_socket, preq.c_str(), preq.size());
        if (n < 0) err("ERROR: writing to socket");

        size_t size;
        if (read(server_socket, buffer, BUFSIZ) < 0) {
            printf("Couldn't get response from the tracker\n");
            return;
        }

        vector<string> resarr;
        string res(buffer);
        splitutility(res, ':', resarr);

        if (resarr[0] == "2") {
            printf("[%s]: %s\n", resarr[1].c_str(), resarr[2].c_str());
        } else if (resarr[0] == "1") {
            printf("%s\n", resarr[1].c_str());
        }
    } else if (reqarr[0] == "accept_request") {
        if (reqarr.size() != 3) {
            printf("Invalid number of arguments\n");
            printf("USAGE: accept_request <group_id> <user_id>\n");
            return;
        }

        if (!currUser.loggedin) {
            printf("You are not logged! Please login\n");
            return;
        }

        string preq = req + " " + currUser.userid + "\n";

        int n = write(server_socket, preq.c_str(), preq.size());
        if (n < 0) err("ERROR: writing to socket");

        size_t size;
        if (read(server_socket, buffer, BUFSIZ) < 0) {
            printf("Couldn't get response from the tracker\n");
            return;
        }

        vector<string> resarr;
        string res(buffer);
        splitutility(res, ':', resarr);

        if (resarr[0] == "2") {
            printf("[%s]: %s\n", resarr[1].c_str(), resarr[2].c_str());
        } else if (resarr[0] == "1") {
            printf("%s\n", resarr[1].c_str());
        }
    } else if (req == "list_groups") {
        if (reqarr.size() != 1) {
            printf("Invalid number of arguments\n");
            printf("USAGE: list_groups\n");
            return;
        }
        if (!currUser.loggedin) {
            printf("You are not logged! Please login\n");
            return;
        }

        string preq = req + " " + currUser.userid + "\n";

        int n = write(server_socket, preq.c_str(), preq.size());
        if (n < 0) err("ERROR: writing to socket");

        size_t size;
        if (read(server_socket, buffer, BUFSIZ) < 0) {
            printf("Couldn't get response from the tracker\n");
            return;
        }

        vector<string> resarr;
        string res(buffer);
        splitutility(res, ':', resarr);

        if (resarr[0] == "2") {
            printf("[%s]: %s\n", resarr[1].c_str(), resarr[2].c_str());
        } else if (resarr[0] == "1") {
            printf("%s\n", resarr[1].c_str());
        }
    } else if (reqarr[0] == "list_files") {
        if (reqarr.size() != 2) {
            printf("Invalid number of arguments\n");
            printf("USAGE: list_files <group_id>\n");
            return;
        }
        if (!currUser.loggedin) {
            printf("You are not logged! Please login\n");
            return;
        }

        string preq = req + " " + currUser.userid + "\n";

        int n = write(server_socket, preq.c_str(), preq.size());
        if (n < 0) err("ERROR: writing to socket");

        size_t size;
        if (read(server_socket, buffer, BUFSIZ) < 0) {
            printf("Couldn't get response from the tracker\n");
            return;
        }

        vector<string> resarr;
        string res(buffer);
        splitutility(res, ':', resarr);

        if (resarr[0] == "2") {
            printf("[%s]: %s\n", resarr[1].c_str(), resarr[2].c_str());
        } else if (resarr[0] == "1") {
            printf("%s\n", resarr[1].c_str());
        }
    } else if (reqarr[0] == "upload_file") {
        if (reqarr.size() != 3) {
            printf("Invalid number of arguments\n");
            printf("USAGE: upload_file <file_path> <group_id>\n");
            return;
        }

        if (!currUser.loggedin) {
            printf("You are not logged! Please login\n");
            return;
        }
        // string sha = "hello";
        long filesize = getfilesize(reqarr[1]);
        string groupid = reqarr[2];

        char resolvedpath[PATH_MAX];

        if (realpath(reqarr[1].c_str(), resolvedpath) == NULL) {
            printf("ERROR: bad path %s\n", resolvedpath);
            return;
        }

        string resp(resolvedpath);

        vector<string> filenamevec;
        splitutility(resp, '/', filenamevec);

        FileStr* currFile = new FileStr();
        currFile->filepath = resolvedpath;
        currFile->filesize = filesize;
        currFile->chunks = ceil((double)filesize / CHUNKSIZE);
        currFile->owner = true;
        currFile->groupid = reqarr[2];

        vector<bool>
            tempchunkpresent(currFile->chunks, false);
        for (int i = 0; i < currFile->chunks; i++) {
            tempchunkpresent.push_back(false);
        }

        string concatenatedSHA = "";
        long off = 0;
        long flsz = filesize;
        while (flsz > 0) {
            string currsha = generateSHA(resolvedpath, off);
            // string currsha = "hello" + to_string(flsz);
            currFile->chunksha.push_back(
                {ceil((double)off / CHUNKSIZE), currsha});
            tempchunkpresent[ceil((double)off / CHUNKSIZE)] = true;
            concatenatedSHA += currsha;
            flsz -= CHUNKSIZE;
            off += CHUNKSIZE;
        }

        concatenatedSHA = generateSHA(resolvedpath, 0);
        currFile->chunkpresent = tempchunkpresent;
        currFile->SHA = concatenatedSHA;

        string preq = reqarr[0] + " " + filenamevec.back() + " " + reqarr[2] +
                      " " + concatenatedSHA + " " + to_string(filesize) + " " +
                      currUser.userid + "\n";

        printf("%s\n", preq.c_str());

        int n = write(server_socket, preq.c_str(), preq.size());
        if (n < 0) err("ERROR: writing to socket");

        size_t size;
        if (read(server_socket, buffer, BUFSIZ) < 0) {
            printf("Couldn't get response from the tracker\n");
            return;
        }

        vector<string> resarr;
        string res(buffer);
        splitutility(res, ':', resarr);

        if (resarr[0] == "2") {
            printf("[%s]: %s\n", resarr[1].c_str(), resarr[2].c_str());
            if (filetomap.find(filenamevec.back()) != filetomap.end()) {
                string fname = filenamevec.back();
                filetomap[fname] = currFile;
            } else {
                string fname = filenamevec.back();
                filetomap.insert({fname, currFile});
                auto temp = filetomap[fname];
            }
        } else if (resarr[0] == "1") {
            printf("%s\n", resarr[1].c_str());
        }
    } else if (reqarr[0] == "ufile") {
        if (reqarr.size() != 3) {
            printf("Invalid number of arguments\n");
            printf("USAGE: upload_file <file_path> <group_id>\n");
            return;
        }

        if (!currUser.loggedin) {
            printf("You are not logged! Please login\n");
            return;
        }
        long filesize = getfilesize(reqarr[1]);
        string groupid = reqarr[2];

        char resolvedpath[PATH_MAX];

        if (realpath(reqarr[1].c_str(), resolvedpath) == NULL) {
            printf("ERROR: bad path %s\n", resolvedpath);
            return;
        }

        string resp(resolvedpath);

        vector<string> filenamevec;
        splitutility(resp, '/', filenamevec);

        FileStr* currFile = new FileStr();
        currFile->filename = filenamevec.back();
        currFile->filepath = resolvedpath;
        currFile->filesize = filesize;
        currFile->chunks = ceil((double)filesize / CHUNKSIZE);
        // currFile->owner = true;
        currFile->groupid = groupid;

        vector<bool>
            tempchunkpresent(currFile->chunks, false);
        for (int i = 0; i < currFile->chunks; i++) {
            tempchunkpresent.push_back(false);
        }

        string concatenatedSHA = "";
        long off = 0;
        long flsz = filesize;
        while (flsz > 0) {
            // string currsha = generateSHA(resolvedpath, off);
            string currsha = "hello" + to_string(flsz);
            currFile->chunksha.push_back(
                {ceil((double)off / CHUNKSIZE), currsha});
            tempchunkpresent[ceil((double)off / CHUNKSIZE)] = true;
            concatenatedSHA += currsha;
            flsz -= CHUNKSIZE;
            off += CHUNKSIZE;
        }

        concatenatedSHA =
            "68b41f1e817a0f2c20693c5aba6d8aab48919e652a300385b1e023f2d25ab9784"
            "2";
        currFile->chunkpresent = tempchunkpresent;
        currFile->SHA = concatenatedSHA;

        string preq = "upload_file " + filenamevec.back() + " " + reqarr[2] +
                      " " + concatenatedSHA + " " + to_string(filesize) + " " +
                      currUser.userid + "\n";

        printf("%s\n", preq.c_str());

        int n = write(server_socket, preq.c_str(), preq.size());
        if (n < 0) err("ERROR: writing to socket");

        size_t size;
        if (read(server_socket, buffer, BUFSIZ) < 0) {
            printf("Couldn't get response from the tracker\n");
            return;
        }

        vector<string> resarr;
        string res(buffer);
        splitutility(res, ':', resarr);

        if (resarr[0] == "2") {
            printf("[%s]: %s\n", resarr[1].c_str(), resarr[2].c_str());
            if (filetomap.find(filenamevec.back()) != filetomap.end()) {
                string fname = filenamevec.back();
                filetomap[fname] = currFile;
            } else {
                string fname = filenamevec.back();

                filetomap.insert({fname, currFile});

                auto temp = filetomap[fname];
            }
        } else if (resarr[0] == "1") {
            printf("%s\n", resarr[1].c_str());
        }
    } else if (reqarr[0] == "download_file") {
        if (reqarr.size() != 4) {
            printf("Invalid usage of command\n");
            printf(
                "USAGE: download_file <group_id> <file_name> "
                "<destination_path>\n");
            return;
        }

        if (!currUser.loggedin) {
            printf("You are not logged! Please login\n");
            return;
        }

        char resolvedpath[PATH_MAX];
        if (realpath(reqarr[2].c_str(), resolvedpath) == NULL) {
            printf("ERROR: bad path %s\n", resolvedpath);
            return;
        }

        string resp(resolvedpath);
        string preq = req + " " + currUser.userid + "\n";

        int n = write(server_socket, preq.c_str(), preq.size());
        if (n < 0) err("ERROR: writing to socket");

        size_t size;
        if (read(server_socket, buffer, BUFSIZ) < 0) {
            printf("Couldn't get response from the tracker\n");
            return;
        }
        printf("%s\n", buffer);

        if (buffer[0] == 'S') {
            vector<string> clientarr;
            string res(buffer);
            splitutility(res.substr(2), ' ', clientarr);
            string fullfilesha = clientarr[0];
            vector<string> clients;
            for (int i = 0; i < clientarr.size(); i++) {
                clients.push_back(clientarr[i]);
            }

            unordered_map<long, vector<string>> chunktomap;

            userschunkmapinfo(chunktomap, clients, reqarr[2]);

            if (chunktomap.size() == 0) {
                printf(
                    "No user is currently online to serve this file, try again "
                    "later\n");
                return;
            }

            DownloadPassThread* transferdata = new DownloadPassThread();
            transferdata->filename = reqarr[2];
            transferdata->chunktomap = chunktomap;
            transferdata->destinationpath = reqarr[3];
            transferdata->filesha = fullfilesha;
            // transferdata->srcfilepath = resolvedpath;
            transferdata->groupid = reqarr[1];

            pthread_t download_thread;
            pthread_create(&download_thread, NULL, downloadstart, transferdata);

        } else if (buffer[0] == '1') {
            vector<string> resarr;
            string res(buffer);
            splitutility(res, ':', resarr);
            printf("%s\n", resarr[1].c_str());
        }

    } else if (reqarr[0] == "show_downloads") {
        if (currUser.loggedin == false) {
            printf("You are not logged in!\n");
        } else {
            string res = "";
            int i = 1;
            if (filetomap.empty()) {
                printf("You have not downloaded anything yet!\n");
                return;
            }
            for (auto x : filetomap) {
                if (x.second->downloading == true) {
                    res += to_string(i) + ". [D] " + "[" + x.second->groupid + "] " + x.first + "\n";
                } else if (x.second->owner == false) {
                    res += to_string(i) + ". [C] " + "[" + x.second->groupid + "] " + x.first + "\n";
                }
                i++;
            }
            if (res.empty()) {
                printf("You have not downloaded anything yet!\n");
            } else
                printf("%s", res.c_str());
            return;
        }
    } else if (reqarr[0] == "stop_share") {
        if (reqarr.size() != 3) {
            printf("Invalid usage of command\n");
            printf("USAGE: stop_share <group_id> <file_name>\n");
            return;
        }

        if (!currUser.loggedin) {
            printf("You are not logged! Please login\n");
            return;
        }

        string preq = req + " " + currUser.userid + "\n";

        int n = write(server_socket, preq.c_str(), preq.size());
        if (n < 0) err("ERROR: writing to socket");

        size_t size;
        if (read(server_socket, buffer, BUFSIZ) < 0) {
            printf("Couldn't get response from the tracker\n");
            return;
        }

        vector<string> resarr;
        string res(buffer);
        splitutility(res, ':', resarr);

        if (resarr[0] == "2") {
            printf("[%s]: %s\n", resarr[1].c_str(), resarr[2].c_str());
        } else if (resarr[0] == "1") {
            printf("%s\n", resarr[1].c_str());
        }
    } else if (req == "logout") {
        if (currUser.loggedin == false) {
            printf("You are not logged in!\n");

        } else {
            req = req + " " + currUser.userid + "\n";
            printf("Sending req %s", req.c_str());
            char charreq[req.length() + 1];
            strcpy(charreq, req.c_str());
            charreq[req.length() + 1] = 0;
            int n = send(server_socket, charreq, strlen(charreq), 0);
            if (n < 0) err("ERROR: writing to socket");

            size_t size;
            if (read(server_socket, buffer, BUFSIZ) < 0) {
                printf("Couldn't get response from the tracker\n");
                return;
            }

            vector<string> resarr;
            string res(buffer);
            splitutility(res, ':', resarr);

            if (resarr[0] == "2") {
                currUser.userid = resarr[1];
                currUser.loggedin = false;
                printf("%s\n", resarr[2].c_str());
            } else if (resarr[0] == "1") {
                printf("%s\n", resarr[1].c_str());
            }
        }
    } else {
        printf("Invalid command\nTry again!\n");
        return;
    }
    close(server_socket);
}

/// @brief Util to start the thread
/// @param arg
/// @return
void* start_thread(void* arg) {
    while (true) {
        int* pclient;
        pthread_mutex_lock(&mutexQueue);
        if (thread_queue.empty()) {
            pclient = NULL;
            pthread_cond_wait(&condQueue, &mutexQueue);
            if (!thread_queue.empty()) {
                pclient = thread_queue.front();
                thread_queue.pop();
            }
        } else {
            pclient = thread_queue.front();
            thread_queue.pop();
        }
        pthread_mutex_unlock(&mutexQueue);

        if (pclient != NULL) {
            handle_connection(pclient);
        }
    }
    return NULL;
}

/// @brief Util to start the download thread
/// @param arg
/// @return
void* start_down_thread(void* arg) {
    int breakcond = *(int*)arg;
    while (true) {
        DownloadData* pclient;
        pthread_mutex_lock(&mutexDownQueue);
        if (threadDownQueue.empty()) {
            pclient = NULL;
            pthread_cond_wait(&condDownQueue, &mutexDownQueue);
            if (!threadDownQueue.empty()) {
                pclient = threadDownQueue.front();
                threadDownQueue.pop();
            }
        } else {
            pclient = threadDownQueue.front();
            threadDownQueue.pop();
            breakcond--;
        }
        pthread_mutex_unlock(&mutexDownQueue);

        if (pclient != NULL) {
            downloadexec(pclient);
        }
        if (breakcond <= 0) {
            break;
        }
    }
    return NULL;
}

/// @brief Util to handle the connections
/// @param arg
/// @return
void* handle_connection(void* arg) {
    int client_socket = *(int*)arg;
    free(arg);
    char request[BUFSIZ];
    bzero(request, BUFSIZ);

    size_t bytes_read;
    int msgsize = 0;

    while ((bytes_read = read(client_socket, request + msgsize,
                              sizeof(request) - msgsize - 1)) > 0) {
        msgsize += bytes_read;
        if (msgsize > BUFSIZ - 1 or request[msgsize - 1] == '\n') break;
    }
    check(bytes_read, "recv error");
    request[msgsize - 1] = 0;

    vector<string> reqarr;
    string req(request);
    splitutility(req, ' ', reqarr);
    if (reqarr.empty()) {
        string msg = "Something went wrong\n";
        write(client_socket, msg.c_str(), msg.size());
        return NULL;
    }

    if (reqarr[0] == "getchunks") {

        if (filetomap.find(reqarr[1]) == filetomap.end()) {
            string res = "-1";
            write(client_socket, res.c_str(), res.size());
            return NULL;
        }

        auto currFile = filetomap[reqarr[1]];
        string res = "1:";

        unordered_map<int, string> chunktomap;
        for (long i = 0; i < currFile->chunksha.size(); i++) {
            chunktomap.insert(
                {currFile->chunksha[i].first, currFile->chunksha[i].second});
        }
        for (auto x : chunktomap) {
            res += to_string(x.first) + " ";
        }

        res.clear();
        res = "1:";

        for (int i = 0; i < currFile->chunks; i++) {
            res += to_string(i) + " ";
        }
        write(client_socket, res.c_str(), res.size());
        return NULL;

    } else if (reqarr[0] == "download") {
        string filename = reqarr[1];
        int chunknumber = stoi(reqarr[2]);
        // long totalfilesize =
        //     getfilesize(filepath) - (chunknumber * CHUNKSIZE);

        auto currFile = filetomap[filename];
        long totalfilesize =
            getfilesize(currFile->filepath) - (chunknumber * CHUNKSIZE);
        string sha;

        for (auto x : currFile->chunksha) {
            if (x.first == chunknumber) {
                sha = x.second;
            }
        }

        sha += " " + currFile->filepath;

        int n = write(client_socket, sha.c_str(), sha.size());

        char resp[5];
        n = read(client_socket, resp, sizeof(resp));

        int fd = open(currFile->filepath.c_str(), O_RDONLY);
        off64_t offset = chunknumber * CHUNKSIZE;
        int filesize = CHUNKSIZE;
        size_t readbytes;
        char buff[BUFSIZ];
        while ((readbytes = pread64(fd, buff, BUFSIZ, offset)) > 0 &&
               filesize > 0 && totalfilesize > 0) {
            write(client_socket, buff, readbytes);
            offset += readbytes;
            bzero(buff, BUFSIZ);
            filesize -= readbytes;
            totalfilesize -= readbytes;
        }
        close(fd);
        return NULL;
    } else {
        string res = "1:Invalid command\n";
        write(client_socket, res.c_str(), res.size());
        return NULL;
    }
}

/// @brief Util to send chunk info to user
/// @param chunktomap
/// @param clientarr
/// @param filename
void userschunkmapinfo(unordered_map<long, vector<string>>& chunktomap,
                       vector<string>& clientarr, string& filename) {
    for (int i = 0; i < clientarr.size(); i++) {
        vector<string> ipport;
        string curr = clientarr[i];
        splitutility(curr, ':', ipport);
        int port = stoi(ipport[1]);

        int peerfd = socket(AF_INET, SOCK_STREAM, 0);
        if (peerfd < 0) {
            printf("Error: In opening socket\n");
            return;
        }

        struct sockaddr_in peer_address;
        peer_address.sin_family = AF_INET;
        peer_address.sin_addr.s_addr = INADDR_ANY;
        peer_address.sin_port = htons(port);

        if (connect(peerfd, (struct sockaddr*)&peer_address,
                    sizeof(peer_address)) < 0) {
            printf("Error: In creating connection\n");
            return;
        }

        char buffer[BUFSIZ];
        bzero(buffer, BUFSIZ);

        string req = "getchunks ";
        req += filename + "\n";

        int n = write(peerfd, req.c_str(), req.size());
        if (n < 0) {
            printf("Couldn't send the request\n");
            return;
        }

        if (read(peerfd, buffer, BUFSIZ) < 0) {
            printf("Couldn't get response from the tracker\n");
            return;
        }

        vector<string> resarr;
        printf("%s\n", buffer);
        string res(buffer);
        splitutility(res, ':', resarr);

        if (resarr[0] == "1") {
            vector<string> chunkdetails;
            splitutility(resarr[1], ' ', chunkdetails);

            for (auto x : chunkdetails) {
                if (x != "") {
                    if (chunktomap.find(stol(x)) == chunktomap.end()) {
                        chunktomap.insert({stol(x), {}});
                    }
                    chunktomap[stol(x)].push_back(curr);
                }
            }
        }
    }
}

/// @brief Util to start donwload execution
/// @param arg
/// @return
void* downloadexec(void* arg) {
    DownloadData chunkinfo = *(DownloadData*)arg;

    vector<string> clientvec = chunkinfo.peers;
    string fullfilesha = chunkinfo.filesha;
    // cout << clientvec.size() << endl;
    int noofclients = clientvec.size();
    // string filepath = chunkinfo.srcfilepath;
    int chunknumber = chunkinfo.chunknumber;
    string filename = chunkinfo.filename;
    string destpath = chunkinfo.destfilepath;
    if (destpath.back() == '/') {
        destpath += filename;
    } else {
        destpath = destpath + "/" + filename;
    }
    auto currFile = filetomap[filename];

    int n = 5;
    while (n--) {
        string curr = clientvec[rand() % noofclients];
        vector<string> ipport;
        splitutility(curr, ':', ipport);
        int port = stoi(ipport[1]);

        int peerfd = socket(AF_INET, SOCK_STREAM, 0);
        if (peerfd < 0) {
            printf("Error: In opening socket\n");
            continue;
        }

        struct sockaddr_in peer_address;
        peer_address.sin_family = AF_INET;
        peer_address.sin_addr.s_addr = INADDR_ANY;
        peer_address.sin_port = htons(port);

        if (connect(peerfd, (struct sockaddr*)&peer_address,
                    sizeof(peer_address)) < 0) {
            printf("Error: In creating connection\n");
            continue;
        }

        char buffer[BUFSIZ];
        bzero(buffer, BUFSIZ);

        string req =
            "download " + filename + " " + to_string(chunknumber) + "\n";

        int n = write(peerfd, req.c_str(), req.size());
        if (n < 0) {
            printf("Couldn't send the request\n");
            continue;
        }

        char sha[1024];
        n = read(peerfd, sha, 1024);
        n = write(peerfd, "OK", 2);
        vector<string> shadecode;
        string sd(sha);
        splitutility(sd, ' ', shadecode);
        string filepath = shadecode[1];

        int fd = open(destpath.c_str(), O_WRONLY | O_CREAT,
                      S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        long totalfilesize = getfilesize(filepath) - (chunknumber * CHUNKSIZE);
        int filesize = CHUNKSIZE;
        loff_t offset = chunknumber * CHUNKSIZE;
        ssize_t readbytes = 0;

        while (totalfilesize > 0 && filesize > 0 &&
               (readbytes = read(peerfd, buffer, BUFSIZ)) > 0) {
            ssize_t written = pwrite64(fd, buffer, readbytes, offset);
            offset += written;
            // memset(buffer, 0, BUFSIZ);
            filesize -= readbytes;
            totalfilesize -= readbytes;
        }

        if (currFile) {
            currFile->chunksha.push_back({chunknumber, shadecode[0]});
            currFile->filepath = filepath;
            currFile->filesize = getfilesize(filepath);
            currFile->chunks = ceil((double)currFile->filesize / CHUNKSIZE);
        }

        close(fd);
        close(peerfd);

        return NULL;
    }

    printf("Couldn't Download Chunk %d\n", chunknumber);
    return NULL;
}

/// @brief Util to start the download for thread
/// @param arg
/// @return
void* downloadstart(void* arg) {
    DownloadPassThread transferdata = *(DownloadPassThread*)arg;

    vector<DownloadData*> pieceselection;
    for (auto x : transferdata.chunktomap) {
        DownloadData* dd = new DownloadData();
        dd->filename = transferdata.filename;
        dd->chunknumber = x.first;
        dd->filesha = transferdata.filesha;
        dd->peers = x.second;
        dd->destfilepath = transferdata.destinationpath;
        dd->groupid = transferdata.groupid;

        pieceselection.push_back(dd);
    }

    sort(pieceselection.begin(), pieceselection.end(), cmp);

    long i = 0;
    for (; i < pieceselection.size(); i++) {
        auto temp = pieceselection[i];
        if (temp->chunknumber == 0) break;
    }
    DownloadData* chunkToSeed = pieceselection[i];
    downloadexec(chunkToSeed);
    pieceselection.erase(pieceselection.begin() + i);
    if (chunkToSeed->destfilepath.back() == '/') {
        chunkToSeed->destfilepath.pop_back();
    }
    string req = "ufile " + chunkToSeed->destfilepath + "/" +
                 transferdata.filename + " " + chunkToSeed->groupid + "\n";
    client_function(req.c_str(), connection_info.second);

    auto curr = filetomap[transferdata.filename];
    if (!curr) {
        return NULL;
    }
    curr->downloading = true;

    pthread_t th[DOWNLOAD_THREAD_POOL];
    pthread_mutex_init(&mutexDownQueue, NULL);
    pthread_cond_init(&condDownQueue, NULL);
    int breakcond = pieceselection.size();
    for (int i = 0; i < DOWNLOAD_THREAD_POOL; i++) {
        check(pthread_create(&th[i], NULL, start_down_thread, &breakcond),
              "Failed to create the thread");
    }

    for (int i = 0; i < pieceselection.size(); i++) {
        pthread_mutex_lock(&mutexDownQueue);
        threadDownQueue.push(pieceselection[i]);
        pthread_cond_signal(&condDownQueue);
        pthread_mutex_unlock(&mutexDownQueue);
    }

    pthread_mutex_destroy(&mutexDownQueue);
    pthread_cond_destroy(&condDownQueue);

    printf("Download Completed Successfully!\n");
    curr->downloading = false;
    return NULL;
}

/// @brief Util to generate SHA for a chunk
/// @param filepath
/// @param offset
/// @return
string generateSHA(string filepath, long offset) {
    char resolvedpath[PATH_MAX];
    if (realpath(filepath.c_str(), resolvedpath) == NULL) {
        printf("ERR: bad path %s\n", resolvedpath);
        return NULL;
    }

    FILE* fd = fopen(resolvedpath, "rb");
    char shabuf[SHA_DIGEST_LENGTH];
    bzero(shabuf, sizeof(shabuf));
    fseek(fd, offset, SEEK_SET);
    fread(shabuf, 1, SHA_DIGEST_LENGTH, fd);

    unsigned char SHA_Buffer[SHA_DIGEST_LENGTH];
    char buffer[SHA_DIGEST_LENGTH * 2];
    int i;
    bzero(buffer, sizeof(buffer));
    bzero(SHA_Buffer, sizeof(SHA_Buffer));
    SHA1((unsigned char*)shabuf, 20, SHA_Buffer);

    for (i = 0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf((char*)&(buffer[i * 2]), "%02x", SHA_Buffer[i]);
    }

    fclose(fd);
    string shastr(buffer);
    return shastr;
}

/// @brief Util to fetch the file size
/// @param filename
/// @return
long getfilesize(string filename) {
    struct stat sbuf;
    int f = stat(filename.c_str(), &sbuf);
    return f == 0 ? sbuf.st_size : -1;
}