#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <QObject>
#include <QString>
#include <QVector>
#include "shadow_copy.h"

class snapshot : public QObject
{
    Q_OBJECT
    shadow_copy *vss;

public:
    explicit snapshot(QObject *parent = 0);

signals:
    void sendSnapshotObjectCreated(int result);
    void sendSnapshotInitialized(int result);
    void sendPartitionAdded(int result);
    void sendSnapshotExecuted(int result);

public slots:
    void createSnapshotObject();
    void initializeSnapshot();
    void addPartitions(QVector<QString> partitions);
    void doSnapshot();
};

#endif // SNAPSHOT_H
