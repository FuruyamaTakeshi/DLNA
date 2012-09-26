/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cdms_filesys.c
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#include "cdms_filesys.h"
#include <cybergarage/upnp/std/av/cmediaserver.h>
#include <cybergarage/net/cinterface.h>

#include <stdio.h>
#include <ctype.h>

#if defined(WIN32)
#include <windows.h>
#include <tchar.h>
#if !defined(WINCE)
#include <sys/stat.h>
#include <conio.h>
#endif
#else
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#endif

/****************************************
* cg_upnp_dms_filesys_getmimetype
****************************************/

char *cg_upnp_dms_filesys_getmimetype(char *ext)
{
	if (cg_strcaseeq(ext, "jpeg") || cg_strcaseeq(ext, "jpg"))
		return CG_UPNP_MEDIA_MIMETYPE_JPEG;
	if (cg_strcaseeq(ext, "mpeg") || cg_strcaseeq(ext, "mpg"))
		return CG_UPNP_MEDIA_MIMETYPE_MPEG;
	if (cg_strcaseeq(ext, "mp3"))
		return CG_UPNP_MEDIA_MIMETYPE_MP3;
	return NULL;
}

/****************************************
* cg_upnp_dms_filesys_getupnpclass
****************************************/

char *cg_upnp_dms_filesys_getupnpclass(char *ext)
{
	if (cg_strcaseeq(ext, "jpeg") || cg_strcaseeq(ext, "jpg"))
		return CG_UPNP_MEDIA_UPNPCLASS_PHOTO;
	if (cg_strcaseeq(ext, "mpeg") || cg_strcaseeq(ext, "mpg"))
		return CG_UPNP_MEDIA_UPNPCLASS_MOVIE;
	if (cg_strcaseeq(ext, "mp3"))
		return CG_UPNP_MEDIA_UPNPCLASS_MUSIC;
	return NULL;
}

/****************************************
* cg_upnp_dms_filesys_updatecontentlist
****************************************/

void cg_upnp_dms_filesys_updatecontentlist(CgUpnpMediaServer *dms, CgUpnpMediaContentList *parentCon, char *pubdir)
{
	CgUpnpMediaContent *con;
	CgUpnpMediaResource *res;
	CgNetworkInterfaceList *netIfList;
	CgNetworkInterfaceList *netIf;
	int conType;
	char *fileName;
	CgString *fullPathStr;
	int fileExtIdx;
	char *fileExt;
	char idmd5[CG_MD5_STRING_BUF_SIZE];
	char resURL[CG_UPNP_MEDIA_FILESYS_RESURL_MAXLEN];
	char *mimeType;
	char *upnpClass;
	CgUpnpMediaFileSystemContentData *conData;
	char dlnaAttr[CG_UPNP_MEDIA_DLNAPN_MAXLEN];
#if !defined(WINCE)
	struct stat fileStat;
	off_t fileSize;
#else
	fpos_t fileSize;
	FILE *fp;
#endif
#if defined(WIN32)
	CgString *findDirStr;
	#if defined(UNICODE)
	TCHAR wCharBuf[MAX_PATH];
	char mCharBuf[MAX_PATH];
	#endif
	WIN32_FIND_DATA fd;
	HANDLE hFind;
#else
	struct dirent **fileList;
	int n;
#endif

	for (con=cg_upnp_media_content_getchildcontents(parentCon); con; con=cg_upnp_media_content_next(con)) {
		conData =cg_upnp_dms_filesys_content_getdata(con);
		if (!conData)
			continue;
		cg_upnp_dms_filesys_content_data_delete(conData);
	}
	cg_upnp_media_content_clearchildcontents(parentCon);

	if (cg_strlen(pubdir) <= 0)
		return;

	netIfList = cg_net_interfacelist_new();
	cg_net_gethostinterfaces(netIfList);
	netIf = cg_net_interfacelist_gets(netIfList);
	if (!netIf) {
		cg_net_interfacelist_delete(netIfList);
		return;
	}

	conType = CG_UPNP_MEDIA_CONTENT_NONE;

#if defined(WIN32)
	findDirStr = cg_string_new();
	cg_string_addvalue(findDirStr, pubdir);
	cg_string_addvalue(findDirStr, "\\*.*");
	#if defined(UNICODE)
	MultiByteToWideChar(CP_UTF8, 0, cg_string_getvalue(findDirStr), -1, wCharBuf, (MAX_PATH-1));
	hFind = FindFirstFile(wCharBuf, &fd);
	#else
	hFind = FindFirstFile(cg_string_getvalue(findDirStr), &fd);
	#endif
	if (hFind != INVALID_HANDLE_VALUE) {
		do{
			#if defined(UNICODE)
			WideCharToMultiByte(CP_ACP, 0, fd.cFileName, -1, mCharBuf, (MAX_PATH-1), NULL, NULL);
			fileName = cg_strdup(mCharBuf);
			#else
			fileName = cg_strdup(fd.cFileName);
			#endif
			if (!cg_streq(".", fileName) && !cg_streq("..", fileName)) {
				fullPathStr = cg_string_new();
				cg_string_addvalue(fullPathStr, pubdir);
				cg_string_addvalue(fullPathStr, "\\");
				cg_string_addvalue(fullPathStr, fileName);
				conType = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? CG_UPNP_MEDIA_CONTENT_CONTAINER : CG_UPNP_MEDIA_CONTENT_ITEM;
#else
	n = scandir(pubdir, &fileList, 0, alphasort);
	if (0 <= n) {
		while(n--) {
			if (!cg_streq(".", fileList[n]->d_name) && !cg_streq("..", fileList[n]->d_name)) {
				fileName = cg_strdup(fileList[n]->d_name);
				fullPathStr = cg_string_new();
				cg_string_addvalue(fullPathStr, pubdir);
				cg_string_addvalue(fullPathStr, "/");
				cg_string_addvalue(fullPathStr, fileList[n]->d_name);
				if(stat(cg_string_getvalue(fullPathStr), &fileStat) != -1)
					conType = ((fileStat.st_mode & S_IFMT)==S_IFDIR) ? CG_UPNP_MEDIA_CONTENT_CONTAINER : CG_UPNP_MEDIA_CONTENT_ITEM;
#endif
				if (conType == CG_UPNP_MEDIA_CONTENT_NONE)
					continue;

				mimeType = NULL;
				if ((conType == CG_UPNP_MEDIA_CONTENT_ITEM)) {
					fileExtIdx = cg_strrchr(fileName, ".", 1);
					if (0 < fileExtIdx) {
						fileExt = (fileName + fileExtIdx + 1);
						fileName[fileExtIdx] = '\0';
						mimeType = cg_upnp_dms_filesys_getmimetype(fileExt);
						upnpClass = cg_upnp_dms_filesys_getupnpclass(fileExt);
					}
					if (!mimeType || !upnpClass) {
						free(fileName);
						cg_string_delete(fullPathStr);
						continue;
					}
				}

				con = cg_upnp_media_content_new();

				/* id */
				cg_str2md5((char *)cg_string_getvalue(fullPathStr), idmd5);
				cg_upnp_media_content_setid(con, idmd5);

				/* title */
				cg_upnp_media_content_settitle(con, fileName);

				/* parent id */
				cg_upnp_media_content_setparentid(con, cg_upnp_media_content_getid(parentCon));

				/* type */
				cg_upnp_media_content_settype(con, conType);

				/* upnp:class */
				cg_upnp_media_content_setupnpclass(con, upnpClass);

				/* url */
				if ((conType == CG_UPNP_MEDIA_CONTENT_ITEM)) {
					res = cg_upnp_media_resource_new();
					sprintf(resURL, "http://%s:%d/%s/%s",
						cg_net_interface_getaddress(netIf),
						cg_upnp_device_gethttpport(cg_upnp_dms_getdevice(dms)),
						CG_UPNP_MEDIA_FILESYS_RESURL_PATH,
						idmd5
						);
					cg_upnp_media_resource_seturl(res, resURL);
					cg_upnp_media_resource_setmimetype(res, mimeType);
					cg_upnp_media_resource_setdlnaattribute(res, cg_upnp_media_resource_getdlnapnfrommimetype(res, dlnaAttr, sizeof(dlnaAttr)));
					cg_upnp_media_content_addresource(con, res);

					/* size */
#if !defined(WINCE)
					fileSize = 0;
					if (stat(cg_string_getvalue(fullPathStr), &fileStat) == 0)
						fileSize = fileStat.st_size;
#else
					fp = fopen(cg_string_getvalue(fullPathStr), "rb");
					if (fp) {
						fseek(fp, 0, SEEK_END); 
						fgetpos(fp, &fileSize); 
						fclose(fp);
					}
#endif
					cg_upnp_media_resource_setsize(res, (long)fileSize);
				}

				cg_upnp_dms_filesys_content_setpubicdirectory(con, cg_string_getvalue(fullPathStr));

				cg_upnp_media_content_addchildcontent(parentCon, con);
				cg_upnp_dms_condir_updatesystemupdateid(dms);

				free(fileName);
				cg_string_delete(fullPathStr);
				
				conType = CG_UPNP_MEDIA_CONTENT_NONE;
#if defined(WIN32)
			}
		} while(FindNextFile(hFind,&fd) != FALSE);
		FindClose(hFind);
	}
	cg_string_delete(findDirStr);
#else
			}
			free(fileList[n]);
		}
		free(fileList);
	}
#endif

	cg_net_interfacelist_delete(netIfList);
}

/****************************************
* cg_upnp_dms_filesys_updaterootcontentlist
****************************************/

void cg_upnp_dms_filesys_updaterootcontentlist(CgUpnpMediaServer *dms)
{
	char *pubdir;
	CgUpnpMediaContent *rootContent;

	pubdir = cg_upnp_dms_filesys_getpublicationdirectory(dms);
	if (!pubdir)
		return;

	rootContent = cg_upnp_dms_getrootcontent(dms);

	if (!rootContent)
		return;

	cg_upnp_dms_lock(dms);

	cg_upnp_dms_filesys_updatecontentlist(dms, rootContent, pubdir);

	cg_upnp_dms_unlock(dms);
}

/****************************************
* cg_upnp_dms_filesys_setpublicationdirectory
****************************************/

void cg_upnp_dms_filesys_setpublicationdirectory(CgUpnpMediaServer *dms, char *pubdir)
{
	CgUpnpMediaContent *rootContent;

	rootContent = cg_upnp_dms_getrootcontent(dms);
	if (!rootContent)
		return;

	cg_upnp_dms_filesys_content_setpubicdirectory(rootContent, pubdir);
}

/****************************************
* cg_upnp_dms_filesys_getpublicationdirectory
****************************************/

char *cg_upnp_dms_filesys_getpublicationdirectory(CgUpnpMediaServer *dms)
{
	CgUpnpMediaContent *rootContent;

	rootContent = cg_upnp_dms_getrootcontent(dms);
	if (!rootContent)
		return NULL;

	return cg_upnp_dms_filesys_content_getpubicdirectory(rootContent);
}

/****************************************
* cg_upnp_dms_filesys_actionlistner
****************************************/

BOOL cg_upnp_dms_filesys_actionlistner(CgUpnpAction *action)
{
	CgUpnpMediaServer *dms;
	CgUpnpDevice *dev;
	char *contentID;
	CgUpnpMediaContent *content;
	char *contentDir;
	char *actionName;

	actionName = cg_upnp_action_getname(action);
	if (cg_strlen(actionName) <= 0)
		return FALSE;

	/* Browse */
	if (cg_streq(actionName, CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE)) {
		contentID = cg_upnp_action_getargumentvaluebyname(action, CG_UPNP_DMS_CONTENTDIRECTORY_BROWSE_OBJECT_ID);
		if (cg_strlen(contentID) <=0)
			return FALSE;

		dev = cg_upnp_service_getdevice(cg_upnp_action_getservice(action));
		if (!dev)
			return FALSE;

		dms = (CgUpnpMediaServer *)cg_upnp_device_getuserdata(dev);
		if (!dms)
			return FALSE;

		cg_upnp_dms_lock(dms);
		
		content = cg_upnp_dms_findcontentbyid(dms, contentID);
		if (!content) {
			cg_upnp_dms_unlock(dms);
			return FALSE;
		}

		contentDir = (char *)cg_upnp_dms_filesys_content_getpubicdirectory(content);
		if (cg_strlen(contentDir) <= 0) {
			cg_upnp_dms_unlock(dms);
			return FALSE;
		}

		cg_upnp_dms_filesys_updatecontentlist(dms, content, contentDir);
		
		cg_upnp_dms_unlock(dms);
	}

	return FALSE;
}

/****************************************
* cg_upnp_dms_filesys_new
****************************************/

CgUpnpMediaServer *cg_upnp_dms_filesys_new()
{
	CgUpnpMediaServer *dms;

	dms = cg_upnp_dms_new();

	cg_upnp_dms_setactionlistener(dms, cg_upnp_dms_filesys_actionlistner);
	cg_upnp_dms_sethttplistener(dms, cg_upnp_dms_filesys_http_listener);

	return dms;
}

/****************************************
* cg_upnp_dms_filesys_delete
****************************************/

void cg_upnp_dms_filesys_delete(CgUpnpMediaServer *dms)
{
	cg_upnp_dms_delete(dms);
}
