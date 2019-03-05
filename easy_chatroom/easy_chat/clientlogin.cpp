#include "clientlogin.h"
#include "ui_clientlogin.h"

ClientLogin::ClientLogin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClientLogin),
    ip(""),port("")
{
    ui->setupUi(this);
}

ClientLogin::~ClientLogin()
{
    delete ui;
}

void ClientLogin::on_loginButton_clicked()
{
    ip = ui->ipLine->text().toStdString();
    port = ui->portLine->text().toStdString();
    ui->logText->insertPlainText("正在连接中...\n");
    emit login_signal();
}

std::string ClientLogin::get_ip()
{
    return ip;
}

std::string ClientLogin::get_port()
{
    return port;
}

void ClientLogin::change_state(int state)
{
    if(state == -1)
    {
        ui->logText->insertPlainText("连接错误，请检查IP与端口号是否正确！\n");
    }
    else
    {
        ui->logText->insertPlainText("连接成功！\n");
    }
}

