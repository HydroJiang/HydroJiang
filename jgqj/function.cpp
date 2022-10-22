#include<stdio.h>
#include<string.h>
#include<iostream>
#include<fstream>
#include<math.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include "global.h"
#include "../fxs/FXSGlobal.h"

using namespace std;

/* sourcePath为源文件所属路径，targetPath为备份文件路径 */
int backUp(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName,const string &code){
    configEditor config;
    Record record(config.retTargetPath());//每次完成一次备份写一次文件
    int fileId;
    fileId=cpWriteRecord(record,sourcePath,sourceFileName,targetPath,targetFileName);
    if(fileId==-1){
        cout<<fileId<<" back-up: copy fail!"<<endl;
        return -1;
    }else{
        cout<<fileId<<" back-up: copy success!"<<endl;
    }

    if(isRegOrDir(sourcePath,sourceFileName)){

    string fileIdStr=to_string(fileId);
    string tempName="jgqjtemp";//临时文件名
    string tempPath=targetPath;//临时路径

    // cp(targetPath, fileIdStr.c_str(), targetPath, tempName.c_str());

    if(encryption(targetPath, fileIdStr.c_str(), targetPath, tempName.c_str(),code)){//先加密为目标路径下的tempName文件
        cout<<fileId<<" back-up: encryption fail!"<<endl;
        rm(targetPath,tempName.c_str());
        return -1;
    }else{
        cout<<fileId<<" back-up: encryption success!"<<endl;
    }

    rm(targetPath,fileIdStr.c_str());//删除target文件夹下未加密的文件或文件夹

    if(compress(targetPath, tempName.c_str(), targetPath, fileIdStr.c_str())){//再压缩为目标路径下的原文件
        cout<<fileId<<" back-up: compress fail!"<<endl;
        rm(targetPath,tempName.c_str());
        return -1;
    }else{
        cout<<fileId<<" back-up: compress success!"<<endl;
    }
    record.writeRecord();
    rm(targetPath,tempName.c_str());
    return fileId;
    }
    record.writeRecord();
    return fileId;//其他文件不加密和压缩
}

/* sourcePath为备份文件路径     targetPath为恢复路径，可以为空，为空的时候默认源路径 */
int putBack(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName,const string &code){
    configEditor config;
    Record record(config.retTargetPath());//每次完成一次备份写一次文件

    if(isRegOrDir(sourcePath,sourceFileName)){
    char workPath[MAX_PATH];
    getcwd(workPath,MAX_PATH);
    string tempName="compressTemp";//临时文件名1
    string tempPath=workPath;
    tempPath+="/jgqjtemp";//临时路径

    if(deCompress(sourcePath, sourceFileName, tempPath.c_str(), tempName.c_str())){//先解压为临时路径下的tempName
        cout<<sourceFileName<<" restore: decompress fail!"<<endl;
        rm(workPath,"jgqjtemp");//删除临时文件
        return -1;
    }else{
        cout<<sourceFileName<<" restore: decompress success!"<<endl;
    }

    // cp(tempPath.c_str(), tempName.c_str(), tempPath.c_str(), sourceFileName);

    if(deEncryption(tempPath.c_str(), tempName.c_str(), tempPath.c_str(), sourceFileName,code)){//再解密为临时路径下的sourceFileName
        cout<<sourceFileName<<" restore: deencryption fail!"<<endl;
        rm(workPath,"jgqjtemp");//删除临时文件
        return -1;
    }else{
        cout<<sourceFileName<<" restore: deencryption success!"<<endl;
    }

    if(cpReadRecord(record,tempPath.c_str(),sourceFileName,targetPath,targetFileName)){//最后将临时路径下的sourceFileName拷贝到恢复路径
        cout<<sourceFileName<<" restore: copy fail!"<<endl;
        rm(workPath,"jgqjtemp");//删除临时文件
        return -1;
    }else{
        cout<<sourceFileName<<" restore: copy success!"<<endl;
    }
    rm(workPath,"jgqjtemp");//删除临时文件
    return 0;
    }

    return cpReadRecord(record,sourcePath,sourceFileName,targetPath,targetFileName);
}

/* sourcePath为备份文件路径     自动获取targetpath */
int compareFile(const char* sourcePath,const char* sourceFileName,const string &code){
    configEditor config;
    Record record(config.retTargetPath());

    if(isRegOrDir(sourcePath,sourceFileName)){

    char workPath[MAX_PATH];
    getcwd(workPath,MAX_PATH);
    string tempName="compressTemp";//临时文件名1
    string tempPath=workPath;
    tempPath+="/jgqjtemp";//临时路径

    if(deCompress(sourcePath, sourceFileName, tempPath.c_str(), tempName.c_str())){//先解压为临时路径下的tempName
        cout<<sourceFileName<<" compare: decompress fail!"<<endl;
        rm(workPath,"jgqjtemp");//删除临时文件
        return -1;
    }else{
        cout<<sourceFileName<<" compare: decompress success!"<<endl;
    }

    // cp(tempPath.c_str(), tempName.c_str(), tempPath.c_str(), sourceFileName);

    if(deEncryption(tempPath.c_str(), tempName.c_str(), tempPath.c_str(), sourceFileName,code)){//再解密为临时路径下的sourceFileName
        cout<<sourceFileName<<" compare: deencryption fail!"<<endl;
        rm(workPath,"jgqjtemp");//删除临时文件
        return -1;
    }else{
        cout<<sourceFileName<<" compare: deencryption success!"<<endl;
    }

    //反馈信息由cmp输出
    if(cmpReadRecord(record,tempPath.c_str(),sourceFileName)){//最后将临时路径下的sourceFileName与源文件比较
        // cout<<sourceFileName<<" restore: copy fail!"<<endl;
        rm(workPath,"jgqjtemp");//删除临时文件
        return -1;
    }else{
        // cout<<sourceFileName<<" restore: copy success!"<<endl;
    }
    rm(workPath,"jgqjtemp");//删除临时文件
    return 0;

    }

    return cmpReadRecord(record,sourcePath,sourceFileName);
}

int rmBackUp(const char* sourcePath,const char* sourceFileName){
    configEditor config;
    Record record(config.retTargetPath());//每次完成一次备份写一次文件
    int ret=rmWriteRecord(record,sourcePath,sourceFileName);//反馈信息由rm输出
    record.writeRecord();
    return ret;
}
