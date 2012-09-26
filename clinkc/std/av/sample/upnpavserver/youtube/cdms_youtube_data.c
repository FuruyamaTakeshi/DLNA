/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cdms_youtube_data.c
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#include <cybergarage/upnp/std/av/cmediaserver.h>
#include <cybergarage/net/cinterface.h>

#include "cdms_youtube.h"

/****************************************
* cg_upnp_dms_youtube_data_new
****************************************/

CgUpnpDmsYoutubeData *cg_upnp_dms_youtube_data_new()
{
	CgUpnpDmsYoutubeData *data;

	data = (CgUpnpDmsYoutubeData *)malloc(sizeof(CgUpnpDmsYoutubeData));
	data->rssURL = cg_string_new();

	return data;
}

/****************************************
* cg_upnp_dms_youtube_data_delete
****************************************/

void cg_upnp_dms_youtube_data_delete(CgUpnpDmsYoutubeData *data)
{
	if (!data)
		return;

	if (data->rssURL)
		cg_string_delete(data->rssURL);

	free(data);
}

/****************************************
* cg_upnp_dms_youtube_data_setrssurl
****************************************/

void cg_upnp_dms_youtube_data_setrssurl(CgUpnpDmsYoutubeData *data, char *val)
{
	cg_string_setvalue(data->rssURL, val);
}

/****************************************
* cg_upnp_dms_youtube_data_getrssurl
****************************************/

char *cg_upnp_dms_youtube_data_getrssurl(CgUpnpDmsYoutubeData *data)
{
	return cg_string_getvalue(data->rssURL);
}

/****************************************
* cg_upnp_dms_youtube_data_setupdatelistener
****************************************/

void cg_upnp_dms_youtube_data_setupdatelistener(CgUpnpDmsYoutubeData *data, CG_UPNP_MEDIA_YOUTUBE_UPDATE_LISTENER func)
{
	data->updateListener = func;
}

/****************************************
* cg_upnp_dms_youtube_data_getupdatelistener
****************************************/

CG_UPNP_MEDIA_YOUTUBE_UPDATE_LISTENER cg_upnp_dms_youtube_data_getupdatelistener(CgUpnpDmsYoutubeData *data)
{
	return data->updateListener;
}

/****************************************
* cg_upnp_dms_youtube_data_setupdatelistenerdata
****************************************/

void cg_upnp_dms_youtube_data_setupdatelistenerdata(CgUpnpDmsYoutubeData *data, void *val)
{
	data->updateListenerData = val;
}

/****************************************
* cg_upnp_dms_youtube_data_getupdatelistenerdata
****************************************/

void *cg_upnp_dms_youtube_data_getupdatelistenerdata(CgUpnpDmsYoutubeData *data)
{
	return data->updateListenerData;
}
