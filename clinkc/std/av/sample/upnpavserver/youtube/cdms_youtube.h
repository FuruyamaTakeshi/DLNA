/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cdms_youtube.h
*
*	Revision:
*
*	01/25/05
*		- first revision
*
******************************************************************/

#ifndef _CG_UPNPAV_YOUTUBE_H_
#define _CG_UPNPAV_YOUTUBE_H_

#include <cybergarage/upnp/std/av/cmediaserver.h>

#ifdef  __cplusplus
extern "C" {
#endif

typedef void (*CG_UPNP_MEDIA_YOUTUBE_UPDATE_LISTENER)(int size, int cnt, char *msg, void *userData);

typedef struct _CgUpnpDmsYoutubeData {
	CgString *rssURL;
	CG_UPNP_MEDIA_YOUTUBE_UPDATE_LISTENER updateListener;
	void *updateListenerData;
} CgUpnpDmsYoutubeData;

#define CG_UPNP_MEDIA_YOUTUBE_APP_NAME "Cyber Media Gate for Youtube"
#define CG_UPNP_MEDIA_YOUTUBE_APP_VER "v1.1"

#define CG_UPNP_MEDIA_YOUTUBE_RSS_URL_RECENTLYADDED "http://youtube.com/rss/global/recently_added.rss"
#define CG_UPNP_MEDIA_YOUTUBE_RSS_URL_RECENTLYFRATURED "http://youtube.com/rss/global/recently_featured.rss"
#define CG_UPNP_MEDIA_YOUTUBE_RSS_URL_TOPFAVORITES "http://youtube.com/rss/global/top_favorites.rss"
#define CG_UPNP_MEDIA_YOUTUBE_RSS_URL_TOPRATED "http://youtube.com/rss/global/top_rated.rss"
#define CG_UPNP_MEDIA_YOUTUBE_RSS_URL_MOSTVIEWEDVIDEOS_TODAY "http://youtube.com/rss/global/top_viewed_today.rss"
#define CG_UPNP_MEDIA_YOUTUBE_RSS_URL_MOSTVIEWEDVIDEOS_WEEK "http://youtube.com/rss/global/top_viewed_week.rss"
#define CG_UPNP_MEDIA_YOUTUBE_RSS_URL_MOSTVIEWEDVIDEOS_MONTH "http://youtube.com/rss/global/top_viewed_month.rss"
#define CG_UPNP_MEDIA_YOUTUBE_RSS_URL_MOSTVIEWEDVIDEOS_ALLTIME "http://youtube.com/rss/global/top_viewed.rss"

#define CG_UPNP_MEDIA_YOUTUBE_FLV_FILEEXT "flv"
#define CG_UPNP_MEDIA_YOUTUBE_TRANSCODE_FILEEXT "mpeg"

#define CG_UPNP_MEDIA_YOUTUBE_RESURL_MAXLEN 1024
#define CG_UPNP_MEDIA_YOUTUBE_RESURL_PATH "content"
#define CG_UPNP_MEDIA_YOUTUBE_CONTENT_MIMETYPE CG_UPNP_MEDIA_MIMETYPE_MPEG

CgUpnpMediaServer *cg_upnp_dms_youtube_new();
void cg_upnp_dms_youtube_delete(CgUpnpMediaServer *dms);

#define cg_upnp_dms_youtube_start(dms) cg_upnp_dms_start(dms)
#define cg_upnp_dms_youtube_stop(dms) cg_upnp_dms_stop(dms)

void cg_upnp_dms_youtube_http_listener(CgHttpRequest *httpReq);

void cg_upnp_dms_youtube_update(CgUpnpMediaServer *dms);

CgUpnpDmsYoutubeData *cg_upnp_dms_youtube_data_new();
void cg_upnp_dms_youtube_data_delete(CgUpnpDmsYoutubeData *data);
void cg_upnp_dms_youtube_data_setrssurl(CgUpnpDmsYoutubeData *data, char *val);
char *cg_upnp_dms_youtube_data_getrssurl(CgUpnpDmsYoutubeData *data);
void cg_upnp_dms_youtube_data_setupdatelistener(CgUpnpDmsYoutubeData *data, CG_UPNP_MEDIA_YOUTUBE_UPDATE_LISTENER func);
CG_UPNP_MEDIA_YOUTUBE_UPDATE_LISTENER cg_upnp_dms_youtube_data_getupdatelistener(CgUpnpDmsYoutubeData *data);
void cg_upnp_dms_youtube_data_setupdatelistenerdata(CgUpnpDmsYoutubeData *data, void *val);
void *cg_upnp_dms_youtube_data_getupdatelistenerdata(CgUpnpDmsYoutubeData *data);

#define cg_upnp_dms_youtube_setrssurl(dms, val) cg_upnp_dms_youtube_data_setrssurl((CgUpnpDmsYoutubeData *)cg_upnp_dms_getuserdata(dms), val)
#define cg_upnp_dms_youtube_getrssurl(dms) cg_upnp_dms_youtube_data_getrssurl((CgUpnpDmsYoutubeData *)cg_upnp_dms_getuserdata(dms))
#define cg_upnp_dms_youtube_setupdatelistener(dms, func) cg_upnp_dms_youtube_data_setupdatelistener((CgUpnpDmsYoutubeData *)cg_upnp_dms_getuserdata(dms), func)
#define cg_upnp_dms_youtube_getupdatelistener(dms) cg_upnp_dms_youtube_data_getupdatelistener((CgUpnpDmsYoutubeData *)cg_upnp_dms_getuserdata(dms))
#define cg_upnp_dms_youtube_setupdatelistenerdata(dms, val) cg_upnp_dms_youtube_data_setupdatelistenerdata((CgUpnpDmsYoutubeData *)cg_upnp_dms_getuserdata(dms), val)
#define cg_upnp_dms_youtube_getupdatelistenerdata(dms) cg_upnp_dms_youtube_data_getupdatelistenerdata((CgUpnpDmsYoutubeData *)cg_upnp_dms_getuserdata(dms))

#ifdef  __cplusplus
}
#endif

#endif
