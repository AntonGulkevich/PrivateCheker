#include "cheker.h"
#include "ui_cheker.h"

Cheker::Cheker(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Cheker)
{
    ui->setupUi(this);
    //connect(ui->ProgressBar, SIGNAL(valueChanged(int)), ui->GoodLCD, SLOT(display(int)));
    connect(ui->actionOpen_Base, SIGNAL(triggered() ), ui->OpenBaseButton, SLOT(click()));

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
            ui->statusBar->showMessage("Errors: " +QString::number(Errors)+". Data Base not valid. Need: LoginDelimiterPassword. Exapmle: Ivan@yandex.ru:12345");
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
    QString TempStr;
    ui->ProgressBar->setValue(0);
    ui->ProgressBar->setMaximum(DataBaseList.count());
    int cur=0;
    while(!DataBaseList.isEmpty()){
        TempStr=DataBaseList.takeFirst();
        TempStr=TempStr.simplified();
        ui->CurrentProcess->setText(TempStr);
        ui->ProgressBar->setValue(cur++);
        if (TempStr=="") continue;
        Account TempAcc(TempStr.split(ui->DilimiterSetLineEdit->text())[0],
                TempStr.split(ui->DilimiterSetLineEdit->text())[1]);
        DataBaseVector.push_back(TempAcc);
    }
}
//--------------------------------------------------------------------------------------------------------------------
int Cheker::ForcedFillVector(QString _name){
    ui->MainProcessLable->setText("Parsing base in forced mode");
    QStringList DataBaseList=_name.split("\n");
    QString TempStr;
    QRegExp account ("[^"+ ui->DilimiterSetLineEdit->text() +"]*["+ ui->DilimiterSetLineEdit->text() +"]{1}.*");
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
        Account TempAcc(TempStr.split(ui->DilimiterSetLineEdit->text())[0],
                TempStr.split(ui->DilimiterSetLineEdit->text())[1]);
        DataBaseVector.push_back(TempAcc);

        ui->CurrentProcess->setText(TempStr);
        ui->ProgressBar->setValue(cur++);
    }
    return warnings;
}

//--------------------------------------------------------------------------------------------------------------------
bool Cheker::CheckEmail(const QString &_login,const QString _password){
    if(_login.count()<14)
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
                        other_count++;
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
    SetProgressGUI("Done", "", 100,0);
}
//--------------------------------------------------------------------------------------------------------------------

void Cheker::on_CloseBaseButton_clicked()
{
    ui->YandexCHB->setEnabled(false);
    ui->MailRuCHB->setEnabled(false);
    ui->RamblerCHB->setEnabled(false);
    ui->BKCHB->setEnabled(false);

    ui->YandexCHB->setChecked(false);
    ui->MailRuCHB->setChecked(false);
    ui->RamblerCHB->setChecked(false);
    ui->BKCHB->setChecked(false);

    ui->yandexCountLabel->setText("0");
    ui->mailCountLabel->setText("0");
    ui->ramblerCountLabel->setText("0");
    ui->bkCountLabel->setText("0");
    ui->otherCountLabel->setText("0");

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
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
