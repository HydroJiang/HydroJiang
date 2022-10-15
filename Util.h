#include<stdio.h>
#include<string.h>
#include<iostream>
#include<fstream>
#include<vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

#include "sockutil.h"          /* some utility functions */

using namespace std;

#define MAX_PATH 128    //工作路径名最大长度
#define LINE_SIZE 128   //每次拷贝最大字节数

/* 将字符串s，以c为分隔符，分为部分塞到向量v中 */
void split(const string& s, vector<string>& v, const string& c)
{
	string::size_type pos1, pos2;
	size_t len = s.length();
	pos2 = s.find(c);
	pos1 = 0;
	while (string::npos != pos2)
	{
		v.emplace_back(s.substr(pos1, pos2 - pos1));
 
		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != len)
		v.emplace_back(s.substr(pos1));
}

/*
int access(const char* pathname, int mode);
 
参数介绍：
 
    pathname 是文件的路径名+文件名
 
    mode：指定access的作用，取值如下
 
F_OK 值为0，判断文件是否存在
 
X_OK 值为1，判断对文件是可执行权限
 
W_OK 值为2，判断对文件是否有写权限
 
R_OK 值为4，判断对文件是否有读权限
*/
/* 检查路径上的每一个文件夹是否存在，不存在则创建文件夹，用于还原备份，还原备份之前先创建路径 */
void createDirList(const char* sourcePath){
    vector<string> dir;
    split(sourcePath,dir,"/");
    string path=dir[0];
    for(int i=1 ;i<dir.size();i++){
        string temp=path+'/'+dir[i];
        cout<<"now with : "<<temp<<endl;
        if(access(temp.c_str(),0)){
            cout<<"mkdir "<<temp<<endl;
            mkdir(temp.c_str(),16877);
        }
        path=path+'/'+dir[i];
    }
}

void changeStat(const char* targetFile, const struct stat &s){
    int flag;
    flag=chmod(targetFile,s.st_mode);//同步权限
    if(!flag) cout<<targetFile<<" chmod success!"<<endl;
    flag=utimensat(0,targetFile,&(s.st_atim),AT_SYMLINK_NOFOLLOW);//同步时间,软链接也可同步
    if(!flag) cout<<targetFile<<" change time success!"<<endl;
    // flag=truncate(targetFile,s.st_size);//同步大小
    // if(!flag) cout<<targetFile<<" change time success!"<<endl;
    flag=lchown(targetFile,s.st_uid,s.st_gid);//同步组id和用户id
    if(!flag) cout<<targetFile<<" chown chgrp success!"<<endl;

}

int mksock (const char *__path, __mode_t __mode){
    struct sockaddr_un namesock;
    int fd;
    namesock.sun_family = AF_UNIX;
    strncpy(namesock.sun_path, (char *)__path, sizeof(namesock.sun_path));
    fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(bind(fd, (struct sockaddr *) &namesock, sizeof(struct sockaddr_un))){
        cout<<"socket: "<<__path<<" created fail!"<<endl;
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

string getSourceFile(const char* path,const char* fileName){
    string a=path;
    string b=fileName;
    return a+'/'+b;
}
/* 更新元数据，并返回struct stat */
struct stat getStat(const char* path,const char* fileName){
    struct stat fileData;
    //这里使用lstat，若目标文件为软链接，则获取软连接文件stat，而非它指向的文件的stat
    lstat(getSourceFile(path,fileName).c_str(),&fileData);
    return fileData;
}

/* coutStat部分可重用代码 */
void printStat(const struct stat *buf,const string &file){
    cout<<file<< "元数据: "<<endl;
    cout<<"设备ID: "<<buf->st_dev<<endl;
    cout<<"inode节点号: "<<buf->st_ino<<endl;
    cout<<"文件类型和权限: "<<buf->st_mode<<endl;
    cout<<"硬连接数: "<<buf->st_nlink<<endl;
    cout<<"用户ID: "<<buf->st_uid<<endl;
    cout<<"组ID: "<<buf->st_gid<<endl;
    cout<<"特殊设备ID号: "<<buf->st_rdev<<endl;
    cout<<"总字节数: "<<buf->st_size<<endl;
    cout<<"IO块字节数: "<<buf->st_blksize<<endl;
    cout<<"占用512b的block块的数量: "<<buf->st_blocks<<endl;
    cout<<"最后访问时间: "<<buf->st_atim.tv_sec<<endl;
    cout<<"最后修改时间: "<<buf->st_mtim.tv_sec<<endl;
    cout<<"最后属性的修改时间: "<<buf->st_ctim.tv_sec<<endl;

    if(S_ISREG(buf->st_mode)) cout<<"this is a regular file."<<endl;
    if(S_ISDIR(buf->st_mode)) cout<<"this is a directory."<<endl;
    if(S_ISCHR(buf->st_mode)) cout<<"this is a character device."<<endl;
    if(S_ISBLK(buf->st_mode)) cout<<"this is a block device."<<endl;
    if(S_ISFIFO(buf->st_mode)) cout<<"this is a FIFO (named pipe)."<<endl;
    if(S_ISLNK(buf->st_mode)) cout<<"this is a symbolic link."<<endl;
    if(S_ISSOCK(buf->st_mode)) cout<<"this is a socket."<<endl;
}

/* 输出文件stat信息 */
void coutStat(const char* path,const char* fileName){
    struct stat *buf=new struct stat;
    string name=getSourceFile(path,fileName);
    if(lstat(name.c_str(),buf)!=0) {
        cout<<"didn't find file "<<getSourceFile(path,fileName)<<endl;
        return;
    }
    printStat(buf,name);
    delete(buf);
}
/* 比对2个stat是否相同，仅比对内容 */
int cmpStat(const struct stat &a,const struct stat &b){
    if(a.st_size!=b.st_size) return -1;
    if((a.st_mode&S_IFMT)!=(b.st_mode&S_IFMT)) return -1;
    return 0;
}

/* 复制两个文件内容，不管文件格式
不可用于软链接复制，否则会覆盖链接指向文件的内容;
不需要复制管道，因为它存不了东西; */
int copyContent(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    string sourceFile=getSourceFile(sourcePath,sourceFileName);
    string targetFile=getSourceFile(targetPath,targetFileName);
    
    ofstream outFile;
    ifstream inFile;
    outFile.open(targetFile,ios::out|ios::binary|ios::trunc);
    inFile.open(sourceFile,ios::in|ios::binary);
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
        cout<<"file: "<<targetFile<<" created!"<<endl;
    }
    outFile<<inFile.rdbuf();//输入流直接对输出流输出
    cout<<"file: "<<targetFile<<" copy sucucess!"<<endl;
    inFile.close();
    outFile.close();
    return 0;
}

struct recordLine{
    int num;
    char sourcePath[MAX_PATH];
    char fileName[MAX_PATH];
    struct stat s;
};

class Record{
private:
    string recordPath;
    vector<struct recordLine> file;
    int len;

/*
    void charToStat(struct stat& s,const string &str,const int &size){
        int start=0;
        s.st_dev=(dev_t)str.substr(start,sizeof(dev_t)).c_str();
        start+=sizeof(dev_t);
        s.st_ino=(ino_t)str.substr(start,sizeof(ino_t)).c_str();
        start+=sizeof(ino_t);
        s.st_mode=(mode_t)str.substr(start,sizeof(mode_t)).c_str();
        start+=sizeof(mode_t);
        s.st_nlink=(nlink_t)str.substr(start,sizeof(nlink_t)).c_str();
        start+=sizeof(nlink_t);
        s.st_uid=(uid_t)str.substr(start,sizeof(uid_t)).c_str();
        start+=sizeof(uid_t);
        s.st_gid=(gid_t)str.substr(start,sizeof(gid_t)).c_str();
        start+=sizeof(gid_t);
        s.st_rdev=(dev_t)str.substr(start,sizeof(dev_t)).c_str();
        start+=sizeof(dev_t);
        s.st_size=(off_t)str.substr(start,sizeof(off_t)).c_str();
        start+=sizeof(off_t);
        s.st_blksize=(blksize_t)str.substr(start,sizeof(blksize_t)).c_str();
        start+=sizeof(blksize_t);
        s.st_blocks=(blkcnt_t)str.substr(start,sizeof(blkcnt_t)).c_str();
        start+=sizeof(blkcnt_t);
        s.st_atim=(struct timespec)str.substr(start,sizeof(struct timespec)).c_str();
        start+=sizeof(struct timespec);
        s.st_mtim=(struct timespec)str.substr(start,sizeof(struct timespec)).c_str();
        start+=sizeof(struct timespec);
        s.st_ctim=(struct timespec)str.substr(start,sizeof(struct timespec)).c_str();
        start+=sizeof(struct timespec);
    }
*/

    /* 从本地record文件中获取记录信息,构造函数用 */
    int readRecord(){
        ifstream inFile;
        inFile.open(recordPath,ios::in|ios::binary);
        if(!inFile.is_open()){
            cout<<"fail to open file: "<<recordPath<<endl;
            inFile.close();
            return -1;
        }
        this->len=0;
        struct recordLine temp;
        int count;
        while(!inFile.eof()){
            inFile.read(reinterpret_cast<char *>(&temp),sizeof(recordLine));
            this->file.push_back(temp);
            this->len++;
        }
        if(file.size()){
            this->len--;
            this->file.erase(file.end()-1);
        }

        cout<<"file: "<<recordPath<<" read! "<<endl;
        return 0;
    }
    /* 写本地record文件,析构函数用 */
    int writeRecord(){
        ofstream outFile;
        outFile.open(recordPath,ios::out|ios::binary|ios::trunc);
        if(!outFile.is_open()){
            cout<<"fail to open file: "<<recordPath<<endl;
            outFile.close();
            return -1;
        }
        struct recordLine temp;
        for(int i=0;i<this->file.size();i++){
            temp=this->file[i];
            outFile.write(reinterpret_cast<char *>(&temp),sizeof(recordLine));
        }
        cout<<"file: "<<recordPath<<" write! "<<endl;
        return 0;
    }
    
public:
    Record(){
        char workPath[MAX_PATH];  
        getcwd(workPath, MAX_PATH);//获得当前工作路径,用于写日志
        this->recordPath=workPath;
        this->recordPath+="/record";
        readRecord();
    }
    ~Record(){
        writeRecord();
    }
    
    /* 全部输出 */
    void coutRecord(){
        for(int i=0;i<this->file.size();i++){
            cout<<"num: "<<this->file[i].num<<endl;
            printStat(&file[i].s,getSourceFile(this->file[i].sourcePath,this->file[i].fileName));
        }
    }
    /* 通过文件名和文件路径，搜索是否在其中，返回在链表中位置 */
    int getRecord(const char* sourcePath,const char* fileName){
        int i;
        int length=file.size();
        for(i=0;i<length;i++){
            if(!strcmp(file[i].sourcePath,sourcePath)&&!strcmp(file[i].fileName,fileName))
                return i;
        }
        if(i==length) return -1;
    }

    /* 通过唯一序号，搜索是否在其中，返回在链表中位置 */
    int getRecord(const int& num){
        int i;
        int length=file.size();
        for(i=0;i<length;i++){
            if(num==file[i].num)
                return i;
        }
        if(i==length) return -1;
    }

    /* 备份文件后，在record中加一行，并返回唯一序号 */
    int addRecord(const char* sourcePath,const char* fileName,struct stat s){
        struct recordLine temp;
        strcpy(temp.fileName,fileName);
        strcpy(temp.sourcePath,sourcePath);
        temp.s=s;
        if(file.size()) temp.num=(file.end()-1)->num+1;
        else temp.num=0;
        this->file.push_back(temp);
        len++;
        return temp.num;
    }

    /* 根据唯一序号删除文件，需要先调用getRecord获取唯一序号，注意先判断返回值是否为-1 */
    int rmRecord(int index){
        this->file.erase(file.begin()+index);
        if(len)len--;
        else {
            cout<<"error!"<<endl;
            return -1;
        }
        return 0;
    }
};

/* 测试函数 */
void add1234(){
    Record record;
    char* path1="/home/jgqj/source";
    char* path2="/home/jgqj/target";
    char* file1="test.docx";
    char* file2="hard";
    char* file3="hello_world";    
    char* file4="hello_world.cpp"; 

    record.addRecord(path1,file1,getStat(path1,file1));   
    record.addRecord(path1,file2,getStat(path1,file2));   
    record.addRecord(path1,file3,getStat(path1,file3));   
    record.addRecord(path1,file4,getStat(path1,file4));   

    record.coutRecord();

}
/* 测试函数 */
int rm3(){
    Record record;
    char* path1="/home/jgqj/source";
    char* path2="/home/jgqj/target";
    char* file1="test.docx";
    char* file2="hard";
    char* file3="hello_world";    
    char* file4="hello_world.cpp"; 

    record.coutRecord();
    record.rmRecord(record.getRecord(path1,file3));
    record.coutRecord();
}
/* 测试函数 */
int add4(){
    Record record;
    char* path1="/home/jgqj/source";
    char* path2="/home/jgqj/target";
    char* file1="test.docx";
    char* file2="hard";
    char* file3="hello_world";    
    char* file4="hello_world.cpp"; 

    record.coutRecord(); 
    record.addRecord(path1,file4,getStat(path1,file4));   
    record.coutRecord(); 
}

// int main(){
//     remove("record");
//     cout<<"add1234"<<endl<<endl;
//     add1234();
//     cout<<"rm3"<<endl<<endl;
//     rm3();
//     cout<<"add4"<<endl<<endl;
//     add4();


// }