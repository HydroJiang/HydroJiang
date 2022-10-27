#include "mainwindow.h"
#include "ui_mainwindow.h"

/* 初始化主窗口 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->dirMenu->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->dirMenu->setColumnCount(6);//set col count

    //set up horizontal header
    QStringList strList;
    strList <<"No"<<"mode" << "owner:group" << "sourcePath"<<"fileName"<<"back-up time";
    ui->dirMenu->setHorizontalHeaderLabels(strList);
    ui->dirMenu->horizontalHeader()->setDefaultSectionSize(100);//6 per row
    ui->dirMenu->horizontalHeader()->setSectionsClickable(false);//header cannot be clicked
    ui->dirMenu->horizontalHeader()->setStretchLastSection(true); //full length
    // ui->dirMenu->horizontalHeader()->resizeSection(0,150); //设置表头第一列的宽度为150
    ui->dirMenu->horizontalHeader()->setFixedHeight(50);
    ui->dirMenu->setStyleSheet("selection-background-color:#d6536e;"); //设置选中背景色
    ui->dirMenu->horizontalHeader()->setStyleSheet("QHeaderView::section{background:#ffffff;}"); //header white

    //set up vertical header
    ui->dirMenu->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->dirMenu->verticalHeader()->setDefaultSectionSize(50); //each row hight
    ui->dirMenu->verticalHeader()->setVisible(false); // visible verticalHeader

    ui->dirMenu->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //set up content row
    ui->dirMenu->setSelectionMode(QAbstractItemView::SingleSelection);  //can only select one row
    ui->dirMenu->setSelectionBehavior(QAbstractItemView::SelectRows);  //at a time
    ui->dirMenu->setEditTriggers(QAbstractItemView::NoEditTriggers); // cannot edit row

    freshTalbleWidget();

}

MainWindow::~MainWindow()
{
    delete ui;
}

/* 点击备份文件按钮，备份文件到备份目录并将文件信息写入record */
void MainWindow::on_backUpFileButton_clicked()
{
    string pathAndName=QFileDialog::getOpenFileName(this,"choose file","/home").toStdString();

    backUpInQt(pathAndName);
}

/* 点击备份目录按钮，备份目录和下面的文件到备份目录并将所有信息写入record */
void MainWindow::on_backUpDirButton_clicked()
{
    string pathAndName=QFileDialog::getExistingDirectory(this,"choose file","/home").toStdString();

    backUpInQt(pathAndName);
}

/* 点击设置按钮，跳转到设置页面 */
void MainWindow::on_configButton_clicked()
{
    connect(&newPage,SIGNAL(freshMain()),this,SLOT(getFreshMain()));
    newPage.setFixedSize(600,400);
    newPage.setWindowFlags(Qt::WindowMaximizeButtonHint); //neutralize the maximize botton, however do not work
    newPage.show();
}

/* 表单单击右键，弹出菜单 */
void MainWindow::on_dirMenu_customContextMenuRequested(const QPoint &pos)
{
    //右键菜单
    QMenu *table_widget_menu;
    table_widget_menu = new QMenu(ui->dirMenu);
    QAction *action = new QAction("Compare", this);
    connect(action, SIGNAL(triggered()), this, SLOT(slotActionCompare()));
    table_widget_menu->addAction(action);
    action = new QAction("Restore to source path", this);
    connect(action, SIGNAL(triggered()), this, SLOT(slotActionRestoreNull()));
    table_widget_menu->addAction(action);
    action = new QAction("Restore to selected path", this);
    connect(action, SIGNAL(triggered()), this, SLOT(slotActionRestoreTarget()));
    table_widget_menu->addAction(action);
    action = new QAction("Delete", this);
    connect(action, SIGNAL(triggered()), this, SLOT(slotActionRemove()));
    table_widget_menu->addAction(action);
    
    table_widget_menu->exec(QCursor::pos());
}

/* 点击比较，比较文件和源文件 */
void MainWindow::slotActionCompare(){
    vector<string> wrongList;
    QTableWidgetItem* t=ui->dirMenu->item(ui->dirMenu->currentIndex().row(),0);
    string targetFileName=t->text().toStdString();

    t=ui->dirMenu->item(ui->dirMenu->currentIndex().row(),4);
    string msg=t->text().toStdString();

    configEditor config;//open config to get target dir
    string targetPath=config.retTargetPath();

    string code=this->getPasswd();
    if(code=="")return;

    if(-1!=compareFile(targetPath.c_str(),targetFileName.c_str(),code,wrongList)){
        msg+=" is same to source File!";
    }else{
        msg+=" is different to source File!\n  WrongList: \n";
        for(int i=0;i<wrongList.size();i++){
            string temp=to_string(i);
            temp+=" : ";
            temp+=wrongList[i];
            temp+="\n";
            msg+=temp;
        }
    }

    QMessageBox::information(NULL,"message",QString::fromStdString(msg),QMessageBox::Yes,QMessageBox::Yes);

}

/* 点击删除，在record中删除文件记录后，在备份目录中删除文件 */
void MainWindow::slotActionRemove(){
    QTableWidgetItem* t=ui->dirMenu->item(ui->dirMenu->currentIndex().row(),0);
    string targetFileName=t->text().toStdString();

    t=ui->dirMenu->item(ui->dirMenu->currentIndex().row(),4);
    string msg=t->text().toStdString();

    configEditor config;//open config to get target dir
    string targetPath=config.retTargetPath();

    if(-1!=rmBackUp(targetPath.c_str(),targetFileName.c_str())){
        msg+=" delete success!";
    }else{
        msg+=" delete fail!";
    }

    QMessageBox::information(NULL,"message",QString::fromStdString(msg),QMessageBox::Yes,QMessageBox::Yes);

    //fresh the main menu
    freshTalbleWidget();
}

/* 点击恢复到选择路径按钮，将文件恢复到目标目录下 */
void MainWindow::slotActionRestoreTarget(){
    string targetPath=QFileDialog::getExistingDirectory(this,"choose dir","/home").toStdString();

    if(targetPath=="") return;//cancel restore, return.

    QTableWidgetItem* t=ui->dirMenu->item(ui->dirMenu->currentIndex().row(),0);
    string sourceFileName=t->text().toStdString();

    t=ui->dirMenu->item(ui->dirMenu->currentIndex().row(),4);
    string msg=getSourceFile(targetPath.c_str(),t->text().toStdString().c_str());

    string code=this->getPasswd();
    if(code=="")return;

    configEditor config;//open config to get target dir
    string sourcePath=config.retTargetPath();

    if(-1!=putBack(sourcePath.c_str(),sourceFileName.c_str(),targetPath.c_str(),NULL,code)){
        msg+=" restore success!";
    }else{
        msg+=" restore success!";
    }

    QMessageBox::information(NULL,"message",QString::fromStdString(msg),QMessageBox::Yes,QMessageBox::Yes);

}

/* 点击恢复到选择路径按钮，将文件恢复到原目录下 */
void MainWindow::slotActionRestoreNull(){
    QTableWidgetItem* t=ui->dirMenu->item(ui->dirMenu->currentIndex().row(),0);
    string sourceFileName=t->text().toStdString();

    t=ui->dirMenu->item(ui->dirMenu->currentIndex().row(),3);
    string targetPath=t->text().toStdString();

    t=ui->dirMenu->item(ui->dirMenu->currentIndex().row(),4);
    string msg=getSourceFile(targetPath.c_str(),t->text().toStdString().c_str());

    string code=this->getPasswd();
    if(code=="")return;

    configEditor config;//open config to get target dir
    string sourcePath=config.retTargetPath();

    if(-1!=putBack(sourcePath.c_str(),sourceFileName.c_str(),NULL,NULL,code)){
        msg+=" restore success!";
    }else{
        msg+=" restore success!";
    }

    QMessageBox::information(NULL,"message",QString::fromStdString(msg),QMessageBox::Yes,QMessageBox::Yes);

}

/* 槽函数，接受刷新信号，刷新主页面 */
void MainWindow::getFreshMain(){
    freshTalbleWidget();
}

/* 刷新主界面 */
void MainWindow::freshTalbleWidget(){
    //if dirMenu is on, close it
    if(ui->dirMenu->isActiveWindow())ui->dirMenu->close();

    configEditor config;
    Record record(config.retTargetPath());

    string targetPath=config.retTargetPath();
    string path;
    string name;
    tearPathAndName(targetPath,path,name);
    struct stat fileData=getStat(path.c_str(),name.c_str());
    struct dirent *dirData= NULL;
    DIR* dir;

    if(sizeof(fileData)<0||!S_ISDIR(fileData.st_mode)){
        cout<<targetPath<<" is not a dir"<<endl;
        return;
    }

    if(!(dir=opendir(targetPath.c_str()))){
        cout<<"fail to open "<<targetPath<<endl;
        return;
    }

    int row=0;

    //清空列表
    while(ui->dirMenu->rowCount()){
        ui->dirMenu->removeRow(0);
    }

    while((dirData=readdir(dir))!=NULL){
        if (!(strncmp(dirData->d_name,".",1) && strncmp(dirData->d_name,".",2))){
            continue;//跳过父目录和工作路径和.bashrc
        }

        int fileNo = atoi(dirData->d_name);
        QString strFileNo=dirData->d_name;
        cout<<"now is dealing with: "<<fileNo<<endl;
        int index=record.getRecord(fileNo);
        if(index==-1){
            cout<<fileNo<<" not exist!"<<endl;
            continue;
        }
        cout<<fileNo<<" exist"<<endl;
        struct recordLine temp=record.getLine(index);

        ui->dirMenu->insertRow(ui->dirMenu->rowCount());
//      "No"<<"mode" << "owner:group" << "sourcePath"<<"fileName"<<"back-up time"
        ui->dirMenu->setItem(row,0,new QTableWidgetItem(strFileNo));
        ui->dirMenu->setItem(row,1,new QTableWidgetItem(QString::fromStdString(modeToStr(temp.s.st_mode))));
        ui->dirMenu->setItem(row,2,new QTableWidgetItem(QString::fromStdString(ownerGroup(temp.s))));
        ui->dirMenu->setItem(row,3,new QTableWidgetItem(QString::fromStdString(temp.sourcePath)));
        ui->dirMenu->setItem(row,4,new QTableWidgetItem(QString::fromStdString(temp.fileName)));
        ui->dirMenu->setItem(row,5,new QTableWidgetItem(QString::fromStdString(timeSpecToStr(temp.backUpTime))));
        row++;
    }
    closedir(dir);
    ui->dirMenu->sortItems(0,Qt::DescendingOrder);
}

/* 备份文件和备份目录重用代码 */
void MainWindow::backUpInQt(string pathAndName){
    if(pathAndName=="") return;//cancel back-up, return.
    configEditor config;//open config to get target dir
    string targetPath=config.retTargetPath();

    string sourcePath;
    string sourceFileName;
    string msg=sourceFileName;
    tearPathAndName(pathAndName,sourcePath,sourceFileName);

    string code=this->getPasswd();
    if(code=="")return;

    int fileNo=backUp(sourcePath.c_str(),sourceFileName.c_str(),targetPath.c_str(),sourceFileName.c_str(),code);
    if(-1!=fileNo){
        msg+=" back up success!";
    }else{
        msg+=" back up fail!";
    }
    
    QMessageBox::information(NULL,"message",QString::fromStdString(msg),QMessageBox::Yes,QMessageBox::Yes);
    freshTalbleWidget();
}

/* 弹出输入框输入密码 */
string MainWindow::getPasswd(){
    bool ok=false;
    QString passWord;
    passWord = QInputDialog::getText(this, "password","input your password", QLineEdit::Password,0, &ok);
    string code=passWord.toStdString();
    cout<<code<<endl;
    return code;
}
