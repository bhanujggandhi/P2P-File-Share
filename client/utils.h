#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <openssl/sha.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define THREAD_POOL_SIZE 4
#define DOWNLOAD_THREAD_POOL 4
#define CHUNKSIZE 524288

using namespace std;

/// @brief User structure
struct User {
    string userid;
    bool loggedin;
    string address;
};

/// @brief File structure
struct FileStr {
    string filename;
    string filepath;
    string SHA;
    long filesize;
    bool owner = false;
    bool downloading = false;
    string groupid = "";

    vector<bool> chunkpresent;
    vector<pair<int, string>> chunksha;

    long chunks = ceil(filesize / (long)BUFSIZ);
};

struct DownloadData {
    int chunknumber;
    string filename;
    string srcfilepath;
    string destfilepath;
    vector<string> peers;
    string filesha;
    string groupid;

    DownloadData() {}
};

struct DownloadPassThread {
    string destinationpath;
    string filename;
    unordered_map<long, vector<string>> chunktomap;
    string srcfilepath;
    string filesha;
    string groupid;
};

// GLOBAL VARIABLES
extern pthread_mutex_t mutexQueue;
extern pthread_cond_t condQueue;
extern queue<int*> thread_queue;

extern pthread_mutex_t mutexDownQueue;
extern pthread_cond_t condDownQueue;
extern queue<DownloadData*> threadDownQueue;

extern pair<string, int> connection_info;
extern User currUser;
extern unordered_map<string, FileStr*> filetomap;

// UTILITY FUNCTIONS
void err(const char* msg);
void check(int status, string msg);
bool cmp(DownloadData* p1, DownloadData* p2);
void splitutility(string str, char del, vector<string>& pth);
void* server_function(void* arg);
void connecttotracker(const char* filepath);
long getfilesize(string filename);
string generateSHA(string filepath, long offset);
void userschunkmapinfo(unordered_map<long, vector<string>>& chunktomap,
                       vector<string>& clientarr, string& filepath);
void* downloadexec(void* arg);
void client_function(const char* request, int CLIENTPORT);
void* start_thread(void* arg);
void* start_down_thread(void* arg);
void* handle_connection(void* socket);
void* downloadstart(void* donwloadtransfer);