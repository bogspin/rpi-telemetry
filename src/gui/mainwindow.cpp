#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QMainWindow::showFullScreen();
    this->setStyleSheet("background-color: #333333;");

    ui->menuFile->setStyleSheet("QMenu::item{"
                                "color: rgb(255, 255, 255);"
                                "}");

    connect(ui->actionQuit, &QAction::triggered, this, [] {
        QApplication::quit();
    });
    connect(ui->actionStart, &QAction::triggered, this, &MainWindow::startService);
    connect(ui->actionStop, &QAction::triggered, this, &MainWindow::stopService);
    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::openConnForm);
    connect(ui->subscribeButton, &QPushButton::clicked, this, &MainWindow::openSubWindow);
    connect(ui->removeButton, &QToolButton::clicked, this, &MainWindow::removeButton);
    connect(ui->plotButton, &QPushButton::clicked, this, &MainWindow::plotMeasurement);
    connect(ui->configTree, &QTreeView::doubleClicked, this, &MainWindow::openConfigWindow);

    connect(this, &MainWindow::configChanged, this, &MainWindow::saveConfig);
    connect(&configMonitor, &QFileSystemWatcher::fileChanged, this, [this]() {
        populateTree();
    });

    setConfigPath("../../conf.json");
    loadConfigJson();
    populateTree();

    connectToDB();
    startService();
}

MainWindow::~MainWindow()
{
    delete ui;
    qApp->quit();
}

void MainWindow::startService()
{
    system("sudo systemctl start mqtt-service.service");
}

void MainWindow::stopService()
{
    system("sudo systemctl stop mqtt-service.service");
}

ConnectionWindow* MainWindow::openConnForm()
{
    auto connForm = new ConnectionWindow();

    connForm->setWindowTitle("Add MQTT client");
    connForm->setWindowState(Qt::WindowActive);
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - connForm->width()) / 2;
    int y = (screenGeometry.height() - connForm->height()) / 2;
    connForm->move(x, y);
    connForm->show();

    connect(connForm, &ConnectionWindow::connection, this, &MainWindow::addConnection);

    return connForm;
}

SubscriptionWindow* MainWindow::openSubWindow()
{
    int index = getConnIndex();
    if (index == -1) {
        QMessageBox::warning(this, "No selections", "Select a connection from the list before trying to subscribe to a topic!");
        return nullptr;
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

    return subWindow;
}

void MainWindow::openConfigWindow(const QModelIndex &index)
{
    if (isSubscription(index)) {
        QModelIndex parent = index.parent();
        QJsonObject conn = configObj.value("connections").toArray().at(parent.row()).toObject();
        QJsonObject sub;

        if (conn.contains("subscriptions")) {
            QJsonArray subs = conn.value("subscriptions").toArray();
            sub = subs.at(index.row()).toObject();
        }

        SubscriptionWindow *subWindow = openSubWindow();

        subWindow->setSubscription(sub);
        subWindow->setWindowTitle("Configure subscription");
    }
    else if (isConnection(index)) {
        QJsonObject conn = configObj.value("connections").toArray().at(index.row()).toObject();
        ConnectionWindow *connWindow = openConnForm();

        connWindow->setConnection(conn);
        connWindow->setWindowTitle("Configure MQTT client");
    }
}

void MainWindow::removeButton()
{
    QModelIndex index = ui->configTree->currentIndex();

    if (isSubscription(index)) {
        removeSubscription(index.parent().row(), index.row());
    }
    else if (isConnection(index)) {
        removeConnection(index.row());
    }
}

void MainWindow::populateTree()
{
    configModel.loadJson(configObj.value("connections").toArray());
    ui->configTree->setModel(&configModel);
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

void MainWindow::removeConnection(int connNumber)
{
    QJsonArray connections;

    if (configObj.contains("connections")) {
        connections = configObj.value("connections").toArray();
    } else {
        return;
    }

    if (connNumber < connections.size()) {
        connections.removeAt(connNumber);
        configObj.remove("connections");
        configObj.insert("connections", connections);

        emit configChanged();
    }
}

void MainWindow::removeSubscription(int connNumber, int subNumber)
{
    QJsonArray connections, subs;
    QJsonObject conn;

    if (configObj.contains("connections")) {
        connections = configObj.value("connections").toArray();
    } else {
        return;
    }

    conn = connections.at(connNumber).toObject();
    if (conn.contains("subscriptions")) {
        subs = conn.value("subscriptions").toArray();
        conn.remove("subscriptions");
    }
    if (subNumber < subs.size()) {
        subs.removeAt(subNumber);
        conn.insert("subscriptions", subs);
        connections.replace(connNumber, conn);
        configObj.remove("connections");
        configObj.insert("connections", connections);

        emit configChanged();
    }
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

bool MainWindow::isConnection(const QModelIndex &index)
{
    return !index.parent().isValid();
}

bool MainWindow::isSubscription(const QModelIndex &index)
{
    return index.parent().isValid();
}

void MainWindow::connectToDB()
{
    QString url = "http://localhost:8086?db=rpi_telemetry";

    db = influxdb::InfluxDBFactory::Get(url.toStdString());
    db->createDatabaseIfNotExists();
}

void MainWindow::plotMeasurement()
{
    QString hostname, topic;
    auto item = ui->configTree->currentIndex();

    if (!item.isValid()) {
        QMessageBox::warning(this, "No selections", "First click on a topic to plot its logged data!");
        return;
    }

    if (item.parent().isValid()) {
        hostname = configModel.data(item.parent(), Qt::DisplayRole).toString();
        topic = configModel.data(item, Qt::DisplayRole).toString();
        addPlot();
        QCPGraph *graph = plots.last()->addGraph();
        setGraphData(graph, hostname, topic);
        updatePlot(plots.last());
    }
    else {
        hostname = configModel.data(item, Qt::DisplayRole).toString();
        addPlot();

        QModelIndex child;
        int i = 0;

        while ((child = item.child(i, 0)).isValid()) {
            topic = configModel.data(child, Qt::DisplayRole).toString();
            QCPGraph *graph = plots.last()->addGraph();
            setGraphData(graph, hostname, topic);
            i++;
        }
        updatePlot(plots.last());
    }
}

void MainWindow::addPlot()
{
    QCustomPlot *plot = new QCustomPlot();
    QPoint gridPos = plotPosition(plots.size());

    plots.append(plot);
    QVBoxLayout *verticalLayout = new QVBoxLayout();
    QToolBar *bar = new QToolBar();
    QAction *closePlot = new QAction("Close");
    connect(closePlot, &QAction::triggered, this, &MainWindow::removePlot);
    bar->setStyleSheet("background-color: #505050;"
                       "color: #FFFFFF;");
    bar->addAction(closePlot);

    // verticalLayout->addWidget(bar);
    verticalLayout->addWidget(plot);
    ui->gridLayout->addLayout(verticalLayout, gridPos.x(), gridPos.y());
    setPlotStyle(plot);
}

void MainWindow::updatePlot(QCustomPlot *plot)
{
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("d MMMM\nyyyy");
    plot->xAxis->setTicker(dateTicker);
    plot->rescaleAxes();
    plot->replot();
}

void MainWindow::removePlot() {
    qDebug() << "SADSAFSAFS";
}

void MainWindow::setPlotStyle(QCustomPlot *plot)
{
    plot->xAxis->setBasePen(QPen(Qt::white, 1));
    plot->yAxis->setBasePen(QPen(Qt::white, 1));
    plot->xAxis->setTickPen(QPen(Qt::white, 1));
    plot->yAxis->setTickPen(QPen(Qt::white, 1));
    plot->xAxis->setSubTickPen(QPen(Qt::white, 1));
    plot->yAxis->setSubTickPen(QPen(Qt::white, 1));
    plot->xAxis->setTickLabelColor(Qt::white);
    plot->yAxis->setTickLabelColor(Qt::white);
    plot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    plot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    plot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    plot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    plot->xAxis->grid()->setSubGridVisible(true);
    plot->yAxis->grid()->setSubGridVisible(true);
    plot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    plot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
    plot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    plot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(80, 80, 80));
    plotGradient.setColorAt(1, QColor(50, 50, 50));
    plot->setBackground(plotGradient);
    QLinearGradient axisRectGradient;
    axisRectGradient.setStart(0, 0);
    axisRectGradient.setFinalStop(0, 350);
    axisRectGradient.setColorAt(0, QColor(80, 80, 80));
    axisRectGradient.setColorAt(1, QColor(30, 30, 30));
    plot->axisRect()->setBackground(axisRectGradient);
}

void MainWindow::setGraphData(QCPGraph *graph, QString hostname, QString topic)
{
    for (auto i: db->query(createQuery(hostname, topic))) {
        //qDebug()<< QDateTime::fromMSecsSinceEpoch(timestamp.toLongLong());
        graph->addData(timestampToDouble(i.getTimestamp()), valueToDouble(i.getFields()));
    }

    graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1), QBrush(Qt::white), 10));
    graph->setPen(QPen(QColor(180, 180, 180), 4));
}
