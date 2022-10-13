#include<stdio.h>
#include<string.h>
#include<iostream>
#include<fstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

#define MAX_PATH 100    //工作路径名最大长度
#define LINE_SIZE 128   //每次拷贝最大字节数

/*linux 文件结构*/
//  struct stat 
//         dev_t     st_dev;         /* ID of device containing file */    设备ID
//         ino_t     st_ino;         /* inode number */                    inode节点号
//         mode_t    st_mode;        /* protection */                      文件类型和权限
//         nlink_t   st_nlink;       /* number of hard links */            硬连接数
//         uid_t     st_uid;         /* user ID of owner */                用户ID
//         gid_t     st_gid;         /* group ID of owner */               组ID
//         dev_t     st_rdev;        /* device ID (if special file) */     特殊设备ID号
//         off_t     st_size;        /* total size, in bytes */            总字节数
//         blksize_t st_blksize;     /* blocksize for filesystem I/O */    IO块字节数
//         blkcnt_t  st_blocks;      /* number of 512B blocks allocated */ 占用512b的block块的数量
//         struct timespec st_atim;  /* time of last access */             最后访问时间
//         struct timespec st_mtim;  /* time of last modification */       最后修改时间
//         struct timespec st_ctim;  /* time of last status change */      最后属性的修改时间
/*

    第三行：
    st_mode:
        文件类型：
        S_IFMT     0170000       bit mask for the file type bit field   获取文件类型的掩码
        S_IFSOCK   0140000       socket                                 socket文件
        S_IFLNK    0120000       symbolic link                          软链接文件
        S_IFREG    0100000       regular file                           普通文件
        S_IFBLK    0060000       block device                           块设备文件
        S_IFDIR    0040000       directory                              目录文件
        S_IFCHR    0020000       character device                       字符设备文件
        S_IFIFO    0010000       FIFO                                   管道文件
    上面类型判断在POSIX中定义了以下函数进行类型判断 
        S_ISREG(st_mode)  is it a regular file?     普通文件
        S_ISDIR(m)  directory?                      目录文件
        S_ISCHR(m)  character device?               字符设备文件
        S_ISBLK(m)  block device?                   块设备文件
        S_ISFIFO(m) FIFO (named pipe)?              管道文件
        S_ISLNK(m)  symbolic link?                  软链接文件
        S_ISSOCK(m) socket?                         socket文件
 
    st_mode 包含的权限信息
        S_ISUID     04000   set-user-ID bit             
        S_ISGID     02000   set-group-ID bit (see below)
        S_ISVTX     01000   sticky bit (see below)
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

class FileDealer{
    struct stat metaData;
    fstream file;
    string sourcePath;
    string fileName;
    char* buffer;
    
    public:
    /* 合并文件所在目录和文件名，获得文件路径 */
    string getSourceFile(){
        string tmp=sourcePath;
        tmp+='/';
        tmp+=fileName;
        return tmp;
    }
    /* 构造函数，仅初始化文件所在目录和文件名,并更新元数据 */
    FileDealer(const char* sourcePath,const char*fileName){
        this->sourcePath=sourcePath;
        this->fileName=fileName;
        freshStat();
    }
    /* 复制构造函数 */
    FileDealer(const FileDealer &source){
        this->sourcePath=source.sourcePath;
        this->fileName=source.fileName;
        this->metaData=source.metaData;
        this->buffer=(char*)malloc(sizeof(char)*sizeof(source.buffer));
        if(this->buffer){
            for(int i=0;i<sizeof(source.buffer);i++){
                this->buffer[i]=source.buffer[i];
            }
        }
        
    }
    /* 空构造函数，用于返回错误信息 */
    FileDealer(int a){
        this->sourcePath="Fatal";
        this->fileName="Fatal";
    }
    /* 更新元数据，并返回struct stat */
    struct stat freshStat(){
        stat(getSourceFile().c_str(),&this->metaData);
        return this->metaData;
    }
    /* 析构函数，关闭文件流，释放缓存 */
    ~FileDealer(){
        if(file.is_open()) file.close();
        if(buffer!=NULL) free(buffer);
    }
    /* 更新并输出元数据 */
    void coutStat(){
        struct stat buf=freshStat();
        cout<<getSourceFile()<< "元数据: "<<endl;
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
}
/* 根据新的目录和新的文件名，在新目录下创建一个新文件，并把文件拷贝过去 */
FileDealer copyFile(const char* targetPath,const char* fileName){
    FileDealer outFile(targetPath,fileName);
    string sourceFile=this->getSourceFile();
    string targetFile=outFile.getSourceFile();

    char buffer[LINE_SIZE];
    char workPath[MAX_PATH];  
    //getcwd(workPath, MAX_PATH);//获得当前工作路径,用于写日志
    this->freshStat();
    int size = this->metaData.st_size;
    cout<<"file size: "<<size<<endl;

    outFile.file.open(targetFile,ios::out|ios::binary|ios::trunc);
    this->file.open(sourceFile,ios::in|ios::binary);
    if(!this->file.is_open()){
        cout<<"fail to open file: "<<sourceFile<<endl;
        exit(1);
    }else{
        cout<<"file: "<<sourceFile<<" opened!"<<endl;
    }
    if(!outFile.file.is_open()){
        cout<<"fail to create file: "<<targetFile<<endl;
        exit(1);
    }else{
        cout<<"file: "<<targetFile<<" created!"<<endl;
    }
    while(!this->file.eof()&&(size!=0)){
        int t;
        if(size>=LINE_SIZE) t=LINE_SIZE;
        else t=size;
        this->file.read(buffer,t);
        outFile.file.write(buffer,t);
        size-=t;
    }
    this->file.close();
    outFile.file.close();
    this->freshStat();
    outFile.freshStat();
    return outFile;
}

};



int main(){
    string sourcePath="/home/jgqj/source";
    string targetPath="/home/jgqj/target";
    string fileName="chain.heic";
    FileDealer sourceFile(sourcePath.c_str(),fileName.c_str());
    FileDealer targetFile=sourceFile.copyFile(targetPath.c_str(),fileName.c_str());

//    copyFile(sourcePath.c_str(),targetPath.c_str(),fileName.c_str());
    // coutStat(sourcePath.c_str(),fileName.c_str());
    // coutStat(targetPath.c_str(),fileName.c_str());
}
