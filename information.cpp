#include "information.h"

Information::Information(QObject *parent) :
    QObject(parent)
{
    Info_text="";
}
QString Information::getInfo()const{
    return Info_text;
}
void Information::setInfo(QString info){
    if (Info_text!=info){
        Info_text=info;
        emit infoChanged(info);
    }
}
Information::~Information(){

}
