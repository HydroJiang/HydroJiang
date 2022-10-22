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

int cmp(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName);
int cmpReadRecord(Record &record,const char* sourcePath,const char* sourceFileName);

int cmpNormailFile(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    string sourceFile=getSourceFile(sourcePath,sourceFileName);
    string targetFile=getSourceFile(targetPath,targetFileName);
    char buffer1[LINE_SIZE];
    char buffer2[LINE_SIZE];

    char workPath[MAX_PATH];
    //getcwd(workPath, MAX_PATH);//获得当前工作路径,用于写日志
    struct stat sourceStat=getStat(sourcePath,sourceFileName);
    struct stat targetStat=getStat(targetPath,targetFileName);

    if(S_ISDIR(sourceStat.st_mode)){
        cout<<sourceFile<<" is not a regular file!"<<endl;
        return -1;
    }
    if(S_ISDIR(targetStat.st_mode)){
        cout<<targetFile<<" is not a regular file!"<<endl;
        return -1;
    }
    if(cmpStat(sourceStat,targetStat)){
        cout<<"file stat differs."<<endl;
        return -1;
    }
    if(!S_ISREG(sourceStat.st_mode)){
        cout<<sourceFile<<"this is a special file! stat same!"<<endl;
        return 0;//特殊文件只比对stat
    }

    ifstream inFile,outFile;
    outFile.open(targetFile,ios::in|ios::binary);
    inFile.open(sourceFile,ios::in|ios::binary);
    int size=sourceStat.st_size;
    if(!inFile.is_open()){
        cout<<"fail to open file: "<<sourceFile<<endl;
        outFile.close();
        inFile.close();
        return -1;
    }else{
        cout<<"file: "<<sourceFile<<" opened!"<<endl;
    }
    if(!outFile.is_open()){
      cout<<"fail to create file: "<<targetFile<<endl;
        outFile.close();
        inFile.close();
        return -1;
    }else{
        cout<<"file: "<<targetFile<<" opened!"<<endl;
    }
    cout<<"file size: "<<size<<endl;
    inFile.seekg(0, ios::beg);
    outFile.seekg(0, ios::beg);
    int len;
    while (!inFile.eof()&&!outFile.eof()&&size>0){  //eof到文件末尾返回true
        len=min<int>(size,LINE_SIZE);
        inFile.read(buffer1,len);  
        outFile.read(buffer2,len); 
        // cout<<buffer1<<endl;
        // cout<<buffer2<<endl; 
        size-=len;
        if(strncmp(buffer1,buffer2,len)){
            cout<<"file "<<sourceFile<<" and "<<targetFile<<" differs!"<<endl;
            inFile.close();
            outFile.close();
            return -1;
        }
    }  
    inFile.close();
    outFile.close();
    cout<<"file "<<sourceFile<<" and "<<targetFile<<" are the same!"<<endl;
    return 0;
    
}

int cmpDir(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    string sourceFile=getSourceFile(sourcePath,sourceFileName);
    string targetFile=getSourceFile(targetPath,targetFileName);
    struct dirent *dirData= NULL;
    struct dirent *dirData2= NULL;
    struct stat fileData1=getStat(sourcePath,sourceFileName);
    struct stat fileData2=getStat(targetPath,targetFileName);
    DIR* dir;
    DIR* dir2;
    
    if(sizeof(fileData1)<0||!S_ISDIR(fileData1.st_mode)){
        cout<<sourceFile<<" is not a dir"<<endl;
        return -1;
    }
    if(sizeof(fileData2)<0||!S_ISDIR(fileData2.st_mode)){
        cout<<targetFile<<" is not a dir"<<endl;
        return -1;
    }

    if(!(dir=opendir(sourceFile.c_str()))){
        cout<<"fail to open "<<sourceFile<<endl;
        return -1;
    }
    if(!(dir2=opendir(targetFile.c_str()))){
        cout<<"fail to open "<<targetFile<<endl;
        return -1;
    }

    if(cmpStat(fileData1,fileData2)){
        cout<<"dir stat differs."<<endl;
        return -1;
    }

    while(true){//不能用||判断2个条件，否则第一个条件为真不会执行第2个
        dirData=readdir(dir);
        dirData2=readdir(dir2);
        if(dirData==NULL&&dirData2==NULL){
            //都遍历完
            break;
        }
        if((dirData==NULL&&dirData2!=NULL)||(dirData!=NULL&&dirData2==NULL)){
            cout<<"some file missing, dir differs"<<endl;
            return -1;
        }
        if ((!(strncmp(dirData->d_name,".",1) && strncmp(dirData->d_name,".",2)))||(!(strncmp(dirData2->d_name,".",1) && strncmp(dirData2->d_name,".",2)))){
		    continue;
	    }

        string newSourcePath=sourceFile;
        string newSourceFileName=dirData->d_name;
        string newTargetPath=targetFile;
        string newTargetFileName=dirData2->d_name;
        cout<<"now is dealing with: "<<getSourceFile(newSourcePath.c_str(),newSourceFileName.c_str())<<endl;
        cout<<" and "<<getSourceFile(newTargetPath.c_str(),newTargetFileName.c_str())<<endl;
        struct stat tmpStat1=getStat(newSourcePath.c_str(),newSourceFileName.c_str());
        struct stat tmpStat2=getStat(newTargetPath.c_str(),newTargetFileName.c_str());

        if(cmpStat(tmpStat1,tmpStat2)){
            cout<<newSourcePath+'/'+newSourceFileName<<" and "<<newTargetPath+'/'+newTargetFileName<<" differs. "<<endl;
            closedir(dir);
            closedir(dir2);
            return -1;
        }

        if(cmp(newSourcePath.c_str(),dirData->d_name,newTargetPath.c_str(),dirData->d_name)){
            cout<<newSourcePath+'/'+newSourceFileName<<" and "<<newTargetPath+'/'+newTargetFileName<<" differs. "<<endl;
            closedir(dir);
            closedir(dir2);
            return -1;
        }
    }
    closedir(dir);
    closedir(dir2);
    cout<<"dir "<<sourceFile<<" and "<<targetFile<<" are the same!"<<endl;
    return 0;
}

int cmp(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    string sourceFile=getSourceFile(sourcePath,sourceFileName);
    string targetFile=getSourceFile(targetPath,targetFileName);
    struct stat fileData1=getStat(sourcePath,sourceFileName);
    struct stat fileData2=getStat(targetPath,targetFileName);

    if(cmpStat(fileData1,fileData2)){
        cout<<"stat differs."<<endl;
        return -1;
    }
    if(S_ISDIR(fileData1.st_mode)&&S_ISDIR(fileData2.st_mode)){
        if(cmpDir(sourcePath,sourceFileName,targetPath,targetFileName)){
            return -1;
        }
    }else{
        if(cmpNormailFile(sourcePath,sourceFileName,targetPath,targetFileName)){
            return -1;
        }
    }

}

int cmpDirReadRecord(Record &record,const char* sourcePath,const char* sourceFileName){
    int newFileNum=atoi(sourceFileName);//备份文件名为唯一序列号
    int index=record.getRecord(newFileNum);
    if(index==-1){//不存在于记录中  不可能出现这种情况
        cout<<"palceholder"<<endl;
        return -1;
    }
    struct recordLine fileData=record.getLine(index);
    const char* targetPath=fileData.sourcePath;//获取源文件路径
    const char* targetFileName=fileData.fileName;

    string sourceFile=getSourceFile(sourcePath,sourceFileName);
    string targetFile=getSourceFile(targetPath,targetFileName);
    struct dirent *dirData= NULL;
    struct stat fileData1=getStat(sourcePath,sourceFileName);
    struct stat fileData2=getStat(targetPath,targetFileName);
    DIR* dir;
    
    if(sizeof(fileData1)<0||!S_ISDIR(fileData1.st_mode)){
        cout<<sourceFile<<" is not a dir"<<endl;
        return -1;
    }
    if(sizeof(fileData2)<0||!S_ISDIR(fileData2.st_mode)){
        cout<<targetFile<<" is not a dir"<<endl;
        return -1;
    }

    if(!(dir=opendir(sourceFile.c_str()))){
        cout<<"fail to open "<<sourceFile<<endl;
        return -1;
    }

    if(cmpStat(fileData1,fileData2)){
        cout<<"dir stat differs."<<endl;
        return -1;
    }

    while((dirData=readdir(dir))!=NULL){//不能用||判断2个条件，否则第一个条件为真不会执行第2个
        if (!(strncmp(dirData->d_name,".",1) && strncmp(dirData->d_name,".",2))){
		    continue;
	    }

        const char* newSourcePath=sourceFile.c_str();
        const char* newSourceFileName=dirData->d_name;

        if(cmpReadRecord(record,newSourcePath,newSourceFileName)){
            closedir(dir);
            cout<<"dir "<<sourceFile<<" and "<<targetFile<<" are different!"<<endl;
            return -1;
        }
    }
    closedir(dir);
    cout<<"dir "<<sourceFile<<" and "<<targetFile<<" are the same!"<<endl;
    return 0;
}

int cmpReadRecord(Record &record,const char* sourcePath,const char* sourceFileName){
    int newFileNum=atoi(sourceFileName);//备份文件名为唯一序列号
    int index=record.getRecord(newFileNum);
    if(index==-1){//不存在于记录中  不可能出现这种情况
        cout<<"palceholder"<<endl;
        return -1;
    }
    struct recordLine fileData=record.getLine(index);
    const char* targetPath=fileData.sourcePath;//获取源文件路径
    const char* targetFileName=fileData.fileName;
    
    string sourceFile=getSourceFile(sourcePath,sourceFileName);
    string targetFile=getSourceFile(targetPath,targetFileName);
    struct stat fileData1=getStat(sourcePath,sourceFileName);
    struct stat fileData2=getStat(targetPath,targetFileName);

    if(cmpStat(fileData1,fileData2)){
        cout<<"stat differs."<<endl;
        return -1;
    }
    if(S_ISDIR(fileData1.st_mode)&&S_ISDIR(fileData2.st_mode)){
        if(cmpDirReadRecord(record,sourcePath,sourceFileName)){
            return -1;
        }
    }else{
        if(cmpNormailFile(sourcePath,sourceFileName,targetPath,targetFileName)){
            return -1;
        }
    }
    return 0;
}

