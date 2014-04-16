#include "snapshot.h"

Snapshot::Snapshot(QObject *parent) :
    QObject(parent)
{

}

void Snapshot::createSnapshotObject()
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

void Snapshot::initializeSnapshot()
{
    // Initialize the backup and send back the result
    emit sendSnapshotInitialized(this->vss->initializeSnapshot());
}

void Snapshot::addPartitions(stringVector partitions)
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

void Snapshot::doSnapshot()
{
    // Return the result of the snapshot creation
    emit sendSnapshotExecuted( this->vss->createSnapshot() );

    // And clean up
    delete this->vss;
}
