#include "mw.h"
#include "ui_mw.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MW w;
    w.show();
    return a.exec();
}


MW::MW(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MW)
{
    ui->setupUi(this);
    ui->statusbar->addWidget(&label);
    label.setText(HDR_MSG.arg(windowTitle()));
    label.show();
    label.setOpenExternalLinks(true);
}

MW::~MW()
{
    delete ui;
}

void MW::Wait(int time)
{
    QTime dieTime = QTime::currentTime().addMSecs(time);
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

void MW::SetEnable(bool enable)
{
    if(!enable)
        ui->textBrowser->clear();
    ui->cmbPorts->setEnabled(enable);
    ui->btnSwitch2NORMAL->setEnabled(enable);
    ui->btnSwitch2DL->setEnabled(enable);
    ui->btnReadInfo->setEnabled(enable);
}

void MW::DoJobLGE(LGE_METHODS method)
{
    SetEnable(0);
    if(method==READ_INFO){
        InsertLine(">>>>>","Read Info");
    }else if(method==SWITCH2DL){
        InsertLine(">>>>>","Switch To Download");
    }else if(method==SWITCH2NORMAL){
        InsertLine(">>>>>","Switch To Normal Mode");
    }
    QString sp,AndroidVersion,Model,SPC,SwVersion,IMEI;
    if(SearchLGPort(sp)){
        QSerialPort port(sp);
        Append("Opening Port");
        if(port.open(QIODevice::ReadWrite)){
            Insert("ok");
            port.write(PackStructure(LoadDiagInfo));
            port.waitForReadyRead(100);
            Wait(1000);
            QByteArray data,tmp;
            data=port.readAll().toHex();
            if(data.length()>=174){
                tmp=QByteArray::fromHex(data.mid(160, 10));
                Filter(tmp);
                AndroidVersion=tmp;
                tmp=QByteArray::fromHex(data.mid(14, 18));
                Filter(tmp);
                Model=tmp;
                tmp=QByteArray::fromHex(data.mid(34, 16));
                Filter(tmp);
                SwVersion=tmp;
                tmp=QByteArray::fromHex(data.mid(96, 30));
                Filter(tmp);
                IMEI=tmp;
            }
            port.write(PackStructureQC(GET_LGE_SPC));
            port.waitForReadyRead(100);
            Wait(1000);
            data=port.readAll().toHex();
            if(data.length()>20 and data.startsWith("1117"))
                SPC=QByteArray::fromHex(data.mid(8,12));
            QD(data);
            InsertLine("Model",Model);
            InsertLine("Android Version",AndroidVersion);
            InsertLine("Sw Version",SwVersion);
            InsertLine("IMEI",IMEI);
            InsertLine("SPC",SPC);
            QApplication::processEvents();
            QApplication::processEvents();

            if(method==SWITCH2DL){
                Append("Rebooting to Download Mode");
                port.write(PackStructure(Switch2LAF));
                Wait(1000);
                port.waitForReadyRead(100);
                Insert("ok");
            }else if(method==SWITCH2NORMAL){
                Append("Rebooting to Normal Mode");
                port.write(PackStructureLaf(LAF_CMD_CONTROL,
                                            LAF_CMD_CONTROL_RESET,
                                            LAF_FILL(20), // +4 for include crc space
                                            LAF_MAGIC(LAF_CMD_CONTROL)));
                port.waitForReadyRead(100);
                Wait(1000);
                Insert("ok");
            }
            port.clear();
            port.close();
        }else{
            Insert(port.errorString());
        }
    }
    SetEnable(1);
}

void MW::Append(const QString &text){
    ui->textBrowser->insertHtml(QString("<p>%1 ...</p>").arg(text));
}

void MW::Insert(const QString &text){
    ui->textBrowser->insertHtml(QString("<b>%1</b><br>").arg(text));
}

void MW::InsertLine(const QString &text,const QString &result){
    if(result.isEmpty())
        return;
    Append(text);
    Insert(result);
}

bool MW::SearchLGPort(QString &sp)
{
    ui->cmbPorts->clear();
    Append("Waiting LG Serial Port");
    SearchPorts sports;
    QString portName11;
    //    connect(&sports,SIGNAL(Update(int)),ui->progressBar,SLOT(setValue(int)));
    //    QObject::connect(log,SIGNAL(SignalStop()),&sports,SLOT(SetCancel()));
    connect(&sports,&SearchPorts::UpdateCmb,this,[this](const QString& PortName,QVariant portNum){
        QD("add"<<PortName<<portNum);
        ui->cmbPorts->addItem(PortName,QString("COM%1").arg(portNum.toInt()));
    });
    sports.InsertPortSearch("LG","Port");
    bool state=sports.StartThread();
    state ? Insert(QString("OK[%1]").arg(ui->cmbPorts->currentText())):Insert("timeout");
    if(state)
        sp=ui->cmbPorts->currentData().toString();
    return state;
}

void MW::on_btnReadInfo_clicked()
{
    DoJobLGE(READ_INFO);
}

void MW::on_btnSwitch2DL_clicked()
{
    DoJobLGE(SWITCH2DL);
}

void MW::on_btnSwitch2NORMAL_clicked()
{
    DoJobLGE(SWITCH2NORMAL);
}
