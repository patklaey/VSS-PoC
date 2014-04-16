#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QVector>
#include "counter.h"
#include <QDebug>
#include <QThread>
#include "snapshot.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->quit_button, SIGNAL(clicked()), this, SLOT(quit()));
    connect(ui->backup_button, SIGNAL(clicked()), this, SLOT(backup()));

    counter* count = new counter(ui->counter);
    count->start();
}

void MainWindow::quit()
{
    QApplication::quit();
}

void MainWindow::backup()
{

    QMessageBox msgBox;

    // Clear the partitions
    this->partitions.clear();

    // Check which checkboxes are checked
    if ( this->ui->c_checkbox->isChecked() )
        this->partitions.append("C:\\");

    if ( this->ui->e_checkbox->isChecked() )
        this->partitions.append("E:\\");

    if ( this->partitions.size() == 0 )
    {
        msgBox.critical(0,"No partition selected","Please select at least one partition to backup",QMessageBox::Ok);
        return;
    }

    QString partition_string;
    QString name;
    foreach (name, partitions)
    {
        partition_string.append(name + "\n");
    }

    msgBox.setText("Backing up the following partitions:");
    msgBox.setInformativeText(partition_string);
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();

    if ( ret == QMessageBox::Ok )
    {
        // Initialize the snapshot and move it to a new thread
        QThread snapshot_thread;
        snapshot snapshot;
        snapshot.connect(&snapshot_thread, SIGNAL(started()), SLOT(createSnapshotObject()));
        snapshot.moveToThread(&snapshot_thread);

        // Connect the slots and signals
        snapshot.connect( this, SIGNAL( sendInitializeSnapshot() ), SLOT( initializeSnapshot() ) );
        snapshot.connect( this, SIGNAL( sendAddPartitionsToSnapshot( QVector<QString> ) ), SLOT( addPartitions( QVector<QString> ) ) );
        snapshot.connect( this, SIGNAL( sendCreateSnapshot() ), SLOT( doSnapshot() ) );
        this->connect( &snapshot, SIGNAL( sendSnapshotObjectCreated(int) ), SLOT( snapshotCreated(int) ) );
        this->connect( &snapshot, SIGNAL( sendSnapshotInitialized(int) ), SLOT( snapshotInitialized(int) ) );
        this->connect( &snapshot, SIGNAL( sendPartitionAdded(int) ), SLOT( partitionsAddedToSnapshot(int) ) );
        this->connect( &snapshot, SIGNAL( sendSnapshotExecuted(int) ), SLOT( snapshotCreated(int) ) );

        // Start the snapshot thread
        qDebug() << "Starting snapshot thread" ;
        snapshot_thread.start();
    }

}

void MainWindow::snapshotObjectCreated(int result)
{
    if ( result != SUCCESS )
    {
        QMessageBox msg;
        msg.setText(  "ERROR" );
        msg.setInformativeText("Creating Snapshot Object failed");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
    } else
    {
        qDebug() << "Successfully created backup object, going to initialize snapshot" ;
        emit sendInitializeSnapshot();
    }
}

void MainWindow::snapshotInitialized(int result)
{
    if ( result != SUCCESS )
    {
        QMessageBox msg;
        msg.setText(  "ERROR" );
        msg.setInformativeText("Initializing Backup failed");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
    } else
    {
        // Add the partitions to the snapshot
        qDebug() << "Successfully initialized backup, going to add partitions" ;
        emit sendAddPartitionsToSnapshot( this->partitions );
    }
}

void MainWindow::partitionsAddedToSnapshot( int result )
{
    if ( result != SUCCESS )
    {
        QMessageBox msg;
        msg.setText(  "ERROR" );
        msg.setInformativeText("Adding partitions to snapshot set failed");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
    } else
    {
        // Execute the snapshot
        qDebug() << " Successfully added partitions to snapshot set, going to create snapshot" ;
        emit sendCreateSnapshot();
    }
}

void MainWindow::snapshotCreated(int result)
{
    if ( result != SUCCESS )
    {
        QMessageBox msg;
        msg.setText( "ERROR" );
        msg.setInformativeText("Creating Backup failed");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
    } else
    {
        QMessageBox msg;
        msg.setText(  "SUCCESS" );
        msg.setInformativeText("Backup Successfully Created!");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}
