#ifndef INFORMATION_H
#define INFORMATION_H

/*
u can simple change the value Info_text and connect it with object which show this information
for ex:
connect(&user_info, SIGNAL(infoChanged(QString)), ui->statusBar, SLOT(showMessage(QString)));
*/

#include <QObject>
#include <QString>

class Information : public QObject
{
    Q_OBJECT
public:
    explicit Information(QObject *parent = 0);
    ~Information();
    QString getInfo() const;

signals:
    void infoChanged(QString info);

public slots:
    void setInfo(QString info);
private:
    QString Info_text;
};

#endif // INFORMATION_H
