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

#include <sys/stat.h> 

#include "cdms_youtube.h"
#include "crss.h"
#include "cwebservice.h"

/****************************************
* cg_upnp_dms_youtube_rssurl2flv
****************************************/

#define CG_UPNP_MEDIA_YOUTUBE_PARAM_VIDEOID "video_id="
#define CG_UPNP_MEDIA_YOUTUBE_PARAM_T "t="
#define CG_UPNP_MEDIA_YOUTUBE_PARAM_MAX_LEN 512
#define CG_UPNP_MEDIA_YOUTUBE_GETVIDEO_URL "http://youtube.com/get_video.php"

BOOL cg_upnp_dms_youtube_rssurl2flv(char *rssURL, char *flvURL, int flvURLSize)
{
	char videoidParam[CG_UPNP_MEDIA_YOUTUBE_PARAM_MAX_LEN];
	char tParam[CG_UPNP_MEDIA_YOUTUBE_PARAM_MAX_LEN];
	int idx;
	int n;
	int offset;
	char c;
	char getVideoURL[CG_UPNP_MEDIA_YOUTUBE_RESURL_MAXLEN];

	if (cg_http_getredirecturl(rssURL, flvURL, flvURLSize) == FALSE)
		return FALSE;

	/***** video_id ****/
	idx = cg_strstr(flvURL, CG_UPNP_MEDIA_YOUTUBE_PARAM_VIDEOID);
	if (idx < 0)
		return FALSE;
	n = 0;
	offset = cg_strlen(CG_UPNP_MEDIA_YOUTUBE_PARAM_VIDEOID);
	c = flvURL[idx + offset + n];
	while (c != '\0' && c !='&') {
		videoidParam[n] = c;
		n++;
		c = flvURL[idx + offset + n];
	}
	videoidParam[n] = '\0';

	/***** t ****/
	idx = cg_strstr(flvURL, CG_UPNP_MEDIA_YOUTUBE_PARAM_T);
	if (idx < 0)
		return FALSE;
	n = 0;
	offset = cg_strlen(CG_UPNP_MEDIA_YOUTUBE_PARAM_T);
	c = flvURL[idx + offset + n];
	while (c != '\0' && c !='&') {
		tParam[n] = c;
		n++;
		c = flvURL[idx + offset + n];
	}
	tParam[n] = '\0';

#if !defined(WIN32)
	snprintf(
#else
	_snprintf(
#endif
		getVideoURL, sizeof(getVideoURL), "%s?%s%s&%s%s",
		CG_UPNP_MEDIA_YOUTUBE_GETVIDEO_URL,
		CG_UPNP_MEDIA_YOUTUBE_PARAM_VIDEOID,
		videoidParam,
		CG_UPNP_MEDIA_YOUTUBE_PARAM_T,
		tParam
	);

	if (cg_http_getredirecturl(getVideoURL, flvURL, flvURLSize) == FALSE)
		return FALSE;

	return TRUE;
}

/****************************************
* cg_upnp_dms_youtube_transcode2mpeg2
****************************************/

#define CG_UPNP_MEDIA_YOUTUBE_FFMPEG_CMD_MAX_LEN 512

#if defined(WIN32)
#define FFMPEG_CMD "ffmpeg.exe -y -i %s -vcodec mpeg1video -acodec mp3 -ar 44100 -ab 128 %s.%s"
#else
#define FFMPEG_CMD "ffmpeg -y -i %s -vcodec mpeg1video -acodec mp3 -ar 44100 -ab 128 %s.%s"
#endif

BOOL cg_upnp_dms_youtube_fetchflv(char *objectID, char *flvURL, char *flvFileName)
{
	CgNetURL *netUrl;
	CgString *postUri;
	CgHttpRequest *httpReq;
	CgHttpResponse *httpRes;
	int statusCode;
	FILE *fp;
	int flvSize;
	char *flvBuf;
	int wroteBytes;

	/**** Fetch Flash File ****/
	netUrl = cg_net_url_new();
	cg_net_url_set(netUrl, flvURL);
	httpReq = cg_http_request_new();
	cg_http_request_setmethod(httpReq, CG_HTTP_GET);
	postUri = cg_string_new();
	cg_string_setvalue(postUri, cg_net_url_getpath(netUrl));
	if (cg_net_url_getquery(netUrl) != NULL) {
		cg_string_addvalue(postUri, "?");
		cg_string_addvalue(postUri, cg_net_url_getquery(netUrl));
	}
	cg_http_request_seturi(httpReq, cg_string_getvalue(postUri));
	cg_http_request_setcontentlength(httpReq, 0);
	cg_http_request_settimeout(httpReq, 0);
	httpRes = cg_http_request_post(httpReq, cg_net_url_gethost(netUrl), cg_net_url_getport(netUrl));
	cg_string_delete(postUri);
	cg_net_url_delete(netUrl);

	/**** Check Response ****/
	statusCode = cg_http_response_getstatuscode(httpRes);
	flvBuf = cg_http_response_getcontent(httpRes);
	flvSize = (int)cg_http_response_getcontentlength(httpRes);
	if (statusCode != 200 || flvSize <= 0) {
		cg_http_request_delete(httpReq);
		return FALSE;
	}

	/**** Store FLV File ****/
	fp = fopen(flvFileName, "wb");
	if (!fp) {
		cg_http_request_delete(httpReq);
		return FALSE;
	}
	wroteBytes = 0;
	while (wroteBytes < flvSize)
		wroteBytes += (int)fwrite(flvBuf, sizeof(char), (flvSize-wroteBytes), fp);
	fclose(fp);

	cg_http_request_delete(httpReq);

	return TRUE;
}

BOOL cg_upnp_dms_youtube_transcode2mpeg2(char *objectID, char *flvFileName)
{
	char ffmpegCmd[CG_UPNP_MEDIA_YOUTUBE_FFMPEG_CMD_MAX_LEN];
#if defined(WIN32)
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	#if defined(UNICODE)
	TCHAR wCharBuf[MAX_PATH];
	#endif
#endif
	int cmdStat;
	struct stat fileStat;
	off_t fileSize;
	char transFileName[CG_MD5_STRING_BUF_SIZE+8];

	/**** FLV -> MPEG ****/
#if !defined(WIN32)
	snprintf(
#else
	_snprintf(
#endif
		ffmpegCmd,
		sizeof(ffmpegCmd),
		FFMPEG_CMD,
		flvFileName,
		objectID,
		CG_UPNP_MEDIA_YOUTUBE_TRANSCODE_FILEEXT);

#if defined(WIN32)
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	cmdStat = 0xff;
	#if defined(UNICODE)
	MultiByteToWideChar(CP_UTF8, 0, ffmpegCmd, -1, wCharBuf, (MAX_PATH-1));
	if (CreateProcess(	NULL,	wCharBuf, NULL,	NULL,	FALSE,	 0, NULL,  NULL, &si, &pi)) {
	#else
	if (CreateProcess(	NULL,	ffmpegCmd, NULL,	NULL,	FALSE,	 0, NULL,  NULL, &si, &pi)) {
	#endif
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		cmdStat = 0;
	}
#else
	cmdStat = system(ffmpegCmd);
#endif
	remove(flvFileName);

	cg_strcpy(transFileName, objectID);
	cg_strcat(transFileName, "." CG_UPNP_MEDIA_YOUTUBE_TRANSCODE_FILEEXT);

	fileSize = 0;
	if (stat(transFileName, &fileStat) == 0)
		fileSize = fileStat.st_size;

	if (cmdStat != 0 || fileSize <= 0) {
		remove(transFileName);
		return FALSE;
	}

	return TRUE;
}

/****************************************
* cg_upnp_dms_youtube_updaterootcontentlist
****************************************/

char *cg_upnp_dms_youtube_getupdatemessage(char *action, char *title, int cnt, int size, char *msgBuf, int msgBufSize)
{
#if !defined(WIN32)
	snprintf(
#else
	_snprintf(
#endif
		msgBuf, msgBufSize, "%s : %s (%d/%d)",
			action,
			title,
			cnt,
			size
		);

	return msgBuf;
}

void cg_upnp_dms_youtube_updaterootcontentlist(CgUpnpMediaServer *dms)
{
	char *rssURL;
	CgUpnpMediaContent *rootCon;
	CgUpnpMediaContent *titleCon;
	CgUpnpMediaContent *copyTitleCon;
	CgUpnpMediaContent *copyCon;
	CgUpnpMediaContent *con;
	CgUpnpMediaResource *res;
	CgNetworkInterfaceList *netIfList;
	CgNetworkInterfaceList *netIf;
	char *objectID;
	char flvURL[CG_UPNP_MEDIA_YOUTUBE_RESURL_MAXLEN];
	char resURL[CG_UPNP_MEDIA_YOUTUBE_RESURL_MAXLEN];
	int conSize;
	int conCnt;
	CG_UPNP_MEDIA_YOUTUBE_UPDATE_LISTENER updateListener;
	void *updateListenerData;
	char msgBuf[CG_UPNP_MEDIA_YOUTUBE_RESURL_MAXLEN];
	char flvFileName[CG_MD5_STRING_BUF_SIZE+8];
	char dlnaAttr[CG_UPNP_MEDIA_DLNAPN_MAXLEN];

	/**** Update URL ****/
	rssURL = cg_upnp_dms_youtube_getrssurl(dms);
	if (cg_strlen(rssURL) <= 0)
		return;

	/**** Clear Root Contents ****/
	cg_upnp_dms_lock(dms);
	rootCon = cg_upnp_dms_getrootcontent(dms);
	cg_upnp_media_content_clearchildcontents(rootCon);
	cg_upnp_dms_unlock(dms);

	titleCon = cg_http_getrsscontents(rssURL);
	if (!titleCon) 
		return;

	/**** RSS Title ****/
	copyTitleCon = cg_upnp_media_content_new();
	cg_upnp_media_content_copy(copyTitleCon, titleCon);
	cg_upnp_dms_lock(dms);
	cg_upnp_media_content_addchildcontent(rootCon, copyTitleCon);
	cg_upnp_dms_unlock(dms);

	netIfList = cg_net_interfacelist_new();
	cg_net_gethostinterfaces(netIfList);
	netIf = cg_net_interfacelist_gets(netIfList);
	if (!netIf) {
		cg_net_interfacelist_delete(netIfList);
		return;
	}

	/**** RSS Contents ****/
	conSize = cg_upnp_media_content_getnchildcontents(titleCon);
	conCnt = 0;
	updateListener = cg_upnp_dms_youtube_getupdatelistener(dms);
	updateListenerData = cg_upnp_dms_youtube_getupdatelistenerdata(dms);
	for (con=cg_upnp_media_content_getchildcontents(titleCon); con; con = cg_upnp_media_content_next(con)) {
		conCnt++;

		if (updateListener)
			updateListener(conSize, (conCnt-1), "", updateListenerData);

		objectID = cg_upnp_media_content_getid(con);
		if (cg_strlen(objectID) <= 0)
			continue;
		res = cg_upnp_media_content_getresources(con);
		if (!res)
			continue;

		if (updateListener)
			updateListener(conSize, (conCnt-1), cg_upnp_dms_youtube_getupdatemessage("Searching Redirect URL", cg_upnp_media_content_gettitle(con), conCnt, conSize, msgBuf, sizeof(msgBuf)), updateListenerData);
		if (!cg_upnp_dms_youtube_rssurl2flv(cg_upnp_media_resource_geturl(res), flvURL, sizeof(flvURL)))
			continue;

		cg_strcpy(flvFileName, objectID);
		cg_strcat(flvFileName, "." CG_UPNP_MEDIA_YOUTUBE_FLV_FILEEXT);

		if (updateListener)
			updateListener(conSize, (conCnt-1), cg_upnp_dms_youtube_getupdatemessage("Fetching", cg_upnp_media_content_gettitle(con), conCnt, conSize, msgBuf, sizeof(msgBuf)), updateListenerData);
		if (!cg_upnp_dms_youtube_fetchflv(objectID, flvURL, flvFileName))
			continue;

		if (updateListener)
			updateListener(conSize, (conCnt-1), cg_upnp_dms_youtube_getupdatemessage("Transcording", cg_upnp_media_content_gettitle(con), conCnt, conSize, msgBuf, sizeof(msgBuf)), updateListenerData);
		if (!cg_upnp_dms_youtube_transcode2mpeg2(objectID, flvFileName))
			continue;

		/**** New Content ****/
		copyCon = cg_upnp_media_content_new();
		cg_upnp_media_content_copy(copyCon, con);
		cg_upnp_media_content_setupnpclass(copyCon, CG_UPNP_MEDIA_UPNPCLASS_MOVIE);

		/**** Update Resource ****/
		res = cg_upnp_media_content_getresources(copyCon);
		if (!res) {
			cg_upnp_media_content_delete(copyCon);
			continue;
		}
#ifdef WIN32
		sprintf_s(
#else
		snprintf(
#endif
			resURL, 
			sizeof(resURL),
			"http://%s:%d/%s/%s",
			cg_net_interface_getaddress(netIf),
			cg_upnp_device_gethttpport(cg_upnp_dms_getdevice(dms)),
			CG_UPNP_MEDIA_YOUTUBE_RESURL_PATH,
			objectID
		);
		cg_upnp_media_resource_seturl(res, resURL);
		cg_upnp_media_resource_setmimetype(res, CG_UPNP_MEDIA_YOUTUBE_CONTENT_MIMETYPE);
		cg_upnp_media_resource_setdlnaattribute(res, cg_upnp_media_resource_getdlnapnfrommimetype(res, dlnaAttr, sizeof(dlnaAttr)));

		cg_upnp_dms_lock(dms);
		cg_upnp_media_content_addchildcontent(copyTitleCon, copyCon);		
		cg_upnp_dms_condir_updatesystemupdateid(dms);
		cg_upnp_dms_unlock(dms);

		if (updateListener)
			updateListener(conSize, (conCnt-1), cg_upnp_dms_youtube_getupdatemessage("Done", cg_upnp_media_content_gettitle(con), conCnt, conSize, msgBuf, sizeof(msgBuf)), updateListenerData);
	}

	if (updateListener)
		updateListener(conSize, conSize, "Done", updateListenerData);

	cg_upnp_media_content_delete(titleCon);
	cg_net_interfacelist_delete(netIfList);
}

/****************************************
* cg_upnp_dms_youtube_update
****************************************/

void cg_upnp_dms_youtube_update(CgUpnpMediaServer *dms)
{
	cg_upnp_dms_youtube_updaterootcontentlist(dms);
}
