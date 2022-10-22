#include<stdio.h>
#include<string.h>
#include<iostream>
#include<fstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include "FileCopy.h"

using namespace std;

/* sourcePath为源文件所属路径，targetPath为备份文件路径 */
int backUp(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    Record record;//每次完成一次备份写一次文件
    int fileId;
    fileId=cpWriteRecord(record,sourcePath,sourceFileName,targetPath,targetFileName);
    return fileId;
}

/* sourcePath为备份文件路径     targetPath为恢复路径，可以为空，为空的时候默认源路径 */
int putBack(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    Record record;//每次完成一次恢复写一次文件
    return cpReadRecord(record,sourcePath,sourceFileName,targetPath,targetFileName);
}

/* sourcePath为备份文件路径     自动获取targetpath */
int compareFile(const char* sourcePath,const char* sourceFileName){
    Record record;//每次完成一次比较写一次文件
    return cmpReadRecord(record,sourcePath,sourceFileName);//反馈信息由cmp输出
}

int rmBackUp(const char* sourcePath,const char* sourceFileName){
    Record record;//每次完成一次删除写一次文件
    return rmWriteRecord(record,sourcePath,sourceFileName);//反馈信息由rm输出
}