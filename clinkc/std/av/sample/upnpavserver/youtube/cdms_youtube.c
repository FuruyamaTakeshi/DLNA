/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cdms_youtube.c
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#include <cybergarage/upnp/std/av/cmediaserver.h>
#include <cybergarage/net/cinterface.h>

#include "cdms_youtube.h"
#include "crss.h"
#include "cwebservice.h"

#if defined(WIN32)
#include <TCHAR.h>
#endif

/****************************************
* cg_upnp_dms_youtube_actionlistner
****************************************/

BOOL cg_upnp_dms_youtube_actionlistner(CgUpnpAction *action)
{
#if defined(CG_UPDATE_BY_ROOTBROWSE)
	CgUpnpMediaServer *dms;
	CgUpnpDevice *dev;
	char *contentID;
	char *actionName;

	actionName = cg_upnp_action_getname(action);
	if (cg_strlen(actionName) <= 0)
		return FALSE;

	/* Browse */
	if (cg_streq(actionName, CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE)) {
		contentID = cg_upnp_action_getargumentvaluebyname(action, CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_OBJECT_ID);
		if (cg_strlen(contentID) <=0)
			return FALSE;

		if (cg_streq(CG_UPNP_MEDIA_ROOT_CONTENT_ID, contentID) == FALSE)
			return FALSE;

		dev = cg_upnp_service_getdevice(cg_upnp_action_getservice(action));
		if (!dev)
			return FALSE;

		dms = (CgUpnpMediaServer *)cg_upnp_device_getuserdata(dev);
		if (!dms)
			return FALSE;

		cg_upnp_dms_lock(dms);
		
		//cg_upnp_dms_youtube_update(dms);

		cg_upnp_dms_unlock(dms);
	}
#endif

	return FALSE;
}

/****************************************
* cg_upnp_dms_youtube_new
****************************************/

CgUpnpMediaServer *cg_upnp_dms_youtube_new()
{
	CgUpnpMediaServer *dms;
	CgUpnpDmsYoutubeData *dmsData;
	CgUpnpDevice *dev;

	dms = cg_upnp_dms_new();

	dev = cg_upnp_dms_getdevice(dms);
	cg_upnp_device_setfriendlyname(dev, CG_UPNP_MEDIA_YOUTUBE_APP_NAME " " CG_UPNP_MEDIA_YOUTUBE_APP_VER);

	cg_upnp_dms_setactionlistener(dms, cg_upnp_dms_youtube_actionlistner);
	cg_upnp_dms_sethttplistener(dms, cg_upnp_dms_youtube_http_listener);

	dmsData = cg_upnp_dms_youtube_data_new();
	if (!dmsData) {
		cg_upnp_dms_delete(dms);
		return NULL;
	}
	cg_upnp_dms_setuserdata(dms, dmsData);

	cg_upnp_dms_youtube_setupdatelistener(dms, NULL);
	cg_upnp_dms_youtube_setupdatelistenerdata(dms, NULL);

	return dms;
}
/****************************************
* cg_upnp_dms_filesys_updatecontentlist
****************************************/

void cg_upnp_dms_youtube_clearcontentfiles(CgUpnpMediaServer *dms)
{
#if defined(WIN32)
	WIN32_FIND_DATA fd;
	HANDLE hFind;
#else
	struct dirent **fileList;
	int n;
#endif

#if defined(WIN32)
	hFind = FindFirstFile(_T("*.") _T(CG_UPNP_MEDIA_YOUTUBE_TRANSCODE_FILEEXT), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do{
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;
			DeleteFile(fd.cFileName);
		} while(FindNextFile(hFind,&fd) != FALSE);
		FindClose(hFind);
	}
	hFind = FindFirstFile(_T("*.") _T(CG_UPNP_MEDIA_YOUTUBE_FLV_FILEEXT), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do{
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;
			DeleteFile(fd.cFileName);
		} while(FindNextFile(hFind,&fd) != FALSE);
		FindClose(hFind);
	}
#else
	n = scandir("*." CG_UPNP_MEDIA_YOUTUBE_TRANSCODE_FILEEXT, &fileList, 0, alphasort);
	if (0 <= n) {
		while(n--) {
			if ((fileStat.st_mode & S_IFMT)==S_IFDIR)
				continue;
			remove(fileList[n]->d_name);
		}
		free(fileList[n]);
	}
	free(fileList);
	n = scandir("*." CG_UPNP_MEDIA_YOUTUBE_FLV_FILEEXT, &fileList, 0, alphasort);
	if (0 <= n) {
		while(n--) {
			if ((fileStat.st_mode & S_IFMT)==S_IFDIR)
				continue;
			remove(fileList[n]->d_name);
		}
		free(fileList[n]);
	}
	free(fileList);
#endif
}

/****************************************
* cg_upnp_dms_youtube_delete
****************************************/

void cg_upnp_dms_youtube_delete(CgUpnpMediaServer *dms)
{
	CgUpnpDmsYoutubeData *dmsData;

	cg_upnp_dms_youtube_clearcontentfiles(dms);

	dmsData = cg_upnp_dms_getuserdata(dms);
	if (dmsData)
		cg_upnp_dms_youtube_data_delete(dmsData);

	cg_upnp_dms_delete(dms);
}
