/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: crss.c
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#include "crss.h"
#include "cwebservice.h"

#include <cybergarage/http/chttp.h>
#include <cybergarage/xml/cxml.h>

#include <cybergarage/upnp/std/av/ccontent.h>
#include <cybergarage/upnp/std/av/cmd5.h>

/************************************************************
* cg_http_getrestresponse
************************************************************/

CgUpnpMediaContent *cg_http_getrsscontents(char *rssURL)
{
	CgString *content_str;
	char *content_string;
	CgXmlParser *xmlParser;
	CgXmlNodeList *nodeList;
	CgXmlNode *rootNode;
	CgXmlNode *channelNode;
	CgXmlNode *node;
	CgXmlNode *childNode;
	char *container_title;
	char *content_title;
	char *contentURL;
	char containerID[CG_MD5_STRING_BUF_SIZE];
	char contentID[CG_MD5_STRING_BUF_SIZE];
	long contentSize;
	int nContentent;
	char *contentMimeType;
	CgUpnpMediaContent *content;
	CgUpnpMediaContent *container;
	CgUpnpMediaResource *res;

	content_str = cg_string_new();

	if (!cg_http_getrestresponse(rssURL, content_str)) {
		cg_string_delete(content_str);
		return NULL;
	}

	content_string = cg_string_getvalue(content_str);
	if (cg_strlen(content_string) <= 0) {
		cg_string_delete(content_str);
		return NULL;
	}

	nodeList = cg_xml_nodelist_new();
	xmlParser = cg_xml_parser_new();
	if (cg_xml_parse(xmlParser, nodeList, content_string, cg_strlen(content_string)) == FALSE) {
		cg_string_delete(content_str);
		cg_xml_parser_delete(xmlParser);
		cg_xml_nodelist_delete(nodeList); 
		return NULL;
	}

	cg_string_delete(content_str);
	cg_xml_parser_delete(xmlParser);

	nContentent = 0;
	rootNode = cg_xml_nodelist_gets(nodeList);
	if (rootNode == NULL) {
		cg_xml_nodelist_delete(rootNode); 
		return NULL;
	}

	channelNode = cg_xml_node_getchildnode(rootNode, "channel");
	if (channelNode == NULL) {
		cg_xml_nodelist_delete(rootNode); 
		return NULL;
	}

	/**** container ****/

	// Title
	container_title = "";
	childNode = cg_xml_node_getchildnode(channelNode, "title");
	if (childNode) {
		if (cg_xml_node_getvalue(childNode))
			container_title = cg_xml_node_getvalue(childNode);
	}

	if (cg_strlen(container_title) <= 0) {
		cg_xml_nodelist_delete(rootNode); 
		return NULL;
	}

	container = cg_upnp_media_content_new();
	cg_upnp_media_content_settype(container, CG_UPNP_MEDIA_CONTENT_CONTAINER);
	cg_upnp_media_content_settitle(container, container_title);
	cg_str2md5(container_title, containerID);
	cg_upnp_media_content_setid(container, containerID);

	/**** item ****/
	for (node=cg_xml_node_getchildnodes(channelNode); node; node = cg_xml_node_next(node)) {
		
		if (!cg_xml_node_isname(node, "item"))
			continue;

		content_title = "";
		contentURL = "";
		contentSize = 0;

		// Title
		childNode = cg_xml_node_getchildnode(node, "title");
		if (childNode) {
			if (cg_xml_node_getvalue(childNode))
				content_title = cg_xml_node_getvalue(childNode);
		}

		// Enclosure
		childNode = cg_xml_node_getchildnode(node, "enclosure");
		if (childNode) {
			// url
			if (cg_xml_node_getattributevalue(childNode, "url"))
				contentURL = cg_xml_node_getattributevalue(childNode, "url");
			// type
			if (cg_xml_node_getattributevalue(childNode, "type"))
				contentMimeType = cg_xml_node_getattributevalue(childNode, "type");
			// type
			if (cg_xml_node_getattributevalue(childNode, "length"))
				contentSize = atol(cg_xml_node_getattributevalue(childNode, "length"));
		}

		if (cg_strlen(content_title) <= 0 || cg_strlen(contentURL) <= 0)
			continue;

		content = cg_upnp_media_content_new();
		cg_upnp_media_content_settype(content, CG_UPNP_MEDIA_CONTENT_ITEM);

		/**** content name ****/
		content_title = cg_strtrim(content_title, " ", 1);
		if (cg_strlen(content_title) <= 0) {
			continue;
		}
		cg_upnp_media_content_settitle(content, content_title);

		/**** content id ****/
		cg_str2md5(contentURL, contentID);
		cg_upnp_media_content_setid(content, contentID);
		cg_upnp_media_content_addchildcontent(container, content);

		res = cg_upnp_media_resource_new();
		cg_upnp_media_resource_setmimetype(res, contentMimeType);
		cg_upnp_media_resource_seturl(res, contentURL);
		cg_upnp_media_resource_setsize(res, contentSize);
		cg_upnp_media_content_addresource(content, res);

		nContentent++;
	}

	cg_xml_nodelist_delete(nodeList);

	return container;
}