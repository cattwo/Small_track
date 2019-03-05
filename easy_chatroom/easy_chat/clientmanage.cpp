#include "clientmanage.h"

ClientManage::ClientManage(QWidget *parent) : QWidget(parent),connect_check(-1)
{

}

ClientManage::~ClientManage()
{
    client.stop();
}

void ClientManage::run()
{
    connect(&client_login, &ClientLogin::login_signal, this, &ClientManage::connect_server);
    client_login.show();
}

void ClientManage::connect_server()
{
    connect_check = client.easy_connect(client_login.get_ip(),client_login.get_port());
    client_login.change_state(connect_check);
    if(connect_check == -1)
    {
        return;
    }
    client_login.close();
    client_user.show();
    pthread_t pthread_new[2];
    pthread_create(&pthread_new[0],nullptr,&client_run_s,this);
    pthread_create(&pthread_new[1],nullptr,&chat_run_s,this);
}

void* ClientManage::client_run_s(void* temp)
{
    ClientManage* manager = (ClientManage*) temp;
    manager->client_run();
}

void* ClientManage::chat_run_s(void* temp)
{
    ClientManage* manager = (ClientManage*) temp;
    manager->chat_run();
}

void ClientManage::client_run()
{
    client.easy_run(client_user.get_send(),client_user.get_recv());
}

void ClientManage::chat_run()
{
    client_user.chat_log_run();
}
