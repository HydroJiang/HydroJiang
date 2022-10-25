#ifndef GLOBAL_H
#define GLOBAL_H

#define MAX_PATH 128    //工作路径名最大长度
#define LINE_SIZE 128   //每次拷贝最大字节数
#define UNIXEPOCH 2208988800UL
#define UNIX_PATH_MAX 108

#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <dirent.h>

// #include "../fxs/FXSGlobal.h"

using namespace std;

//sockutil.cpp
struct sockaddr_un
{

    sa_family_t sun_family; /*PF_UNIX或AF_UNIX */

    char sun_path[UNIX_PATH_MAX]; /* 路径名 */
};

extern void die(char *message);

extern void copyData(int from, int to);

//Util.cpp
extern void split(const string& s, vector<string>& v, const string& c);

extern void createDirList(const char* sourcePath);

extern void changeStat(const char* targetFile, const struct stat &s);

extern int mksock (const char *__path, const __mode_t &__mode);

extern string getSourceFile(const char* path,const char* fileName);

extern struct stat getStat(const char* path,const char* fileName);

extern void printStat(const struct stat *buf,const string &file);

extern void coutStat(const char* path,const char* fileName);

extern int cmpStat(const struct stat &a,const struct stat &b);

extern int copyContent(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName);

extern void tearPathAndName(const string &pathAndName,string &path,string &name);

struct recordLine{
    int newFileNum;
    char sourcePath[MAX_PATH];
    char fileName[MAX_PATH];
    struct stat s;
    struct timespec backUpTime;
};

class Record{
private:
    string recordPath;

    vector<struct recordLine> file;

    int len;

    int readRecord();
    
public:
    Record();

    Record(string path);

    int writeRecord();
    
    void coutRecord();
    
    int getRecord(const char* sourcePath,const char* fileName);
    
    int getRecord(const int& newFileNum);
    
    const struct recordLine& getLine(const int &index);

    int addRecord(const char* sourcePath,const char* fileName,const struct stat &s);

    int rmRecord(const int &index);
};

struct config{
    char backUpPath[MAX_PATH];
    char compress;
    char encryption;
};

class configEditor{
    string workPath;
    string configFilePath;
    struct config CONFIG;

    void placeHolder();

    int readConfig();

    public:
    configEditor();

    int writeConfig();

    void changeConfig(const config &temp);
    
    void coutConfig();

    bool retIsCompress();
    
    bool retIsEncryption();

    string retTargetPath();
};

extern string modeToStr(const mode_t &mode);

extern string timeSpecToStr(const timespec &t);

extern string ownerGroup(const struct stat &info);

extern bool isRegOrDir(const char* path,const char* name);

//function.cpp
extern int backUp(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName,const string &code);

extern int putBack(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName,const string &code);

extern int compareFile(const char* sourcePath,const char* sourceFileName,const string &code,vector<string> &wrongList);

extern int rmBackUp(const char* sourcePath,const char* sourceFileName);

//FileCompare.cpp
extern int cmp(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName);

extern int cmpDir(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName);

extern vector<string> cmpReadRecord(Record &record,const char* sourcePath,const char* sourceFileName);

extern int cmpNormailFile(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName);

extern vector<string>  cmpDirReadRecord(Record &record,const char* sourcePath,const char* sourceFileName);

//FileCopy.cpp
extern int cp(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName);

extern int cpWriteRecord(Record &record,const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName);

extern int cpReadRecord(Record &record,const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName);

extern int copyNormailFile(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName);

extern int copyDir(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName);

extern int copyLink(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName);

extern int copyPipe(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName);

extern int copyDev(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName);

extern int copySocket(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName);

extern int copyOtherFile(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName);

extern int copyDirWriteRecord(Record &record,const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName);

extern int copyDirReadRecord(Record &record,const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName);

//FileRemove.cpp
extern int rm(const char* path,const char* fileName);

extern int rmWriteRecord(Record &record,const char* path,const char* fileName);

extern int deleteFile(const char* path,const char* fileName);

extern int deleteDir(const char* path,const char* fileName);

extern int rmDirWriteRecord(Record &record,const char* path,const char* fileName);

#endif
