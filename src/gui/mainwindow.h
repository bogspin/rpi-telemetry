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
#include "InfluxDBFactory.h"
#include "Point.h"
#include "qcustomplot.h"
#include "qjsontree.h"
#include "connectionform.h"
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
    int getConnIndex();

    void connectToDB();
    void plotMeasurement();

    void addPlot();
    void updatePlot(QCustomPlot *plot);
    void removePlot();
    void setPlotStyle(QCustomPlot *plot);
    void setGraphData(QCPGraph *graph, QString hostname,
                  QString topic = QString());

private slots:
    void openConnForm();
    void openSubWindow();
    void openConfigWindow(const QModelIndex &index);
    void loadTree();
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
