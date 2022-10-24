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

/* 输入目标路径，删除目标路径指向的文件，成功返回0，失败返回-1，只能删除普通文件 */
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

/* 输入目标路径，删除目标路径指向的目录及下面所有文件，成功返回0，失败返回-1，只能删除目录 */
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

        if(rm(newpath.c_str(),newFileName.c_str())){
            return -1;
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

/* 输入目标路径，删除目标路径指向的文件或路径，成功返回0，失败返回-1 */
int rm(const char* path,const char* fileName){
    if(access(getSourceFile(path,fileName).c_str(),F_OK)) return -1;
    struct stat buf=getStat(path,fileName);
    if(!S_ISDIR(buf.st_mode)){
        return deleteFile(path,fileName);
    }
    else{
        return deleteDir(path,fileName);
    }
}

/* 输入目标路径，删除目标路径指向的目录及下面所有文件，并删除record中的对应文件信息，成功返回0，失败返回-1，只能删除目录 */
int rmDirWriteRecord(Record &record,const char* path,const char* fileName){
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

        if(rmWriteRecord(record,newpath.c_str(),newFileName.c_str())){
            return -1;
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

/* 输入目标路径，删除目标路径指向的文件或目录，并删除record中的对应文件信息，成功返回0，失败返回-1 */
int rmWriteRecord(Record &record,const char* path,const char* fileName){
    string name=getSourceFile(path,fileName);
    int newFileNum=atoi(fileName);//备份文件名为唯一序列号
    int index=record.getRecord(newFileNum);
    if(index==-1){//不存在于记录中  不可能出现这种情况
        cout<<"palceholder"<<endl;
        return -1;
    }
    struct recordLine fileData=record.getLine(index);
    const char* targetPath=fileData.sourcePath;//获取源文件路径
    const char* targetFileName=fileData.fileName;

    struct stat buf=getStat(path,fileName);
    if(!S_ISDIR(buf.st_mode)){
        if(deleteFile(path,fileName)){
            return -1;
        }
    }
    else{
        if(rmDirWriteRecord(record,path,fileName)){
            return -1;
        }
    }
    return record.rmRecord(index);

}

