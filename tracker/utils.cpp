#include "utils.h"

void err(const char* msg) { printf("%s\n", msg); }

void check(int status, string msg) {
    if (status < 0) {
        err(msg.c_str());
    }
}

void splitutility(string str, char del, vector<string>& pth) {
    string temp = "";

    for (int i = 0; i < str.size(); i++) {
        if (str[i] != del)
            temp += str[i];
        else {
            pth.push_back(temp);
            temp = "";
        }
    }

    pth.push_back(temp);
}

void getipport(char* filename, int number) {
    char resolvedpath[_POSIX_PATH_MAX];
    if (realpath(filename, resolvedpath) == NULL) {
        printf("ERROR: bad path %s\n", resolvedpath);
        exit(1);
    }

    std::ifstream file(resolvedpath);
    int num = number;
    string ipport;
    while (num--) {
        if (file.is_open()) {
            string line;
            getline(file, line);
            ipport = line;
        } else {
            printf("Cannot open the file!\n");
            exit(1);
        }
    }
    file.close();

    vector<string> ipportsplit;
    splitutility(ipport, ':', ipportsplit);
    connectioninfo.first = ipportsplit[0];
    connectioninfo.second = stoi(ipportsplit[1]);
}

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

    int n = write(server_socket, request, strlen(request));
    if (n < 0) err("ERROR: writing to socket");

    char buff[BUFSIZ];
    bzero(buff, BUFSIZ);

    /* Take source destination from the args and realpath */

    int d = open("copied.txt", O_WRONLY | O_CREAT,
                 S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if (d == -1) {
        err("Destination file cannot be opened");
        close(d);
        return;
    }

    size_t size;
    while ((size = read(server_socket, buff, BUFSIZ)) > 0) {
        sleep(2);
        write(d, buff, size);
    }

    close(d);

    printf("File Transferred Succesfully!\n");
    close(server_socket);
}

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

void* handle_connection(void* arg) {
    int client_socket = *(int*)arg;
    free(arg);
    char request[BUFSIZ];
    bzero(request, BUFSIZ);

    size_t bytes_read;
    int req_size = 0;

    while ((bytes_read = read(client_socket, request + req_size,
                              sizeof(request) - req_size - 1)) > 0) {
        req_size += bytes_read;
        if (req_size > BUFSIZ - 1 or request[req_size - 1] == '\n') break;
    }
    check(bytes_read, "recv error");
    request[req_size - 1] = 0;

    vector<string> reqarr;
    string req(request);
    splitutility(req, ' ', reqarr);
    if (reqarr.empty()) {
        string msg = "Something went wrong\n";
        write(client_socket, msg.c_str(), msg.size());
        return NULL;
    }

    if (reqarr[0] == "create_user") {
        string userid = reqarr[1];
        string password = reqarr[2];
        string address = reqarr[3];

        if (usertomap.find(userid) != usertomap.end()) {
            string msg =
                "1:User ID already exists\nEither Login or create an account "
                "with unique User ID\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        User* newUser = new User();
        newUser->userid = userid;
        newUser->password = password;
        newUser->address = address;
        usertomap.insert({userid, newUser});
        loggedin_map[userid] = true;
        string res = "2:" + userid + ":User created successfully!\n";
        write(client_socket, res.c_str(), res.size());
    } else if (reqarr[0] == "login") {
        string userid = reqarr[1];
        string password = reqarr[2];
        string address = reqarr[3];

        if (usertomap.find(userid) == usertomap.end()) {
            string msg =
                "1:User ID does not exist\nPlease create an account to "
                "continue\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        if (password == usertomap[userid]->password) {
            loggedin_map[userid] = true;
            auto currUser = usertomap[userid];
            currUser->address = address;
            string res = "2:" + userid + ":User logged in successfully!\n";
            write(client_socket, res.c_str(), res.size());
            return NULL;
        }

        loggedin_map[userid] = false;
        string msg = "1:Password is incorrect, please try again\n";
        write(client_socket, msg.c_str(), msg.size());
        return NULL;

    } else if (reqarr[0] == "create_group") {
        string groupid = reqarr[1];
        string userid = reqarr[2];

        if (usertomap.find(userid) == usertomap.end()) {
            string msg = "1:User does not exist\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }
        if (loggedin_map[userid] == false) {
            string msg = "1:Please login first\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }
        if (grouptomap.find(groupid) != grouptomap.end()) {
            string msg =
                "1:Group id already exists, please enter a unique one\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        Group* newGroup = new Group();
        newGroup->adminid = userid;
        newGroup->groupid = groupid;
        newGroup->members.insert(userid);

        grouptomap.insert({groupid, newGroup});

        auto* curruser = usertomap[userid];
        curruser->groups.insert(groupid);
        curruser->mygroups.insert(groupid);

        string msg = "2:" + groupid + ":Group created successfully\n";
        write(client_socket, msg.c_str(), msg.size());
        return NULL;
    } else if (reqarr[0] == "join_group") {
        string groupid = reqarr[1];
        string userid = reqarr[2];

        if (usertomap.find(userid) == usertomap.end()) {
            string msg = "1:User does not exist\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }
        if (loggedin_map[userid] == false) {
            string msg = "1:Please login first\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }
        if (grouptomap.find(groupid) == grouptomap.end()) {
            string msg =
                "1:Group id does not exist. Please enter a valid one\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        auto currGroup = grouptomap[groupid];

        if (currGroup->members.find(userid) != currGroup->members.end()) {
            string msg = "1:User is already a member of the group\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }
        currGroup->requests.insert(userid);
        printf("Group size: %ld", currGroup->requests.size());
        string msg = "2:" + groupid + ":Request sent successfully\n";
        write(client_socket, msg.c_str(), msg.size());
        return NULL;
    } else if (reqarr[0] == "leave_group") {
        string groupid = reqarr[1];
        string userid = reqarr[2];

        if (usertomap.find(userid) == usertomap.end()) {
            string msg = "1:User does not exist\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        if (loggedin_map[userid] == false) {
            string msg = "1:Please login first\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        if (grouptomap.find(groupid) == grouptomap.end()) {
            string msg =
                "1:Group id does not exist. Please enter a valid one\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        auto currGroup = grouptomap[groupid];

        if (currGroup->members.find(userid) == currGroup->members.end()) {
            string msg = "1:User is not the member of this group\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        if (currGroup->adminid == userid) {
            string msg = "1:Admin cannot leave the group\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        currGroup->members.erase(userid);
        string msg = "2:" + groupid + ":Group left successfully\n";
        write(client_socket, msg.c_str(), msg.size());
        return NULL;
    } else if (reqarr[0] == "list_requests") {
        string groupid = reqarr[1];
        string userid = reqarr[2];

        if (usertomap.find(userid) == usertomap.end()) {
            string msg = "1:User does not exist\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        if (loggedin_map[userid] == false) {
            string msg = "1:Please login first\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        if (grouptomap.find(groupid) == grouptomap.end()) {
            string msg =
                "1:Group id does not exist. Please enter a valid one\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        auto currGroup = grouptomap[groupid];

        if (currGroup->adminid != userid) {
            string msg = "1:User does not have admin rights\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        string greqParse = "";
        long i = 1;
        for (auto uid : currGroup->requests) {
            greqParse += to_string(i) + ". " + uid + "\n";
            i++;
        }
        string msg = "2:Pending Requests:\n" + greqParse;
        write(client_socket, msg.c_str(), msg.size());
        return NULL;

    } else if (reqarr[0] == "accept_request") {
        string groupid = reqarr[1];
        string userid = reqarr[2];
        string curruserid = reqarr[3];

        if (usertomap.find(userid) == usertomap.end() or
            usertomap.find(curruserid) == usertomap.end()) {
            string msg = "1:User does not exist\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        if (loggedin_map[curruserid] == false) {
            string msg = "1:Please login first\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        if (grouptomap.find(groupid) == grouptomap.end()) {
            string msg =
                "1:Group id does not exist. Please enter a valid one\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        auto currGroup = grouptomap[groupid];

        if (currGroup->requests.find(userid) == currGroup->requests.end()) {
            string msg = "1:User has not requested to join the group\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        if (currGroup->adminid != curruserid) {
            string msg = "1:You do not have admin rights for this group\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        currGroup->requests.erase(userid);
        currGroup->members.insert(userid);

        string msg =
            "2:" + groupid + ":" + userid + " is the member of the group now\n";
        write(client_socket, msg.c_str(), msg.size());
        return NULL;
    } else if (reqarr[0] == "list_groups") {
        string userid = reqarr[1];

        if (usertomap.find(userid) == usertomap.end()) {
            string msg = "1:User does not exist\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        if (loggedin_map[userid] == false) {
            string msg = "1:Please login first\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        string greqParse = "";
        long i = 1;
        for (auto gid : grouptomap) {
            greqParse += to_string(i) + ". " + gid.first + "\n";
            i++;
        }
        string msg = "2:All Groups:\n" + greqParse;
        write(client_socket, msg.c_str(), msg.size());
        return NULL;

    } else if (reqarr[0] == "list_files") {
        string groupid = reqarr[1];
        string userid = reqarr[2];

        if (usertomap.find(userid) == usertomap.end()) {
            string msg = "1:User does not exist\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        if (loggedin_map[userid] == false) {
            string msg = "1:Please login first\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        if (grouptomap.find(groupid) == grouptomap.end()) {
            string msg =
                "1:Group id does not exist. Please enter a valid one\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        auto currGroup = grouptomap[groupid];

        if (currGroup->members.find(userid) == currGroup->members.end()) {
            string msg = "1:User is not a member of this group\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        string greqParse = "";
        long i = 1;
        for (auto fid : currGroup->files) {
            greqParse += to_string(i) + ". " + fid + "\n";
            i++;
        }
        string msg = "2:All Files:\n" + greqParse;
        write(client_socket, msg.c_str(), msg.size());
        return NULL;

    } else if (reqarr[0] == "upload_file") {
        string filename = reqarr[1];
        string groupid = reqarr[2];
        string sha = reqarr[3];
        long filesize = stol(reqarr[4]);
        string userid = reqarr[5];

        if (usertomap.find(userid) == usertomap.end()) {
            string msg = "1:User does not exist\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        if (loggedin_map[userid] == false) {
            string msg = "1:Please login first\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        if (grouptomap.find(groupid) == grouptomap.end()) {
            string msg =
                "1:Group id does not exist. Please enter a valid one\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        auto currGroup = grouptomap[groupid];

        if (currGroup->members.find(userid) == currGroup->members.end()) {
            string msg =
                "1:You are not the member of the group! Please send the "
                "request to join!\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        if (filetomap.find(filename) != filetomap.end()) {
            auto currFile = filetomap[filename];

            currFile->userids.insert(userid);
            currFile->users.insert(usertomap[userid]->address);
            string msg = "2:" + groupid + ":File uploaded succesfully";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        FileStr* newfile = new FileStr();

        newfile->filename = filename;
        newfile->filesize = filesize;
        newfile->SHA = sha;
        newfile->users.insert(usertomap[userid]->address);
        newfile->userids.insert(userid);

        filetomap.insert({filename, newfile});

        currGroup->files.insert(filename);

        string msg = "2:" + groupid + ":File uploaded succesfully";
        write(client_socket, msg.c_str(), msg.size());
        return NULL;

    } else if (reqarr[0] == "download_file") {
        string groupid = reqarr[1];
        string filepath = reqarr[2];
        string userid = reqarr[4];

        if (usertomap.find(userid) == usertomap.end()) {
            string msg = "1:User does not exist\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        if (loggedin_map[userid] == false) {
            string msg = "1:Please login first\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        if (grouptomap.find(groupid) == grouptomap.end()) {
            string msg =
                "1:Group id does not exist. Please enter a valid one\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        auto currGroup = grouptomap[groupid];

        if (currGroup->members.find(userid) == currGroup->members.end()) {
            string msg = "1:User is not a member of this group\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        if (filetomap.find(filepath) == filetomap.end() or
            currGroup->files.find(filepath) == currGroup->files.end()) {
            string msg = "1:File does not exist\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        auto currFile = filetomap[filepath];
        string res = "S";
        long countusers = 0;
        for (auto uid : currFile->userids) {
            auto peer = usertomap[uid];
            if (loggedin_map[uid] == false) {
                continue;
            }

            if (currFile->users.find(peer->address) == currFile->users.end()) {
                continue;
            }

            res += " " + peer->address;
            countusers++;
        }
        if (countusers == 0) {
            string msg = "1:File is currently unavailable\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        write(client_socket, res.c_str(), res.size());
        return NULL;

    } else if (reqarr[0] == "logout") {
        if (loggedin_map.find(reqarr[1]) == loggedin_map.end()) {
            string msg = "1:User does not exist\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }
        if (loggedin_map[reqarr[1]] == false) {
            string msg = "1:User is not logged in\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }
        loggedin_map[reqarr[1]] = false;
        string msg = "2:" + reqarr[1] + ":Logged out successfully!\n";
        write(client_socket, msg.c_str(), msg.size());
        return NULL;
    } else if (reqarr[0] == "stop_share") {
        string filename = reqarr[2];
        string groupid = reqarr[1];
        string userid = reqarr[3];

        if (usertomap.find(userid) == usertomap.end()) {
            string msg = "1:User does not exist\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        if (loggedin_map[userid] == false) {
            string msg = "1:Please login first\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        if (grouptomap.find(groupid) == grouptomap.end()) {
            string msg =
                "1:Group id does not exist. Please enter a valid one\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        auto currGroup = grouptomap[groupid];

        if (currGroup->members.find(userid) == currGroup->members.end()) {
            string msg = "1:User is not a member of this group\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        if (filetomap.find(filename) == filetomap.end() or
            currGroup->files.find(filename) == currGroup->files.end()) {
            string msg = "1:File does not exist\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        auto currFile = filetomap[filename];
        if (currFile->userids.find(userid) == currFile->userids.end()) {
            string msg = "1:You are not sharing the file\n";
            write(client_socket, msg.c_str(), msg.size());
            return NULL;
        }

        currFile->userids.erase(userid);
        currFile->users.erase(usertomap[userid]->address);

        if (currFile->userids.size() == 0) {
            filetomap.erase(currFile->filename);
            currGroup->files.erase(currFile->filename);

            delete currFile;
        }
        string res = "2:" + groupid + ":You have stopped sharing the file!";

        write(client_socket, res.c_str(), res.size());
        return NULL;

    } else {
        string res = "1:Please enter a valid command";
        write(client_socket, res.c_str(), res.size());
        return NULL;
    }

    return NULL;
}