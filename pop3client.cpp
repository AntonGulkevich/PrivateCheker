#include "pop3client.h"

//public

POP3Client::POP3Client(const QString &email, const QString &password,
                       const QString &host, quint16 port, int timeout) :
    socket_(new QSslSocket(this)),
    email_(email), password_(password), host_(host), port_(port), timeout_(timeout), state_(NotConnected) {
    connect(socket_, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket_, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(errorReceived(QAbstractSocket::SocketError)));
    connect(socket_, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(stateChanged(QAbstractSocket::SocketState)));
    connect(socket_, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

// public

POP3Client::~POP3Client() {
    qDebug() << "~POP3Client()";
    if (socket_) delete socket_;
}

// public

bool POP3Client::init() {
    if (state_ == Authorization) {
        return true;
    }

    socket_->connectToHostEncrypted(host_, port_);

    if (!socket_->waitForConnected(timeout_)) {
        qDebug() << socket_->errorString();
        return false;
    }

    QString response;
    readResponse(response);

    if (response.startsWith("+OK")) {
        state_ = Authorization;
        return true;
    } else {
        return false;
    }
}
//public

bool POP3Client::login() {
    if (state_ == Transaction)
        return true;
    if (!sendUser(email_))
        return false;
    if (!sendPasswd(password_))
        return false;
    state_ = Transaction;
    return true;
}

//public

bool POP3Client::getMsgList(QList< QPair<QString, quint64> >& list)
{

    QString res = doCommand("LIST\r\n");
    qDebug() << res;
    if (res.startsWith("+OK")) {
        QStringList lines = res.split("\r\n", QString::SkipEmptyParts);
        int i;
        for (i = 1; i < lines.count(); i++) {
            QStringList words = lines[i].split(' ', QString::SkipEmptyParts);
            /*for (auto word : words) {
                qDebug() << word;
            }*/
            bool ok = true;
            words.value(0).toInt(&ok);
            words.value(1).toInt(&ok);

            if (ok) {
                list.append(QPair<QString, quint64>(words[0], words[1].toInt()));
            }
        }
        return true;
    } else {
        return false;
    }

}

//public

bool POP3Client::getMessageTop(QString msgId, int nbLines, QString& msgTop) {
    QString res = doCommand("TOP " + msgId + " " + QString::number(nbLines) + "\r\n");
    if (res.startsWith("+OK")) {
        msgTop = res.section("\r\n", 1);
        return true;
    } else {
        return false;
    }
}

//public

bool POP3Client::getMessage(QString msgId, quint64 msgSize, QString& msg) {
    QString res = doCommand("RETR " + msgId + "\r\n", msgSize);
    if (res.size() == 0)
        return false;
    if (res.startsWith("+OK")) {
        msg = res.section("\r\n", 1);
        return true;
    } else {
        return false;
    }
}

// public

bool POP3Client::removeMessage(QString msgId) {
    qDebug() << "remove";
    QString res = doCommand("DELE " + msgId + "\r\n");
    qDebug() << "remove response: " << res;
    if (res.startsWith("+OK")) {
        return true;
    } else {
        return false;
    }
}

// public

bool POP3Client::quit() {
    QString res = doCommand("QUIT\r\n");
    if (res.startsWith("+OK")) {
        qDebug() << "quit: " << res;
        if (!socket_->waitForDisconnected(timeout_)) {
            return false;
        } else {
            state_ = NotConnected;
            return true;
        }
    } else {
        return false;
    }
}

//private

QString POP3Client::doCommand(QString command, quint64 bytesAvailable) {
    QString response;
    qint64 writeResult = socket_->write(command.toUtf8());
    if (writeResult > 0 && !socket_->waitForBytesWritten(timeout_))
        return "";
    if (!readResponse(response, bytesAvailable))
        return "";
    return response;
}

//private

bool POP3Client::readResponse(QString& response, quint64 bytesAvailable) {

    bool complete = false;
    bool couldRead = socket_->waitForReadyRead(timeout_);
    qDebug() << "available: " << socket_->bytesAvailable();

    quint64 bytesRead = 0;

    do {
        QByteArray all(socket_->readAll());
        bytesRead += all.size();
        response.append(all);
        if (bytesRead < bytesAvailable) socket_->waitForReadyRead(100);
    } while (bytesRead < bytesAvailable);

    if (response.size() > 0) {
        complete = true;
    }

    return couldRead && complete;
}

//private

bool POP3Client::sendUser(QString& user) {
    QString res = doCommand("USER " + user + "\r\n");
    qDebug() << res;
    if (res.startsWith("+OK"))
        return true;
    else
        return false;
}
//private

bool POP3Client::sendPasswd(QString& password) {
    QString res = doCommand("PASS " + password + "\r\n");
    qDebug() << res;
    if (res.startsWith("+OK"))
        return true;
    else
        return false;
}

// private slot

void POP3Client::stateChanged(QAbstractSocket::SocketState socketState) {
    qDebug() << "stateChanged " << socketState;
}

// private slot

void POP3Client::errorReceived(QAbstractSocket::SocketError socketError) {
    qDebug() << "error " << socketError;
}

// private slot

void POP3Client::disconnected() {
    qDebug() << "disconneted";
    qDebug() << "error " << socket_->errorString();
}
// private slot

void POP3Client::connected() {
    qDebug() << "Connected ";
}
