#ifndef CLIENTUSER_H
#define CLIENTUSER_H

#include <QMainWindow>
#include "locker.h"
#include <unistd.h>
#include <QTime>
#include <QDate>
#include <sys/socket.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <QDebug>

namespace Ui {
class ClientUser;
}

class ClientUser : public QMainWindow
{
    Q_OBJECT

public:
    explicit ClientUser(QWidget *parent = nullptr);
    ~ClientUser();
    bool lock();
    bool unlock();
    void chat_log_run();
    int get_send();
    int get_recv();

private slots:
    void on_pushButton_clicked();

private:
    Ui::ClientUser *ui;
    locker client_lock;
    int pipe_send[2];
    int pipe_recv[2];
    QString userId;
};

#endif // CLIENTUSER_H
