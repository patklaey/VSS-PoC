#include "snapshot.h"

snapshot::snapshot(QObject *parent) :
    QObject(parent)
{

}

void snapshot::createSnapshotObject()
{
    // Create a new concrete snapshot object
    this->vss = new shadow_copy(true);

    // Check if the object could be initialized
    if ( this->vss == NULL )
    {
        emit sendSnapshotObjectCreated(CANNOT_INITIALIZE_BACKUP);
    } else
    {
        emit sendSnapshotObjectCreated(SUCCESS);
    }
}

void snapshot::initializeSnapshot()
{
    // Initialize the backup and send back the result
    emit sendSnapshotInitialized(this->vss->initializeSnapshot());
}

void snapshot::addPartitions(QVector<QString> partitions)
{
    int result = 0;
    QString name;
    foreach ( name, partitions)
    {
        // Convert the given partition to a WCHAR
        WCHAR partition_name[4] = {0};
        name.toWCharArray(partition_name);

        result += this->vss->addPartitionToSnapshot(partition_name);
    }

    // Add the partition to snapshot set and return the result
    emit sendPartitionAdded( result );
}

void snapshot::doSnapshot()
{
    // Return the result of the snapshot creation
    emit sendSnapshotExecuted( this->vss->createSnapshot() );
}
