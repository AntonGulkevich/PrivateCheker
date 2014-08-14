#ifndef CHEKER_H
#define CHEKER_H

#include <QMainWindow>

#include<qfiledialog.h>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QVector>
#include <QStringList>
#include <QRegExp>
#include <qnetwork.h>
#include <QtNetwork/QTcpSocket>
#include <QStandardPaths>
#include <QTime>

#include "account.h"

namespace Ui {
class Cheker;
}

class Cheker : public QMainWindow
{
    Q_OBJECT

public:
    explicit Cheker(QWidget *parent = 0);
    ~Cheker();

private slots:
    void on_OpenBaseButton_clicked();

    void on_CheckButton_clicked();

    void on_CloseBaseButton_clicked();

private:
    Ui::Cheker *ui;
    //structures
    struct SortedBase {
        QString _domain_name;
        QVector <Account> accounts;

    };
    //vars
    QVector <Account> DataBaseVector;    

    //functions
    QString OpenFile();
    void FillVector(QString _name);//fill existing vector of accounts
    int ValidationDataBase(QStringList DataBaseText);// chek data base to be "login:password" like, returns count of errors
    bool MixCheck();//cheking base for different domains existance
    int ForcedFillVector(QString _name);//trying to fill vector if there are errors in database, returns number of errors
    bool CheckEmail(const QString &_login,const QString _password);
    void EnabledDomains(QStringList & List);//creating a list with domains
    void SetEnabledDomains();//make domain's chek boxes enabled if prototypes exist in database
    void SetProgressGUI(QString main_process, QString sub_process, int ProgressBarMaximum,int  StartValue);//setup giu due to current process
    QVector <SortedBase> CreateSortedBase(QVector <Account> vect,const QStringList &domains);
    void CreateDefaultStructure(QVector <Cheker::SortedBase> & vect,const QStringList &domains);

};
#endif // CHEKER_H
