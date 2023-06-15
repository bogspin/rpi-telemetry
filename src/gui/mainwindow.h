#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScreen>
#include <QDateTime>
#include <QMessageBox>
#include <QFile>
#include <QFileSystemWatcher>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <string.h>
#include <unistd.h>
#include "InfluxDBFactory.h"
#include "Point.h"
#include "qcustomplot.h"
#include "qjsontree.h"
#include "connectionwindow.h"
#include "subscriptionwindow.h"
#include "daterangeselector.h"
#include "utils.h"

#define DAY_SECONDS 86400

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

enum graphColor {
    white = 0,
    magenta,
    cyan,
    red
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void startService();
    void stopService();
    void restartService();
    void serviceStatus();

    QString getConfigPath();
    void setConfigPath(const QString &path);
    QJsonParseError loadConfigJson();
    void addConnection(QJsonObject connInfo);
    void addSubscription(QJsonObject subInfo);
    void removeConnection(int connNumber);
    void removeSubscription(int connNumber, int subNumber);
    int getConnIndex();
    bool isConnection(const QModelIndex &index);
    bool isSubscription(const QModelIndex &index);

    void connectToDB();
    void plotMeasurement(qint64 startTime, qint64 endTime, bool allTime);

    void addPlot();
    void updatePlot(QCustomPlot *plot);
    void removePlot();
    void setPlotStyle(QCustomPlot *plot);
    void setGraphData(QCPGraph *graph, std::string query);
    void setGraphColor(QCPGraph *graph, graphColor color);
    void resizeWidgets();
    void refreshGraphs();
    void updateLayout();
    void toggleLegend();

    void exportCSV();
    void exportPNG();

private slots:
    ConnectionWindow* openConnWindow();
    SubscriptionWindow* openSubWindow();
    DateRangeSelector* openDateRangeSelector();
    void openConfigWindow(const QModelIndex &index);
    void removeButton();
    void populateTree();
    void saveConfig();

private:
    Ui::MainWindow *ui;
    QList<QWidget*> widgets;

    QTimer timer;
    qint64 refreshInterval = 300000;

    QFileSystemWatcher configMonitor;
    QJsonObject configObj;
    QJsonTree configModel;

    std::unique_ptr<influxdb::InfluxDB> db;

signals:
    void configChanged();
};

#endif // MAINWINDOW_H
