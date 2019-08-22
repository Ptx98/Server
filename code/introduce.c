#include "EngineeringIntroductionWindow.h"
#include "ui_EngineeringIntroductionWindow.h"
#include "ExternalAppWindowLauncher.h"

#include<QWebEngineView>
#include <QtQuick>
#include <QWebEngineSettings>
#include <QRect>
#include <QTextEdit>
#include <QPushButton>

#pragma comment(lib, "user32.lib")
#pragma execution_character_set("utf-8")//引用目的，可以识别中文字符串，不会出现

EngineeringIntroductionWindow::EngineeringIntroductionWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EngineeringIntroductionWindow)
{
    ui->setupUi(this);
    view = new QWebEngineView(this);
/*    QWebChannel *pWebChannel = new QWebChannel(this);      //为网页视图页面创建通道channel
    pInteractObj = new InteractObj(this);                   //创建通道对象用于与JS交互
    //"interactObj"为注册名，JS调用的对象名必须和它相同
    pWebChannel->registerObject(QStringLiteral("interactObj"), pInteractObj);//注册通道对象供JS调用

    view->page()->setWebChannel(pWebChannel);*/                         //设置通道
    view->load(QUrl("file:///" + ExternalAppWindowLauncher::getAppUpDir() + "/data/webpages/introduce/index.html"));
    //view->load(QUrl("http://192.168.17.8:8080/introduce"));
    view->page()->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);

    ui->verticalLayout->addWidget(view);

//    //Qt::CustomizeWindowHint 标题栏也没有 按钮也没有 在那里出现就站在那里不
//    this->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);


    //创建数据库
    pJsSQLite = new JsSQLite();
    QString str_path = ExternalAppWindowLauncher::getAppUpDir() +"/database/MyDataBase1.db";
    pJsSQLite->createDbConnection(str_path,"qt_sql_default_connection");
    
    
//    //从js接收数据
//    //connect(pInteractObj, SIGNAL(SigReceivedMessFromJS(QString,QString)), this,SLOT(OnReceiveMessageFromJS(QString,QString)));
//    connect(pInteractObj, SIGNAL(SigJSAddNewEvent(QString,QString,QString)), this,SLOT(OnJSAddNewEvent(QString,QString,QString)));
//    connect(pInteractObj, SIGNAL(SigJSDeleteEvent(QString,QString)), this,SLOT(OnJSDeleteEvent(QString,QString)));
//    connect(pInteractObj,SIGNAL(SigJSUpdateEventContent(QString,QString)),this,SLOT(OnJSUpdateEventContent(QString,QString)));
//    //向JS发送数据
//    connect(this,SIGNAL(SigSendMessageToJS(QString,QString,QString,QString,QString)),pInteractObj,SIGNAL(SigSendMessageToJS(QString,QString,QString,QString,QString)));
//    //初始化时发送的信息
//    connect(pJsSQLite,SIGNAL(SigSendToHTML(QString,QString,QString,QString)),this,SLOT(OnReceiveMegFromSQL(QString,QString,QString,QString)));
//    connect(pInteractObj,SIGNAL(SigHTMLLoaded()),this,SLOT(OnHTMLLoaded()));
}


EngineeringIntroductionWindow::~EngineeringIntroductionWindow()
{
    delete ui;
}

void EngineeringIntroductionWindow::OnReceiveMessageFromJS(QString Date,QString Title,QString Content)
{
    qDebug()<<"RECEIVE:"<<Date<<Content<<endl;
    emit SigSendMessageToJS(Date,Title,Content,"null","null");
}

void EngineeringIntroductionWindow::messageFromSQLite(QString strdate,QString strtitle, QString strcontent)
{
    emit SigSendMessageToJS(strdate,strtitle,strcontent,"null","null");
    qDebug()<<"SEND:"<<strdate<<strcontent<<endl;
}

void EngineeringIntroductionWindow::OnJSAddNewEvent(QString Date,QString Title, QString Content)
{
    //传过来的时候传的应该是新闻发生的日期

    if(Content.length()>0)//检查是否合法,content不为空
    {
        QDateTime current_date_time =QDateTime::currentDateTime();
        QString current_date =current_date_time.toString("yyyy-MM-dd hh:mm:ss");

        if(pJsSQLite->insertItem(current_date,Date,Title,Content,0))//在数据库中插入一条记录
        {
            qDebug()<<current_date<<":"<<Date<<":"<<Content<<endl;
            emit SigSendMessageToJS("ADD_OK",current_date,Date,Title,Content);//向js返回信息;
        }
        else
        {
            emit SigSendMessageToJS("ADD_FAIL",current_date,Date,Title,Content);
        }
    }
    else
    {
        emit SigSendMessageToJS("ADD_FAIL","",Date,Title,Content);
    }
}

void EngineeringIntroductionWindow::OnJSDeleteEvent(QString id, QString Content)
{
    //需要的是唯一标识一条记录的主键（创建日期
    //检查是否合法(既然js是通过日历控件输入的是不是就不用判断了？
    //在数据库中更新这条记录
    if(pJsSQLite->updateItem(id,1))
    {
        QString date;
        QString title;
        int flag;
        pJsSQLite->getItem(id,date,title,Content,flag);
        if(flag==1)//更新成功
        {
            emit SigSendMessageToJS("DELETE_OK",id,date,title,Content);//向js返回信息;
        }
        else
        {
            emit SigSendMessageToJS("DELETE_FAIL",id,date,title,Content);//向js返回信息;
        }
    }
    else
    {
        emit SigSendMessageToJS("DELETE_FAIL",id,"","",Content);//向js返回信息;
    }
}

void EngineeringIntroductionWindow::OnJSUpdateEventContent(QString id, QString newcontent)
{
    if(pJsSQLite->updateContent(id,newcontent))
    {
        QString date;
        QString title;
        QString content;
        int flag;
        pJsSQLite->getItem(id,date,title,content,flag);
        if(content==newcontent)//更新成功
        {
            emit SigSendMessageToJS("UPDATE_OK",id,date,title,newcontent);//向js返回信息;
        }
        else
        {
            emit SigSendMessageToJS("UPDATE_FAIL",id,date,title,newcontent);//向js返回信息;
        }
    }
    else
    {
        emit SigSendMessageToJS("UPDATE_FAIL",id,"","",newcontent);//向js返回信息;
    }
}

//初始化HTML用
void EngineeringIntroductionWindow::OnReceiveMegFromSQL(QString id,QString date,QString title,QString content)
{
    emit SigSendMessageToJS("ADD_OK",id,date,title,content);//向js发送信息;
}

void EngineeringIntroductionWindow::OnHTMLLoaded()
{
    pJsSQLite->initHTML();
}
