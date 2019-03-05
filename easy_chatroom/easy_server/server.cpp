#include "server.h"
#include "ui_server.h"

Server::Server(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Server),
    pool(10,10,50)
{
    ui->setupUi(this);
    ui->closeButton->hide();
}

Server::~Server()
{
    delete ui;
}

void Server::epoll_run()
{
    pool.epoll_run();
}

void* Server::run(void* arg)
{
    Server* temp = (Server*)arg;
    temp->epoll_run();
}

void Server::on_startButton_clicked()
{
    std::string ip = ui->ipLine->text().toStdString();
    std::string port = ui->portLine->text().toStdString();
    QString temp("正在创建...\n");
    ui->logText->insertPlainText(temp);
    if(pool.connect_listen(ip.c_str(),port.c_str()))
    {
        temp = "创建成功\n正在尝试监听\n";
        ui->logText->insertPlainText(temp);
        pthread_create(&pthread_server[0],nullptr,run,this);
        ui->startButton->hide();
        ui->closeButton->show();
    }
    else
    {
        qDebug()<<2;
        temp = "创建失败，请检查输入是否正确...\n";
        ui->logText->insertPlainText(temp);
    }
}

void Server::on_closeButton_clicked()
{
    pool.stop();
    ui->startButton->show();
    ui->closeButton->hide();
}
