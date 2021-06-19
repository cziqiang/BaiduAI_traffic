#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QBuffer>
#include <QTextCodec>
#include <QDesktopServices>

#include <string>
#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonObject>
#include <QJsonDocument>

#include <QMediaPlayer>
#include <QVideoWidget>

#include "helppage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
using namespace std;
//using namespace cv;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void fileInformation(QString filePath);
    void actionOpen_clicked();
    void setDefaultArea(int width,int height);
    bool videoCapture();
    bool getAccessToken(const QString *url,QString *accessToken);
    QByteArray imageToBase64(QString imgPath);
    bool postPrameterInit(QNetworkRequest *request,QString *post_paramStr);
    bool parsingReplyData(QByteArray replyData,int serialNum);
    bool framePostToBaidu();
    void frameToVideo();
    void checkWorkDir();
    void clearFolder(const QString &folderFullPath);
    void setVideoPlayer(bool enable);
    void mainWindowClear();
    void menuBarInit();

private slots:
    void on_startAnalyze_clicked();

    void on_startFrame_clicked();

    void on_playPause_clicked();

protected:
//    void paintEvent(QPaintEvent *e);

private:
    Ui::MainWindow *ui;
    HelpPage *hp;

    QMediaPlayer    *pPlayerSource,*pPlayerResult;
    QVideoWidget    *pVideoSource,*pVideoResult;

    bool isSucceed = false;   //分析结果标志位
    QLabel *trafficInfo;    //状态栏——路况信息

    int curTraffic; //平均车流量
};
#endif // MAINWINDOW_H
