#ifndef CLIENTLOGIN_H
#define CLIENTLOGIN_H

#include <QMainWindow>
#include <string>
#include <unistd.h>

namespace Ui {
class ClientLogin;
}

class ClientLogin : public QMainWindow
{
    Q_OBJECT

public:
    explicit ClientLogin(QWidget *parent = nullptr);
    ~ClientLogin();
    std::string get_ip();
    std::string get_port();
    void change_state(int state);

signals:
    void login_signal();

private slots:
    void on_loginButton_clicked();

private:
    Ui::ClientLogin *ui;
    std::string ip;
    std::string port;
};

#endif // CLIENTLOGIN_H
