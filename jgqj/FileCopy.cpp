#include<stdio.h>
#include<string.h>
#include<iostream>
#include<fstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include "global.h"

using namespace std;

#define MAX_PATH 100    //工作路径名最大长度
#define LINE_SIZE 128   //每次拷贝最大字节数

#define WRITE_RECORD true       //标志位
#define NOT_WRITE_RECORD false

int cp(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName);
int cpWriteRecord(Record &record,const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName);
int cpReadRecord(Record &record,const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName);

/* 根据新的目录和新的文件名，在新目录下创建一个新文件，并把普通文件拷贝过去 */
int copyNormailFile(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    createDirList(targetPath);//在复制文件/文件夹之前先创造路径
    string sourceFile=getSourceFile(sourcePath,sourceFileName);
    string targetFile=getSourceFile(targetPath,targetFileName);

    char workPath[MAX_PATH];
    //getcwd(workPath, MAX_PATH);//获得当前工作路径,用于写日志
    struct stat fileData=getStat(sourcePath,sourceFileName);
    int size = fileData.st_size;
    cout<<"file size: "<<size<<endl;

    if(!S_ISREG(fileData.st_mode)){
        cout<<"this not a regular file!"<<endl;
        return -1;
    }
    
    return copyContent(sourcePath,sourceFileName,targetPath,targetFileName);
}

/* 根据新的目录和新的目录名，在目标目录下创建一个新目录，并递归把所有文件拷贝过去 */
int copyDir(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    createDirList(targetPath);//在复制文件/文件夹之前先创造路径
    string sourceFile=getSourceFile(sourcePath,sourceFileName);
    string targetFile=getSourceFile(targetPath,targetFileName);
    struct dirent *dirData= NULL;
    struct stat fileData=getStat(sourcePath,sourceFileName);
    DIR* dir;
    
    if(sizeof(fileData)<0||!S_ISDIR(fileData.st_mode)){
        cout<<sourceFile<<" is not a dir"<<endl;
        return -1;
    }

    if(!(dir=opendir(sourceFile.c_str()))){
        cout<<"fail to open "<<sourceFile<<endl;
        return -1;
    }
    mkdir(targetFile.c_str(),fileData.st_mode);//创建文件夹
    while((dirData=readdir(dir))!=NULL){
        if (!(strncmp(dirData->d_name,".",1) && strncmp(dirData->d_name,".",2))){
		    continue;//跳过父目录和工作路径和.bashrc
	    }

        string newSourcePath=sourceFile;
        string newSourceFileName=dirData->d_name;
        string newTargetPath=targetFile;
        string newTargetFileName=dirData->d_name;
        cout<<"now is dealing with: "<<getSourceFile(newSourcePath.c_str(),newSourceFileName.c_str())<<endl;
        struct stat tmpStat=getStat(newSourcePath.c_str(),newSourceFileName.c_str());

        cp(newSourcePath.c_str(),dirData->d_name,newTargetPath.c_str(),dirData->d_name);
    }
    closedir(dir);
    return 0;
}

/*
int link(const char *oldpath,const char *newpath);
功能：创建硬链接文件

int symlink(const char *linkpath,const char *targetPath);
功能：创建软链接文件

ssize_t readlink(const char *pathname, char *buf, size_t bufsiz);
功能：读取软链接文件链接路径，返回值为软链接文件大小

*/
/* 输入旧目录，旧文件名，新目录，新文件名，并把软链接文件拷过去 */
int copyLink(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    createDirList(targetPath);//在复制文件/文件夹之前先创造路径
    string sourceFile=getSourceFile(sourcePath,sourceFileName);
    string targetFile=getSourceFile(targetPath,targetFileName);

    char workPath[MAX_PATH];
    //getcwd(workPath, MAX_PATH);//获得当前工作路径,用于写日志
    struct stat fileData=getStat(sourcePath,sourceFileName);
    int size = fileData.st_size;
    cout<<"file size: "<<size<<endl;
    char* linkPath=(char*)calloc(sizeof(char),size);//用malloc会出错，因为尾部没初始化为0，肯定有问题
    struct stat tmp;
    
    if(!S_ISLNK(fileData.st_mode)){
        cout<<"this not a soft link!"<<endl;
        return -1;
    }
    
    if(!lstat(getSourceFile(targetPath,targetFileName).c_str(),&tmp)){//文件存在，结果为0。access判断不了软链接是否存在
        cout<<targetFile<<" exist!"<<endl;
        rm(targetPath,targetFileName);//若存在则先删除，否则无法复制软链接文件
    }

    cout<<"link size: "<<readlink(sourceFile.c_str(),linkPath,size)<<endl;
    if(symlink(linkPath,targetFile.c_str())==0){
        //无需复制内容，软链接内容即为路径
        cout<<targetFile<<" copy success!"<<endl;
        free(linkPath);
        // changeStat(targetFile.c_str(),fileData);
        return 0;
    }
    else{
        cout<<targetFile<<" created fail!"<<endl;
        free(linkPath);
        return -1;
    }
}

/*
Create a new FIFO named PATH, with permission bits MODE.
extern int mkfifo (const char *__path, __mode_t __mode)
输入旧目录，旧文件名，新目录，新文件名，并把管道文件拷过去
*/
int copyPipe(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    createDirList(targetPath);//在复制文件/文件夹之前先创造路径
    string sourceFile=getSourceFile(sourcePath,sourceFileName);
    string targetFile=getSourceFile(targetPath,targetFileName);

    char workPath[MAX_PATH];
    //getcwd(workPath, MAX_PATH);//获得当前工作路径,用于写日志
    struct stat fileData=getStat(sourcePath,sourceFileName);
    int size = fileData.st_size;
    cout<<"file size: "<<size<<endl;

    if(!S_ISFIFO(fileData.st_mode)){
        cout<<"this not a FIFO!"<<endl;
        return -1;
    }

    if(access(targetFile.c_str(),F_OK)==0){
        cout<<targetFile<<" exist!"<<endl;
        rm(targetPath,targetFileName);//若存在则先删除，否则无法复制
    }

    if(mkfifo(targetFile.c_str(),fileData.st_mode)==0){
        cout<<targetFile<<" copy success!"<<endl;
        //试试在管道文件尾部增加内容，再试试进程通讯
        //管道文件存不了东西
        // changeStat(targetFile.c_str(),fileData);
        truncate(targetFile.c_str(), fileData.st_size);
        return 0;
    }
    else{
        cout<<targetFile<<" copy fail!"<<endl;
        return -1;
    }
}

/*
创建设备文件
int mknod (const char *__path, __mode_t __mode, __dev_t __dev)
只能在sudo模式运行，才能执行此函数
*/
int copyDev(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    createDirList(targetPath);//在复制文件/文件夹之前先创造路径
    string sourceFile=getSourceFile(sourcePath,sourceFileName);
    string targetFile=getSourceFile(targetPath,targetFileName);

    char workPath[MAX_PATH];
    //getcwd(workPath, MAX_PATH);//获得当前工作路径,用于写日志
    struct stat fileData=getStat(sourcePath,sourceFileName);
    int size = fileData.st_size;
    cout<<"file size: "<<size<<endl;

    if(!(S_ISCHR(fileData.st_mode)||S_ISBLK(fileData.st_mode))){
        cout<<"this not a dev file!"<<endl;
        return -1;
    }

    if(access(targetFile.c_str(),F_OK)==0){
        cout<<targetFile<<" exist!"<<endl;
        rm(targetPath,targetFileName);//若存在则先删除，否则无法复制
    }

    if(mknod(targetFile.c_str(),fileData.st_mode,fileData.st_rdev)==0){
        cout<<targetFile<<" copy success!"<<endl;
        // changeStat(targetFile.c_str(),fileData);
        truncate(targetFile.c_str(), fileData.st_size);
        return 0;
    }
    else{
        cout<<targetFile<<" copy fail!"<<endl;
        return -1;
    }
}

/* 复制套接字文件 */
int copySocket(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    createDirList(targetPath);//在复制文件/文件夹之前先创造路径
    string sourceFile=getSourceFile(sourcePath,sourceFileName);
    string targetFile=getSourceFile(targetPath,targetFileName);

    char workPath[MAX_PATH];
    //getcwd(workPath, MAX_PATH);//获得当前工作路径,用于写日志
    struct stat fileData=getStat(sourcePath,sourceFileName);
    int size = fileData.st_size;
    cout<<"file size: "<<size<<endl;

    if(!S_ISSOCK(fileData.st_mode)){
        cout<<"this not a socket!"<<endl;
        return -1;
    }

    if(access(targetFile.c_str(),F_OK)==0){
        cout<<targetFile<<" exist!"<<endl;
        rm(targetPath,targetFileName);//若存在则先删除，否则无法复制
    }

    if(mksock(targetFile.c_str(),fileData.st_mode)==0){
        cout<<targetFile<<" copy success!"<<endl;
        // changeStat(targetFile.c_str(),fileData);
        truncate(targetFile.c_str(), fileData.st_size);
        return 0;
    }
    else{
        cout<<targetFile<<" copy fail!"<<endl;
        return -1;
    }
}

/* copy除了dir外所有文件类型 */
int copyOtherFile(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    struct stat fileData=getStat(sourcePath,sourceFileName);
    mode_t mode=fileData.st_mode;
    int flag=0;

    if(S_ISREG(mode)){
        flag=copyNormailFile(sourcePath,sourceFileName,targetPath,targetFileName);
        // changeStat(targetFile.c_str(),fileData);
    }else if(S_ISCHR(mode)||S_ISBLK(mode)){
        flag=copyDev(sourcePath,sourceFileName,targetPath,targetFileName);
        // changeStat(targetFile.c_str(),fileData);
    }else if(S_ISFIFO(mode)){
        flag=copyPipe(sourcePath,sourceFileName,targetPath,targetFileName);
        // changeStat(targetFile.c_str(),fileData);
    }else if(S_ISLNK(mode)){
        flag=copyLink(sourcePath,sourceFileName,targetPath,targetFileName);
        // changeStat(targetFile.c_str(),fileData);
    }else if(S_ISSOCK(mode)){
        flag=copySocket(sourcePath,sourceFileName,targetPath,targetFileName);
        // changeStat(targetFile.c_str(),fileData);
    }else{
        cout<<"unkown file type, unable to copy."<<endl;
        flag=-1;
    }
    return flag;
}

/* cp,自动判断文件类型进行复制 */
int cp(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    struct stat fileData=getStat(sourcePath,sourceFileName);
    if(S_ISDIR(fileData.st_mode)){
        return copyDir(sourcePath,sourceFileName,targetPath,targetFileName);
    }else{
        return copyOtherFile(sourcePath,sourceFileName,targetPath,targetFileName);
    }
}

/* 根据新的目录和新的目录名，在目标目录下创建一个新目录，并递归把所有文件拷贝过去 */
int copyDirWriteRecord(Record &record,const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    createDirList(targetPath);//在复制文件/文件夹之前先创造路径
    string sourceFile=getSourceFile(sourcePath,sourceFileName);
    string targetFile=getSourceFile(targetPath,targetFileName);
    struct dirent *dirData= NULL;
    struct stat fileData=getStat(sourcePath,sourceFileName);
    DIR* dir;
    
    if(sizeof(fileData)<0||!S_ISDIR(fileData.st_mode)){
        cout<<sourceFile<<" is not a dir"<<endl;
        return -1;
    }

    if(!(dir=opendir(sourceFile.c_str()))){
        cout<<"fail to open "<<sourceFile<<endl;
        return -1;
    }

    mkdir(targetFile.c_str(),fileData.st_mode);//创建文件夹
    while((dirData=readdir(dir))!=NULL){
        if (!(strncmp(dirData->d_name,".",1) && strncmp(dirData->d_name,".",2))){
		    continue;//跳过父目录和工作路径和.bashrc
	    }

        string newSourcePath=sourceFile;
        string newSourceFileName=dirData->d_name;
        string newTargetPath=targetFile;
        string newTargetFileName=dirData->d_name;
        cout<<"now is dealing with: "<<getSourceFile(newSourcePath.c_str(),newSourceFileName.c_str())<<endl;
        struct stat tmpStat=getStat(newSourcePath.c_str(),newSourceFileName.c_str());

        //仅修改这个地方
        cpWriteRecord(record,newSourcePath.c_str(),dirData->d_name,newTargetPath.c_str(),dirData->d_name);
    }
    closedir(dir);
    return 0;
}

/* 复制的同时写record */
int cpWriteRecord(Record &record,const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    struct stat fileData=getStat(sourcePath,sourceFileName);
    int newFileNum=record.addRecord(sourcePath,sourceFileName,fileData);//先加记录获取唯一序列号
    string strNewFileNum=to_string(newFileNum);
    rm(targetPath,strNewFileNum.c_str());
    if(S_ISDIR(fileData.st_mode)){
        if(copyDirWriteRecord(record,sourcePath,sourceFileName,targetPath,strNewFileNum.c_str())){
            record.rmRecord(newFileNum);//复制失败，删除record新增行
            cout<<"复制现有记录失败！"<<endl;
            return -1;
        }
    }else{
        if(copyOtherFile(sourcePath,sourceFileName,targetPath,strNewFileNum.c_str())){
            record.rmRecord(newFileNum);//复制失败，删除record新增行
            cout<<"复制现有记录失败！"<<endl;
            return -1;
        }
    }
    cout<<"back up success! back-up file: "<<newFileNum<<endl;
    changeStat(getSourceFile(targetPath,strNewFileNum.c_str()).c_str(),fileData);
    return newFileNum;//返回文件名，方便后续压缩、加密
}

/* 根据新的目录和新的目录名，在目标目录下创建一个新目录，并递归把所有文件拷贝过去 */
int copyDirReadRecord(Record &record,const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    createDirList(targetPath);//在复制文件/文件夹之前先创造路径
    int newFileNum=atoi(sourceFileName);//备份文件名为唯一序列号
    int index=record.getRecord(newFileNum);
    if(index==-1){//不存在于记录中,先复制再增加记录，先删除文件再删除记录，不可能出现这种情况
        cout<<"palceholder"<<endl;
        return -1;
    }
    struct recordLine fileData=record.getLine(index);
    string restorePath=fileData.sourcePath;
    string restoreFileName=fileData.fileName;
    if(targetFileName!=NULL){//指定恢复文件名
        restoreFileName=targetFileName;
    }
    if(targetPath!=NULL){//指定恢复路径
        restorePath=targetPath;
    }

    string sourceFile=getSourceFile(sourcePath,sourceFileName);
    string targetFile=getSourceFile(restorePath.c_str(),restoreFileName.c_str());

    struct dirent *dirData= NULL;
    DIR* dir;
    
    if(sizeof(fileData.s)<0||!S_ISDIR(fileData.s.st_mode)){
        cout<<sourceFile<<" is not a dir"<<endl;
        return -1;
    }

    if(!(dir=opendir(sourceFile.c_str()))){
        cout<<"fail to open "<<sourceFile<<endl;
        return -1;
    }

    mkdir(targetFile.c_str(),fileData.s.st_mode);//创建文件夹
    while((dirData=readdir(dir))!=NULL){
        if (!(strncmp(dirData->d_name,".",1) && strncmp(dirData->d_name,".",2))){
		    continue;//跳过父目录和工作路径和.bashrc
	    }

        string newSourcePath=sourceFile;
        string newSourceFileName=dirData->d_name;
        int sonName=atoi(dirData->d_name);//子文件的唯一序列号
        int sonIndex=record.getRecord(sonName);
        struct recordLine sonFileData=record.getLine(sonIndex);
        string newTargetPath=targetFile;
        string newTargetFileName=sonFileData.fileName;//文件夹下文件无法指定名字，默认为原文件名
        cout<<"now is dealing with: "<<getSourceFile(newSourcePath.c_str(),newSourceFileName.c_str())<<endl;

        cpReadRecord(record,newSourcePath.c_str(),newSourceFileName.c_str(),newTargetPath.c_str(),newTargetFileName.c_str());
    }
    closedir(dir);
    return 0;
}

/* 读record来还原文件 */
int cpReadRecord(Record &record,const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    int newFileNum=atoi(sourceFileName);//备份文件名为唯一序列号
    int index=record.getRecord(newFileNum);
    if(index==-1){//不存在于记录中,先复制再增加记录，先删除文件再删除记录，不可能出现这种情况
        cout<<"palceholder"<<endl;
        return -1;
    }
    struct recordLine fileData=record.getLine(index);
    string restorePath=fileData.sourcePath;
    string restoreFileName=fileData.fileName;
    if(targetFileName!=NULL){//指定恢复文件名
        restoreFileName=targetFileName;
    }
    if(targetPath!=NULL){//指定恢复路径
        restorePath=targetPath;
    }
    rm(restorePath.c_str(),restoreFileName.c_str());
     if(S_ISDIR(fileData.s.st_mode)){
        if(copyDirReadRecord(record,sourcePath,sourceFileName,restorePath.c_str(),restoreFileName.c_str())){
            cout<<"还原失败！路径："<<getSourceFile(restorePath.c_str(),restoreFileName.c_str())<<endl;
            return -1;
        }
    }else{
        if(copyOtherFile(sourcePath,sourceFileName,restorePath.c_str(),restoreFileName.c_str())){
            cout<<"还原失败！路径："<<getSourceFile(restorePath.c_str(),restoreFileName.c_str())<<endl;
            return -1;
        }
    } 
    changeStat(getSourceFile(restorePath.c_str(),restoreFileName.c_str()).c_str(),fileData.s);
    cout<<"还原成功！路径："<<getSourceFile(restorePath.c_str(),restoreFileName.c_str())<<endl;
    return 0;
}

