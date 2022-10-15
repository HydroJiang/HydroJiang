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

#define MAX_PATH 100    //工作路径名最大长度
#define LINE_SIZE 128   //每次拷贝最大字节数

string recordPath;
int len;

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
/* 输出文件stat信息 */
void coutStat(const char* path,const char* fileName){
    struct stat *buf=new struct stat;
    if(lstat(getSourceFile(path,fileName).c_str(),buf)!=0) {
        cout<<"didn't find file "<<getSourceFile(path,fileName)<<endl;
        return;
    }
    cout<<getSourceFile(path,fileName)<< "元数据: "<<endl;
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



class Record{
public:
    string sourcePath;
    string fileName;
    struct stat s;
    Record* next;
    Record(){
        char workPath[MAX_PATH];  
        getcwd(workPath, MAX_PATH);//获得当前工作路径,用于写日志
        recordPath=workPath;
        recordPath=recordPath+"/record";
        next=NULL;
    }
    /* 从本地record文件中获取记录信息 */
    void getRecord(){
        char tmp[LINE_SIZE];
        Record* node=this;
        ifstream file;
        file.open(recordPath,ios::in);
        file.getline(tmp,LINE_SIZE);
        len=atoi(tmp);
        for(int i=0;i<len;i++){
            vector<string> v;
            file.getline(tmp,LINE_SIZE);
            split(tmp,v," ");
            node->sourcePath=v[0];
            node->fileName=v[1];
            node->next=new Record;
            node=node->next;
        }
        delete node;//删除最后一个多余节点
    }
    /* 全部输出 */
    void coutRecord(){
        Record* node=this;
        for(int i=0;i<len;i++){
            cout<<node->sourcePath<<'/'<<node->fileName<<endl;
            node=node->next;
        }
    }
    /* 搜索是否在其中，返回在链表中位置 */
    int isInRecord(const char* sourcePath,const char* fileName){
        for(int i=0;i<len;i++){

        }
    }
};

// int main(){
//     cout<<mknod("/home/jgqj/source/block/demo",33188,0);
// }