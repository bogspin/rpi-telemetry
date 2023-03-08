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
#include "connectionform.h"

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

    void makePlot();
    QJsonParseError loadConfigJson(const QString &path);
    void displayConfigJson();
    void addConnection(QJsonObject connInfo);

private slots:
    void openConnForm();

private:
    Ui::MainWindow *ui;

    QStringListModel *model;
    QJsonObject configObj;
    std::unique_ptr<influxdb::InfluxDB> db;

signals:
    void configChanged();
};

#endif // MAINWINDOW_H
