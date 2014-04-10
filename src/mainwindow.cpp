#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QVector>
#include "counter.h"
#include "shadow_copy.h"

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

    QVector<QString> partitions;
    QMessageBox msgBox;

    // Check which checkboxes are checked
    if ( this->ui->c_checkbox->isChecked() )
        partitions.append("C:\\");

    if ( this->ui->e_checkbox->isChecked() )
        partitions.append("E:\\");

    if ( partitions.size() == 0 )
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
        // Initialize the backup
        shadow_copy* vss = new shadow_copy(true);
        if ( vss->initializeSnapshot() != SUCCESS )
        {
            QMessageBox msg;
            msg.setText(  "ERROR" );
            msg.setInformativeText("Initializing Backup failed");
            msg.setStandardButtons(QMessageBox::Ok);
            msg.setDefaultButton(QMessageBox::Ok);
            msg.exec();
            return;
        }

        // Add the partition tho the backup
        foreach (name, partitions)
        {
            // Convert the QString name to a wchar*
            wstring partition = name.toStdWString();

            // Add the partition to the snapshot set
            if (vss->addPartitionToSnapshot(partition) != SUCCESS )
            {
                QMessageBox msg;
                msg.setText(  "ERROR" );
                msg.setInformativeText("Adding partition " + name + " to snapshot set failed");
                msg.setStandardButtons(QMessageBox::Ok);
                msg.setDefaultButton(QMessageBox::Ok);
                msg.exec();
                return;
            }
        }

        if (vss->createSnapshot() != SUCCESS)
        {
            QMessageBox msg;
            msg.setText( "ERROR" );
            msg.setInformativeText("Creating Backup failed");
            msg.setStandardButtons(QMessageBox::Ok);
            msg.setDefaultButton(QMessageBox::Ok);
            msg.exec();
            return;
        }

        QMessageBox msg;
        msg.setText(  "SUCCESS" );
        msg.setInformativeText("Backup Successfully Created!");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        return;

        delete vss;

    }

}

MainWindow::~MainWindow()
{
    delete ui;
}
