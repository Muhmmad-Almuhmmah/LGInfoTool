#ifndef MW_H
#define MW_H

#include <QMainWindow>
#include <PackStructure.h>
#include <SearchPorts.h>
#include <QSerialPort>
#include <QLabel>
QT_BEGIN_NAMESPACE
namespace Ui { class MW; }
QT_END_NAMESPACE

enum LGE_METHODS{
    READ_INFO,
    SWITCH2DL,
    SWITCH2NORMAL
};


#define Switch2LAF 0x3AA16E7E
#define LoadDiagInfo 0xEFA01CC07E
#define GET_LGE_SPC 0x06001711

#define LAF_CMD_CONTROL                         0x4C525443 // CONTROL
#define LAF_CMD_CONTROL_RESET                   0x54455352 // RESET
#define LAF_FILL(len)                           QByteArray().fill('0',len*2)
#define LAF_MAGIC(cmd_laf)                      0xFFFFFFFF^cmd_laf

#define HDR_MSG QString("<a href=\"https://github.com/Muhmmad-Almuhmmah/\" target=\"_blank\">Githup</a> <a href=\"https://www.facebook.com/X.Dev.Ye\" target=\"_blank\">Facebook</a>\t|\t%1")

class MW : public QMainWindow
{
    Q_OBJECT
    QLabel label;
public:
    MW(QWidget *parent = nullptr);
    ~MW();
    void Wait(int time);
    void SetEnable(bool enable);
    void DoJobLGE(LGE_METHODS method);
    bool SearchLGPort(QString &sp);
    void Append(const QString &text);
    void Insert(const QString &text);
    void InsertLine(const QString &text, const QString &result);
private slots:
    void on_btnReadInfo_clicked();

    void on_btnSwitch2DL_clicked();

    void on_btnSwitch2NORMAL_clicked();

private:
    Ui::MW *ui;
};
#endif // MW_H
