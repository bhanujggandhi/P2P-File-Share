#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define THREAD_POOL_SIZE 4

using namespace std;

struct User {
    string userid;
    string password;
    string address;
    unordered_set<string> mygroups;
    unordered_set<string> groups;
};
struct Group {
    string groupid;
    string adminid;
    unordered_set<string> members;
    unordered_set<string> files;
    unordered_set<string> requests;
};
struct FileStr {
    string filename;
    string SHA;
    long filesize;
    unordered_set<string> userids;
    unordered_set<string> users;
};

extern pair<string, int> connectioninfo;
extern pthread_mutex_t mutexQueue;
extern pthread_cond_t condQueue;
extern queue<int*> thread_queue;
extern unordered_map<string, User*> usertomap;
extern unordered_map<string, Group*> grouptomap;
extern unordered_map<string, bool> loggedin_map;
extern unordered_map<string, FileStr*> filetomap;

// Functions
void err(const char* msg);
void check(int status, string msg);
void splitutility(string str, char del, vector<string>& pth);
void getipport(char* filepath, int number);
void* server_function(void* arg);
void client_function(const char* request, int CLIENTPORT);
void* start_thread(void* arg);
void download_file(char* path, int client_socket);
void* handle_connection(void* socket);