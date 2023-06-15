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
    ui->configTree->setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);

    timer.start(refreshInterval);

    connect(ui->actionas_CSV, &QAction::triggered, this, &MainWindow::exportCSV);
    connect(ui->actionas_PNG, &QAction::triggered, this, &MainWindow::exportPNG);
    connect(ui->actionQuit, &QAction::triggered, this, [] {
        QApplication::quit();
    });
    connect(ui->actionStart, &QAction::triggered, this, &MainWindow::startService);
    connect(ui->actionStop, &QAction::triggered, this, &MainWindow::stopService);
    connect(ui->actionRestart, &QAction::triggered, this, &MainWindow::restartService);
    connect(ui->actionStatus, &QAction::triggered, this, &MainWindow::serviceStatus);
    connect(ui->removeButton, &QToolButton::clicked, this, &MainWindow::removeButton);
    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::openConnWindow);
    connect(ui->subscribeButton, &QPushButton::clicked, this, &MainWindow::openSubWindow);
    connect(ui->plotButton, &QPushButton::clicked, this, &MainWindow::openDateRangeSelector);
    connect(ui->configTree, &QTreeView::doubleClicked, this, &MainWindow::openConfigWindow);

    connect(this, &MainWindow::configChanged, this, &MainWindow::saveConfig);
    connect(&configMonitor, &QFileSystemWatcher::fileChanged, this, [this]() {
        populateTree();
    });
    connect(&timer, &QTimer::timeout, this, &MainWindow::refreshGraphs);

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

void MainWindow::restartService()
{
    system("sudo systemctl restart mqtt-service.service");
}

void MainWindow::serviceStatus()
{
    FILE *fp;
    char path[1035];

    fp = popen("sudo systemctl status mqtt-service.service", "r");
    if (fp == NULL) {
        QMessageBox::warning(this, "Error", "Failed to check service status!");
    }

    QTextBrowser *status = new QTextBrowser(this);
    QString statusText;

    while (fgets(path, sizeof(path), fp) != NULL) {
        statusText.append(path);
    }

    status->setText(statusText);
    status->setStyleSheet("color: rgb(255, 255, 255);");
    widgets.append(status);
    updateLayout();

    pclose(fp);
}

ConnectionWindow* MainWindow::openConnWindow()
{
    ConnectionWindow* connWindow = new ConnectionWindow();

    connWindow->setWindowTitle("Add MQTT client");
    connWindow->setWindowState(Qt::WindowActive);
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - connWindow->width()) / 2;
    int y = (screenGeometry.height() - connWindow->height()) / 2;
    connWindow->move(x, y);
    connWindow->show();

    connect(connWindow, &ConnectionWindow::connection, this, &MainWindow::addConnection);

    return connWindow;
}

SubscriptionWindow* MainWindow::openSubWindow()
{
    int index = getConnIndex();
    if (index == -1) {
        QMessageBox::warning(this, "No selections", "Select a connection from the list before trying to subscribe to a topic!");
        return nullptr;
    }

    SubscriptionWindow* subWindow = new SubscriptionWindow();
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

DateRangeSelector* MainWindow::openDateRangeSelector()
{
    auto item = ui->configTree->currentIndex();

    if (!item.isValid()) {
        QMessageBox::warning(this, "No selections", "First click on a topic to plot its logged data!");
        return nullptr;
    }

    DateRangeSelector* dateRange = new DateRangeSelector();

    dateRange->setWindowState(Qt::WindowActive);
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - dateRange->width()) / 2;
    int y = (screenGeometry.height() - dateRange->height()) / 2;
    dateRange->move(x, y);
    dateRange->show();

    connect(dateRange, &DateRangeSelector::timestampRange, this, &MainWindow::plotMeasurement);

    return dateRange;
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
        ConnectionWindow *connWindow = openConnWindow();

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

void MainWindow::plotMeasurement(qint64 startTime, qint64 endTime, bool allTime)
{
    QString hostname, topic, alias, type, unit;
    int colorIndex = 0;
    auto selectedTopics = ui->configTree->selectionModel()->selectedIndexes();

    addPlot();
    QCustomPlot *plot = qobject_cast<QCustomPlot*>(widgets.last());

    for (auto item : selectedTopics) {
        if (!item.parent().isValid()) {
            QModelIndex child;
            int i = 0;

            while ((child = item.child(i, 0)).isValid()) {
                if (!selectedTopics.contains(child)) {
                    selectedTopics.append(child);
                }
                i++;
            }
        }
    }
    for (auto item : selectedTopics) {
        if (item.parent().isValid()) {
            QCPGraph *graph = plot->addGraph();
            GraphInfo graphInfo;

            hostname = configModel.data(item.parent(), QJsonTreeItem::mItem::itemValue).toString();
            topic = configModel.data(item, QJsonTreeItem::mItem::itemValue).toString();
            alias = configModel.data(item, QJsonTreeItem::mItem::itemAlias).toString();
            type = configModel.data(item, QJsonTreeItem::mItem::itemType).toString();
            unit = configModel.data(item, QJsonTreeItem::mItem::itemUnit).toString();
            graphInfo.setHostname(hostname);
            graphInfo.setTopic(topic);
            graphInfo.setAlias(alias);
            graphInfo.setType(type);
            graphInfo.setUnit(unit);
            if (!allTime) {
                graphInfo.setRange(startTime, endTime);
            }
            graph->setGraphInfo(graphInfo);
            graph->setName(createGraphName(alias, unit));
            setGraphData(graph, graphInfo.selectQuery());
            setGraphColor(graph, static_cast<graphColor>(colorIndex++));
        }
    }

    updatePlot(plot);
}

void MainWindow::addPlot()
{
    QCustomPlot *plot = new QCustomPlot();

    connect(plot, &QCustomPlot::mouseDoubleClick, this, &MainWindow::removePlot);
    connect(plot, &QCustomPlot::mousePress, this, &MainWindow::toggleLegend);
    widgets.append(plot);
    setPlotStyle(plot);
    updateLayout();
}

void MainWindow::updatePlot(QCustomPlot *plot)
{
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);

    for (int i = 0; i < plot->graphCount(); i++) {
        QCPGraph *graph = plot->graph(i);
        bool foundRange;
        QCPRange range = graph->data()->keyRange(foundRange);

        if (range.size() < DAY_SECONDS * 3) {
            dateTicker->setDateTimeFormat("d/M/yy\nHH:mm");
        } else {
            dateTicker->setDateTimeFormat("d MMMM\nyyyy");
        }
    }
    plot->xAxis->setTicker(dateTicker);
    plot->rescaleAxes();
    plot->replot();
}

void MainWindow::removePlot() {
    QCustomPlot* plot = qobject_cast<QCustomPlot*>(sender());

    plot->clearGraphs();
    widgets.removeOne(plot);
    ui->gridLayout->removeWidget(plot);
    if (plot->close()) {
        delete plot;
    }
    updateLayout();
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

void MainWindow::setGraphData(QCPGraph *graph, std::string query)
{
    std::vector<influxdb::Point> records;

    for (auto i : db->query(query)) {
        graph->addData(timestampToDouble(i.getTimestamp()), valueToDouble(i.getFields()));
    }
}

void MainWindow::setGraphColor(QCPGraph *graph, graphColor color)
{
    switch (color) {
    case graphColor::white: {
        graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1), QBrush(Qt::white), 6));
        graph->setPen(QPen(QColor(180, 180, 180), 3));
        break;
    }
    case graphColor::magenta: {
        graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1), QBrush(Qt::darkMagenta), 6));
        graph->setPen(QPen(QColor(187, 134, 252), 3));
        break;
    }
    case graphColor::cyan: {
        graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1), QBrush(Qt::darkCyan), 6));
        graph->setPen(QPen(QColor(3, 218, 198), 3));
        break;
    }
    case graphColor::red: {
        graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1), QBrush(Qt::darkRed), 6));
        graph->setPen(QPen(QColor(207, 102, 121), 3));
        break;
    }
    default: {
        graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1), QBrush(Qt::white), 6));
        graph->setPen(QPen(QColor(180, 180, 180), 3));
        break;
    }
    }
}

void MainWindow::resizeWidgets()
{
    for (int c = 0; c < ui->gridLayout->columnCount(); c++) {
        ui->gridLayout->setColumnStretch(c, 1);
    }
    for (int r = 0; r < ui->gridLayout->rowCount(); r++) {
        ui->gridLayout->setRowStretch(r, 1);
    }
}

void MainWindow::refreshGraphs()
{
    for (QWidget* widget : widgets) {
        QCustomPlot* plot = qobject_cast<QCustomPlot*>(widget);

        if (plot != nullptr) {
            for (int i = 0; i < plot->graphCount(); i++) {
                QCPGraph *graph = plot->graph(i);
                GraphInfo graphInfo = graph->info();

                setGraphData(graph, graphInfo.refreshQuery(refreshInterval));
                setGraphColor(graph, static_cast<graphColor>(i));
            }
            updatePlot(plot);
        }
    }
    timer.start(refreshInterval);
}

void MainWindow::updateLayout()
{
    QPoint gridPos;

    for (int i = 0; i < widgets.size(); i++) {
        gridPos = plotPosition(i);
        ui->gridLayout->addWidget(widgets.at(i), gridPos.x(), gridPos.y());
    }

    resizeWidgets();
}

void MainWindow::toggleLegend()
{
    QCustomPlot* plot = qobject_cast<QCustomPlot*>(sender());

    plot->legend->setVisible(!plot->legend->visible());
    plot->replot();
}

void MainWindow::exportCSV()
{
    if (widgets.isEmpty()) {
        QMessageBox::warning(this, "Error", "No plots to be exported!");
        return;
    }

    QString dirPath = QFileDialog::getExistingDirectory(this,
                                                        "Select save location",
                                                        QDir::currentPath());
    for (auto w : widgets) {
        QCustomPlot *plot;
        if ((plot = qobject_cast<QCustomPlot*>(w)) != nullptr) {
            for (int i = 0; i < plot->graphCount(); i++) {
                QCPGraph* graph = plot->graph(i);
                QString fileName = graph->getGraphInfo().getAlias() + ".csv";
                QFile saveFile;
                QTextStream out(&saveFile);

                saveFile.setFileName(dirPath + "/" + fileName);
                if (!saveFile.open(QIODevice::WriteOnly)) {
                    QString errMsg = "Could not save " + fileName;
                    QMessageBox::critical(this, "Error", errMsg);
                    continue;
                }
                auto graphData = graph->data();
                QCPDataContainer<QCPGraphData>::iterator it;

                for (it = graphData->begin(); it != graphData->end(); it++) {
                    out << it->key;
                    out << ',';
                    out << it->value;
                    out << ',';
                }
                /* remove last comma */
                saveFile.resize(saveFile.size() - 1);
            }
        }
    }
}

void MainWindow::exportPNG()
{
    if (widgets.isEmpty()) {
        QMessageBox::warning(this, "Error", "No plots to be exported!");
        return;
    }

    QString dirPath = QFileDialog::getExistingDirectory(this,
                                                        "Select save location",
                                                        QDir::currentPath());
    int i = 1;
    for (auto w : widgets) {
        QCustomPlot *plot;
        if ((plot = qobject_cast<QCustomPlot*>(w)) != nullptr) {
            QString fileName = "plot_" + QString::number(i++);
            plot->savePng(fileName, 800, 600);
        }
    }
}
