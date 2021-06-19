#include "mainwindow.h"
#include "ui_mainwindow.h"

const QString access_token_url = "https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials&client_id=%1&client_secret=%2&";
const char *client_id = "igzwxhIXuPYS1Godqt5IX6Ce";
const char *secret_id = "HgqBCdw022aXec9ua5UZEYmvWsYwafjQ";
const QString traffic_flow_url = "https://aip.baidubce.com/rest/2.0/image-classify/v1/traffic_flow?access_token=%1";
const QString trafficStr = "当前画面 | 车辆数: %1辆 | 车流量: %2辆/min |   ";

static QString videoPath;
static string sourcePath;
static string resultPath;
QString AccessToken;//AccessToken 一个月有效，仅供测试
bool openVideoFlag = false;
bool setPrameterFlag = false;
int DrawFrameNum = 0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    pPlayerSource   = new QMediaPlayer;
    pVideoSource    = new QVideoWidget(ui->labelSourceVideo);
    pPlayerResult   = new QMediaPlayer;
    pVideoResult    = new QVideoWidget(ui->labelResultVideo);

    pVideoSource->resize(ui->labelSourceVideo->size());
    pVideoResult->resize(ui->labelResultVideo->size());
    pPlayerSource->setVideoOutput(pVideoSource);
    pPlayerResult->setVideoOutput(pVideoResult);

    isSucceed = false;
    setVideoPlayer(isSucceed);

    sourcePath = (QDir::currentPath()+"/source/").toStdString();
    resultPath = (QDir::currentPath()+"/result/").toStdString();
    connect(ui->actionOpen,&QAction::triggered,this,&MainWindow::actionOpen_clicked);
    connect(ui->actionClose,&QAction::triggered,this,&MainWindow::close);
    connect(ui->actionClear,&QAction::triggered,this,&MainWindow::mainWindowClear);

    menuBarInit();  //系统菜单栏初始化
    ui->statusbar->setSizeGripEnabled(false);       //去掉状态栏右下角缩放

    trafficInfo = new QLabel(this);
    trafficInfo->setText("");
    ui->statusbar->addPermanentWidget(trafficInfo);
    trafficInfo->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::menuBarInit()
{
    QAction *menuSource = new QAction(QStringLiteral("Source"),this);
    menuBar()->insertAction(ui->menuMenu->menuAction(),menuSource);
    connect(menuSource,&QAction::triggered,this,[=](){
        bool result = QDesktopServices::openUrl("file:///" + QString::fromStdString(sourcePath));
        if(result){
            ui->textEdit->append(">>>Source路径：\n"+ QString::fromStdString(sourcePath));
            ui->textEdit->append("-----------------------------------------------");
        }
        else{
            ui->textEdit->append(">>>Source路径不存在!");
            ui->textEdit->append("-----------------------------------------------");
        }
    });

    QAction *menuResult = new QAction(QStringLiteral("Result"),this);
    menuBar()->insertAction(ui->menuMenu->menuAction(),menuResult);
    connect(menuResult,&QAction::triggered,this,[=](){
        bool result = QDesktopServices::openUrl("file:///" + QString::fromStdString(resultPath));
        if(result){
            ui->textEdit->append(">>>Result路径：\n"+ QString::fromStdString(resultPath));
            ui->textEdit->append("-----------------------------------------------");
        }
        else{
            ui->textEdit->append(">>>Result路径不存在!");
            ui->textEdit->append("-----------------------------------------------");
        }
    });

    QAction *menuHelp = new QAction(QStringLiteral("Help"),this);
    menuBar()->insertAction(ui->menuMenu->menuAction(),menuHelp);
    connect(menuHelp,&QAction::triggered,this,[=]{
//        hp = new HelpPage(this);
//        hp->show();
    });

    QAction *menuAbout = new QAction(QStringLiteral("About"),this);
    menuBar()->insertAction(ui->menuMenu->menuAction(),menuAbout);
    connect(menuAbout,&QAction::triggered,this,[=]{
        QMessageBox::about(this,"关于","毕业设计作品《百度AI · 路况分析系统》\n"
                                     "————————————————\n"
                                     "设计者：程自强\n"
                                     "学号：1904039009\n"
                                     "班级：电子信息1701\n"
                                     "Email：ziqiang.cheng@qq.com\n"
                                     "————————————————\n");
    });
}

void MainWindow::mainWindowClear()
{
    isSucceed = false;
    ui->playPause->setText("播放/暂停");
    setVideoPlayer(isSucceed);
    ui->lineEdit_Path->clear();
    ui->lineEdit_FileName->clear();
    ui->lineEdit_type->clear();
    ui->lineEdit_duration->clear();
    ui->lineEdit_frameSize->clear();
    ui->lineEdit_frameRate->clear();
    ui->lineEdit_AL->clear();
    ui->lineEdit_AR->clear();
    ui->lineEdit_BL->clear();
    ui->lineEdit_BR->clear();
    ui->lineEdit_CL->clear();
    ui->lineEdit_CR->clear();
    ui->lineEdit_DL->clear();
    ui->lineEdit_DR->clear();
    ui->textEdit->clear();
    ui->lineEdit_carIn->clear();
    ui->lineEdit_carOut->clear();
    trafficInfo->setEnabled(false);
    trafficInfo->setText("");
}

void MainWindow::setVideoPlayer(bool enable)
{
//    QString videoPath = "E:/Develop Design/Graduation design/GDproject/build-GDproj-Desktop_Qt_5_12_10_MinGW_64_bit-Release/002_Trim.mp4";
//    string resultPath = "E:/Develop Design/Graduation design/GDproject/build-GDproj-Desktop_Qt_5_12_10_MinGW_64_bit-Release/result/";
    if(enable){
        pPlayerSource->setMedia(QUrl::fromLocalFile(videoPath));
        pPlayerSource->pause();

        pPlayerResult->setMedia(QUrl::fromLocalFile(QString::fromStdString(resultPath)+"result_video.mp4"));
        pPlayerResult->pause();

        ui->labelSourceVideo->setVisible(true);
        ui->labelResultVideo->setVisible(true);
        ui->labelSourceImg->setVisible(false);
        ui->labelResultImg->setVisible(false);
        ui->labelResultImg->clear();
        ui->labelSourceImg->clear();
    }
    else
    {
        ui->labelSourceVideo->setVisible(false);
        ui->labelResultVideo->setVisible(false);
        ui->labelSourceImg->setVisible(true);
        ui->labelResultImg->setVisible(true);
        ui->labelResultImg->clear();
        ui->labelSourceImg->clear();
    }
}

void MainWindow::checkWorkDir()
{
    qDebug() << "applicationDirPath:" << QCoreApplication::applicationDirPath();
    qDebug() << "currentPath" << QDir::currentPath();
    QString path = QDir::currentPath();
    QDir *dir = new QDir();
    bool exist = dir->exists(path+"/source");
    if(exist){
        clearFolder(path+"/source");
    }
    else{
        dir->mkdir(path+"/source");
    }
    sourcePath = (path+"/source/").toStdString();
    ui->textEdit->append(">>>Source Path:"+path+"/source/");

    exist = dir->exists(path+"/result");
    if(exist){
        clearFolder(path+"/result");
    }
    else{
        dir->mkdir(path+"/result");
    }
    resultPath = (path+"/result/").toStdString();
    ui->textEdit->append(">>>>>>Result Path:"+path+"/result/");
    ui->textEdit->append("-----------------------------------------------");
}

void MainWindow::clearFolder(const QString &folderFullPath)
{
    QDir             dir(folderFullPath);
    QFileInfoList    fileList;
    QFileInfo        curFile;
    QFileInfoList    fileListTemp;
    int            infoNum;
    int            i;
    int            j;
    /* 首先获取目标文件夹内所有文件及文件夹信息 */
    fileList=dir.entryInfoList(QDir::Dirs|QDir::Files
                               |QDir::Readable|QDir::Writable
                               |QDir::Hidden|QDir::NoDotAndDotDot
                               ,QDir::Name);

    while(fileList.size() > 0)
    {
        infoNum = fileList.size();

        for(i = infoNum - 1; i >= 0; i--)
        {
            curFile = fileList[i];
            if(curFile.isFile()) /* 如果是文件，删除文件 */
            {
                QFile fileTemp(curFile.filePath());
                fileTemp.remove();
                fileList.removeAt(i);
            }

            if(curFile.isDir()) /* 如果是文件夹 */
            {
                QDir dirTemp(curFile.filePath());
                fileListTemp = dirTemp.entryInfoList(QDir::Dirs | QDir::Files
                                                     | QDir::Readable | QDir::Writable
                                                     | QDir::Hidden | QDir::NoDotAndDotDot
                                                     , QDir::Name);
                if(fileListTemp.size() == 0) /* 下层没有文件或文件夹 则直接删除*/
                {
                    dirTemp.rmdir(".");
                    fileList.removeAt(i);
                }
                else /* 下层有文件夹或文件 则将信息添加到列表*/
                {
                    for(j = 0; j < fileListTemp.size(); j++)
                    {
                        if(!(fileList.contains(fileListTemp[j])))
                        {
                            fileList.append(fileListTemp[j]);
                        }
                    }
                }
            }
        }
    }
//    dir.rmdir(".");/*删除目标文件夹,如果只是清空文件夹folderFullPath的内容而不删除folderFullPath本身,则删掉本行即可*/
}

bool MainWindow::videoCapture()
{
    ui->textEdit->append("------------视频抽帧-------------");
    cv::VideoCapture capture(ui->lineEdit_Path->text().toStdString());
    if(capture.isOpened()){
        ui->textEdit->append(">>>视频文件读取成功：");
        int frameRate = capture.get(cv::CAP_PROP_FPS);
        int captureRate = ui->spinBox->text().toInt();
        int interval = frameRate/captureRate;
        int factor = capture.get(cv::CAP_PROP_FRAME_COUNT)/100;
        QString tempStr;
        tempStr.sprintf("interVal(%d) = frameRate(%d)/captureRate(%d)",interval,frameRate,captureRate);
        ui->textEdit->append(tempStr);
        ui->textEdit->append(">>>抽帧中·····");

        cv::Mat frame;
        int totalFrame = 0, currentFrame = 0;
        while(true){
            if(capture.read(frame)){
                if(++totalFrame % interval == 0){
                    stringstream str;
                    str << "f_" << ++currentFrame << ".jpg";
                    imwrite(sourcePath+str.str(),frame);
                    ui->progressBar->setValue(currentFrame/factor);
                }
            }
            else{
                DrawFrameNum = currentFrame;
                qDebug() << "Video is all read,total frame:" << totalFrame << "draw frame:" << currentFrame;
                tempStr.sprintf(">>>视频抽帧完毕，原视频共%d帧，抽取了%d帧\n保存路径：",totalFrame,currentFrame);
                ui->textEdit->append(tempStr + QString::fromStdString(sourcePath));
                ui->progressBar->setValue(100);
                break;
            }
        }
        ui->textEdit->append("-----------------------------------------------");
        return true;
    }
    else{
        QMessageBox::warning(this, "提示", "视频路径无效，请选择视频文件（*.mp4）");
        ui->textEdit->append(">>>路径无效，请选择视频文件（*.mp4）");
        ui->textEdit->append("-----------------------------------------------");
        return false;
    }
}

void MainWindow::setDefaultArea(int width, int height)
{
    int coordinateAL = 1, coordinateAR = height*2/3;
    int coordinateBL = width-1, coordinateBR = height*2/3;
    int coordinateCL = width-1, coordinateCR = height-1;
    int coordinateDL = 1, coordinateDR = height-1;
    QString tempStr;
    tempStr.sprintf("%d",coordinateAL);
    ui->lineEdit_AL->setText(tempStr);
    tempStr.sprintf("%d",coordinateAR);
    ui->lineEdit_AR->setText(tempStr);
    tempStr.sprintf("%d",coordinateBL);
    ui->lineEdit_BL->setText(tempStr);
    tempStr.sprintf("%d",coordinateBR);
    ui->lineEdit_BR->setText(tempStr);
    tempStr.sprintf("%d",coordinateCL);
    ui->lineEdit_CL->setText(tempStr);
    tempStr.sprintf("%d",coordinateCR);
    ui->lineEdit_CR->setText(tempStr);
    tempStr.sprintf("%d",coordinateDL);
    ui->lineEdit_DL->setText(tempStr);
    tempStr.sprintf("%d",coordinateDR);
    ui->lineEdit_DR->setText(tempStr);

    tempStr.sprintf(">>>默认监测区域：\nA:[%d,%d]  B:[%d,%d]\nC:[%d,%d]  D:[%d,%d]"
                    ,coordinateAL,coordinateAR,coordinateBL,coordinateBR,
                    coordinateCL,coordinateCR,coordinateDL,coordinateDR);
    ui->textEdit->append(tempStr);
    ui->textEdit->append("-----------------------------------------------");
}

void MainWindow::fileInformation(QString filePath)
{
    QFileInfo fileInfo = QFileInfo(filePath);
    QString fileName = fileInfo.fileName();
    ui->lineEdit_FileName->setText(fileInfo.fileName());
    ui->lineEdit_type->setText(fileInfo.suffix());
    ui->lineEdit_Path->setText(fileInfo.absoluteFilePath());

    cv::VideoCapture capture(filePath.toStdString());
    double totalFrame = capture.get(cv::CAP_PROP_FRAME_COUNT);
    double frameRate = capture.get(cv::CAP_PROP_FPS);
    int frameWidth = capture.get(cv::CAP_PROP_FRAME_WIDTH);
    int frameHeight = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
    capture.release();

    QString tempStr;
    tempStr.sprintf("%s %.0f","总帧数：",totalFrame);
    ui->textEdit->append(tempStr);

    tempStr.sprintf("%.0f fps",frameRate);
    ui->lineEdit_frameRate->setText(tempStr);
    ui->textEdit->append("帧率：" + tempStr);

    tempStr.sprintf("%.0f s",totalFrame/frameRate);
    ui->lineEdit_duration->setText(tempStr);
    ui->textEdit->append("时长：" + tempStr);

    tempStr.sprintf("%dx%d",frameWidth,frameHeight);
    ui->lineEdit_frameSize->setText(tempStr);
    ui->textEdit->append("帧尺寸：" + tempStr);

    ui->textEdit->append("-----------------------------------------------");
    setDefaultArea(frameWidth,frameHeight);
}

void MainWindow::actionOpen_clicked()
{
    videoPath = QFileDialog::getOpenFileName(this, "选择视频","","Video(*.mp4)");
    if (videoPath.isEmpty())
    {
        QMessageBox::warning(this, "提示", "请选择视频文件");
        ui->textEdit->append(">>>路径无效，请选择视频文件（*.mp4）");
        ui->textEdit->append("-----------------------------------------------");
    }
    else
    {
        fileInformation(videoPath);
        checkWorkDir(); //检查目标文件夹
        ui->textEdit->append(">>>文件打开成功!\n路径:" + videoPath);
        ui->textEdit->append("-----------------------------------------------");
        qDebug() << "open file:" << videoPath;
    }
}

bool MainWindow::getAccessToken(const QString *url, QString *accessToken)
{
    QNetworkAccessManager manager;  //请求动作
    QNetworkRequest request;
    request.setUrl(QString(*url).arg(client_id).arg(secret_id));
    QNetworkReply *reply = manager.get(request);
    QEventLoop loop;
    connect(reply,&QNetworkReply::finished,&loop,&QEventLoop::quit);
    loop.exec();
    if (reply != nullptr && reply->error() == QNetworkReply::NoError)
    {
        QByteArray replyData = reply->readAll();
        QJsonObject obj = QJsonDocument::fromJson(replyData).object();
        if(obj.contains("error")){
            ui->textEdit->append(">>>返回错误码，获取Access Token失败");
            ui->textEdit->append("-----------------------------------------------");
            return false;
        }
        else{
            *accessToken = obj.value("access_token").toString();
            qDebug() << "Access token obtained successfully:" << AccessToken;
            ui->textEdit->append(">>>获取Access Token成功:["+(*accessToken)+"]");
            ui->textEdit->append("-----------------------------------------------");
            return true;
        }
    }
    ui->textEdit->append(">>>获取Access Token失败");
    ui->textEdit->append("-----------------------------------------------");
    return false;
}

QByteArray MainWindow::imageToBase64(QString imgPath)
{
    QImage img(imgPath);
    QByteArray ba;
    QBuffer buf(&ba);                   //用QByteArray构造QBuffer
    buf.open(QIODevice::WriteOnly);
    img.save(&buf, "JPG");              //把img写入QBuffer
    QByteArray base64 = ba.toBase64();  //对图片做base64编码(不包含编码头)

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    //对图片urlencode
    QByteArray imgData = codec->fromUnicode(base64).toPercentEncoding();

    return imgData;
}

bool MainWindow::postPrameterInit(QNetworkRequest *request,QString *post_paramStr)
{
    bool result = false;
    int coordinateAL = ui->lineEdit_AL->text().toInt(&result);
    int coordinateAR = ui->lineEdit_AR->text().toInt(&result);
    int coordinateBL = ui->lineEdit_BL->text().toInt(&result);
    int coordinateBR = ui->lineEdit_BR->text().toInt(&result);
    int coordinateCL = ui->lineEdit_CL->text().toInt(&result);
    int coordinateCR = ui->lineEdit_CR->text().toInt(&result);
    int coordinateDL = ui->lineEdit_DL->text().toInt(&result);
    int coordinateDR = ui->lineEdit_DR->text().toInt(&result);

    result = getAccessToken(&access_token_url,&AccessToken);
    if(result){
        (*post_paramStr).sprintf("area=%d,%d,%d,%d,%d,%d,%d,%d&case_id=1&case_init=false&show=true&image=",
                              coordinateAL,coordinateAR,coordinateBL,coordinateBR,
                              coordinateCL,coordinateCR,coordinateDL,coordinateDR);
        QString requestUrl = traffic_flow_url.arg(AccessToken);
        qDebug() << "Request Url:" << requestUrl;
        (*request).setUrl(requestUrl);
        (*request).setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
        if(DrawFrameNum!=0)
            return true;
        else{
            ui->textEdit->append(">>>未进行抽帧，请重试！");
            ui->textEdit->append("-----------------------------------------------");
        }
    }
    return false;
}

bool MainWindow::framePostToBaidu()
{
    QString imgPath = QString::fromStdString(sourcePath) + "f_%1.jpg";
    QNetworkRequest request;
    QString post_paramStr;

    bool result = postPrameterInit(&request,&post_paramStr);
    if(result){
        trafficInfo->setEnabled(true);
        ui->labelResultImg->setVisible(true);
        ui->textEdit->append(">>>分析中·····");
        ui->textEdit->append("-----------------------------------------------");
        QByteArray post_param = post_paramStr.toLatin1();
        qDebug() << "post_param:" << post_param;
        double process_factor = (double)100/(double)DrawFrameNum;
        ui->progressBar->setValue(0);

        for(int i=1;i<=DrawFrameNum;i++){
            QString pathStr = imgPath.arg(i);
            qDebug() << "ImgPath:" << pathStr;
            QByteArray imgData = imageToBase64(pathStr);
            QNetworkAccessManager manager;
            QNetworkReply *reply = manager.post(request,post_param+imgData);

            QEventLoop loop;
            connect(reply,&QNetworkReply::finished,&loop,&QEventLoop::quit);
            loop.exec();

            if (reply != nullptr && reply->error() == QNetworkReply::NoError){
                ui->labelSourceImg->setPixmap(QPixmap(pathStr));
                bool result = parsingReplyData(reply->readAll(),i);
                if(result)
                    ui->progressBar->setValue(i*process_factor);
                else{
                    return false;
                }
            }
            else
                return false;
        }
        return true;
    }
    return false;
}

bool MainWindow::parsingReplyData(QByteArray replyData,int serialNum)
{
    static int busIn=0,busOut=0,motorbikeIn=0,motorbikeOut=0,
            carIn=0,carOut=0,truckIn=0,truckOut=0,tricycleIn=0,tricycleOut=0;
    if(serialNum == 1){     //重新统计
        busIn=0,busOut=0,motorbikeIn=0,motorbikeOut=0,carIn=0,
        carOut=0,truckIn=0,truckOut=0,tricycleIn=0,tricycleOut=0;
    }
    QString tempStr;
    qDebug() << "--->>>Serial num:" << serialNum;
    qDebug() << "ReplyData:" << replyData;
    QJsonObject obj = QJsonDocument::fromJson(replyData).object();

    int error_code = obj.value("error_code").toInt();
    if(error_code!=0){
        qDebug() << "Error Code:" << obj.value("error_code").toInt();
        qDebug() << "Error Msg:" << obj.value("error_msg").toString();
        return false;
    }

    QJsonValue image = obj.value("image");
    if(image.isString()){
        QString imgStr = image.toString();
        QByteArray base64 = imgStr.toUtf8();
        string imgDecode = (QByteArray::fromBase64(base64)).toStdString();
        vector<uchar> data(imgDecode.begin(),imgDecode.end());
        cv::Mat imgData = imdecode(data,cv::IMREAD_UNCHANGED);

        stringstream str;    //stringstream字符串流，将long类型的转换成字符型传给对象str
        str << "R_" << serialNum << ".jpg";
        string path = resultPath + str.str();
        imwrite(path, imgData); // 将帧转成图片输出

        QImage *img = new QImage();
        if(img->load(QString::fromStdString(path))){
            ui->labelResultImg->setPixmap(QPixmap::fromImage(*img));
        }
        else{
            qDebug() << "The image from replyData read failed." << QString::fromStdString(str.str());
        }
    }

    QJsonValue vehicle_count = obj.value("vehicle_count");
    if(vehicle_count.isObject())
    {
        QJsonValue bus_count = vehicle_count.toObject().value("bus");
        if(bus_count.isObject())
        {
            busIn += bus_count.toObject().value("in").toInt();
            tempStr.sprintf("%d",busIn);
            ui->lineEdit_busIn->setText(tempStr);

            busOut += bus_count.toObject().value("out").toInt();
            tempStr.sprintf("%d",busOut);
            ui->lineEdit_busOut->setText(tempStr);
        }
        QJsonValue motorbike_count = vehicle_count.toObject().value("motorbike");
        if(motorbike_count.isObject())
        {
            motorbikeIn += motorbike_count.toObject().value("in").toInt();
            tempStr.sprintf("%d",motorbikeIn);
            ui->lineEdit_motorIn->setText(tempStr);

            motorbikeOut += motorbike_count.toObject().value("out").toInt();
            tempStr.sprintf("%d",motorbikeOut);
            ui->lineEdit_motorOut->setText(tempStr);
        }
        QJsonValue car_count = vehicle_count.toObject().value("car");
        if(car_count.isObject())
        {
            carIn += car_count.toObject().value("in").toInt();
            tempStr.sprintf("%d",carIn);
            ui->lineEdit_carIn->setText(tempStr);
            qDebug() << "Car In:" << carIn;

            carOut += car_count.toObject().value("out").toInt();
            tempStr.sprintf("%d",carOut);
            ui->lineEdit_carOut->setText(tempStr);
            qDebug() << "Car Out:" << carOut;
        }
        QJsonValue truck_count = vehicle_count.toObject().value("truck");
        if(truck_count.isObject())
        {
            truckIn += truck_count.toObject().value("in").toInt();
            tempStr.sprintf("%d",truckIn);
            ui->lineEdit_truckIn->setText(tempStr);

            truckOut += truck_count.toObject().value("out").toInt();
            tempStr.sprintf("%d",truckOut);
            ui->lineEdit_truckOut->setText(tempStr);
        }
        QJsonValue tricycle_count = vehicle_count.toObject().value("tricycle");
        if(tricycle_count.isObject())
        {
            tricycleIn += tricycle_count.toObject().value("in").toInt();
            tempStr.sprintf("%d",tricycleIn);
            ui->lineEdit_tricycleIn->setText(tempStr);

            tricycleOut += tricycle_count.toObject().value("out").toInt();
            tempStr.sprintf("%d",tricycleOut);
            ui->lineEdit_tricycleOut->setText(tempStr);
        }

        int inNum = busIn + motorbikeIn + carIn + truckIn + tricycleIn + tricycleIn;
        ui->lcdNumber_In->display(inNum);
        int outNum = busOut + motorbikeOut + carOut + truckOut + tricycleOut + tricycleOut;
        ui->lcdNumber_Out->display(outNum);

        QJsonValue vehicle_num = obj.value("vehicle_num");
        int vehicleNum = 0;
        if(vehicle_num.isObject()){
            int carNum = vehicle_num.toObject().value("car").toInt();
            int truckNum = vehicle_num.toObject().value("truck").toInt();
            int busNum = vehicle_num.toObject().value("bus").toInt();
            int motorbikeNum = vehicle_num.toObject().value("motorbike").toInt();
            int tricycleNum = vehicle_num.toObject().value("tricycle").toInt();
            vehicleNum = carNum + truckNum + busNum + motorbikeNum + tricycleNum;
        }
        double currentSec = (double)serialNum/5.0;
        curTraffic = (inNum + outNum)*60/currentSec;
        trafficInfo->setText(trafficStr.arg(vehicleNum).arg(curTraffic));
    }
    return true;
}

void MainWindow::frameToVideo()
{
    ui->textEdit->append(">>>视频合成中·····");
    ui->textEdit->append("-----------------------------------------------");
    bool result;
    int frameRate = ui->spinBox->text().toInt(&result);
    if(result){
        cv::Mat frame = cv::imread(resultPath+"R_1.jpg");
        cv::VideoWriter writer;
        writer.open(resultPath+"result_video.mp4",
                    cv::VideoWriter::fourcc('m','p','4','v'),
                    frameRate,
                    cv::Size(frame.cols,frame.rows),
                    true);

        for(int i=1;i<=DrawFrameNum;i++){
            stringstream suffix;
            suffix << "R_" << i << ".jpg";
            string imgPath = resultPath + suffix.str();
            frame = cv::imread(imgPath);
            if(frame.empty())
            {
                qDebug() << "Read frame failed!";
                qDebug() << QString::fromStdString(imgPath);
                writer.release();
                ui->textEdit->append(">>>Read frame failed!");
                ui->textEdit->append(">>>分析结果视频合成失败！");
                ui->textEdit->append("-----------------------------------------------");
                return;
            }
            writer << frame;
            ui->progressBar->setValue(i*100/DrawFrameNum);
        }
        writer.release();
        isSucceed = true;
        setVideoPlayer(isSucceed);
        ui->textEdit->append(">>>分析结果视频合成成功！");
        ui->textEdit->append("-----------------------------------------------");
    }
    else{
        ui->textEdit->append(">>>抽帧参数错误！");
        ui->textEdit->append("-----------------------------------------------");
    }
}

void MainWindow::on_startAnalyze_clicked()
{
    bool result;
    result = framePostToBaidu();
    trafficInfo->setText("");
    if(result){
        frameToVideo();
        ui->textEdit->append(">>>分析成功！");
        QString tempStr = "--->当前路段平均车流量：%1辆/分";
        ui->textEdit->append(tempStr.arg(curTraffic));
        ui->textEdit->append("-----------------------------------------------");
    }
    else{
        ui->textEdit->append(">>>分析失败！");
        ui->textEdit->append("-----------------------------------------------");
    }
}

void MainWindow::on_startFrame_clicked()
{
    videoCapture();
}

void MainWindow::on_playPause_clicked()
{
    static bool play_state = true;
    if(isSucceed){
        if(play_state){
            ui->playPause->setText("暂停");
            pPlayerSource->play();
            pPlayerResult->play();
        }
        else{
            ui->playPause->setText("播放");
            pPlayerSource->pause();
            pPlayerResult->pause();
        }
        play_state=!play_state;
    }
    else{
        ui->textEdit->append(">>>没有视频分析结果，请重试！");
        ui->textEdit->append("-----------------------------------------------");
    }
}
