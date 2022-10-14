#include<stdio.h>
#include<string.h>
#include<iostream>
#include<fstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include"Util.cpp"

using namespace std;

#define MAX_PATH 100    //工作路径名最大长度
#define LINE_SIZE 128   //每次拷贝最大字节数

int cp(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName);
/* 合并文件所在目录和文件名，获得文件路径 */
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
    struct stat buf=getStat(path,fileName);
    cout<<getSourceFile(path,fileName)<< "元数据: "<<endl;
    cout<<"设备ID: "<<buf.st_dev<<endl;
    cout<<"inode节点号: "<<buf.st_ino<<endl;
    cout<<"文件类型和权限: "<<buf.st_mode<<endl;
    cout<<"硬连接数: "<<buf.st_nlink<<endl;
    cout<<"用户ID: "<<buf.st_uid<<endl;
    cout<<"组ID: "<<buf.st_gid<<endl;
    cout<<"特殊设备ID号: "<<buf.st_rdev<<endl;
    cout<<"总字节数: "<<buf.st_size<<endl;
    cout<<"IO块字节数: "<<buf.st_blksize<<endl;
    cout<<"占用512b的block块的数量: "<<buf.st_blocks<<endl;
    cout<<"最后访问时间: "<<buf.st_atim.tv_sec<<endl;
    cout<<"最后修改时间: "<<buf.st_mtim.tv_sec<<endl;
    cout<<"最后属性的修改时间: "<<buf.st_ctim.tv_sec<<endl;

    if(S_ISREG(buf.st_mode)) cout<<"this is a regular file."<<endl;
    if(S_ISDIR(buf.st_mode)) cout<<"this is a directory."<<endl;
    if(S_ISCHR(buf.st_mode)) cout<<"this is a character device."<<endl;
    if(S_ISBLK(buf.st_mode)) cout<<"this is a block device."<<endl;
    if(S_ISFIFO(buf.st_mode)) cout<<"this is a FIFO (named pipe)."<<endl;
    if(S_ISLNK(buf.st_mode)) cout<<"this is a symbolic link."<<endl;
    if(S_ISSOCK(buf.st_mode)) cout<<"this is a socket."<<endl;
}

/* 若本文件不为文件夹，则删除 */
int deleteFile(const char* path,const char* fileName){
    struct stat buf=getStat(path,fileName);
    string name=getSourceFile(path,fileName);
    if(S_ISDIR(buf.st_mode)){
        cout<<"this is a dir!"<<endl;
        return -1;
    }
    if (remove(name.c_str())) {
        cout<<name<<" 删除失败！"<<endl;
        return -1;
    }
    else {
        cout<<name<<" 删除成功！"<<endl;
        return 0;
    }
}

/* 若本文件为文件夹，则递归删除此文件夹, rm -r */
int deleteDir(const char* path,const char* fileName){
    int flag=0;
    struct stat buf=getStat(path,fileName);
    struct dirent *dirData= NULL;
    DIR* dir;
    string sourceFile=getSourceFile(path,fileName);
    
    if(sizeof(buf)<0||!S_ISDIR(buf.st_mode)){
        cout<<sourceFile<<" is not a dir"<<endl;
        return -1;
    }

    if(!(dir=opendir(sourceFile.c_str()))){
        cout<<"fail to open "<<sourceFile<<endl;
        return -1;
    }

    while((dirData=readdir(dir))!=NULL){
        if (!(strncmp(dirData->d_name,".",1) && strncmp(dirData->d_name,".",2))){
		    continue;//跳过父目录和工作路径和.bashrc
	    }

        string newpath=sourceFile;
        string newFileName=dirData->d_name;
        cout<<"now is dealing with: "<<getSourceFile(newpath.c_str(),newFileName.c_str())<<endl;
        struct stat newStat=getStat(newpath.c_str(),newFileName.c_str());

        if(S_ISDIR(newStat.st_mode)){//若该文件是目录
            deleteDir(newpath.c_str(),newFileName.c_str());
        }else{
            deleteFile(newpath.c_str(),newFileName.c_str());
        }
    }
    closedir(dir);
        
    if (rmdir(sourceFile.c_str())) {
        cout<<sourceFile<<"删除失败！"<<endl;
        return -1;
    }
    else {
        cout<<sourceFile<<"删除成功！"<<endl;
        return 0;
    }
}

/* 通用rm -r，自动判断文件类型 */
int rm(const char* path,const char* fileName){
    struct stat buf=getStat(path,fileName);
    string name=getSourceFile(path,fileName);
    if(!S_ISDIR(buf.st_mode)){
        return deleteFile(path,fileName);
    }
    else{
        return deleteDir(path,fileName);
    }
}

/* 根据新的目录和新的文件名，在新目录下创建一个新文件，并把普通文件拷贝过去 */
int copyNormailFile(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    createDirList(targetPath);//在复制文件/文件夹之前先创造路径
    string sourceFile=getSourceFile(sourcePath,sourceFileName);
    string targetFile=getSourceFile(targetPath,targetFileName);

    char workPath[MAX_PATH];
    //getcwd(workPath, MAX_PATH);//获得当前工作路径,用于写日志
    struct stat fileData=getStat(sourcePath,sourceFileName);
    int size = fileData.st_size;
    cout<<"file size: "<<size<<endl;

    if(!S_ISREG(fileData.st_mode)){
        cout<<"this not a regular file!"<<endl;
        return -1;
    }

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
    changeStat(targetFile.c_str(),fileData);
    return 0;
}

/* 根据新的目录和新的目录名，在目标目录下创建一个新目录，并递归把所有文件拷贝过去 */
int copyDir(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    createDirList(targetPath);//在复制文件/文件夹之前先创造路径
    string sourceFile=getSourceFile(sourcePath,sourceFileName);
    string targetFile=getSourceFile(targetPath,targetFileName);
    struct dirent *dirData= NULL;
    struct stat fileData=getStat(sourcePath,sourceFileName);
    DIR* dir;
    
    if(sizeof(fileData)<0||!S_ISDIR(fileData.st_mode)){
        cout<<sourceFile<<" is not a dir"<<endl;
        return -1;
    }

    if(!(dir=opendir(sourceFile.c_str()))){
        cout<<"fail to open "<<sourceFile<<endl;
        return -1;
    }
    mkdir(targetFile.c_str(),fileData.st_mode);//创建文件夹
    while((dirData=readdir(dir))!=NULL){
        if (!(strncmp(dirData->d_name,".",1) && strncmp(dirData->d_name,".",2))){
		    continue;//跳过父目录和工作路径和.bashrc
	    }

        string newSourcePath=sourceFile;
        string newSourceFileName=dirData->d_name;
        string newTargetPath=targetFile;
        string newTargetFileName=dirData->d_name;
        cout<<"now is dealing with: "<<getSourceFile(newSourcePath.c_str(),newSourceFileName.c_str())<<endl;
        struct stat tmpStat=getStat(newSourcePath.c_str(),newSourceFileName.c_str());

        cp(newSourcePath.c_str(),dirData->d_name,newTargetPath.c_str(),dirData->d_name);
    }
    closedir(dir);
    changeStat(targetFile.c_str(),fileData);
    return 0;
}

/*
int link(const char *oldpath,const char *newpath);
功能：创建硬链接文件

int symlink(const char *linkpath,const char *targetPath);
功能：创建软链接文件

ssize_t readlink(const char *pathname, char *buf, size_t bufsiz);
功能：读取软链接文件链接路径，返回值为软链接文件大小

*/
/* 输入旧目录，旧文件名，新目录，新文件名，并把软链接文件拷过去 */
int copyLink(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    createDirList(targetPath);//在复制文件/文件夹之前先创造路径
    string sourceFile=getSourceFile(sourcePath,sourceFileName);
    string targetFile=getSourceFile(targetPath,targetFileName);

    char workPath[MAX_PATH];
    //getcwd(workPath, MAX_PATH);//获得当前工作路径,用于写日志
    struct stat fileData=getStat(sourcePath,sourceFileName);
    int size = fileData.st_size;
    cout<<"file size: "<<size<<endl;
    char* linkPath=(char*)malloc(sizeof(char)*size);
    

    if(!S_ISLNK(fileData.st_mode)){
        cout<<"this not a soft link!"<<endl;
        return -1;
    }

    if(getStat(targetPath,targetFileName).st_size){//大小不为0,access判断不了软链接是否存在
        cout<<targetFile<<" exist!"<<endl;
        rm(targetPath,targetFileName);//若存在则先删除，否则无法复制软链接文件
    }

    cout<<"link size: "<<readlink(sourceFile.c_str(),linkPath,size)<<endl;
    if(symlink(linkPath,targetFile.c_str())==0){
        cout<<targetFile<<" copy success!"<<endl;
        changeStat(targetFile.c_str(),fileData);
        return 0;
    }
    else{
        cout<<targetFile<<" copy fail!"<<endl;
        return -1;
    }
}

/*
Create a new FIFO named PATH, with permission bits MODE.
extern int mkfifo (const char *__path, __mode_t __mode)
输入旧目录，旧文件名，新目录，新文件名，并把管道文件拷过去
*/
int copyPipe(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    createDirList(targetPath);//在复制文件/文件夹之前先创造路径
    string sourceFile=getSourceFile(sourcePath,sourceFileName);
    string targetFile=getSourceFile(targetPath,targetFileName);

    char workPath[MAX_PATH];
    //getcwd(workPath, MAX_PATH);//获得当前工作路径,用于写日志
    struct stat fileData=getStat(sourcePath,sourceFileName);
    int size = fileData.st_size;
    cout<<"file size: "<<size<<endl;

    if(!S_ISFIFO(fileData.st_mode)){
        cout<<"this not a FIFO!"<<endl;
        return -1;
    }

    if(access(targetFile.c_str(),F_OK)==0){
        cout<<targetFile<<" exist!"<<endl;
        rm(targetPath,targetFileName);//若存在则先删除，否则无法复制
    }

    if(mkfifo(targetFile.c_str(),fileData.st_mode)==0){
        cout<<targetFile<<" copy success!"<<endl;
        changeStat(targetFile.c_str(),fileData);
        return 0;
    }
    else{
        cout<<targetFile<<" copy fail!"<<endl;
        return -1;
    }
}

/*
创建设备文件
int mknod (const char *__path, __mode_t __mode, __dev_t __dev)
*/
int copyDev(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    createDirList(targetPath);//在复制文件/文件夹之前先创造路径
    string sourceFile=getSourceFile(sourcePath,sourceFileName);
    string targetFile=getSourceFile(targetPath,targetFileName);

    char workPath[MAX_PATH];
    //getcwd(workPath, MAX_PATH);//获得当前工作路径,用于写日志
    struct stat fileData=getStat(sourcePath,sourceFileName);
    int size = fileData.st_size;
    cout<<"file size: "<<size<<endl;

    if(!(S_ISCHR(fileData.st_mode)||S_ISBLK(fileData.st_mode))){
        cout<<"this not a dev file!"<<endl;
        return -1;
    }

    if(access(targetFile.c_str(),F_OK)==0){
        cout<<targetFile<<" exist!"<<endl;
        rm(targetPath,targetFileName);//若存在则先删除，否则无法复制
    }

    if(mknod(targetFile.c_str(),fileData.st_mode,fileData.st_dev)==0){
        cout<<targetFile<<" copy success!"<<endl;
        changeStat(targetFile.c_str(),fileData);
        return 0;
    }
    else{
        cout<<targetFile<<" copy fail!"<<endl;
        return -1;
    }
}

/* cp,自动判断文件类型进行复制 */
int cp(const char* sourcePath,const char* sourceFileName,const char* targetPath,const char* targetFileName){
    struct stat fileData=getStat(sourcePath,sourceFileName);
    mode_t mode=fileData.st_mode;
    string sourceFile=getSourceFile(sourcePath,sourceFileName);
    string targetFile=getSourceFile(targetPath,targetFileName);
    int flag=0;

    if(S_ISREG(mode)){
        flag=copyNormailFile(sourcePath,sourceFileName,targetPath,targetFileName);
        changeStat(targetFile.c_str(),fileData);
    }else if(S_ISDIR(mode)){
        flag=copyDir(sourcePath,sourceFileName,targetPath,targetFileName);
        changeStat(targetFile.c_str(),fileData);
    }else if(S_ISCHR(mode)||S_ISBLK(mode)){
        flag=copyDev(sourcePath,sourceFileName,targetPath,targetFileName);
        changeStat(targetFile.c_str(),fileData);
    }else if(S_ISFIFO(mode)){
        flag=copyPipe(sourcePath,sourceFileName,targetPath,targetFileName);
        changeStat(targetFile.c_str(),fileData);
    }else if(S_ISLNK(mode)){
        flag=copyLink(sourcePath,sourceFileName,targetPath,targetFileName);
        changeStat(targetFile.c_str(),fileData);
    }else{
        cout<<"unkown file type, unable to copy."<<endl;
        flag=-1;
    }
    return flag;
}

int main(){
    char* sourcePath="/home/jgqj/source";
    char* targetPath="/home/jgqj/target";
    char* file1="source";
    char* file2="soft";

    // cp test
    cp(sourcePath,file2,targetPath,file2);

    // // copyLink测试
    // copyLink(sourcePath,file1,targetPath,file1);

    // copypipe测试
    // copyPipe(sourcePath,file1,targetPath,file1);

    // // copyNormailfile测试
    // copyNormailFile(sourcePath,file2,targetPath,file2);

    // // copyDir测试
    // copyDir(sourcePath,file1,targetPath,file1);

    // // coutStat测试
    // coutStat(sourcePath,file2);
    // coutStat(targetPath,file2);

    // // deleteFile测试
    // coutStat(targetPath,file2);
    // deleteFile(targetPath,file2);

    // // deleteDir测试
    // coutStat(targetPath,file1);
    // deleteDir(targetPath,file1);

    // // createDirList测试
    // char* t="/home/jgqj/test1/test2/test3";
    // createDirList(t);
}
