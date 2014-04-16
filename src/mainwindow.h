#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "snapshot.h"
#include <QThread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void snapshotObjectCreated( int result );
    void snapshotInitialized( int result );
    void partitionsAddedToSnapshot( int result );
    void snapshotCreated( int result );

signals:
    void sendInitializeSnapshot();
    void sendAddPartitionsToSnapshot( stringVector partitions );
    void sendCreateSnapshot();

protected slots:
    void quit();
    void backup();

private:
    Ui::MainWindow *ui;
    QVector<QString> partitions;
    QThread* snapshot_thread;
    Snapshot* snapshot;

};

#endif // MAINWINDOW_H
