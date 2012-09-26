/******************************************************************
*
*      CyberNet for C
*
*      Copyright (C) Nokia Corporation 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*      File: chttp_persistent_connection.c
*
*      Revision:
*
*      12/02/05
*              - first revision
*
******************************************************************/

#include <cybergarage/util/clist.h>
#include <cybergarage/util/ctime.h>
#include <cybergarage/util/cmutex.h>
#include <cybergarage/http/chttp.h>
#include <cybergarage/net/csocket.h>
#include <cybergarage/util/clog.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef CG_HTTP_CURL
#include <curl/curl.h>
#endif

#define CG_HTTP_PERSISTENT_CACHE_SIZE  5
#define CG_HTTP_PERSISTENT_TIMEOUT_PERIOD 60


typedef struct _CgHttpPersistentConnection {
       BOOL headFlag;
       struct _CgHttpPersistentConnection *prev;
       struct _CgHttpPersistentConnection *next;

       CgString *host;
       int port;
       void *cacheData;

       CgSysTime timestamp;

} CgHttpPersistentConnection, CgHttpPersistentConnectionList;

static CgHttpPersistentConnectionList *cache = NULL;
static CgMutex *persistent_connection_mutex = NULL;

/****************************************
* cg_http_persistentconnection_init
****************************************/

BOOL cg_http_persistentconnection_init(void)
{
	cg_log_debug_l4("Entering...\n");

       if (cache == NULL)
       {
               persistent_connection_mutex = cg_mutex_new();

               if (persistent_connection_mutex == NULL)
               {
                       return FALSE;
               }

               cg_http_persistentconnection_lock();
               cache = (CgHttpPersistentConnectionList *)malloc(sizeof(CgHttpPersistentConnectionList));

               if (cache == NULL)
               {
                       if (persistent_connection_mutex != NULL)
                       {
                               cg_http_persistentconnection_unlock();
                               cg_mutex_delete(persistent_connection_mutex);
                               persistent_connection_mutex = NULL;
                       }
                       return FALSE;
               }

               cg_list_header_init((CgList *)cache);

               cache->host = NULL;
               cache->port = 0;
               cache->cacheData = NULL;

               cache->timestamp = 0;

               cg_http_persistentconnection_unlock();
       }

       return TRUE;

	cg_log_debug_l4("Leaving...\n");
}


/****************************************
* cg_http_persistentconnection_new
****************************************/

CgHttpPersistentConnection *cg_http_persistentconnection_new(void)
{
       CgHttpPersistentConnection *new_node;
       
       cg_log_debug_l4("Entering...\n");

       new_node = (CgHttpPersistentConnection *)malloc(sizeof(CgHttpPersistentConnection));

       if ( NULL != new_node )
       {
	       new_node->headFlag = FALSE;
	       new_node->next = NULL;
	       new_node->prev = NULL;

	       new_node->host = cg_string_new();
	       new_node->port = 0;
	       new_node->cacheData = NULL;

	       new_node->timestamp = 0;
       }

       return new_node;

	cg_log_debug_l4("Leaving...\n");
}


/****************************************
* cg_http_persistentconnection_delete
****************************************/

void cg_http_persistentconnection_delete(CgHttpPersistentConnection *node)
{
	cg_log_debug_l4("Entering...\n");

       cg_string_delete(node->host);

       /* Terminate and delete connection according libcurl usage */
#if !defined(CG_HTTP_CURL)
       cg_socket_close((CgSocket *)node->cacheData);
       cg_socket_delete((CgSocket *)node->cacheData);
#else
       curl_easy_cleanup((CURL *)node->cacheData);
#endif
       free(node);

	cg_log_debug_l4("Leaving...\n");
}


/****************************************
* cg_http_persistentconnection_get
****************************************/

void *cg_http_persistentconnection_get(char *host, int port)
{
       CgHttpPersistentConnection *node;
       CgSysTime sys_time = cg_getcurrentsystemtime();
       BOOL iterate;

cg_log_debug_l4("Entering...\n");

       /* If we dont have cache, then just exit */
       if (cache == NULL) { 
	       cg_log_debug("(get) No cache! Persistent connections not initialized?\n");
	       return NULL;
	}

       /* Clear all expired nodes */
       do {
               iterate = FALSE;
               for (node = (CgHttpPersistentConnection*)cg_list_gets((CgList*)cache);
                    node != NULL;
                    node = (CgHttpPersistentConnection*)cg_list_next((CgList*)node))
               {
                       if (sys_time > node->timestamp + CG_HTTP_PERSISTENT_TIMEOUT_PERIOD)
                       {
			      cg_log_debug_s("Timeout for persistent HTTP Connection to %s:%d "
				       "(timestamp: %d)\n",
				      cg_string_getvalue(node->host), node->port,
				      node->timestamp);
                               cg_list_remove((CgList*)node);
                               cg_http_persistentconnection_delete(node);
                               iterate = TRUE;
                               break;
                       }
               }
       } while (iterate);

       /* Get persistent node */
       for (node = (CgHttpPersistentConnection*)cg_list_gets((CgList*)cache);
            node != NULL;
            node = (CgHttpPersistentConnection*)cg_list_next((CgList*)node))
       {
               if (cg_strcmp(cg_string_getvalue(node->host), host) == 0 &&
                   node->port == port)
               {
                       /* Node was required, remove and add again to refresh
                          cache */
                       cg_list_remove((CgList*)node);
                       cg_list_add((CgList*)cache, (CgList*)node);

                       node->timestamp = cg_getcurrentsystemtime();

		      cg_log_debug_s("Persistent HTTP Connection cache HIT for %s:%d\n",
			       host, port);

                       return node->cacheData;
               }
       }

      cg_log_debug_s("Persistent HTTP Connection cache MISS for %s:%d\n",
	      host, port);

       return NULL;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_persistentconnection_put
****************************************/

BOOL cg_http_persistentconnection_put(char *host, int port, void *data)
{
       CgHttpPersistentConnection *new_node = NULL, *node = NULL;

	cg_log_debug_l4("Entering...\n");

       /* If we dont have cache, then just exit */
       if (cache == NULL) {
	       cg_log_debug("(put) No cache! Persistent connections not initialized?\n");
	       return FALSE;
       }

       /* Check if we already have this one cached */
       for (node = (CgHttpPersistentConnection*)cg_list_gets((CgList*)cache);
            node != NULL;
            node = (CgHttpPersistentConnection*)cg_list_next((CgList*)node))
       {
               if (cg_strcmp(cg_string_getvalue(node->host), host) == 0 &&
                   node->port == port)
               {
                       /* If also data is the same, then update just
                          timestamp */
                       if (node->cacheData == data)
                       {
                               node->timestamp = cg_getcurrentsystemtime();
                               return TRUE;
                       }

		      cg_log_debug_s("Found cached persistent connection for %s:%d\n",
			      cg_string_getvalue(node->host), node->port);
                       new_node = node;
                       cg_list_remove((CgList*)new_node);
                       break;
               }
       }

       /* We didn't find it */
       if (new_node == NULL)
       {
               /* Check if we have already too many cached things */
               if (cg_list_size((CgList*)cache) >= CG_HTTP_PERSISTENT_CACHE_SIZE)
               {
                       /* Take last node (not refreshed for a long time) */
                       new_node = (CgHttpPersistentConnection *)cg_list_next((CgList *)cache);
                       cg_list_remove((CgList*)new_node);
                       cg_http_persistentconnection_delete(new_node);
                       new_node = NULL;

		      cg_log_debug_s("Max persistent HTTP connection cache reached.\n");
               }

               if (new_node == NULL)
               {
                       if (data == NULL) return TRUE;

                       new_node = cg_http_persistentconnection_new();
                       if (new_node == NULL) return FALSE;

		      cg_log_debug_s("Adding persistent HTTP Connection %s:%d to cache\n",
			       host, port);
		      cg_log_debug_s("Persistent connections: %d\n", cg_list_size((CgList*)cache));
               }
       }

       if (data != NULL)
       {
               /* Set appropriate values for the node */
               cg_string_setvalue(new_node->host, host);
               new_node->port = port;
               new_node->cacheData = data;
               new_node->timestamp = cg_getcurrentsystemtime();

               cg_list_add((CgList*)cache, (CgList*)new_node);
       } else {
               /* remove and delete node */
               cg_http_persistentconnection_delete(new_node);
       }

       return TRUE;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_persistentconnection_clear
****************************************/

void cg_http_persistentconnection_clear(void)
{
	cg_log_debug_l4("Entering...\n");

       /* If we dont have cache, then just exit */
       if (cache == NULL) return;

       cg_http_persistentconnection_lock();
       cg_list_clear((CgList*)cache, (CG_LIST_DESTRUCTORFUNC)cg_http_persistentconnection_delete);
       free(cache);
       cache = NULL;
       cg_http_persistentconnection_unlock();

       cg_mutex_delete(persistent_connection_mutex);
       persistent_connection_mutex = NULL;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_persistentconnection_lock
****************************************/

void cg_http_persistentconnection_lock(void)
{
	cg_log_debug_l4("Entering...\n");

       if (persistent_connection_mutex == NULL) return;
       cg_mutex_lock(persistent_connection_mutex);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_persistentconnection_unlock
****************************************/

void cg_http_persistentconnection_unlock(void)
{
	cg_log_debug_l4("Entering...\n");

       if (persistent_connection_mutex == NULL) return;
       cg_mutex_unlock(persistent_connection_mutex);

	cg_log_debug_l4("Leaving...\n");
}
