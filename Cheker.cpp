#include "cheker.h"
#include "ui_cheker.h"
#include "pop3client.h"

Cheker::Cheker(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Cheker)
{
    ui->setupUi(this);
    // ui->ListToSearchCB->addItem();
    //connect(ui->ProgressBar, SIGNAL(valueChanged(int)), ui->GoodLCD, SLOT(display(int)));
    connect(ui->actionOpen_Base, SIGNAL(triggered() ), ui->OpenBaseButton, SLOT(click()));
    connect(ui->testButton, SIGNAL(clicked()),SLOT(on_testButton_clicked()));
    connect(ui->AddButton, SIGNAL(clicked()), SLOT(on_add_button_clicked()));
    connect(ui->DelButton, SIGNAL(clicked()), SLOT(on_dell_button_clicked()));
    connect(ui->SerchButton, SIGNAL(clicked()), SLOT(on_search_button_clicked()));
    connect(ui->TabSearchRes, SIGNAL(currentChanged(int)), SLOT(OnTabClicked(int)));
    connect(ui->ListToSearchCB, SIGNAL(currentIndexChanged(int)),ui->TabSearchRes, SLOT(setCurrentIndex(int)));

}

Cheker::~Cheker()
{
    delete ui;
}
//--------------------------------------------------------------------------------------------------------------------
QString Cheker::OpenFile(){
    QString DefaultPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);;
    QString FileName = QFileDialog::getOpenFileName(this, "Open Data Base", DefaultPath+"\\WOT","*.txt" );
    QFile DBFile(FileName);
    if (DBFile.open(QFile::ReadOnly)==(-1)){
        QMessageBox::about(this, "Error", "Unable to open file");
        return "-1";
    }
    if(!DBFile.isOpen())
        return "-1";

    this->setWindowTitle(FileName+" - Cheker");

    ui->statusBar->showMessage("Opening Base:"+FileName);
    QString ResultString;
    ResultString=QString::fromUtf8(DBFile.readAll());
    DBFile.close();
    return ResultString;
}
//--------------------------------------------------------------------------------------------------------------------

void Cheker::on_OpenBaseButton_clicked(){
    ui->ForcedModeCHB->setEnabled(false);
    ui->CloseBaseButton->click();
    bool ForcedMode = ui->ForcedModeCHB->isChecked();
    DataBaseVector.clear();
    QString DataBaseData=OpenFile();
    if (DataBaseData=="-1") return;
    QStringList DataBaseList=DataBaseData.split("\n");
    SetProgressGUI("Validating data base", "Validating...",DataBaseList.count() , 0);
    int Errors=ValidationDataBase(DataBaseList);
    ui->statusBar->showMessage("Validation completed");
    SetProgressGUI("Done", "", 100,0 );

    if (ForcedMode){
        int warnings=ForcedFillVector(DataBaseData);
        ui->statusBar->showMessage("Successfully opened(Forced mode). Ammount: " +QString::number(DataBaseVector.count())+"Warnings:"+QString::number(warnings));
    }
    else{
        if (Errors>0){
            ui->statusBar->showMessage("Errors: " +QString::number(Errors)+". Data Base not valid. Need: LoginDelimiterPassword. Exapmle: Alehka@yandex.ru:superdeer");
            return;
        }
        FillVector(DataBaseData);
        SetProgressGUI("Done", "", 100,0 );
        ui->statusBar->showMessage("Successfully opened. Ammount: " +QString::number(DataBaseVector.count()));
    }
    ui->CheckButton->setEnabled(true);
    SetEnabledDomains();
}
//--------------------------------------------------------------------------------------------------------------------

int Cheker::ValidationDataBase(QStringList DataBaseText){
    QRegExp account ("[^"+ ui->DilimiterSetLineEdit->text() +"]*["+ ui->DilimiterSetLineEdit->text() +"]{1}.*");
    QString TempString;
    int cur=0;
    int Errors=0;
    while (!DataBaseText.isEmpty()){
        ui->ProgressBar->setValue(cur++);
        TempString=DataBaseText.takeFirst();
        TempString=TempString.simplified();
        if ((account.indexIn(TempString,0)==-1)&&(TempString!="")){
            Errors++;
        }
    }
    return Errors;
}
//--------------------------------------------------------------------------------------------------------------------

void Cheker::FillVector(const QString &_name){
    ui->MainProcessLable->setText("Parsing base");
    QStringList DataBaseList=_name.split("\n");
    QString TempStr, dlm;
    dlm=ui->DilimiterSetLineEdit->text();
    ui->ProgressBar->setValue(0);
    ui->ProgressBar->setMaximum(DataBaseList.count());
    int cur=0;
    while(!DataBaseList.isEmpty()){
        TempStr=DataBaseList.takeFirst();
        TempStr=TempStr.simplified();
        ui->CurrentProcess->setText(TempStr);
        ui->ProgressBar->setValue(cur++);
        if (TempStr=="") continue;
        Account TempAcc(TempStr.split(dlm)[0],
                TempStr.split(dlm)[1]);
        DataBaseVector.push_back(TempAcc);
    }
}
//--------------------------------------------------------------------------------------------------------------------
int Cheker::ForcedFillVector(QString _name){
    ui->MainProcessLable->setText("Parsing base in forced mode");

    QStringList DataBaseList=_name.split("\n");
    QString TempStr, dlm;
    dlm=ui->DilimiterSetLineEdit->text();
    QRegExp account ("[^"+ dlm +"]*["+ dlm +"]{1}.*");
    int warnings=0;
    int cur=0;
    ui->ProgressBar->setValue(cur);
    ui->ProgressBar->setMaximum(DataBaseList.count());


    while(!DataBaseList.isEmpty()){
        TempStr=DataBaseList.takeFirst();
        TempStr=TempStr.simplified();
        if (TempStr=="") continue;

        if ((account.indexIn(TempStr,0)==-1)&&(TempStr!="")){
            warnings++;
            continue;
        }
        Account TempAcc(TempStr.split(dlm)[0],
                TempStr.split(dlm)[1]);
        DataBaseVector.push_back(TempAcc);

        ui->CurrentProcess->setText(TempStr);
        ui->ProgressBar->setValue(cur++);
    }
    return warnings;
}

//--------------------------------------------------------------------------------------------------------------------
bool Cheker::CheckEmail(const QString &_login,const QString _password){
    if(true)
        return true;
    else
        return false;
}
//--------------------------------------------------------------------------------------------------------------------

void Cheker::on_CheckButton_clicked()
{
    QStringList domains;
    QVector <SortedBase> AccountsToCheck;
    int AccCount=0, cur=0, good=0, bad=0, total=0;
    int row=0, col=0;

    EnabledDomains(domains);
    AccountsToCheck=CreateSortedBase(DataBaseVector, domains);//create vector of structures with enabled domains

    for (QVector<Cheker::SortedBase>::iterator it=AccountsToCheck.begin(); it!=AccountsToCheck.end(); ++it){
        AccCount+=it->accounts.count();
    }

    SetProgressGUI("Cheking email","Cheking login/password",AccCount,cur);

    for (QVector<Cheker::SortedBase>::iterator it=AccountsToCheck.begin(); it!=AccountsToCheck.end(); ++it){
        for (QVector <Account>::iterator it2=it->accounts.begin(); it2!=it->accounts.end();++it2){
            it2->SetValid(CheckEmail(it2->GetLogin(), it2->GetPassword()));
            ui->ProgressBar->setValue(cur++);
            if (it2->GetValid()){
                good++;
            }
            else{
                bad++;
            }
            total=good+bad;
            ui->GoodLCD->display(QString::number(good));
            ui->BadLCD->display(QString::number(bad));
            ui->TotalLCD->display(QString::number(total));
        }
    }
    ui->ValidEmailsTable->setRowCount(good);

    for (QVector<Cheker::SortedBase>::iterator it=AccountsToCheck.begin(); it!=AccountsToCheck.end(); ++it){
        for (QVector <Account>::iterator it2=it->accounts.begin(); it2!=it->accounts.end();++it2){
            if (it2->GetValid()){
                ui->ValidEmailsTable->setItem(row,col++, new QTableWidgetItem(it2->GetLogin()));
                ui->ValidEmailsTable->setItem(row++,col--, new QTableWidgetItem(it2->GetPassword()));
            }
        }
    }

    SetProgressGUI("Done", "", 100, 0);
}
//--------------------------------------------------------------------------------------------------------------------
void Cheker::EnabledDomains(QStringList &List){
    if (ui->MailRuCHB->isChecked())
        List.append("@mail.ru");
    if (ui->BKCHB->isChecked())
        List.append("@bk.ru");
    if (ui->RamblerCHB->isChecked())
        List.append("@rambler.ru");
    if (ui->YandexCHB->isChecked())
        List.append("@yandex.ru");
    if (ui->GmailCHB->isChecked())
        List.append("@gmail.com");
}
//--------------------------------------------------------------------------------------------------------------------
void Cheker::SetProgressGUI(QString main_process, QString sub_process, int ProgressBarMaximum, int StartValue){
    ui->MainProcessLable->setText(main_process);
    ui->CurrentProcess->setText(sub_process);
    ui->ProgressBar->setMaximum(ProgressBarMaximum);
    ui->ProgressBar->setValue(StartValue);
}
//--------------------------------------------------------------------------------------------------------------------
void Cheker::SetEnabledDomains(){
    QString tmp;
    int pr=0;
    int mail_count=0;
    int yandex_count=0;
    int bk_count=0;
    int rambler_count=0;
    int gmail_count=0;
    int other_count=0;
    SetProgressGUI("Looking for domains","domains...",DataBaseVector.count(), pr  );
    for (QVector <Account>::iterator it = DataBaseVector.begin() ; it != DataBaseVector.end(); ++it){
        ui->ProgressBar->setValue(pr++);
        tmp=(*it).GetLogin();
        if (tmp.contains("@mail.ru")){
            mail_count++;
            if (!ui->MailRuCHB->isEnabled())
                ui->MailRuCHB->setEnabled(true);
        }
        else{
            if (tmp.contains("@rambler.ru")){
                rambler_count++;
                if (!ui->RamblerCHB->isEnabled())
                    ui->RamblerCHB->setEnabled(true);
            }
            else{
                if (tmp.contains("@yandex.ru")){
                    yandex_count++;
                    if (!ui->YandexCHB->isEnabled())
                        ui->YandexCHB->setEnabled(true);
                }
                else{
                    if (tmp.contains("@bk.ru")){
                        bk_count++;
                        if (!ui->BKCHB->isEnabled())
                            ui->BKCHB->setEnabled(true);
                    }
                    else{
                        if (tmp.contains("@gmail.com")){
                            gmail_count++;
                            if (!ui->GmailCHB->isEnabled())
                                ui->GmailCHB->setEnabled(true);
                        }
                        else{
                            other_count++;
                        }
                    }
                }
            }
        }
    }
    ui->mailCountLabel->setText(QString::number(mail_count));
    ui->yandexCountLabel->setText(QString::number(yandex_count));
    ui->ramblerCountLabel->setText(QString::number(rambler_count));
    ui->bkCountLabel->setText(QString::number(bk_count));
    ui->otherCountLabel->setText(QString::number(other_count));
    ui->gmailCountLabel->setText(QString::number(gmail_count));
    SetProgressGUI("Done", "", 100,0);
}
//--------------------------------------------------------------------------------------------------------------------

void Cheker::on_CloseBaseButton_clicked()
{
    ui->YandexCHB->setEnabled(false);
    ui->MailRuCHB->setEnabled(false);
    ui->RamblerCHB->setEnabled(false);
    ui->BKCHB->setEnabled(false);
    ui->GmailCHB->setEnabled(false);
    ui->ForcedModeCHB->setEnabled(true);

    ui->YandexCHB->setChecked(false);
    ui->MailRuCHB->setChecked(false);
    ui->RamblerCHB->setChecked(false);
    ui->BKCHB->setChecked(false);
    ui->GmailCHB->setChecked(false);


    ui->yandexCountLabel->setText("0");
    ui->mailCountLabel->setText("0");
    ui->ramblerCountLabel->setText("0");
    ui->bkCountLabel->setText("0");
    ui->otherCountLabel->setText("0");
    ui->gmailCountLabel->setText("0");

    ui->statusBar->showMessage("Base closed");
    ui->MainProcessLable->setText("Ready");
    ui->CurrentProcess->setText("");

    ui->ValidEmailsTable->setRowCount(0);
    this->setWindowTitle("Cheker");
    ui->CheckButton->setEnabled(false);

    ui->GoodLCD->display("0");
    ui->BadLCD->display("0");
    ui->TotalLCD->display("0");

    DataBaseVector.clear();
}
//--------------------------------------------------------------------------------------------------------------------
QVector <Cheker::SortedBase> Cheker::CreateSortedBase(QVector <Account> vect, const QStringList &domains){
    QVector <Cheker::SortedBase> resultVector;
    CreateDefaultStructure(resultVector, domains);
    QString curDomain;
    for (QVector <Account>::iterator it=vect.begin(); it!=vect.end(); ++it){
        curDomain=it->GetLogin();
        for (QVector <Cheker::SortedBase>::iterator it2=resultVector.begin(); it2!=resultVector.end();++it2){
            if (curDomain.contains(it2->_domain_name)){
                it2->accounts.push_back(*it);
            }
        }
    }
    return resultVector;
}
//--------------------------------------------------------------------------------------------------------------------
void Cheker:: CreateDefaultStructure(QVector <Cheker::SortedBase> & vect,const  QStringList &domains){
    Cheker::SortedBase tmpstr;
    for (int i=0; i<domains.count();i++){
        tmpstr._domain_name=domains.value(i);
        vect.push_back(tmpstr);
    }
}

//--------------------------------------------------------------------------------------------------------------------
void Cheker::on_add_button_clicked(){
    QString name;
    QWidget *newTab;

    name=ui->TextSearchLE->text();
    if (name=="")
        return;

    if(ui->ListToSearchCB->count()>0){
        int index;
        index=ui->ListToSearchCB->findText(name);
        if(index>-1){
            return;
        }
    }
    ui->ListToSearchCB->addItem(name);
    newTab= new QWidget(ui->TabSearchRes);
    ui->TabSearchRes->addTab(newTab,name);
    ui->TabSearchRes->setCurrentIndex(ui->TabSearchRes->count()-1);
}

//--------------------------------------------------------------------------------------------------------------------
void Cheker::on_dell_button_clicked(){
    int index;
    if (ui->ListToSearchCB->count()>0){
        index=ui->ListToSearchCB->findText(ui->TextSearchLE->text());
        if(index>-1){
            ui->ListToSearchCB->removeItem(index);
            ui->TabSearchRes->removeTab(index);
        }
    }
    ui->TabSearchRes->setCurrentIndex(0);
}
//--------------------------------------------------------------------------------------------------------------------
void Cheker::on_search_button_clicked(){

    /* ui->statusBar->showMessage("test");
    ui->ValidEmailsTable->item(0, 0)->setBackground(Qt::red);*/
}
//--------------------------------------------------------------------------------------------------------------------
void  Cheker::on_testButton_clicked(){
    QSslSocket *socket_;
    ui->statusBar->showMessage("Validation test");
    QString login="ycykensmit@gmail.com";                 //example "testvalidation@mail.ru";
    QString pass="222091Anton";                           //example  "09Hj3d5hd1";
    QString host="pop.googlemail.com";
    qint16 port=995;

    if(!QSslSocket::supportsSsl()) {
        QMessageBox::critical(0, QMessageBox::tr("Cheker"), QMessageBox::tr("I couldn't detect any SSL supporton this system."));
        return;
    }

    socket_ = new QSslSocket(this);

    socket_->connectToHostEncrypted(host, port);

    if (!socket_->waitForConnected(100)) {
        qDebug() << socket_->errorString();
        delete socket_;
        return;
    }

    QString response;
    quint64 bytesRead;
    quint64 bytesAvailable;
    socket_->waitForReadyRead(100);

    bytesRead=0;
    bytesAvailable=socket_->bytesAvailable();

    qDebug() << "available: " <<bytesAvailable ;

    do {
        QByteArray all(socket_->readAll());
        bytesRead += all.size();
        response.append(all);
        if (bytesRead < bytesAvailable)
            socket_->waitForReadyRead(100);
    }
    while (bytesRead < bytesAvailable);

    ui->statusBar->showMessage(response);

    delete socket_;
}
//--------------------------------------------------------------------------------------------------------------------
void Cheker::OnTabClicked(int index){
    QString text;
    text=ui->TabSearchRes->tabText(index);
    ui->TextSearchLE->setText(text);
}

//--------------------------------------------------------------------------------------------------------------------
