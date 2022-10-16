#include<stdio.h>
#include<string.h>
#include<iostream>
#include<fstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include "FileCompare.h"

using namespace std;

#define MAX_PATH 100    //工作路径名最大长度
#define LINE_SIZE 128   //每次拷贝最大字节数



/* 若本文件不为文件夹，则删除 */
int deleteFile(const char* path,const char* fileName){
    struct stat buf=getStat(path,fileName);
    string name=getSourceFile(path,fileName);
    if(S_ISDIR(buf.st_mode)){
        cout<<"this is a dir!"<<endl;
        return -1;
    }
    if (remove(name.c_str())) {
        cout<<name<<" 删除失败！"<<endl;
        return -1;
    }
    else {
        cout<<name<<" 删除成功！"<<endl;
        return 0;
    }
}

/* 若本文件为文件夹，则递归删除此文件夹, rm -r */
int deleteDir(const char* path,const char* fileName){
    int flag=0;
    struct stat buf=getStat(path,fileName);
    struct dirent *dirData= NULL;
    DIR* dir;
    string sourceFile=getSourceFile(path,fileName);
    
    if(sizeof(buf)<0||!S_ISDIR(buf.st_mode)){
        cout<<sourceFile<<" is not a dir"<<endl;
        return -1;
    }

    if(!(dir=opendir(sourceFile.c_str()))){
        cout<<"fail to open "<<sourceFile<<endl;
        return -1;
    }

    while((dirData=readdir(dir))!=NULL){
        if (!(strncmp(dirData->d_name,".",1) && strncmp(dirData->d_name,".",2))){
		    continue;//跳过父目录和工作路径和.bashrc
	    }

        string newpath=sourceFile;
        string newFileName=dirData->d_name;
        cout<<"now is dealing with: "<<getSourceFile(newpath.c_str(),newFileName.c_str())<<endl;
        struct stat newStat=getStat(newpath.c_str(),newFileName.c_str());

        if(S_ISDIR(newStat.st_mode)){//若该文件是目录
            deleteDir(newpath.c_str(),newFileName.c_str());
        }else{
            deleteFile(newpath.c_str(),newFileName.c_str());
        }
    }
    closedir(dir);
        
    if (rmdir(sourceFile.c_str())) {
        cout<<sourceFile<<"删除失败！"<<endl;
        return -1;
    }
    else {
        cout<<sourceFile<<"删除成功！"<<endl;
        return 0;
    }
}

/* 通用rm -r，自动判断文件类型 */
int rm(const char* path,const char* fileName){
    struct stat buf=getStat(path,fileName);
    if(!S_ISDIR(buf.st_mode)){
        return deleteFile(path,fileName);
    }
    else{
        return deleteDir(path,fileName);
    }
}
