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
int backUp(Record &record,const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    string sourceFile=getSourceFile(sourcePath,sourceFileName);
    string targetFile=getSourceFile(targetPath,targetFileName);
    struct stat s=getStat(sourcePath,sourceFileName);

    int index=record.getRecord(sourcePath,sourceFileName);//查询是否已备份
    int newFileNum;
    string strNewFileNum;
    if(index!=-1){//已备份，先删除
        newFileNum=record.getLine(index).newFileNum;
        strNewFileNum=to_string(newFileNum);
        cout<<"已备份! file: "<<newFileNum<<endl;
        if(!rm(targetPath,strNewFileNum.c_str())){//删除成功，return 0
            record.rmRecord(index);//删除record中已存在行
        }else{
            cout<<"删除原有记录失败！"<<endl;
            return -1;
        }
    }
    newFileNum=record.addRecord(sourcePath,sourceFileName,s);//先加记录获取唯一序列号
    strNewFileNum=to_string(newFileNum);
    if(!cp(sourcePath,sourceFileName,targetPath,strNewFileNum.c_str())){//复制成功
        cout<<"back up success! back-up file: "<<newFileNum<<endl;
        changeStat(getSourceFile(targetPath,strNewFileNum.c_str()).c_str(),s);
        return newFileNum;//返回文件名，方便后续压缩、加密
    }else{
        record.rmRecord(newFileNum);//复制失败，删除record新增行
        cout<<"复制现有记录失败！"<<endl;
        return -1;
    }
}

/* sourcePath为要源文件路径，为空的时候返回备份路径，可以指定。targetPath为备份文件路径 */
int putBack(Record &record,const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    int newFileNum=atoi(targetFileName);//备份文件名为唯一序列号
    int index=record.getRecord(newFileNum);
    if(index==-1){//不存在于记录中,先复制再增加记录，先删除文件再删除记录，不可能出现这种情况
        cout<<"palceholder"<<endl;
        return -1;
    }
    struct recordLine fileData=record.getLine(index);
    string restorePath=sourcePath;
    string restoreFileName=sourceFileName;
    if(sourcePath==NULL||sourceFileName==NULL){//为空的时候返回备份路径
        restorePath=fileData.sourcePath;
        restoreFileName=fileData.fileName;
    }
    if(cp(targetPath,targetFileName,restorePath.c_str(),restoreFileName.c_str())){//复制失败
        cout<<"还原失败！路径："<<getSourceFile(restorePath.c_str(),restoreFileName.c_str())<<endl;
        return -1;
    }
    changeStat(getSourceFile(restorePath.c_str(),restoreFileName.c_str()).c_str(),fileData.s);
    cout<<"还原成功！路径："<<getSourceFile(restorePath.c_str(),restoreFileName.c_str())<<endl;
    return 0;
}

/* sourcePath为要源文件路径，为空的时候默认备份路径，可以指定。targetPath为备份文件路径 */
int compareFile(Record &record,const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    int newFileNum=atoi(targetFileName);//备份文件名为唯一序列号
    int index=record.getRecord(newFileNum);
    if(index==-1){//不存在于记录中,先复制再增加记录，先删除文件再删除记录，不可能出现这种情况
        cout<<"palceholder"<<endl;
        return -1;
    }
    struct recordLine fileData=record.getLine(index);
    string restorePath=sourcePath;
    string restoreFileName=sourceFileName;
    if(sourcePath==NULL||sourceFileName==NULL){//为空的时候默认备份路径
        restorePath=fileData.sourcePath;
        restoreFileName=fileData.fileName;
    }
    return cmp(targetPath,targetFileName,restorePath.c_str(),restoreFileName.c_str());//反馈信息由cmp输出
}