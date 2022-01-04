#ifndef SEARCHPORTS_H
#define SEARCHPORTS_H

#include <QObject>
#include <QThread.h>
#include <QDeadlineTimer>
#include <QSerialPortInfo>
#include <QApplication>
#include <windows.h>
#include <QSharedPointer>
#include <QDebug>
#define BrandHisi    QString("DBAdapter")
#define typePortHisi QString("Interface")
#define BrandQlm     QString("android")
#define typePortQlm  QString("pcui")

class SearchPorts : public QThread
{
    Q_OBJECT
    bool checkOpenPort=false,state=false,isStop=false;
    int timeout=6*10000,CustomVID=0x0,CustomPID=0x0;
    QMap<QString,QString> sports;
public:
    SearchPorts(){}
    SearchPorts(const QString &PortName);
    SearchPorts(const int &vid,const int &pid);
    SearchPorts(const QString &PortBrand,const QString &PortName);
    SearchPorts(const QString &PortBrand,const QString &PortName,const QString &PortBrand2,const QString &PortName2);
    ~SearchPorts();
    typedef enum:int{
        DEFAUALT_CASE,UPGRADE_CASE
    }SearchMode;
    SearchMode mode=DEFAUALT_CASE;
    Q_ENUM(SearchMode)
    void SetMode(SearchMode mode=DEFAUALT_CASE);
    void InsertPortSearch(QString arg1,QString arg2=QString());
    bool StartThread();
    void SetCheckOpenPort(bool state);
    bool TryOpen(const int &port);
    void run();
    int getTimeout() const;
    void setTimeout(int value);
signals:
    void Update(const int &value);
    void UpdateCmb(const QString &fullName,const QVariant &variant);
public slots:
    void SetCancel();
};

#endif // SEARCHPORTS_H
