#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QMainWindow::showFullScreen();
    this->setStyleSheet("background-color: #333333;");

    ui->comboQoS->addItem("0");
    ui->comboQoS->addItem("1");
    ui->comboQoS->addItem("2");
    ui->comboQoS->setCurrentIndex(0);
    ui->menuFile->setStyleSheet("QMenu::item{"
                                "color: rgb(255, 255, 255);"
                                "}");

    connect(ui->actionQuit, &QAction::triggered, this, [] {
        QApplication::quit();
    });
    connect(ui->addBroker, &QPushButton::clicked, this, &MainWindow::openConnForm);
    connect(ui->subscribeButton, &QPushButton::clicked, this, &MainWindow::openSubWindow);

    connect(this, &MainWindow::configChanged, this, &MainWindow::saveConfig);
    connect(&configMonitor, &QFileSystemWatcher::fileChanged, this, [this](QString path) {
        loadTree();
    });

    makePlot();
    setConfigPath("../../conf.json");
    loadConfigJson();
    loadTree();

    connectToDB();
    //getValues("mqtt3.thingspeak.com", "channels/1992747/subscribe/fields/field1");
}

MainWindow::~MainWindow()
{
    delete ui;
    qApp->quit();
}

void MainWindow::openConnForm()
{
    auto connForm = new ConnectionForm();
    connForm->setWindowTitle("Add MQTT connection");
    connForm->setWindowState(Qt::WindowActive);
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - connForm->width()) / 2;
    int y = (screenGeometry.height() - connForm->height()) / 2;
    connForm->move(x, y);
    connForm->show();

    connect(connForm, &ConnectionForm::connection, this, &MainWindow::addConnection);
}

void MainWindow::openSubWindow()
{
    int index = getConnIndex();
    if (index == -1) {
        QMessageBox::warning(this, "No selections", "Select a connection from the list before trying to subscribe to a topic!");
        return;
    }

    auto subWindow = new SubscriptionWindow();
    subWindow->setWindowTitle("Subscribe to topic");
    subWindow->setWindowState(Qt::WindowActive);
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - subWindow->width()) / 2;
    int y = (screenGeometry.height() - subWindow->height()) / 2;
    subWindow->move(x, y);
    subWindow->show();

    connect(subWindow, &SubscriptionWindow::subscription, this, &MainWindow::addSubscription);
}

void MainWindow::loadTree()
{
    QJsonTree *model = new QJsonTree();

    model->loadJson(configObj.value("connections").toArray());
    ui->configTree->setModel(model);
    ui->configTree->expandAll();
}

void MainWindow::saveConfig()
{
    QFile outFile(getConfigPath());
    QJsonDocument jsonDoc(configObj);

    outFile.open(QIODevice::WriteOnly|QIODevice::Text);
    outFile.write(jsonDoc.toJson());
}

QString MainWindow::getConfigPath()
{
    try {
        if (configMonitor.files().size() == 1) {
            return configMonitor.files().at(0);
        } else {
            throw configMonitor.files().size();
        }
    }  catch (int size) {
        qDebug() << "Multiple config files loaded!" << size;
        return QString();
    }
}

void MainWindow::setConfigPath(const QString &path)
{
    configMonitor.removePaths(configMonitor.files());
    configMonitor.addPath(path);
}

QJsonParseError MainWindow::loadConfigJson()
{
    QFile inFile(getConfigPath());
    inFile.open(QIODevice::ReadOnly|QIODevice::Text);
    QByteArray data = inFile.readAll();
    QJsonParseError errorPtr;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &errorPtr);

    if (errorPtr.error == QJsonParseError::NoError) {
        configObj = jsonDoc.object();
        emit configChanged();
    }

    return errorPtr;
}

void MainWindow::addConnection(QJsonObject connInfo)
{
    QJsonArray connections;

    if (configObj.contains("connections")) {
        connections = configObj.value("connections").toArray();
    }

    connections.append(connInfo);
    configObj.remove("connections");
    configObj.insert("connections", connections);

    emit configChanged();
}

void MainWindow::addSubscription(QJsonObject subInfo)
{
    QJsonArray connections, subs;
    QJsonObject conn;
    int index = getConnIndex();

    if (configObj.contains("connections")) {
        connections = configObj.value("connections").toArray();
    } else {
        return;
    }

    conn = connections.at(index).toObject();
    if (conn.contains("subscriptions")) {
        subs = conn.value("subscriptions").toArray();
        conn.remove("subscriptions");
    }
    subs.append(subInfo);
    conn.insert("subscriptions", subs);
    connections.replace(index, conn);
    configObj.remove("connections");
    configObj.insert("connections", connections);

    emit configChanged();
}

int MainWindow::getConnIndex()
{
    auto item = ui->configTree->currentIndex();
    int index;

    if (!item.isValid()) {
        return -1;
    }
    if (item.parent().isValid()) {
        index = item.parent().row();
    } else {
        index = item.row();
    }
    return index;
}

void MainWindow::connectToDB()
{
    QString url = "http://localhost:8086?db=rpi_telemetry";

    db = influxdb::InfluxDBFactory::Get(url.toStdString());
    db->createDatabaseIfNotExists();
}

void MainWindow::getValues(QString hostname, QString topic)
{
    QString query = "SELECT * FROM \"" + hostname + "\" WHERE topic='" + topic + "'";
    qDebug() << query;

    for (auto i: db->query(query.toStdString())) {
         qDebug()<<i.getName().c_str()<<":";
         qDebug()<<i.getTags().c_str()<<":";
         qDebug()<<i.getFields().c_str()<<":";
         QString timestamp(std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(i.getTimestamp().time_since_epoch()).count()).c_str());
         qDebug()<< QDateTime::fromMSecsSinceEpoch(timestamp.toLongLong());
        }
}


void MainWindow::makePlot()
{
    // prepare data:
    QVector<double> x1(20), y1(20);
    QVector<double> x2(100), y2(100);
    QVector<double> x3(20), y3(20);
    QVector<double> x4(20), y4(20);
    for (int i=0; i<x1.size(); ++i)
    {
      x1[i] = i/(double)(x1.size()-1)*10;
      y1[i] = qCos(x1[i]*0.8+qSin(x1[i]*0.16+1.0))*qSin(x1[i]*0.54)+1.4;
    }
    for (int i=0; i<x2.size(); ++i)
    {
      x2[i] = i/(double)(x2.size()-1)*10;
      y2[i] = qCos(x2[i]*0.85+qSin(x2[i]*0.165+1.1))*qSin(x2[i]*0.50)+1.7;
    }
    for (int i=0; i<x3.size(); ++i)
    {
      x3[i] = i/(double)(x3.size()-1)*10;
      y3[i] = 0.05+3*(0.5+qCos(x3[i]*x3[i]*0.2+2)*0.5)/(double)(x3[i]+0.7)+qrand()/(double)RAND_MAX*0.01;
    }
    for (int i=0; i<x4.size(); ++i)
    {
      x4[i] = x3[i];
      y4[i] = (0.5-y3[i])+((x4[i]-2)*(x4[i]-2)*0.02);
    }

    // create and configure plottables:
    QCPGraph *graph1 = ui->customPlot->addGraph();
    graph1->setData(x1, y1);
    graph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));
    graph1->setPen(QPen(QColor(120, 120, 120), 2));

    QCPGraph *graph2 = ui->customPlot->addGraph();
    graph2->setData(x2, y2);
    graph2->setPen(Qt::NoPen);
    graph2->setBrush(QColor(200, 200, 200, 20));
    graph2->setChannelFillGraph(graph1);

    QCPBars *bars1 = new QCPBars(ui->customPlot->xAxis, ui->customPlot->yAxis);
    bars1->setWidth(9/(double)x3.size());
    bars1->setData(x3, y3);
    bars1->setPen(Qt::NoPen);
    bars1->setBrush(QColor(10, 140, 70, 160));

    QCPBars *bars2 = new QCPBars(ui->customPlot->xAxis, ui->customPlot->yAxis);
    bars2->setWidth(9/(double)x4.size());
    bars2->setData(x4, y4);
    bars2->setPen(Qt::NoPen);
    bars2->setBrush(QColor(10, 100, 50, 70));
    bars2->moveAbove(bars1);

    // move bars above graphs and grid below bars:
    ui->customPlot->addLayer("abovemain", ui->customPlot->layer("main"), QCustomPlot::limAbove);
    ui->customPlot->addLayer("belowmain", ui->customPlot->layer("main"), QCustomPlot::limBelow);
    graph1->setLayer("abovemain");
    ui->customPlot->xAxis->grid()->setLayer("belowmain");
    ui->customPlot->yAxis->grid()->setLayer("belowmain");

    // set some pens, brushes and backgrounds:
    ui->customPlot->xAxis->setBasePen(QPen(Qt::white, 1));
    ui->customPlot->yAxis->setBasePen(QPen(Qt::white, 1));
    ui->customPlot->xAxis->setTickPen(QPen(Qt::white, 1));
    ui->customPlot->yAxis->setTickPen(QPen(Qt::white, 1));
    ui->customPlot->xAxis->setSubTickPen(QPen(Qt::white, 1));
    ui->customPlot->yAxis->setSubTickPen(QPen(Qt::white, 1));
    ui->customPlot->xAxis->setTickLabelColor(Qt::white);
    ui->customPlot->yAxis->setTickLabelColor(Qt::white);
    ui->customPlot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->customPlot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->customPlot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->customPlot->xAxis->grid()->setSubGridVisible(true);
    ui->customPlot->yAxis->grid()->setSubGridVisible(true);
    ui->customPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    ui->customPlot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
    ui->customPlot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    ui->customPlot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(80, 80, 80));
    plotGradient.setColorAt(1, QColor(50, 50, 50));
    ui->customPlot->setBackground(plotGradient);
    QLinearGradient axisRectGradient;
    axisRectGradient.setStart(0, 0);
    axisRectGradient.setFinalStop(0, 350);
    axisRectGradient.setColorAt(0, QColor(80, 80, 80));
    axisRectGradient.setColorAt(1, QColor(30, 30, 30));
    ui->customPlot->axisRect()->setBackground(axisRectGradient);

    ui->customPlot->rescaleAxes();
    ui->customPlot->yAxis->setRange(0, 2);
}
