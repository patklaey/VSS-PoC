#include "stdafx.h"
#include "shadow_copy.h"
#include <QString>
#include <QDebug>

shadow_copy::shadow_copy(bool _debug)
{
	// Set the debug variable
	this->debug = _debug;

    CoInitialize(NULL);

    // Load the vssapi library
    this->vssapiBase = LoadLibrary(L"vssapi.dll");

    // Get the
    CreateVssBackupComponentsInternal_I = (_CreateVssBackupComponentsInternal)GetProcAddress(this->vssapiBase, "CreateVssBackupComponentsInternal");

    // Create the shadow copy backup component (VSSBackupComponent)
    this->result = CreateVssBackupComponentsInternal_I(&(this->pBackup));

	// Check if the operation succeeded
	if (this->result != S_OK)
    {
        QString error = QString("Cannot create VSSBackupComponent, operation failed with error: 0x%08lx").arg(this->result);
        qCritical() << error;
    }

}

int shadow_copy::initializeSnapshot()
{
	// Initialize the backup
    if ( this->pBackup == NULL )
    {
        QString error = QString("Cannot initialize backup, VSSBackupComponent is null");
        qCritical() << error;
        return CANNOT_INITIALIZE_BACKUP;
    }

	this->result = this->pBackup->InitializeForBackup();

	// Check if the operation succeeded
	if (this->result != S_OK)
	{
        QString error = QString("Initialize for backup failed with error: = 0x%08lx").arg(this->result);
        qCritical() << error;
		return CANNOT_INITIALIZE_BACKUP;
	}

	// Set the context, we want an non-persistant backup which involves writers
	this->result = this->pBackup->SetContext(this->SC_SNAPSHOT_CONTEXT);

	// Check if the operation succeeded
	if (this->result != S_OK)
	{
        QString error = QString("Setting backup context to %i failed with error: 0x%08lx").arg(this->SC_SNAPSHOT_CONTEXT).arg(this->result);
        qCritical() << error;
		return CANNOT_SET_BACKUP_CONTEXT;
	}

	// Tell the writers to gather metadata
	this->result = this->pBackup->GatherWriterMetadata(&(this->pAsync));
	
	// Check if the operation succeeded
	if (this->result != S_OK)
	{
        QString error = QString("Writers gathering metadata failed with error: 0x%08lx").arg(this->result);
        qCritical() << error;
		return WRITER_GATHERING_METADATA_FAILED;
	}

	// Wait until all writers collected the metadata
	this->pAsync->Wait();

	// Check if the operation succeeded
	if (this->result != S_OK)
	{
        QString error = QString("Waiting for writers collecting metadata failed with error: 0x%08lx").arg(this->result);
        qCritical() << error;
		return ASYNC_WAIT_FAILED;
	}

	VSS_ID tmp_snapshot_set_id;

	// Start the snapshot set
	this->result = this->pBackup->StartSnapshotSet(&tmp_snapshot_set_id);

	// Check if the operation succeeded
	if (this->result != S_OK)
	{
        QString error = QString("Strating snapshot set failed with error: 0x%08lx").arg(this->result);
        qCritical() << error;
		return CANNOT_START_SNAPSHOT_SET;
	}

	return SUCCESS;
}

int shadow_copy::addPartitionToSnapshot(WCHAR* partition)
{
	// Create temporary VSS_ID
	VSS_ID tmp_snapshot_set_id;
	
	// Add the partition to the snapshot set
	this->result = this->pBackup->AddToSnapshotSet(partition, GUID_NULL, &tmp_snapshot_set_id);

	// Check if the operation succeeded
	if (this->result != S_OK)
	{
		_tprintf(_T("Adding %s to snapshot set failed with error: 0x%08lx\n"), partition, this->result);
		return CANNOT_ADD_TO_SNAPSHOT_SET;
	}

	// Add the partition name and the snapshot set id the the hash map
	this->snapshot_set_ids[partition] = tmp_snapshot_set_id;

	return SUCCESS;

}

int shadow_copy::createSnapshot()
{
	// Set the backup state
	this->result = this->pBackup->SetBackupState(this->SC_SNAPSHOT_SELECT_COMPONENTS, this->SC_SNAPSHOT_BOOTABLE_STATE, this->SC_SNAPSHOT_TYPE);

	// Check if the operation succeeded
	if (this->result != S_OK)
	{
		_tprintf(_T("Setting backup state failed with error: 0x%08lx\n"), this->result);
		return CANNOT_SET_SNAPSHOT_STATE;
	}

	// Tell everyone to prepare for the backup
	this->result = this->pBackup->PrepareForBackup(&(this->pPrepare));

	// Check if the operation succeeded
	if (this->result != S_OK)
	{
		_tprintf(_T("Preparing for backup failed with error: 0x%08lx\n"), this->result);
		return CANNOT_PREPARE_FOR_BACKUP;
	}

	// Wait for everyone to be ready
	this->result = this->pPrepare->Wait();

	// Check if the operation succeeded
	if (this->result != S_OK)
	{
		_tprintf(_T("Waiting for preparing for backup failed with error: 0x%08lx\n"), this->result);
		return ASYNC_WAIT_FAILED;
	}

	// And create the shadow copy
	this->result = this->pBackup->DoSnapshotSet(&(this->pDoShadowCopy));

	// Check if the operation succeeded
	if (this->result != S_OK)
	{
		_tprintf(_T("Creating shadow copy snapshot failed with error: 0x%08lx\n"), this->result);
		return CANNOT_CREATE_SNAPSHOT;
	}
	
	// Wait until the shadow copy is created
	this->result = this->pDoShadowCopy->Wait();

	// Check if the operation succeeded
	if (this->result != S_OK)
	{
		_tprintf(_T("Waiting for shadow copy to finish failed with error: 0x%08lx\n"), this->result);
		return ASYNC_WAIT_FAILED;
	}

	// Go thriugh all snapshots and get the according properties
	for (auto &tmp : this->snapshot_set_ids)
	{
		VSS_SNAPSHOT_PROP tmp_snapshot_prop;

		// Get the and set them to the snapshot properties field
		this->result = this->pBackup->GetSnapshotProperties(tmp.second, &tmp_snapshot_prop);
		
		// Check if the operation succeeded
		if (this->result != S_OK)
		{
			_tprintf(_T("Getting snapshot properties failed with error: 0x%08lx\n"), this->result);
			return CANNOT_GET_SNAPSHOT_PROPERTIES;
		}

		// Store the snapshot properties according to the partition name
		this->properties[tmp.first] = tmp_snapshot_prop;
	}

	return SUCCESS;
}

wstring shadow_copy::getSnapshotPath(WCHAR* partition)
{
	return this->properties.at(partition).m_pwszSnapshotDeviceObject;
}


shadow_copy::~shadow_copy()
{
	// Release the shadow copy creation request
	if (this->pDoShadowCopy != NULL)
		this->pDoShadowCopy->Release();

	// Release the prepeare for backup request
	if (this->pPrepare != NULL)
		this->pPrepare->Release();

	// Release the metadata gathering request
	if (this->pAsync != NULL)
		pAsync->Release();

	// Release the main VSS request
	if (this->pBackup != NULL)
		this->pBackup->Release();
}
