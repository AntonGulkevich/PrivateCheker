#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QString>

class Account
{
private:
    QString login;
    QString password;
    bool valid;


public:
    Account();
    Account(const QString &_login,const QString &_password);
    ~Account();
    bool Validation();
    QString GetLogin();
    QString GetPassword();
    void SetLogin(const QString &_login);
    void SetPass(const QString &_password);
    void Set(const QString &_login, const QString &_password, bool _valid);
    void SetValid(bool status);
    bool GetValid();
};

#endif // ACCOUNT_H
