#ifndef SERVER_H
#define SERVER_H

#include <QMainWindow>
#include "threadpool.h"
#include <QDebug>

namespace Ui {
class Server;
}

class Server : public QMainWindow
{
    Q_OBJECT

public:
    explicit Server(QWidget *parent = nullptr);
    ~Server();
    static void* run(void*);
    void epoll_run();

private slots:
    void on_startButton_clicked();

    void on_closeButton_clicked();

private:
    Ui::Server *ui;
    Threadpool pool;
    pthread_t pthread_server[2];
};

#endif // SERVER_H
