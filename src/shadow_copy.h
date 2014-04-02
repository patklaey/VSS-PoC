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
#include <hash_map>

using namespace std;

#pragma comment (lib, "VssApi.lib")


class shadow_copy
{
private:
	bool debug = false;
	static const _VSS_SNAPSHOT_CONTEXT SC_SNAPSHOT_CONTEXT = VSS_CTX_BACKUP;
	static const VSS_BACKUP_TYPE SC_SNAPSHOT_TYPE = VSS_BT_COPY;
	static const bool SC_SNAPSHOT_BOOTABLE_STATE = false;
	static const bool SC_SNAPSHOT_SELECT_COMPONENTS = false;

	HRESULT result;
	IVssBackupComponents *pBackup = NULL;
	IVssAsync *pAsync = NULL;
	IVssAsync* pPrepare = NULL;
	IVssAsync* pDoShadowCopy = NULL;
	hash_map<WCHAR*,VSS_ID> snapshot_set_ids;
	hash_map<WCHAR*, VSS_SNAPSHOT_PROP> properties;


public:
	shadow_copy(bool debug);

	int initializeSnapshot();
	int addPartitionToSnapshot(WCHAR* partition);
	int createSnapshot();
	wstring getSnapshotPath(WCHAR* partition);

	~shadow_copy();
};

