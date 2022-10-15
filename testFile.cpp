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

int FileTest(){
    char* sourcePath="/home/jgqj/source";
    char* targetPath="/home/jgqj/target";
    char* file1="floder";
    char* file2="floder";

    // // cp test
    // cp(sourcePath,file1,targetPath,file1);

    // // copyLink测试
    // copyLink(sourcePath,file1,targetPath,file1);

    // copypipe测试
    // copyPipe(sourcePath,file1,targetPath,file1);

    // // copyNormailfile测试
    // copyNormailFile(sourcePath,file2,targetPath,file2);

    // // copyDir测试
    // copyDir(sourcePath,file1,targetPath,file1);

    // coutStat测试
    coutStat(sourcePath,file1);
    coutStat(targetPath,file2);

    // // deleteFile测试
    // coutStat(targetPath,file2);
    // deleteFile(targetPath,file2);

    // // deleteDir测试
    // coutStat(targetPath,file1);
    // deleteDir(targetPath,file1);

    // // createDirList测试
    // char* t="/home/jgqj/test1/test2/test3";
    // createDirList(t);

    // // cmpNormaiFile test
    // cmpNormailFile(sourcePath,file1,targetPath,file1);

    // // cmpStat test
    // struct stat a=getStat(sourcePath,file1);
    // struct stat b=getStat(targetPath,file2);
    // cout<< cmpStat(a,b)<<endl;

    // // cmpDir test
    // cout<<cmpDir(sourcePath,file1,targetPath,file1)<<endl;

    // // cmp test
    // cout<<cmp(sourcePath,file1,targetPath,file1)<<endl;
}

int main(){
    char* sourcePath="/home/jgqj/source";
    char* targetPath="/home/jgqj/target";
    char* file1="floder";
    char* file2="floder";

    
}