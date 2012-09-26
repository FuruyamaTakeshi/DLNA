/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: ccontent.c
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#include <cybergarage/upnp/std/av/ccontent.h>
#include <cybergarage/util/cstring.h>

#define CG_UPNPAV_CONTENT_CONTAINER_STRING "container"
#define CG_UPNPAV_CONTENT_ITEM_STRING "item"

/****************************************
* cg_upnpav_content_settype
****************************************/

void cg_upnpav_content_settype(CgUpnpAvContent *con, int type)
{
	switch (type) {
	case CG_UPNPAV_CONTENT_CONTAINER:
		{
			cg_xml_node_setname(con, CG_UPNPAV_CONTENT_CONTAINER_STRING);
			cg_upnpav_content_setupnpclass(con, CG_UPNPAV_UPNPCLASS_CONTAINER);
		}
		break;
	case CG_UPNPAV_CONTENT_ITEM:
		{
			cg_xml_node_setname(con, CG_UPNPAV_CONTENT_ITEM_STRING);
		}
		break;
	}
}

/****************************************
* cg_upnpav_content_gettype
****************************************/

int cg_upnpav_content_gettype(CgUpnpAvContent *con)
{
	if (cg_streq(cg_xml_node_getname(con), CG_UPNPAV_CONTENT_ITEM_STRING))
		return CG_UPNPAV_CONTENT_ITEM;
	if (cg_streq(cg_xml_node_getname(con), CG_UPNPAV_CONTENT_CONTAINER_STRING))
		return CG_UPNPAV_CONTENT_CONTAINER;
	return CG_UPNPAV_CONTENT_NONE;
}

/****************************************
* cg_upnpav_content_copy
****************************************/

void cg_upnpav_content_copy(CgUpnpAvContent *destContent, CgUpnpAvContent *srcContent)
{
	CgXmlAttribute *attr;
	CgUpnpAvContent *destNode;
	CgUpnpAvContent *srcNode;

	cg_xml_node_setname(destContent, cg_xml_node_getname(srcContent));
	cg_xml_node_setvalue(destContent, cg_xml_node_getvalue(srcContent));
	for (attr=cg_xml_node_getattributes(srcContent); attr; attr=cg_xml_attribute_next(attr))
		cg_xml_node_setattribute(destContent, cg_xml_attribute_getname(attr), cg_xml_attribute_getvalue(attr));

	for (srcNode=cg_xml_node_getchildnodes(srcContent); srcNode; srcNode=cg_xml_node_next(srcNode)) {
		if (cg_upnpav_content_gettype(srcNode) != CG_UPNPAV_CONTENT_NONE)
			continue;
		if (cg_streq(cg_xml_node_getname(srcNode), CG_UPNPAV_RESOURCE_NAME)) {
			destNode = cg_upnpav_resource_new();
			cg_upnpav_resource_copy(destNode, srcNode);
		}
		else {
			destNode = cg_upnpav_content_new();
			cg_xml_node_setname(destNode, cg_xml_node_getname(srcNode));
			cg_xml_node_setvalue(destNode, cg_xml_node_getvalue(srcNode));
			for (attr=cg_xml_node_getattributes(srcNode); attr; attr=cg_xml_attribute_next(attr))
				cg_xml_node_setattribute(destNode, cg_xml_attribute_getname(attr), cg_xml_attribute_getvalue(attr));
		}
		cg_xml_node_addchildnode(destContent, destNode);
	}
}

/****************************************
* cg_upnpav_content_findbyuserfunc
****************************************/

CgUpnpAvContent *cg_upnpav_content_findbyuserfunc(CgUpnpAvContent *con, CG_UPNPAV_CONTENT_COMPARE_FUNC userFunc, void *userData)
{
	CgUpnpAvContent *childCon, *lower;

	if (userFunc(con, userData))
		return con;

	for (childCon=cg_upnpav_content_getchildcontents(con); childCon; childCon=cg_upnpav_content_next(childCon)) {
		if (userFunc(childCon, userData))
			return childCon;
        if (cg_upnpav_content_haschildcontents(childCon))
            if ((lower = cg_upnpav_content_findbyuserfunc(childCon,
                                                 userFunc,
                                                          userData)))
                return lower;
	}

	return NULL;
}

/****************************************
* cg_upnpav_content_getbyobjectid
****************************************/

static BOOL cg_upnpav_content_comparebyid(CgUpnpAvContent *con, void *userData)
{
	if (!userData)
		return FALSE;
	return cg_streq(cg_upnpav_content_getid(con), (char *)userData);
}

CgUpnpAvContent *cg_upnpav_content_getbyid(CgUpnpAvContent *con, char *id)
{
	return cg_upnpav_content_findbyuserfunc(con, cg_upnpav_content_comparebyid, id);
}

/****************************************
* cg_upnpav_content_getbytitle
****************************************/

static BOOL cg_upnpav_content_comparebytitle(CgUpnpAvContent *con, void *userData)
{
	if (!userData)
		return FALSE;
	return cg_streq(cg_upnpav_content_gettitle(con), (char *)userData);
}

CgUpnpAvContent *cg_upnpav_content_getbytitle(CgUpnpAvContent *con, char *title)
{
	return cg_upnpav_content_findbyuserfunc(con, cg_upnpav_content_comparebytitle, title);
}

/****************************************
* cg_upnpav_content_addchildcontent
****************************************/

void cg_upnpav_content_addchildcontent(CgUpnpAvContent *con, CgUpnpAvContent *childCon)
{
	cg_xml_node_addchildnode(con, childCon);
	cg_upnpav_content_setparentid(childCon, cg_upnpav_content_getid(con));
}

/****************************************
* cg_upnpav_content_istitlenode
****************************************/

BOOL cg_upnpav_content_iscontentnode(CgXmlNode *node)
{
	if (cg_upnpav_content_isitemnode(node))
		return TRUE;
	if (cg_upnpav_content_iscontainernode(node))
		return TRUE;
	return FALSE;
}

/****************************************
* cg_upnpav_content_getnchildcontents
****************************************/

int cg_upnpav_content_getnchildcontents(CgUpnpAvContent *con)
{
	CgXmlNode *childNode;
	int contentNodeCnt;
	contentNodeCnt = 0;
	for (childNode = cg_xml_node_getchildnodes(con); childNode; childNode=cg_xml_node_next(childNode)) {
		if (cg_upnpav_content_iscontentnode(childNode))
			contentNodeCnt++;
	}
	return contentNodeCnt;
}
/****************************************
* cg_upnpav_content_getchildcontents
****************************************/

CgUpnpAvContent *cg_upnpav_content_getchildcontents(CgUpnpAvContent *con)
{
	CgXmlNode *childNode;
	for (childNode = cg_xml_node_getchildnodes(con); childNode; childNode=cg_xml_node_next(childNode)) {
		if (cg_upnpav_content_iscontentnode(childNode))
			return childNode;
	}
	return NULL;
}

/****************************************
* cg_upnpav_content_istitlenode
****************************************/

BOOL cg_upnpav_content_haschildcontents(CgUpnpAvContent *con)
{
	CgXmlNode *childNode;
	for (childNode = cg_xml_node_getchildnodes(con); childNode; childNode=cg_xml_node_next(childNode)) {
		if (cg_upnpav_content_iscontentnode(childNode))
			return TRUE;
	}
	return FALSE;
}

/****************************************
* cg_upnpav_content_next
****************************************/

CgUpnpAvContent *cg_upnpav_content_next(CgUpnpAvContent *con)
{
	CgXmlNode *nextNode;
	nextNode = cg_xml_node_next(con);
	while (nextNode) {
		if (cg_upnpav_content_iscontentnode(nextNode))
			return nextNode;
		nextNode = cg_xml_node_next(con);
	}
	return NULL;
}
