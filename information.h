#ifndef INFORMATION_H
#define INFORMATION_H

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
