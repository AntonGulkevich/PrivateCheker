#include "account.h"

Account::Account():
    login(""),
    password(""),
    valid(false)
{}
Account::Account(const QString &_login, const QString &_password)
    :login(_login),
     password(_password),
     valid(false)
{}
Account::~Account(){}

void Account::SetValid(bool status){
    valid=status;
}
QString Account::GetLogin(){
    return login;
}
QString Account::GetPassword(){
    return password;
}
bool Account::GetValid(){
    return valid;
}

void Account::SetLogin(const QString &_login){
    login=_login;
}

void Account::SetPass(const QString &_password){
    password=_password;
}

void Account::Set(const QString &_login, const QString &_password, bool _valid){
    Account::SetLogin (_login);
    Account::SetPass  (_password);
    Account::SetValid (_valid);
}




