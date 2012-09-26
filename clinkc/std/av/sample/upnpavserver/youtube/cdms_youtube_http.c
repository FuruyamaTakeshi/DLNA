/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cdms_youtube_http.c
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#include "cdms_youtube.h"

#include <sys/stat.h> 

#define CG_FILE_READ_CHUNK_SIZE (32*1024)
#define CG_USE_CHUNKED_STREAM 1
#define CG_UPNP_DMS_SOCKET_SEND_RETRY_CNT 10

/**********************************************************************
* cg_upnp_dms_youtube_http_listener
**********************************************************************/

void cg_upnp_dms_youtube_http_listener(CgHttpRequest *httpReq)
{
	CgUpnpMediaServer *dms;
	CgUpnpDevice *dev;
	char *httpURI;
	int contentMD5Idx;
	char *contentMd5;
	CgHttpResponse *httpRes;
	CgSocket *sock;
	char chunkedChar[32];
	BOOL isHeadRequest;
	struct stat fileStat;
	off_t fileSize;
	FILE *fp;
	char readBuf[CG_FILE_READ_CHUNK_SIZE];
	off_t nRead;
	off_t nReadCnt;
	off_t nWroteCnt;
	char contentFile[CG_MD5_STRING_BUF_SIZE+8];

	dev = (CgUpnpDevice *)cg_http_request_getuserdata(httpReq);
	if (!dev) {
		cg_http_request_postbadrequest(httpReq);
		return;
	}

	dms = (CgUpnpMediaServer *)cg_upnp_device_getuserdata(dev);
	if (!dms) {
		cg_http_request_postbadrequest(httpReq);
		return;
	}

	httpURI = cg_http_request_geturi(httpReq);
	if (cg_strlen(httpURI) <= 0) {
		cg_http_request_postbadrequest(httpReq);
		return;
	}

	if (cg_strstr(httpURI, CG_UPNP_MEDIA_YOUTUBE_RESURL_PATH) < 0) {
		cg_upnp_device_httprequestrecieved(httpReq);
		return;
	}

	contentMD5Idx = cg_strrchr(httpURI, "/", 1);
	
	if (contentMD5Idx < 0) {
		cg_http_request_postbadrequest(httpReq);
		return;
	}
	
	contentMd5 = httpURI + contentMD5Idx + 1;
	cg_strcpy(contentFile, contentMd5);
	cg_strcat(contentFile, "." CG_UPNP_MEDIA_YOUTUBE_TRANSCODE_FILEEXT);

	cg_upnp_dms_lock(dms);

	isHeadRequest = cg_http_request_isheadrequest(httpReq);
	
	httpRes = cg_http_response_new();
#if defined(CG_USE_CHUNKED_STREAM)
	cg_http_response_setversion(httpRes, CG_HTTP_VER11);
#else
	cg_http_response_setversion(httpRes, CG_HTTP_VER10);
#endif
	cg_http_response_setstatuscode(httpRes, CG_HTTP_STATUS_OK);
	cg_http_response_setcontenttype(httpRes, CG_UPNP_MEDIA_YOUTUBE_CONTENT_MIMETYPE);
	
	sock = cg_http_request_getsocket(httpReq);
	cg_socket_settimeout(sock, 0);

	fileSize = 0;
	if (stat(contentFile, &fileStat) == 0)
		fileSize = fileStat.st_size;
#if defined(CG_USE_CHUNKED_STREAM)
	cg_http_packet_setheadervalue((CgHttpPacket*)httpRes, "Transfer-Encoding", "chunked");
#else
	cg_http_response_setcontentlength(httpRes, fileSize);
#endif
	cg_http_request_postresponse(httpReq, httpRes);

	if (0 < fileSize) {		
		nReadCnt = 0;
		nWroteCnt = 0;
		fp = fopen(contentFile, "rb");
		if (fp) {
			nRead = fread(readBuf, sizeof(char), CG_FILE_READ_CHUNK_SIZE, fp);
			while (nReadCnt < fileSize && 0 < nRead) {
				nReadCnt += nRead;
#if defined(CG_USE_CHUNKED_STREAM)
				sprintf(chunkedChar, "%x%s", nRead, CG_HTTP_CRLF);
				cg_socket_write(sock, chunkedChar, cg_strlen(chunkedChar));
#endif
				nWroteCnt += cg_socket_write(sock, readBuf, nRead);
#if defined(CG_USE_CHUNKED_STREAM)
				cg_socket_write(sock, CG_HTTP_CRLF, sizeof(CG_HTTP_CRLF)-1);
#endif
				nRead = fread(readBuf, sizeof(char), CG_FILE_READ_CHUNK_SIZE, fp);
			}
			fclose(fp);
		}
	}
	
#if defined(CG_USE_CHUNKED_STREAM)
	sprintf(chunkedChar, "%x%s", 0, CG_HTTP_CRLF);
	cg_socket_write(sock, chunkedChar, cg_strlen(chunkedChar));
#endif
	
	cg_socket_close(sock);
	cg_http_response_delete(httpRes);

	cg_upnp_dms_unlock(dms);
}

