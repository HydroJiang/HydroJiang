#include<stdio.h>
#include<string.h>
#include<iostream>
#include<fstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include "back-up.cpp"

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

char* sourcePath="/home/jgqj/source";
char* targetPath="/home/jgqj/target";
char* otherPath="/home/jgqj/other";

int testNormal(){
    Record record;
    char* file1="hello_world.cpp";//cpp
    char* file2="hello_world";//可执行文件
    char* file3="test.docx";//docx
    char* file4="chain.heic";//图片

    string num1=backUp(record,sourcePath,file1,targetPath,file1)+"";
    string num2=backUp(record,sourcePath,file2,targetPath,file2)+"";
    string num3=backUp(record,sourcePath,file3,targetPath,file3)+"";
    string num4=backUp(record,sourcePath,file4,targetPath,file4)+"";

    cout<<file1<<" : "<<num1;
    cout<<file2<<" : "<<num2;
    cout<<file3<<" : "<<num3;
    cout<<file4<<" : "<<num4;

    cout<<putBack(record,otherPath,file1,targetPath,num1.c_str())<<endl;
    cout<<putBack(record,otherPath,file2,targetPath,num2.c_str())<<endl;
    cout<<putBack(record,otherPath,file3,targetPath,num3.c_str())<<endl;
    cout<<putBack(record,otherPath,file4,targetPath,num4.c_str())<<endl;
}

int testDir(){
    Record record;
    char* file1="floder";

    string num1=backUp(record,sourcePath,file1,targetPath,file1)+"";
    cout<<file1<<" : "<<num1;

    cout<<putBack(record,otherPath,file1,targetPath,num1.c_str())<<endl;
}

int testLink(){
    Record record;
    char* file1="soft";

    string num1=backUp(record,sourcePath,file1,targetPath,file1)+"";
    cout<<file1<<" : "<<num1;

    cout<<putBack(record,otherPath,file1,targetPath,num1.c_str())<<endl;
}

int testBlock(){
    Record record;
    char* file1="block";

    string num1=backUp(record,sourcePath,file1,targetPath,file1)+"";
    cout<<file1<<" : "<<num1;

    cout<<putBack(record,otherPath,file1,targetPath,num1.c_str())<<endl;
}

int testFIFO(){
    Record record;
    char* file1="fifo";

    string num1=backUp(record,sourcePath,file1,targetPath,file1)+"";
    cout<<file1<<" : "<<num1;

    cout<<putBack(record,otherPath,file1,targetPath,num1.c_str())<<endl;
}

int testSocket(){
    Record record;
    char* file1="socket";

    string num1=backUp(record,sourcePath,file1,targetPath,file1)+"";
    cout<<file1<<" : "<<num1;

    cout<<putBack(record,otherPath,file1,targetPath,num1.c_str())<<endl;
}

int main(){
    testNormal();
}
