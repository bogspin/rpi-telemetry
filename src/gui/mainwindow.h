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
#include "utils.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

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
    void plotMeasurement();

    void addPlot();
    void updatePlot(QCustomPlot *plot);
    void removePlot();
    void setPlotStyle(QCustomPlot *plot);
    void setGraphData(QCPGraph *graph, QString hostname,
                  QString topic = QString());

private slots:
    ConnectionWindow* openConnForm();
    SubscriptionWindow* openSubWindow();
    void openConfigWindow(const QModelIndex &index);
    void removeButton();
    void populateTree();
    void saveConfig();

private:
    Ui::MainWindow *ui;
    QList<QCustomPlot*> plots;

    QFileSystemWatcher configMonitor;
    QJsonObject configObj;
    QJsonTree configModel;

    std::unique_ptr<influxdb::InfluxDB> db;

signals:
    void configChanged();
};

#endif // MAINWINDOW_H
