#include "SearchPorts.h"

SearchPorts::SearchPorts(const QString &PortName)
{
    sports.insert(PortName,"");
}

SearchPorts::SearchPorts(const QString &PortBrand, const QString &PortName)
{
    sports.insert(PortBrand,PortName);
}

SearchPorts::SearchPorts(const QString &PortBrand, const QString &PortName, const QString &PortBrand2, const QString &PortName2)
{
    sports.insert(PortBrand,PortName);
    sports.insert(PortBrand2,PortName2);
}

SearchPorts::SearchPorts(const int &vid, const int &pid)
{
    CustomVID=vid;
    CustomPID=pid;
}

SearchPorts::~SearchPorts()
{
    qDebug() <<"distructor searchports";
}

void SearchPorts::SetMode(SearchPorts::SearchMode mode)
{
    this->mode=mode;
}

int SearchPorts::getTimeout() const
{
    return timeout;
}

void SearchPorts::setTimeout(int value)
{
    timeout = value;
}

void SearchPorts::SetCancel()
{
    qDebug() <<"Set cancel";
    state=false;
    isStop=true;
}

void SearchPorts::InsertPortSearch(QString arg1, QString arg2)
{
    sports.insert(arg1,arg2);
}

bool SearchPorts::StartThread()
{
    state=false;
    start();
    while(isFinished()==false)
        QApplication::processEvents();
    return state;
}

void SearchPorts::SetCheckOpenPort(bool state)
{
    checkOpenPort=state;
}

bool SearchPorts::TryOpen(const int &port)
{
    if(isStop) return false;
    qDebug() <<"try port"<<port;
    TCHAR tPath[32];
    swprintf_s(tPath,32,L"\\\\.\\COM%i",port);
    HANDLE hPort=CreateFile(tPath, (GENERIC_READ | GENERIC_WRITE),
                            FILE_SHARE_READ,
                            NULL,
                            OPEN_EXISTING,
                            0,
                            NULL);
    qDebug() <<"hPort.isNull()"<<hPort<<port;
    if(hPort!=INVALID_HANDLE_VALUE)
        return false;
    CloseHandle(hPort);
    return true;
}

void SearchPorts::run()
{
    qDebug() <<"run ...";
    QDeadlineTimer deadline(timeout);
    const QString blankString = "N/A";
    QString desc,manufacturer,serialNumber,str,portNum;

    while(!deadline.hasExpired()){
        if(isStop) return;
        if(state) break;
        emit Update((float(timeout-deadline.remainingTime())/timeout)*100);
        //        qDebug() <<"1";
        QThread::msleep(1500);
        auto serialPortInfos = QSerialPortInfo::availablePorts();
        if(mode==DEFAUALT_CASE){
            for(const QSerialPortInfo &serialPortInfo : serialPortInfos){
                //                if(state) break;
                if(isStop) return;
                desc = serialPortInfo.description().trimmed();
                manufacturer = serialPortInfo.manufacturer().trimmed();
                serialNumber = serialPortInfo.serialNumber().trimmed();
                str=QString(!desc.isEmpty() ? desc : blankString).append(" ("+serialPortInfo.portName()+")");
                portNum=serialPortInfo.portName().toLower().remove("com");
                desc=desc.toLower();
                //            qDebug() <<"str"<<str;
                qDebug() <<"desc"<<desc;
                QThread::msleep(30);
                if(CustomVID!=0x0 and CustomPID !=0x0){
                    qDebug() <<"by VID,PID";
                    if(serialPortInfo.hasProductIdentifier() and serialPortInfo.hasVendorIdentifier()){
                        if(serialPortInfo.productIdentifier()==CustomVID and serialPortInfo.productIdentifier()==CustomPID){
                            if(checkOpenPort){
                                if(!TryOpen(portNum.toInt()))
                                    continue;
                            }
                            if(isStop) return;
                            emit UpdateCmb(str,portNum);
                            state=true;
                        }
                    }
                }

                QMap<QString, QString>::const_iterator i = sports.constBegin();
                while (i != sports.constEnd()) {
                    if(isStop) return;
                    //if(state) break;
                    qDebug()<< i.key() << ": " << i.value() << Qt::endl;
                    if(desc.toLower().contains(i.key().toLower()) and desc.toLower().contains(i.value().toLower())){
                        if(checkOpenPort){
                            if(!TryOpen(portNum.toInt()))
                                continue;
                        }
                        if(isStop) return;
                        emit UpdateCmb(str,portNum);
                        state=true;
                    }
                    ++i;
                }
            }
        }else if(mode==UPGRADE_CASE){
            //qDebug()<<"Method2"<< BrandHisi << ": " << typePortHisi << Qt::endl;
            for(const QSerialPortInfo &serialPortInfo : serialPortInfos){
                //if(state) break;
                if(isStop) return;
                desc = serialPortInfo.description().trimmed();
                manufacturer = serialPortInfo.manufacturer().trimmed();
                serialNumber = serialPortInfo.serialNumber().trimmed();
                str=QString(!desc.isEmpty() ? desc : blankString).append(" ("+serialPortInfo.portName()+")");
                portNum=serialPortInfo.portName().toLower().remove("com");
                desc=desc.toLower();
                if(desc.toLower().contains(BrandHisi.toLower()) and desc.toLower().contains(typePortHisi.toLower())){
                    if(checkOpenPort){
                        if(!TryOpen(portNum.toInt()))
                            continue;
                    }
                    if(isStop) return;
                    emit UpdateCmb(str,portNum);
                    state=true;
                }
            }
            //qDebug()<<"Method2"<< BrandQlm << ": " << typePortQlm << Qt::endl;
            for(const QSerialPortInfo &serialPortInfo : serialPortInfos){
                //if(state) break;
                if(isStop) return;
                desc = serialPortInfo.description().trimmed();
                manufacturer = serialPortInfo.manufacturer().trimmed();
                serialNumber = serialPortInfo.serialNumber().trimmed();
                str=QString(!desc.isEmpty() ? desc : blankString).append(" ("+serialPortInfo.portName()+")");
                portNum=serialPortInfo.portName().toLower().remove("com");
                desc=desc.toLower();
                if(desc.toLower().contains(BrandQlm.toLower()) and desc.toLower().contains(typePortQlm.toLower())){
                    if(checkOpenPort){
                        if(!TryOpen(portNum.toInt()))
                            continue;
                    }
                    if(isStop) return;
                    emit UpdateCmb(str,portNum);
                    state=true;
                }
            }
        }
    }
    if(state)
        emit Update(100);
    else
        emit Update(0);
}
