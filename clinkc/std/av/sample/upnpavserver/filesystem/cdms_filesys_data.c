/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cdms_filesys_data.c
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#include "cdms_filesys.h"

/****************************************
* cg_upnp_dms_filesys_content_data_new
****************************************/

CgUpnpMediaFileSystemContentData *cg_upnp_dms_filesys_content_data_new()
{
	CgUpnpMediaFileSystemContentData *data;

	data = (CgUpnpMediaFileSystemContentData *)malloc(sizeof(CgUpnpMediaFileSystemContentData));
	data->pubdir = NULL;

	return data;
}

/****************************************
* cg_upnp_dms_filesys_content_data_delete
****************************************/

void cg_upnp_dms_filesys_content_data_delete(CgUpnpMediaFileSystemContentData *data)
{
	if (!data)
		return;

	if (data->pubdir)
		free(data->pubdir);

	free(data);
}

/****************************************
* cg_upnp_dms_filesys_content_getdata
****************************************/

CgUpnpMediaFileSystemContentData *cg_upnp_dms_filesys_content_getdata(CgUpnpMediaContent *con)
{
	CgUpnpMediaFileSystemContentData *data;

	data = cg_upnp_media_content_getuserdata(con);
	if (!data) {
		data = cg_upnp_dms_filesys_content_data_new();
		cg_upnp_media_content_setuserdata(con, data);
	}

	return data;
}

/****************************************
* cg_upnp_dms_filesys_content_setpubicdirectory
****************************************/

void cg_upnp_dms_filesys_content_setpubicdirectory(CgUpnpMediaContent *con, char *dir)
{
	CgUpnpMediaFileSystemContentData *data;

	data = cg_upnp_dms_filesys_content_getdata(con);
	if (data->pubdir)
		free(data->pubdir);
	data->pubdir = cg_strdup(dir);
}

/****************************************
* cg_upnp_dms_filesys_content_getpubicdirectory
****************************************/

char *cg_upnp_dms_filesys_content_getpubicdirectory(CgUpnpMediaContent *con)
{
	CgUpnpMediaFileSystemContentData *data;

	data = cg_upnp_dms_filesys_content_getdata(con);
	return data->pubdir;
}
