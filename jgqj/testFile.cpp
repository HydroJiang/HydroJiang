#include<stdio.h>
#include<string.h>
#include<iostream>
#include<fstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include "global.h"
#include "../fxs/FXSGlobal.h"


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
    
    char* file1="hello_world.cpp";//cpp
    char* file2="hello_world";//可执行文件
    char* file3="test.docx";//docx
    char* file4="0.png";//图片

    string num1=to_string(backUp(sourcePath,file1,targetPath,file1));
    string num2=to_string(backUp(sourcePath,file2,targetPath,file2));
    string num3=to_string(backUp(sourcePath,file3,targetPath,file3));
    string num4=to_string(backUp(sourcePath,file4,targetPath,file4));

    cout<<file1<<" : "<<num1;
    cout<<file2<<" : "<<num2;
    cout<<file3<<" : "<<num3;
    cout<<file4<<" : "<<num4;

    cout<<putBack(targetPath,num1.c_str(),otherPath,file1)<<endl;
    cout<<putBack(targetPath,num2.c_str(),otherPath,file2)<<endl;
    cout<<putBack(targetPath,num3.c_str(),otherPath,file3)<<endl;
    cout<<putBack(targetPath,num4.c_str(),otherPath,file4)<<endl;

    //用法：在这里打断点,并修改已备份文件
    cout<<compareFile(targetPath,num1.c_str())<<endl;
    cout<<compareFile(targetPath,num2.c_str())<<endl;
    cout<<compareFile(targetPath,num3.c_str())<<endl;
    cout<<compareFile(targetPath,num4.c_str())<<endl;

    cout<<rmBackUp(targetPath,num2.c_str())<<endl;
    cout<<rmBackUp(targetPath,num3.c_str())<<endl;
}

int testDir(){
    
    char* file1="floder";

    string num1=to_string(backUp(sourcePath,file1,targetPath,file1));

    cout<<file1<<" : "<<num1;

    cout<<putBack(targetPath,num1.c_str(),otherPath,file1)<<endl;
    //用法：在这里打断点,并修改已备份文件
    cout<<compareFile(targetPath,num1.c_str())<<endl;

    cout<<rmBackUp(targetPath,num1.c_str())<<endl;
}

int testLink(){
    
    char* file1="soft";

    string num1=to_string(backUp(sourcePath,file1,targetPath,file1));

    cout<<file1<<" : "<<num1;

    cout<<putBack(targetPath,num1.c_str(),otherPath,file1)<<endl;
    cout<<compareFile(targetPath,num1.c_str())<<endl;
}

int testBlock(){
    
    char* file1="block";

    string num1=to_string(backUp(sourcePath,file1,targetPath,file1));

    cout<<file1<<" : "<<num1;

    cout<<putBack(targetPath,num1.c_str(),otherPath,file1)<<endl;
    cout<<compareFile(targetPath,num1.c_str())<<endl;
}

int testFIFO(){
    
    char* file1="fifo";

    string num1=to_string(backUp(sourcePath,file1,targetPath,file1));

    cout<<file1<<" : "<<num1;

    cout<<putBack(targetPath,num1.c_str(),otherPath,file1)<<endl;
    cout<<compareFile(targetPath,num1.c_str())<<endl;
}

int testSocket(){
    
    char* file1="socket";

    string num1=to_string(backUp(sourcePath,file1,targetPath,file1));

    cout<<file1<<" : "<<num1;

    cout<<putBack(targetPath,num1.c_str(),otherPath,file1)<<endl;
    cout<<compareFile(targetPath,num1.c_str())<<endl;
}

int testCompressEncryption(){
   char* sourcePath="/home/jgqj/source";
   char* targetPath="/home/jgqj/target";
   char* otherPath="/home/jgqj/other";

   char* file1="hello_world.cpp";//cpp
   char* file2="floder";//可执行文件
   char* file3="1.txt";//docx
   char* file4="0.png";//图片
   string code="201214";

   encryption(sourcePath, file4, targetPath, file4,code);
   deEncryption(targetPath, file4, otherPath, file4,code);
   encryption(sourcePath, file2, targetPath, file2,code);
   deEncryption(targetPath, file2, otherPath, file2,code);

   compress(sourcePath, file4, targetPath, file4);
   deCompress(targetPath, file4, otherPath, file4);
   compress(sourcePath, file2, targetPath, file2);
   deCompress(targetPath, file2, otherPath, file2);

}

//int main(){
//   configEditor c;
//   c.coutConfig();
//   struct config temp;
//   strcpy(temp.backUpPath,"placeholder");
//   temp.compress=true;
//   temp.encryption=true;
//   // c.changeConfig(temp);
//   c.coutConfig();
//}

//int main(){
//    struct stat s=getStat("/home/jgqj/source","block");
//    cout<<modeToStr(s.st_mode)<<endl;
//    cout<<timeSpecToStr(s.st_mtim)<<endl;
//    cout<<ownerGroup(s)<<endl;
//}

//int main(){
//    char* sourcePath="/home/jgqj/source";
//    char* targetPath="/home/jgqj/target";
//    char* otherPath="/home/jgqj/other";

//    char* file1="hello_world.cpp";//cpp
//    char* file2="hello_world";//可执行文件
//    char* file3="test.docx";//docx
//    char* file4="0.png";//图片
//    string code="123456";

//    int a=compress(sourcePath, file1, targetPath, file1);

//}


