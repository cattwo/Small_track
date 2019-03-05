#ifndef CLIENTMANAGE_H
#define CLIENTMANAGE_H

#include <QWidget>
#include "clientlogin.h"
#include "client.h"
#include "clientuser.h"
#include <pthread.h>
#include <QDebug>

class ClientManage : public QWidget
{
    Q_OBJECT
public:
    explicit ClientManage(QWidget *parent = nullptr);
    void run();
    static void* client_run_s(void*);
    static void* chat_run_s(void*);
    void client_run();
    void chat_run();
    ~ClientManage();
signals:

public slots:
    void connect_server();
private:
    ClientLogin client_login;
    Client client;
    ClientUser client_user;
    int connect_check;
};

#endif // CLIENTMANAGE_H
