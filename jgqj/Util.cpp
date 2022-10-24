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
#include <pwd.h>
#include <grp.h>

#include "global.h"          /* some utility functions */

using namespace std;

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

/* 输入路径，检查路径上的每一个文件夹是否存在，不存在则创建文件夹 */
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

/* 输入目标路径和stat，将目标路径的stat改为s */
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

/* 输入路径和权限，新建套接字文件 */
int mksock (const char *__path, const __mode_t &__mode){
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

/* 拼接path和filename，返回拼接后的string */
string getSourceFile(const char* path,const char* fileName){
    string a=path;
    string b=fileName;
    return a+'/'+b;
}
/* 输入路径，返回struct stat */
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

/* 输入文件路径，输出文件stat信息 */
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

/* 比对2个stat是否相同，仅比对部分字段 */
int cmpStat(const struct stat &a,const struct stat &b){
    if(a.st_size!=b.st_size) return -1;
    if((a.st_mode&S_IFMT)!=(b.st_mode&S_IFMT)) return -1;
    return 0;
}

/* 输入源路径和目标路径，复制源路径文件内容到目标路径，不管文件格式
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

/* 输入绝对路径，将绝对路径分为路径和文件名 */
void tearPathAndName(const string &pathAndName,string &path,string &name){
    int pos=pathAndName.find_last_of('/');
    cout<<pos<<endl;
    path=pathAndName.substr(0,pos);
    name=pathAndName.substr(pos+1,pathAndName.size()-1);
    cout<<path<<endl;
    cout<<name<<endl;
}

/* 从本地record文件中获取记录信息，存到对象中 */
int Record::readRecord(){
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
    inFile.close();
    return 0;
}

/* 构造函数，获取当前工作路径并读取当前目录下record中信息 */
Record::Record(){
    char workPath[MAX_PATH];  
    getcwd(workPath, MAX_PATH);//获得当前工作路径,用于写日志
    this->recordPath=workPath;
    this->recordPath+="/record";
    readRecord();
}

/* 构造函数，输入record所在的路径，获取record信息 */
Record::Record(string path){
    this->recordPath=path;
    this->recordPath+="/record";
    readRecord();
}

/* 写record文件,备份完文件用 */
int Record::writeRecord(){
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
    outFile.close();
    return 0;
}
    
/* 输出record文件，调试用 */
void Record::coutRecord(){
    for(int i=0;i<this->file.size();i++){
        cout<<"newFileNum: "<<this->file[i].newFileNum<<endl;
        printStat(&file[i].s,getSourceFile(this->file[i].sourcePath,this->file[i].fileName));
    }
}

/* 输入文件名和文件路径，搜索是否在record中，是则返回在链表中位置index，否则返回-1 */
int Record::getRecord(const char* sourcePath,const char* fileName){
    int i;
    int length=file.size();
    for(i=0;i<length;i++){
        if(!strcmp(file[i].sourcePath,sourcePath)&&!strcmp(file[i].fileName,fileName))
            return i;
    }
    if(i==length) return -1;
}

/* 通过文件唯一标识符，搜索是否在record中，返回在链表中位置index，否则返回-1 */
int Record::getRecord(const int& newFileNum){
    int i;
    int length=file.size();
    for(i=0;i<length;i++){
        if(newFileNum==file[i].newFileNum)
            return i;
    }
    if(i==length) return -1;
}

/* 输入index，返回对应的文件信息 */
const struct recordLine& Record::getLine(const int &index){
    return file[index];
}

/* 输入文件路径，文件名，文件信息，在record中加一行，并返回新生成的文件唯一序号newFileNum */
int Record::addRecord(const char* sourcePath,const char* fileName,const struct stat &s){
    struct recordLine temp;
    strcpy(temp.fileName,fileName);
    strcpy(temp.sourcePath,sourcePath);
    temp.s=s;
    clock_gettime(CLOCK_REALTIME, &temp.backUpTime);//备份文件时，自动获取当前系统时间
    if(file.size()) temp.newFileNum=(file.end()-1)->newFileNum+1;
    else temp.newFileNum=1;
    this->file.push_back(temp);
    len++;
    return temp.newFileNum;
}

/* 根据index删除文件，需要先调用getRecord获取index，注意先判断返回值是否为-1 */
int Record::rmRecord(const int &index){
    this->file.erase(file.begin()+index);
    if(len)len--;
    else {
        cout<<"rm record fail!"<<endl;
        return -1;
    }
    cout<<"rm record success!"<<endl;
    return 0;
}


/* CONFIG的默认参数 */
void configEditor::placeHolder(){
    string temp=workPath+"/target";
    strcpy(CONFIG.backUpPath,temp.c_str());
    CONFIG.compress=true;
    CONFIG.encryption=true;
}

/* 从本地config文件中获取设置信息,构造函数用 */
int configEditor::readConfig(){
    ifstream inFile;
    inFile.open(configFilePath,ios::in|ios::binary);
    if(!inFile.is_open()){
        cout<<"fail to open file: "<<configFilePath<<endl;
        inFile.close();
        placeHolder();//设为默认参数
        return -1;
    }
    int count;
    inFile.read((char*)(&CONFIG),sizeof(config));
    inFile.close();
    cout<<"config: "<<configFilePath<<" read! "<<endl;
    return 0;
}

/* 构造函数，获取本地config信息 */ 
configEditor::configEditor(){
    char temp[MAX_PATH];  
    getcwd(temp, MAX_PATH);//获得当前工作路径,用于写日志
    this->workPath=temp;
    this->configFilePath=this->workPath+"/do_not_touch!";
    readConfig();
}

/* 写本地config文件,修改完设置用 */
int configEditor::writeConfig(){
    ofstream outFile;
    outFile.open(configFilePath,ios::out|ios::binary|ios::trunc);
    if(!outFile.is_open()){
        cout<<"fail to open file: "<<configFilePath<<endl;
        outFile.close();
        return -1;
    }
    outFile.write(reinterpret_cast<char *>(&CONFIG),sizeof(config));
    outFile.close();
    cout<<"config: "<<configFilePath<<" write! "<<endl;
    return 0;
}

/* 修改设置 */
void configEditor::changeConfig(const config &temp){
    strcpy(CONFIG.backUpPath,temp.backUpPath);
    CONFIG.compress=temp.compress;
    CONFIG.encryption=temp.encryption;
}
    
/* 输出设置信息，调试用 */
void configEditor::coutConfig(){
    cout<<"back up dir: "<<CONFIG.backUpPath<<endl;
    if(!CONFIG.compress)
        cout<<"Do not ";
    cout<<"compress while back up."<<endl;
    if(!CONFIG.encryption)
        cout<<"Do not ";
    cout<<"encryption while back up."<<endl;
}

/* 返回备份目录 */
string configEditor::retTargetPath(){
    string temp=this->CONFIG.backUpPath;
    return temp;
}

/* 返回是否压缩，废弃 */
bool configEditor::retIsCompress(){
    return this->CONFIG.compress;
}

/* 返回是否加密，废弃 */
bool configEditor::retIsEncryption(){
    return this->CONFIG.encryption;
}

/* 将文件mode化为_rw_r__r__的字符串形式 */
string modeToStr(const mode_t &mode){
    string str="";

/*
        S_ISREG(st_mode)  is it a regular file?     普通文件
        S_ISDIR(m)  directory?                      目录文件
        S_ISCHR(m)  character device?               字符设备文件
        S_ISBLK(m)  block device?                   块设备文件
        S_ISFIFO(m) FIFO (named pipe)?              管道文件
        S_ISLNK(m)  symbolic link?                  软链接文件
        S_ISSOCK(m) socket?                         socket文件
*/
    //类型
    if(S_ISDIR(mode)){
        str+="d";
    }else if(S_ISCHR(mode)){
        str+="c";
    }else if(S_ISBLK(mode)){
        str+="b";
    }else if(S_ISFIFO(mode)){
        str+="p";
    }else if(S_ISLNK(mode)){
        str+="l";
    }else{
        str+="_";
    }
/*
        S_IRWXU     00700   owner has read, write, and execute permission
        S_IRUSR     00400   owner has read permission
        S_IWUSR     00200   owner has write permission
        S_IXUSR     00100   owner has execute permission
        S_IRWXG     00070   group has read, write, and execute permission
        S_IRGRP     00040   group has read permission
        S_IWGRP     00020   group has write permission
        S_IXGRP     00010   group has execute permission
        S_IRWXO     00007   others (not in group) have read,  write,  and execute permission
        S_IROTH     00004   others have read permission
        S_IWOTH     00002   others have write permission
        S_IXOTH     00001   others have execute permission
*/
    //usr
    //&优先级低于==，需要加括号
    if((mode&S_IRUSR)==S_IRUSR){
        str+="r";
    }else{
        str+="_";
    }
    if((mode&S_IWUSR)==S_IWUSR){
        str+="w";
    }else{
        str+="_";
    }
    if((mode&S_IXUSR)==S_IXUSR){
        str+="x";
    }else{
        str+="_";
    }

    //group
    if((mode&S_IRGRP)==S_IRGRP){
        str+="r";
    }else{
        str+="_";
    }
    if((mode&S_IWGRP)==S_IWGRP){
        str+="w";
    }else{
        str+="_";
    }
    if((mode&S_IXGRP)==S_IXGRP){
        str+="x";
    }else{
        str+="_";
    }

    //other
    if((mode&S_IROTH)==S_IROTH){
        str+="r";
    }else{
        str+="_";
    }
    if((mode&S_IWOTH)==S_IWOTH){
        str+="w";
    }else{
        str+="_";
    }
    if((mode&S_IXOTH)==S_IXOTH){
        str+="x";
    }else{
        str+="_";
    }
    return str;
}

/* 将timespec化为字符串的可读形式 */
string timeSpecToStr(const timespec &t){
    time_t now=t.tv_sec;
	return ctime(&now);
}

/* 将组信息和用户信息化为字符串的可读形式 */
string ownerGroup(const struct stat &info){
    struct passwd *pw = getpwuid(info.st_uid);
    struct group  *gr = getgrgid(info.st_gid);

    string temp=pw->pw_name;
    temp+=":";
    temp+=+gr->gr_name;
    return temp;
}

/* 判断是否为目录或普通文件，是返回1，否返回0 */
bool isRegOrDir(const char* path,const char* name){
    struct stat s=getStat(path,name);
    mode_t m=s.st_mode;

    return (S_ISDIR(m)|S_ISREG(m));
}