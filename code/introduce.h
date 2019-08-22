#ifndef GONGCHENGJIESHAO_H
#define GONGCHENGJIESHAO_H
#include <QMainWindow>
#include"interactobj.h"
#include<QWebEngineView>
#include <QWebChannel>
#include<signal.h>
#include "jssqlite.h"
#include<QDateTime>
#include <QRegExp>
#include <QRegExpValidator>
namespace Ui {
class EngineeringIntroductionWindow;
}

class EngineeringIntroductionWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EngineeringIntroductionWindow(QWidget *parent = 0);
    ~EngineeringIntroductionWindow();
    Ui::EngineeringIntroductionWindow * getUI() {

        return ui;
    }
    void messageFromSQLite(QString strdate,QString strtitle,QString strcontent);
    void inputEvents();
signals:
    void SigSendMessageToJS(QString stranswer,QString strid,QString strdate,QString strtitle,QString strcontent);
public slots:
    void OnReceiveMessageFromJS(QString Date,QString Title,QString Content);
    void OnJSAddNewEvent(QString Date,QString Title,QString Content);
    void OnJSDeleteEvent(QString Date,QString Content);
    void OnJSUpdateEventContent(QString id,QString newcontent);
    void OnReceiveMegFromSQL(QString id,QString date,QString title,QString content);
    void OnHTMLLoaded();
private:
    Ui::EngineeringIntroductionWindow *ui;
    InteractObj *pInteractObj;
    JsSQLite *pJsSQLite;
    QWebEngineView *view;
    QRegExp rx();
};

#endif // GONGCHENGJIESHAO_H
