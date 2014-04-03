#pragma once

#include <stdio.h>
#include <tchar.h>
#include <string>
#include <iostream>
#include "error.h"

#include "stdafx.h"

#include <shlwapi.h>
#include <vss.h>
#include <vswriter.h>
#include <vsbackup.h>
#include <unordered_map>

using namespace std;

#pragma comment (lib, "VssApi.lib")

/* Functions in VSSAPI.DLL */
typedef HRESULT(STDAPICALLTYPE * _CreateVssBackupComponentsInternal)(
    OUT IVssBackupComponents** ppBackup);
typedef void(APIENTRY * _VssFreeSnapshotPropertiesInternal)(IN VSS_SNAPSHOT_PROP* pProp);
static _CreateVssBackupComponentsInternal CreateVssBackupComponentsInternal_I;
static _VssFreeSnapshotPropertiesInternal VssFreeSnapshotPropertiesInternal_I;



class shadow_copy
{
private:
	bool debug = false;
	static const _VSS_SNAPSHOT_CONTEXT SC_SNAPSHOT_CONTEXT = VSS_CTX_BACKUP;
	static const VSS_BACKUP_TYPE SC_SNAPSHOT_TYPE = VSS_BT_COPY;
	static const bool SC_SNAPSHOT_BOOTABLE_STATE = false;
	static const bool SC_SNAPSHOT_SELECT_COMPONENTS = false;

	HRESULT result;
        HMODULE vssapiBase;
	IVssBackupComponents *pBackup = NULL;
	IVssAsync *pAsync = NULL;
	IVssAsync* pPrepare = NULL;
	IVssAsync* pDoShadowCopy = NULL;
	unordered_map<WCHAR*, VSS_ID> snapshot_set_ids;
	unordered_map<WCHAR*, VSS_SNAPSHOT_PROP> properties;


public:
	shadow_copy(bool debug);

	int initializeSnapshot();
	int addPartitionToSnapshot(WCHAR* partition);
	int createSnapshot();
	wstring getSnapshotPath(WCHAR* partition);

	~shadow_copy();
};

