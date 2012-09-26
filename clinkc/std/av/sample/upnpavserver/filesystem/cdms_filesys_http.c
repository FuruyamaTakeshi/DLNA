/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cdms_http.c
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#include "cdms_filesys.h"

#if !defined(WINCE)
#include <sys/stat.h> 
#endif

#define CG_FILE_READ_CHUNK_SIZE (32*1024)
#define CG_USE_CHUNKED_STREAM 1

/**********************************************************************
* cg_upnp_dms_filesys_http_listener
**********************************************************************/

void cg_upnp_dms_filesys_http_listener(CgHttpRequest *httpReq)
{
	CgUpnpMediaServer *dms;
	CgUpnpDevice *dev;
	char *httpURI;
	int contentMD5Idx;
	char *contentMd5;
	CgUpnpMediaContent *content;
	CgUpnpMediaResource *resource;
	char *pubDir;
	CgHttpResponse *httpRes;
	CgSocket *sock;
	char chunkedChar[32];
	BOOL isHeadRequest;
#if !defined(WINCE)
	struct stat fileStat;
	off_t fileSize;
#else
	fpos_t fileSize;
#endif
	FILE *fp;
	char readBuf[CG_FILE_READ_CHUNK_SIZE];
	int nRead;
#if defined(WINCE) && defined(UNICODE)
	TCHAR wCharBuf[MAX_PATH];
#endif

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
	
	if (cg_strstr(httpURI, CG_UPNP_MEDIA_FILESYS_RESURL_PATH) < 0) {
		cg_upnp_device_httprequestrecieved(httpReq);
		return;
	}

	contentMD5Idx = cg_strrchr(httpURI, "/", 1);
	
	if (contentMD5Idx < 0) {
		cg_http_request_postbadrequest(httpReq);
		return;
	}
	
	contentMd5 = httpURI + contentMD5Idx + 1;
	
	cg_upnp_dms_lock(dms);

	content = cg_upnp_dms_findcontentbyid(dms, contentMd5);
	if (content == NULL) {
		cg_upnp_dms_unlock(dms);
		cg_http_request_postbadrequest(httpReq);
		return;
	}

	pubDir = cg_upnp_dms_filesys_content_getpubicdirectory(content);
	resource = cg_upnp_media_content_getresources(content);

	if (!pubDir || !resource) {
		cg_upnp_dms_unlock(dms);
		cg_http_request_postbadrequest(httpReq);
		return;
	}

	isHeadRequest = cg_http_request_isheadrequest(httpReq);
	
	httpRes = cg_http_response_new();
#if defined(CG_USE_CHUNKED_STREAM)
	cg_http_response_setversion(httpRes, CG_HTTP_VER11);
#else
	cg_http_response_setversion(httpRes, CG_HTTP_VER10);
#endif
	cg_http_response_setstatuscode(httpRes, CG_HTTP_STATUS_OK);
	cg_http_response_setcontenttype(httpRes, cg_upnp_media_resource_getmimetype(resource));
	
	sock = cg_http_request_getsocket(httpReq);

#if defined(WINCE)
	#if defined(UNICODE)
	MultiByteToWideChar(CP_UTF8, 0, pubDir, -1, wCharBuf, (MAX_PATH-1));
	#endif
#endif

	fileSize = 0;

#if !defined(WINCE)
	if (stat(pubDir, &fileStat) == 0)
		fileSize = fileStat.st_size;
#else
	#if defined(UNICODE)
	fp = _tfopen(wCharBuf, _T("rb"));
	#else
	fp = fopen(pubDir,"rb");
	#endif
	if (fp) {
		fseek(fp, 0, SEEK_END); 
		fgetpos(fp, &fileSize); 
		fclose(fp);
	}
#endif

#if defined(CG_USE_CHUNKED_STREAM)
	cg_http_packet_setheadervalue((CgHttpPacket*)httpRes, "Transfer-Encoding", "chunked");
#else
	cg_http_response_setcontentlength(httpRes, fileSize);
#endif
	cg_http_request_postresponse(httpReq, httpRes);

	if (0 < fileSize) {		
#if defined(WINCE) && defined(UNICODE)
		fp = _tfopen(wCharBuf, _T("rb"));
#else
		fp = fopen(pubDir, "rb");
#endif
		if (fp) {
			nRead = fread(readBuf, sizeof(char), CG_FILE_READ_CHUNK_SIZE, fp);
			while (0 < nRead) {
#if defined(CG_USE_CHUNKED_STREAM)
				sprintf(chunkedChar, "%x%s", nRead, CG_HTTP_CRLF);
				cg_socket_write(sock, chunkedChar, cg_strlen(chunkedChar));
#endif
				cg_socket_write(sock, readBuf, nRead);
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

