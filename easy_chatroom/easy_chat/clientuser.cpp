#include "clientuser.h"
#include "ui_clientuser.h"

ClientUser::ClientUser(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClientUser)
{
    ui->setupUi(this);
    int ret1 = pipe(pipe_send);
    int ret2 = pipe(pipe_recv);
    if(ret1 == -1 || ret2 == -1)
    {
        throw std::exception();
    }
}

ClientUser::~ClientUser()
{
    delete ui;
}

bool ClientUser::lock()
{
    return client_lock.lock();
}

bool ClientUser::unlock()
{
    return client_lock.unlock();
}

void ClientUser::on_pushButton_clicked()
{
    if(ui->userLine->text() != "")
    {
        QString qtemp = ui->userLine->text();
        QString qtime = QDate::currentDate().toString() + QTime::currentTime().toString();
        qtemp = qtime + "\n" + qtemp + "\n";
        std::string temp = qtemp.toStdString();
        if(client_lock.lock())
        {
            ui->chatLog->insertPlainText(qtemp);
            const char* temp_c = temp.c_str();
            ssize_t ret = write(pipe_send[1],temp_c,strlen(temp_c));
            qDebug()<<"write "<<ret<<" "<<strlen (temp_c)<<"\n";
            if(ret == -1)
            {
                qDebug()<<"ret write wrong\n";
            }
            client_lock.unlock();
        }
    }
}

void ClientUser::chat_log_run()
{
    char tempdata[6096];
    pollfd client_fd[1];
    client_fd[0].fd = pipe_recv[0];
    client_fd[0].events = POLLIN;
    client_fd[0].revents = 0;
    int ret = 0;
    while (1)
    {
        ret = poll(client_fd,1,-1);
        if(ret < 0)
        {
            qDebug()<<"poll wrong\n";
            break;
        }
        if(client_fd[0].revents & POLLIN)
        {
            if(client_lock.lock())
            {
                memset(tempdata,0,sizeof(tempdata));
                int ret = read(client_fd[0].fd,tempdata,sizeof(tempdata));
                qDebug()<<"read "<<ret<<"\n";
                std::string temp = tempdata;
                QString qtemp = QString::fromStdString(temp);
                ui->chatLog->insertPlainText(qtemp);
                client_lock.unlock();
            }
        }
    }
    return;
}

int ClientUser::get_recv()
{
    return pipe_recv[1];
}

int ClientUser::get_send()
{
    return pipe_send[0];
}
