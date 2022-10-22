#include "mainwindow.h"
#include "ui_mainwindow.h"

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

void MainWindow::on_backUpFileButton_clicked()
{
    string pathAndName=QFileDialog::getOpenFileName(this,"choose file","/home").toStdString();

    backUpInQt(pathAndName);
}

void MainWindow::on_backUpDirButton_clicked()
{
    string pathAndName=QFileDialog::getExistingDirectory(this,"choose file","/home").toStdString();

    backUpInQt(pathAndName);
}

void MainWindow::on_configButton_clicked()
{
    connect(&newPage,SIGNAL(freshMain()),this,SLOT(getFreshMain()));
    newPage.setFixedSize(600,400);
    newPage.setWindowFlags(Qt::WindowMaximizeButtonHint); //neutralize the maximize botton, however do not work
    newPage.show();
}

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


void MainWindow::slotActionCompare(){
    QTableWidgetItem* t=ui->dirMenu->item(ui->dirMenu->currentIndex().row(),0);
    string targetFileName=t->text().toStdString();

    t=ui->dirMenu->item(ui->dirMenu->currentIndex().row(),4);
    string msg=t->text().toStdString();

    configEditor config;//open config to get target dir
    string targetPath=config.retTargetPath();

    string code=this->getPasswd();

    if(-1!=compareFile(targetPath.c_str(),targetFileName.c_str(),code)){
        msg+=" is same to source File!";
    }else{
        msg+=" is different to source File!";
    }

    QMessageBox::information(NULL,"message",QString::fromStdString(msg),QMessageBox::Yes,QMessageBox::Yes);

}

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

void MainWindow::slotActionRestoreTarget(){
    string targetPath=QFileDialog::getExistingDirectory(this,"choose dir","/home").toStdString();

    if(targetPath=="") return;//cancel restore, return.

    QTableWidgetItem* t=ui->dirMenu->item(ui->dirMenu->currentIndex().row(),0);
    string sourceFileName=t->text().toStdString();

    t=ui->dirMenu->item(ui->dirMenu->currentIndex().row(),4);
    string msg=getSourceFile(targetPath.c_str(),t->text().toStdString().c_str());

    string code=this->getPasswd();

    configEditor config;//open config to get target dir
    string sourcePath=config.retTargetPath();

    if(-1!=putBack(sourcePath.c_str(),sourceFileName.c_str(),targetPath.c_str(),NULL,code)){
        msg+=" restore success!";
    }else{
        msg+=" restore success!";
    }

    QMessageBox::information(NULL,"message",QString::fromStdString(msg),QMessageBox::Yes,QMessageBox::Yes);

}

void MainWindow::slotActionRestoreNull(){
    QTableWidgetItem* t=ui->dirMenu->item(ui->dirMenu->currentIndex().row(),0);
    string sourceFileName=t->text().toStdString();

    t=ui->dirMenu->item(ui->dirMenu->currentIndex().row(),3);
    string targetPath=t->text().toStdString();

    t=ui->dirMenu->item(ui->dirMenu->currentIndex().row(),4);
    string msg=getSourceFile(targetPath.c_str(),t->text().toStdString().c_str());

    string code=this->getPasswd();

    configEditor config;//open config to get target dir
    string sourcePath=config.retTargetPath();

    if(-1!=putBack(sourcePath.c_str(),sourceFileName.c_str(),NULL,NULL,code)){
        msg+=" restore success!";
    }else{
        msg+=" restore success!";
    }

    QMessageBox::information(NULL,"message",QString::fromStdString(msg),QMessageBox::Yes,QMessageBox::Yes);

}

void MainWindow::getFreshMain(){
    freshTalbleWidget();
}

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
}

void MainWindow::backUpInQt(string pathAndName){
    if(pathAndName=="") return;//cancel back-up, return.
    configEditor config;//open config to get target dir
    string targetPath=config.retTargetPath();

    string sourcePath;
    string sourceFileName;
    string msg=sourceFileName;
    tearPathAndName(pathAndName,sourcePath,sourceFileName);

    string code=this->getPasswd();

    int fileNo=backUp(sourcePath.c_str(),sourceFileName.c_str(),targetPath.c_str(),sourceFileName.c_str(),code);
    if(-1!=fileNo){
        msg+=" back up success!";
    }else{
        msg+=" back up fail!";
    }
    
    QMessageBox::information(NULL,"message",QString::fromStdString(msg),QMessageBox::Yes,QMessageBox::Yes);
    freshTalbleWidget();
}

string MainWindow::getPasswd(){
    bool ok=false;
    QString passWord;
    while((!ok) || passWord.isEmpty()){
        passWord = QInputDialog::getText(this, "input code","input your code", QLineEdit::Password,0, &ok);
    }
    string code=passWord.toStdString();
    cout<<code<<endl;
    return code;
}
