#include<stdio.h>
#include<string.h>
#include<iostream>
#include<fstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include "Util.h"

using namespace std;

int cmp(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName);

int cmpNormailFile(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    string sourceFile=getSourceFile(sourcePath,sourceFileName);
    string targetFile=getSourceFile(targetPath,targetFileName);
    char buffer1[LINE_SIZE];
    char buffer2[LINE_SIZE];

    char workPath[MAX_PATH];
    //getcwd(workPath, MAX_PATH);//获得当前工作路径,用于写日志
    struct stat sourceStat=getStat(sourcePath,sourceFileName);
    struct stat targetStat=getStat(targetPath,targetFileName);

    if(!S_ISREG(sourceStat.st_mode)){
        cout<<sourceFile<<" is not a regular file!"<<endl;
        return -1;
    }
    if(!S_ISREG(targetStat.st_mode)){
        cout<<targetFile<<" is not a regular file!"<<endl;
        return -1;
    }
    if(cmpStat(sourceStat,targetStat)){
        cout<<"file stat differs."<<endl;
        return -1;
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
    struct dirent *dirData1= NULL;
    struct dirent *dirData2= NULL;
    struct stat fileData1=getStat(sourcePath,sourceFileName);
    struct stat fileData2=getStat(targetPath,targetFileName);
    DIR* dir1;
    DIR* dir2;
    
    if(sizeof(fileData1)<0||!S_ISDIR(fileData1.st_mode)){
        cout<<sourceFile<<" is not a dir"<<endl;
        return -1;
    }
    if(sizeof(fileData2)<0||!S_ISDIR(fileData2.st_mode)){
        cout<<targetFile<<" is not a dir"<<endl;
        return -1;
    }

    if(!(dir1=opendir(sourceFile.c_str()))){
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
        dirData1=readdir(dir1);
        dirData2=readdir(dir2);
        if(dirData1==NULL&&dirData2==NULL){
            //都遍历完
            break;
        }
        if((dirData1==NULL&&dirData2!=NULL)||(dirData1!=NULL&&dirData2==NULL)){
            cout<<"some file missing, dir differs"<<endl;
            return -1;
        }
        if ((!(strncmp(dirData1->d_name,".",1) && strncmp(dirData1->d_name,".",2)))||(!(strncmp(dirData2->d_name,".",1) && strncmp(dirData2->d_name,".",2)))){
		    continue;
	    }

        string newSourcePath=sourceFile;
        string newSourceFileName=dirData1->d_name;
        string newTargetPath=targetFile;
        string newTargetFileName=dirData2->d_name;
        cout<<"now is dealing with: "<<getSourceFile(newSourcePath.c_str(),newSourceFileName.c_str())<<endl;
        cout<<" and "<<getSourceFile(newTargetPath.c_str(),newTargetFileName.c_str())<<endl;
        struct stat tmpStat1=getStat(newSourcePath.c_str(),newSourceFileName.c_str());
        struct stat tmpStat2=getStat(newTargetPath.c_str(),newTargetFileName.c_str());

        if(cmpStat(tmpStat1,tmpStat2)){
            cout<<newSourcePath+'/'+newSourceFileName<<" and "<<newTargetPath+'/'+newTargetFileName<<" differs. "<<endl;
            closedir(dir1);
            closedir(dir2);
            return -1;
        }

        if(cmp(newSourcePath.c_str(),dirData1->d_name,newTargetPath.c_str(),dirData1->d_name)){
            cout<<newSourcePath+'/'+newSourceFileName<<" and "<<newTargetPath+'/'+newTargetFileName<<" differs. "<<endl;
            closedir(dir1);
            closedir(dir2);
            return -1;
        }
    }
    closedir(dir1);
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