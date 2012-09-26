/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: csocket.cpp
*
*	Revision:
*
*	01/17/05
*		- first revision
*	08/16/05
*		- Thanks for Theo Beisch <theo.beisch@gmx.de>
*		- Changed cg_socket_startup() for WindowsCE platform.
*	10/31/05
*		- cg_socket_getrawtype and cg_socket_getprototype:
*		  changed sock->type to socket->type to use the macro parameter
*		  correctly.
*	12/05/05
*		- Changed cg_socket_tosockaddrinfo() not to use the AI_NUMERICHOST option.
*	02/13/06 tb
*		- added WINCE support
*	03/21/06 Theo Beisch
*		- changed cg_socket_cleanup() to return socketCnt so
*		  that open socketCnt cleanup can be done on application 
*		  exit (one WSACleanup per WSAStartup / WS2 API) - probably a 
*		  temporary solution but it works in cases of threads
*		  that don't retire properly on exit 
*	04/09/06
*		- (WIN32 only) added int cg_socket_getlasterror()
*		- fixed memory leak in WIN32 recv() (fromLen init)
*		- fixed socket_cleanup() bookkeeping 
*	04/10/06
*		- introduced mutex for socketCnt accounting as 
*		  use of static variable otherwise unreliable
*		- moved cg_socket_startup and cg_socket_cleanup calls to 
*		  start and stop methods for controlpoint and device
*		fixmelater: this is still not 100% safe if multiple devices/CPs
*		should be created 'simultaneous', however already better  
*	    than having a CP/Device race into this with multiple
*		concurrent socket startups - the only truly self contained
*		solution would need a named mutex or alike to protect the counter
*	03/11/07
*		- Added support of OpenSSL to socket functions in csocket.c.
*		- Added CG_USE_OPENSSL define and --enable-openssl option to  disable the SSL functions as default.
*	03/13/07
*		- Fixed cg_socket_setid() to disable the IPPROTO_IP option using XCode on MacOSX.
*
******************************************************************/

#include <cybergarage/net/csocket.h>
#include <cybergarage/net/cinterface.h>
#include <cybergarage/util/ctime.h>
/*
#include <cybergarage/upnp/ssdp/cssdp.h>
*/

#include <cybergarage/util/clog.h>

#include <string.h>

#if !defined(WINCE)
#include <errno.h>
#endif

#if (defined(WIN32) || defined(__CYGWIN__)) && !defined (ITRON)
#include <winsock2.h>
#include <ws2tcpip.h>
#if defined(CG_USE_OPENSSL)
#pragma comment(lib, "libeay32MD.lib")
#pragma comment(lib, "ssleay32MD.lib")
#endif
#else
#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
#include <typedef.h>
#include <net/sock_com.h>
#include <btron/bsocket.h>
#include <string.h> //for mem___()
#elif defined(ITRON)
#include <kernel.h>
	#if defined(NORTiAPI)
	#include <nonet.h>
	#endif
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
#include <tk/tkernel.h>
#include <btron/kasago.h>
#include <sys/svc/ifkasago.h>
#include <string.h> //for mem___()
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#endif
#endif

/*
#define SOCKET_DEBUG
*/

/****************************************
* static variable
****************************************/

static int socketCnt = 0;

#if defined(CG_NET_USE_SOCKET_LIST)
static CgSocketList *socketList;
#endif

#if defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
ttUserInterface kaInterfaceHandle;
#endif

/****************************************
* prototype
****************************************/

#if !defined(ITRON)
BOOL cg_socket_tosockaddrin(char *addr, int port, struct sockaddr_in *sockaddr, BOOL isBindAddr);
#endif

#if !defined(BTRON) && !defined(ITRON) && !defined(TENGINE)
BOOL cg_socket_tosockaddrinfo(int sockType, char *addr, int port, struct addrinfo **addrInfo, BOOL isBindAddr);
#endif

#define cg_socket_getrawtype(socket) ((socket->type == CG_NET_SOCKET_STREAM) ? SOCK_STREAM : SOCK_DGRAM)

#if defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
#define cg_socket_getprototype(socket) ((socket->type == CG_NET_SOCKET_STREAM) ? IPPROTO_TCP : IPPROTO_UDP)
#endif

#if defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
BOOL cg_socket_setmulticastinterface(CgSocket *sock, char *ifaddr);
#endif

#if defined(CG_NET_USE_SOCKET_LIST)
static int cg_socket_getavailableid(int type);
static int cg_socket_getavailableport();
#endif

#if defined(ITRON)
BOOL cg_socket_initwindowbuffer(CgSocket *sock);
BOOL cg_socket_freewindowbuffer(CgSocket *sock);
static ER cg_socket_udp_callback(ID cepid, FN fncd, VP parblk);
static ER cg_socket_tcp_callback(ID cepid, FN fncd, VP parblk);
static BOOL cg_socket_getavailablelocaladdress(T_IPV4EP *localAddr);
#endif

/****************************************
*
* Socket
*
****************************************/

/****************************************
* cg_socket_startup
****************************************/

void cg_socket_startup()
{
	cg_log_debug_l4("Entering...\n");

	if (socketCnt == 0) {
#if (defined(WIN32) || defined(__CYGWIN__)) && !defined (ITRON)
		WSADATA wsaData;
		int err;
	
#if defined DEBUG_MEM
		memdiags_probe("STARTING WINSOCK");
#endif

		err = WSAStartup(MAKEWORD(2, 2), &wsaData);

#if defined (WINCE)
	//Theo Beisch unfriendly exit, at least for WINCE
		if (err) {
			//startup error
#if defined DEBUG
			printf("######## WINSOCK startup error %d\n", err);
#endif // DEBUG
		} 

#if defined DEBUG_MEM
		memdiags_probe("WINSOCK STARTED");
#endif

#endif // WINCE
#elif defined(ITRON) && defined(NORTiAPI)
		tcp_ini();
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
		kaInterfaceHandle = ka_tfAddInterface(CG_NET_DEFAULT_IFNAME);
#endif

#if (!defined(WIN32) || defined(__CYGWIN__)) && !defined(BTRON) && !defined(ITRON) && !defined(TENGINE)
		// Thanks for Brent Hills (10/26/04)
		signal(SIGPIPE,SIG_IGN);
#endif

#if defined(CG_NET_USE_SOCKET_LIST)
		socketList = cg_socketlist_new();
#endif	

#if defined(CG_USE_OPENSSL)
		SSL_library_init(); 
#endif
	}
	socketCnt++;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_socket_cleanup
****************************************/

void cg_socket_cleanup()
{
	cg_log_debug_l4("Entering...\n");

	socketCnt--;
	if (socketCnt <= 0) {
#if (defined(WIN32) || defined(__CYGWIN__)) && !defined (ITRON)
		WSACleanup( );
#endif

#if (!defined(WIN32) || defined(__CYGWIN__)) && !defined(BTRON) && !defined(ITRON) && !defined(TENGINE) 
		// Thanks for Brent Hills (10/26/04)
		signal(SIGPIPE,SIG_DFL);
#endif

#if defined(CG_NET_USE_SOCKET_LIST)
		cg_socketlist_delete(socketList);
#endif
	}

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_socket_new
****************************************/

CgSocket *cg_socket_new(int type)
{
	CgSocket *sock;

	cg_log_debug_l4("Entering...\n");

	cg_socket_startup();

	sock = (CgSocket *)malloc(sizeof(CgSocket));

	if ( NULL != sock )
	{
#if defined(WIN32) && !defined(__CYGWIN__) && !defined(__MINGW32__) && !defined(ITRON)
		sock->id = INVALID_SOCKET;
#else
		sock->id = -1;
#endif

		cg_socket_settype(sock, type);
		cg_socket_setdirection(sock, CG_NET_SOCKET_NONE);

		sock->ipaddr = cg_string_new();

		cg_socket_setaddress(sock, "");
		cg_socket_setport(sock, -1);

#if defined(ITRON)
		sock->sendWinBuf = NULL;
		sock->recvWinBuf = NULL;
#endif

#if defined(CG_USE_OPENSSL)
		sock->ctx = NULL;
		sock->ssl = NULL;
#endif
	}

	cg_log_debug_l4("Leaving...\n");

	return sock;
}

/****************************************
* cg_socket_delete
****************************************/

int cg_socket_delete(CgSocket *sock)
{
	cg_log_debug_l4("Entering...\n");

	cg_socket_close(sock);
	cg_string_delete(sock->ipaddr);
#if defined(ITRON)
	cg_socket_freewindowbuffer(sock);
#endif
	free(sock);
	cg_socket_cleanup();

	cg_log_debug_l4("Leaving...\n");

	return 0;
}

/****************************************
* cg_socket_isbound
****************************************/

BOOL cg_socket_isbound(CgSocket *sock)
{
	cg_log_debug_l4("Entering...\n");

	cg_log_debug_l4("Leaving...\n");

#if defined(WIN32) && !defined(__CYGWIN__) && !defined(__MINGW32__) && !defined(ITRON)
	return (sock->id != INVALID_SOCKET) ? TRUE: FALSE;
#else
	return (0 < sock->id) ? TRUE : FALSE;
#endif
}

/****************************************
* cg_socket_setid
****************************************/

void cg_socket_setid(CgSocket *socket, SOCKET value)
{
#if defined(WIN32) || defined(HAVE_IP_PKTINFO) || (!defined(WIN32) || defined(__CYGWIN__)) && !defined(BTRON) && !defined(ITRON) && !defined(TENGINE) && defined(HAVE_SO_NOSIGPIPE)
	int on=1;
#endif

	cg_log_debug_l4("Entering...\n");

	socket->id=value;

#if defined(WIN32) || defined(HAVE_IP_PKTINFO)
	if ( CG_NET_SOCKET_DGRAM == cg_socket_gettype(socket) ) 
		setsockopt(socket->id, IPPROTO_IP, IP_PKTINFO,  &on, sizeof(on));
#endif

#if (!defined(WIN32) || defined(__CYGWIN__)) && !defined(BTRON) && !defined(ITRON) && !defined(TENGINE) && defined(HAVE_SO_NOSIGPIPE)
	setsockopt(socket->id, SOL_SOCKET, SO_NOSIGPIPE,  &on, sizeof(on));
#endif
	
	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_socket_close
****************************************/

BOOL cg_socket_close(CgSocket *sock)
{
	cg_log_debug_l4("Entering...\n");

	if (cg_socket_isbound(sock) == FALSE)
		return TRUE;

#if defined(CG_USE_OPENSSL)
	if (cg_socket_isssl(sock) == TRUE) {
		if (sock->ctx) {
			SSL_shutdown(sock->ssl); 
			SSL_free(sock->ssl);
			sock->ssl = NULL;
		}
		if (sock->ctx) {
			SSL_CTX_free(sock->ctx);
			sock->ctx = NULL;
		}
	}
#endif

#if (defined(WIN32) || defined(__CYGWIN__)) && !defined(ITRON)
	#if !defined(WINCE)
	WSAAsyncSelect(sock->id, NULL, 0, FD_CLOSE);
	#endif
	shutdown(sock->id, SD_BOTH );
#if defined WINCE
	{
		int nRet = 1;
		char achDiscard[256];
		while (nRet && (nRet != SOCKET_ERROR)){
			if (nRet>0) {
				achDiscard[nRet]=(char)0;
#if defined DEBUG_SOCKET
				printf("DUMMY READ WHILE CLOSING SOCKET \n%s\n",achDiscard);
#endif
			}
			nRet = recv(sock->id,achDiscard,128,0);
		}
	}
#endif
	closesocket(sock->id);
	#if !defined(__CYGWIN__) && !defined(__MINGW32__)
	sock->id = INVALID_SOCKET;
	#else
	sock->id = -1;
	#endif
#else
	#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	so_shutdown(sock->id, 2);
	so_close(sock->id);
	#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	ka_tfClose(sock->id);
	#elif defined(ITRON)
	if (cg_socket_issocketstream(sock) == TRUE) {
		tcp_can_cep(sock->id, TFN_TCP_ALL);
		tcp_sht_cep(sock->id);
		tcp_del_cep(sock->id);
		tcp_cls_cep(sock->id, TMO_FEVR);
		tcp_del_rep(sock->id);
	}
	else {
		udp_can_cep(sock->id, TFN_UDP_ALL);
		udp_del_cep(sock->id);
	}
	#else
	int flag = fcntl(sock->id, F_GETFL, 0);
	if (0 <= flag)
		fcntl(sock->id, F_SETFL, flag | O_NONBLOCK);
	shutdown(sock->id, 2);
	close(sock->id);
	#endif
	sock->id = -1;
#endif

	cg_socket_setaddress(sock, "");
	cg_socket_setport(sock, -1);

	return TRUE;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_socket_listen
****************************************/

BOOL cg_socket_listen(CgSocket *sock)
{
#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	ERR ret = so_listen(sock->id, 10);
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	ERR ret = ka_listen(sock->id, 10);
#elif defined(ITRON)
	/**** Not Supported ****/
	int ret = 0;
#else
	int ret = listen(sock->id, SOMAXCONN);
#endif

	cg_log_debug_l4("Entering...\n");

	return (ret == 0) ? TRUE: FALSE;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_socket_bind
****************************************/

BOOL cg_socket_bind(CgSocket *sock, int bindPort, char *bindAddr,  BOOL bindFlag, BOOL reuseFlag)
{
#if defined(BTRON) || defined(TENGINE)
	struct sockaddr_in sockaddr;
	ERR ret;
#elif defined(ITRON)
	T_UDP_CCEP udpccep = { 0, { IPV4_ADDRANY, UDP_PORTANY }, (FP)cg_socket_udp_callback };
	T_TCP_CREP tcpcrep = { 0, { IPV4_ADDRANY, 0 } };
	T_TCP_CCEP tcpccep = { 0, sock->sendWinBuf, CG_NET_SOCKET_WINDOW_BUFSIZE, sock->recvWinBuf, CG_NET_SOCKET_WINDOW_BUFSIZE, (FP)cg_socket_tcp_callback };
#else
	struct addrinfo *addrInfo;
	int ret;
#endif

	cg_log_debug_l4("Entering...\n");

	if (bindPort <= 0 /* || bindAddr == NULL*/)
		return FALSE;

#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	if (cg_socket_tosockaddrin(bindAddr, bindPort, &sockaddr, bindFlag) == FALSE)
		return FALSE;
   	cg_socket_setid(sock, so_socket(PF_INET, cg_socket_getrawtype(sock), 0));
	if (sock->id < 0)
		return FALSE;
	if (reuseFlag == TRUE) {
		if (cg_socket_setreuseaddress(sock, TRUE) == FALSE) {
			cg_socket_close(sock);
			return FALSE;
		}
	}
	ret = so_bind(sock->id, (SOCKADDR *)&sockaddr, sizeof(struct sockaddr_in));
	if (ret < 0) {
		cg_socket_close(sock);
		return FALSE;
	}
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	if (cg_socket_tosockaddrin(bindAddr, bindPort, &sockaddr, bindFlag) == FALSE)
		return FALSE;
	cg_socket_setid(sock, ka_socket( PF_INET, cg_socket_getrawtype(sock), cg_socket_getprototype(sock)));
	if (sock->id < 0)
		return FALSE;
	/*
	if (cg_socket_setmulticastinterface(sock, bindAddr) == FALSE)
		return FALSE;
	*/
	if (reuseFlag == TRUE) {
		if (cg_socket_setreuseaddress(sock, TRUE) == FALSE) {
			cg_socket_close(sock);
			return FALSE;
		}
	}
	ret = ka_bind(sock->id, (struct sockaddr *)&sockaddr, sizeof(struct sockaddr_in));
	if (ret < 0) {
		cg_socket_close(sock);
		return FALSE;
	}
#elif defined(ITRON)
	cg_socket_setid(sock, cg_socket_getavailableid(cg_socket_issocketstream(sock)));
	if (sock->id < 0)
		return FALSE;
	if (cg_socket_issocketstream(sock) == TRUE) {
		if (bindAddr != NULL)
			tcpcrep.myaddr.ipaddr = ascii_to_ipaddr(bindAddr);
		tcpcrep.myaddr.ipaddr = htons(bindPort);
		if (tcp_cre_rep(sock->id, &tcpcrep) != E_OK) {
			cg_socket_close(sock);
			return FALSE;
		}
		if (tcp_cre_cep(sock->id, &tcpccep) != E_OK) {
			cg_socket_close(sock);
			return FALSE;
		}
	}
	else {
		if (bindAddr != NULL)
			udpccep.myaddr.ipaddr = ascii_to_ipaddr(bindAddr);
		udpccep.myaddr.ipaddr = htons(bindPort);
		if (udp_cre_cep(sock->id, &udpccep) != E_OK) {
			cg_socket_close(sock);
			return FALSE;
		}
	}
#else
	if (cg_socket_tosockaddrinfo(cg_socket_getrawtype(sock), bindAddr, bindPort, &addrInfo, bindFlag) == FALSE)
		return FALSE;
	cg_socket_setid(sock, socket(addrInfo->ai_family, addrInfo->ai_socktype, 0));
	if (sock->id== -1) {
		cg_socket_close(sock);
		return FALSE;
	}
	if (reuseFlag == TRUE) {
		if (cg_socket_setreuseaddress(sock, TRUE) == FALSE) {
			cg_socket_close(sock);
			return FALSE;
		}
	}
	ret = bind(sock->id, addrInfo->ai_addr, addrInfo->ai_addrlen);
	freeaddrinfo(addrInfo);
#endif

#if !defined(ITRON)
	if (ret != 0)
		return FALSE;
#endif

	cg_socket_setdirection(sock, CG_NET_SOCKET_SERVER);
	cg_socket_setaddress(sock, bindAddr);
	cg_socket_setport(sock, bindPort);

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_socket_accept
****************************************/

BOOL cg_socket_accept(CgSocket *serverSock, CgSocket *clientSock)
{
	struct sockaddr_in sockaddr;
	socklen_t socklen;
	char localAddr[CG_NET_SOCKET_MAXHOST];
	char localPort[CG_NET_SOCKET_MAXSERV];
#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	struct sockaddr_in sockaddr;
	W nLength = sizeof(struct sockaddr_in);
	cg_socket_setid(clientSock, so_accept(serverSock->id, (SOCKADDR *)&sockaddr, &nLength));
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	struct sockaddr_in sockaddr;
	int nLength = sizeof(struct sockaddr_in);
	cg_socket_setid(clientSock, ka_accept(serverSock->id, (struct sockaddr *)&sockaddr, &nLength));
#elif defined(ITRON)
	T_IPV4EP dstAddr;
	if (tcp_acp_cep(serverSock->id, serverSock->id, &dstAddr, TMO_FEVR) != E_OK)
		return FALSE;
	cg_socket_setid(clientSock, cg_socket_getid(serverSock));
#else
	struct sockaddr_storage sockClientAddr;
	socklen_t nLength = sizeof(sockClientAddr);
	cg_socket_setid(clientSock, accept(serverSock->id, (struct sockaddr *)&sockClientAddr, &nLength));
#endif

	cg_log_debug_l4("Entering...\n");

#ifdef SOCKET_DEBUG
cg_log_debug_s("clientSock->id = %d\n", clientSock->id);
#endif
	
#if defined (WIN32) && !defined(ITRON)
	if (clientSock->id == INVALID_SOCKET)
		return FALSE;
#else
	if (clientSock->id < 0)
		return FALSE;
#endif
	
	cg_socket_setaddress(clientSock, cg_socket_getaddress(serverSock));
	cg_socket_setport(clientSock, cg_socket_getport(serverSock));
	socklen = sizeof(struct sockaddr_in);
	
	if (getsockname(clientSock->id, (struct sockaddr *)&sockaddr, &socklen) == 0 &&
	    getnameinfo((struct sockaddr *)&sockaddr, socklen, localAddr, sizeof(localAddr), 
			localPort, sizeof(localPort), NI_NUMERICHOST | NI_NUMERICSERV) == 0) 
	{
		/* Set address for the sockaddr to real addr */
		cg_socket_setaddress(clientSock, localAddr);
	}
	
#ifdef SOCKET_DEBUG
cg_log_debug_s("clientSock->id = %s\n", cg_socket_getaddress(clientSock));
cg_log_debug_s("clientSock->id = %d\n", cg_socket_getport(clientSock));
#endif
	
	
	return TRUE;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_socket_connect
****************************************/

BOOL cg_socket_connect(CgSocket *sock, char *addr, int port)
{
#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	ERR ret;
	struct sockaddr_in sockaddr;
	if (cg_socket_tosockaddrin(addr, port, &sockaddr, TRUE) == FALSE)
		return FALSE;
	if (cg_socket_isbound(sock) == FALSE)
	   	cg_socket_setid(sock, so_socket(PF_INET, cg_socket_getrawtype(sock), 0));
	ret = so_connect(sock->id, (SOCKADDR *)&sockaddr, sizeof(struct sockaddr_in));
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	ERR ret;
	struct sockaddr_in sockaddr;
	if (cg_socket_tosockaddrin(addr, port, &sockaddr, TRUE) == FALSE)
		return FALSE;
	if (cg_socket_isbound(sock) == FALSE)
	   	cg_socket_setid(sock, ka_socket(PF_INET, cg_socket_getrawtype(sock), cg_socket_getprototype(sock)));
	ret = ka_connect(sock->id, (struct sockaddr *)&sockaddr, sizeof(struct sockaddr_in));
#elif defined(ITRON)
	T_TCP_CCEP tcpccep = { 0, sock->sendWinBuf, CG_NET_SOCKET_WINDOW_BUFSIZE, sock->recvWinBuf, CG_NET_SOCKET_WINDOW_BUFSIZE, (FP)cg_socket_tcp_callback };
	T_IPV4EP localAddr;
	T_IPV4EP dstAddr;
	ER ret;
	if (cg_socket_getavailablelocaladdress(&localAddr) == FALSE)
		return FALSE;
	if (cg_socket_isbound(sock) == FALSE) {
		cg_socket_initwindowbuffer(sock);
		cg_socket_setid(sock, cg_socket_getavailableid(cg_socket_issocketstream(sock)));
		if (tcp_cre_cep(sock->id, &tcpccep) != E_OK)
			return FALSE;
	}
	dstAddr.ipaddr = ascii_to_ipaddr(addr);
	dstAddr.portno = htons(port);
	ret = tcp_con_cep(sock->id, &localAddr, &dstAddr, TMO_FEVR);
	if (ret == E_OK) {
		cg_socket_setaddress(sock, ""/*ipaddr_to_ascii(localAddr.ipaddr)*/);
		cg_socket_setport(sock, ntohs(localAddr.portno));
		ret = 0;
	}
	else 
		ret = -1;
#else
	struct addrinfo *toaddrInfo;
	int ret;
	if (cg_socket_tosockaddrinfo(cg_socket_getrawtype(sock), addr, port, &toaddrInfo, TRUE) == FALSE)
		return FALSE;
	if (cg_socket_isbound(sock) == FALSE)
		cg_socket_setid(sock, socket(toaddrInfo->ai_family, toaddrInfo->ai_socktype, 0));
	ret = connect(sock->id, toaddrInfo->ai_addr, toaddrInfo->ai_addrlen);
	freeaddrinfo(toaddrInfo);
#endif

	cg_log_debug_l4("Entering...\n");

	cg_socket_setdirection(sock, CG_NET_SOCKET_CLIENT);

#if defined(CG_USE_OPENSSL)
	if (cg_socket_isssl(sock) == TRUE) {
		sock->ctx = SSL_CTX_new( SSLv23_client_method());
		sock->ssl = SSL_new(sock->ctx);
		if (SSL_set_fd(sock->ssl, cg_socket_getid(sock)) == 0) {
			cg_socket_close(sock);
			return FALSE;
		}
		if (SSL_connect(sock->ssl) < 1) {
			cg_socket_close(sock);
			return FALSE;
		}
	}
#endif

	cg_log_debug_l4("Leaving...\n");

	return (ret == 0) ? TRUE : FALSE;
}

/****************************************
* cg_socket_read
****************************************/

int cg_socket_read(CgSocket *sock, char *buffer, int bufferLen)
{
	int recvLen;

#if defined(CG_USE_OPENSSL)
	if (cg_socket_isssl(sock) == FALSE) {
#endif

#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	recvLen = so_recv(sock->id, buffer, bufferLen, 0);
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	recvLen = ka_recv(sock->id, buffer, bufferLen, 0);
#elif defined(ITRON)
	recvLen = tcp_rcv_dat(sock->id, buffer, bufferLen, TMO_FEVR);
#else
	recvLen = recv(sock->id, buffer, bufferLen, 0);
#endif

#if defined(CG_USE_OPENSSL)
	}
	else {
		recvLen = SSL_read(sock->ssl, buffer, bufferLen);
	}
#endif

	cg_log_debug_l4("Entering...\n");

#ifdef SOCKET_DEBUG
	if (0 <= recvLen)
		buffer[recvLen] = '\0';
cg_log_debug_s("r %d : %s\n", recvLen, (0 <= recvLen) ? buffer : "");
#endif

	cg_log_debug_l4("Leaving...\n");

	return recvLen;
}

/****************************************
* cg_socket_write
****************************************/

#define CG_NET_SOCKET_SEND_RETRY_CNT 10
#define CG_NET_SOCKET_SEND_RETRY_WAIT_MSEC 20

int cg_socket_write(CgSocket *sock, char *cmd, int cmdLen)
{
	int nSent;
	int nTotalSent = 0;
	int cmdPos = 0;
	int retryCnt = 0;

	cg_log_debug_l4("Entering...\n");

	if (cmdLen <= 0)
		return 0;

	do {
#if defined(CG_USE_OPENSSL)
		if (cg_socket_isssl(sock) == FALSE) {
#endif

#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
		nSent = so_send(sock->id, (B*)(cmd + cmdPos), cmdLen, 0);
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
		nSent = ka_send(sock->id, (B*)(cmd + cmdPos), cmdLen, 0);
#elif defined(ITRON)
		nSent = tcp_snd_dat(sock->id, cmd + cmdPos, cmdLen, TMO_FEVR);
#else
		nSent = send(sock->id, cmd + cmdPos, cmdLen, 0);
#endif
			
#if defined(CG_USE_OPENSSL)
		}
		else {
			nSent = SSL_write(sock->ssl, cmd + cmdPos, cmdLen);
		}
#endif

		/* Try to re-send in case sending has failed */
		if (nSent <= 0)
		{
			retryCnt++;
			if (CG_NET_SOCKET_SEND_RETRY_CNT < retryCnt)
			{
				/* Must reset this because otherwise return
				   value is interpreted as something else than
				   fault and this function loops forever */
				nTotalSent = 0;
				break;
			}

			cg_wait(CG_NET_SOCKET_SEND_RETRY_WAIT_MSEC);
		}
		else
		{
			nTotalSent += nSent;
			cmdPos += nSent;
			cmdLen -= nSent;
			retryCnt = 0;
		}

	} while (0 < cmdLen);

#ifdef SOCKET_DEBUG
cg_log_debug_s("w %d : %s\n", nTotalSent, ((cmd != NULL) ? cmd : ""));
#endif
	
	cg_log_debug_l4("Leaving...\n");

	return nTotalSent;
}
/****************************************
* cg_socket_readline
****************************************/

int cg_socket_readline(CgSocket *sock, char *buffer, int bufferLen)
{
	int readCnt;
	int readLen;
	char c;
	
	cg_log_debug_l4("Entering...\n");

	readCnt = 0;
	while (readCnt < (bufferLen-1)) {
		readLen = cg_socket_read(sock, &buffer[readCnt], sizeof(char));
		if (readLen <= 0)
			return -1;
		readCnt++;
		if (buffer[readCnt-1] == CG_SOCKET_LF)
			break;
	}
	buffer[readCnt] = '\0';
	if (buffer[readCnt-1] != CG_SOCKET_LF) {
		do {
			readLen = cg_socket_read(sock, &c, sizeof(char));
			if (readLen <= 0)
				break;
		} while (c != CG_SOCKET_LF);
	}

	cg_log_debug_l4("Leaving...\n");

	return readCnt;	
}

/****************************************
* cg_socket_skip
****************************************/

long cg_socket_skip(CgSocket *sock, long skipLen)
{
	int readCnt;
	int readLen;
	char c;
	
	cg_log_debug_l4("Entering...\n");

	readCnt = 0;
	while (readCnt < skipLen) {
		readLen = cg_socket_read(sock, &c, sizeof(char));
		if (readLen <= 0)
			break;
		readCnt++;
	}

	cg_log_debug_l4("Leaving...\n");

	return readCnt;
}

/****************************************
* cg_socket_sendto
****************************************/

int cg_socket_sendto(CgSocket *sock, char *addr, int port, char *data, int dataLen)
{
#if defined(BTRON) || defined(TENGINE)
	struct sockaddr_in sockaddr;
#elif defined(ITRON)
	T_IPV4EP dstaddr;
	T_UDP_CCEP udpccep = { 0, { IPV4_ADDRANY, UDP_PORTANY }, (FP)cg_socket_udp_callback };
#else
	struct addrinfo *addrInfo;
#endif
	int sentLen;
	BOOL isBoundFlag;

	cg_log_debug_l4("Entering...\n");

	if (data == NULL)
		return 0;
	if (dataLen < 0)
		dataLen = cg_strlen(data);
	if (dataLen <= 0)
		return 0;

	isBoundFlag = cg_socket_isbound(sock);
	sentLen = -1;
#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	if (cg_socket_tosockaddrin(addr, port, &sockaddr, TRUE) == FALSE)
		return -1;
	if (isBoundFlag == FALSE)
	   	cg_socket_setid(sock, so_socket(PF_INET, cg_socket_getrawtype(sock), 0));
	if (0 <= sock->id)
		sentLen = so_sendto(sock->id, (B*)data, dataLen, 0, (SOCKADDR*)&sockaddr, sizeof(struct sockaddr_in));
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	if (cg_socket_tosockaddrin(addr, port, &sockaddr, TRUE) == FALSE)
		return -1;
	if (isBoundFlag == FALSE) {
	   	cg_socket_setid(sock, ka_socket(PF_INET, cg_socket_getrawtype(sock), cg_socket_getprototype(sock)));
		cg_socket_setmulticastinterface(sock, NULL);
	}
	if (0 <= sock->id)
		sentLen = ka_sendto(sock->id, data, dataLen, 0, (struct sockaddr *)&sockaddr, sizeof(struct sockaddr_in));
#elif defined(ITRON)
	if (isBoundFlag == FALSE) {
		cg_socket_setid(sock, cg_socket_getavailableid(cg_socket_issocketstream(sock)));
		if (sock->id < 0)
			return FALSE;
		if (udp_cre_cep(sock->id, &udpccep) != E_OK)
			return FALSE;
	}
	dstaddr.ipaddr = ascii_to_ipaddr(addr);
	dstaddr.portno = htons(port);
	sentLen = udp_snd_dat(sock->id, &dstaddr, data, dataLen, TMO_FEVR);
#else
	if (cg_socket_tosockaddrinfo(cg_socket_getrawtype(sock), addr, port, &addrInfo, TRUE) == FALSE)
		return -1;
	if (isBoundFlag == FALSE)
		cg_socket_setid(sock, socket(addrInfo->ai_family, addrInfo->ai_socktype, 0));
	
	/* Setting multicast time to live in any case to default */
	cg_socket_setmulticastttl(sock, CG_NET_SOCKET_MULTICAST_DEFAULT_TTL);
	
	if (0 <= sock->id)
		sentLen = sendto(sock->id, data, dataLen, 0, addrInfo->ai_addr, addrInfo->ai_addrlen);
	freeaddrinfo(addrInfo);
#endif

	if (isBoundFlag == FALSE)
		cg_socket_close(sock);

#ifdef SOCKET_DEBUG
cg_log_debug_s("sentLen : %d\n", sentLen);
#endif

	cg_log_debug_l4("Leaving...\n");

	return sentLen;
}

/****************************************
* cg_socket_recv
****************************************/

int cg_socket_recv(CgSocket *sock, CgDatagramPacket *dgmPkt)
{
	int recvLen = 0;
	char recvBuf[CG_NET_SOCKET_DGRAM_RECV_BUFSIZE+1];
	char remoteAddr[CG_NET_SOCKET_MAXHOST];
	char remotePort[CG_NET_SOCKET_MAXSERV];
	char *localAddr;

#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	struct sockaddr_in from;
	W fromLen = sizeof(from);
	recvLen = so_recvfrom(sock->id, recvBuf, sizeof(recvBuf)-1, 0, (struct sockaddr *)&from, &fromLen);
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	struct sockaddr_in from;
	int fromLen = sizeof(from);
	recvLen = ka_recvfrom(sock->id, recvBuf, sizeof(recvBuf)-1, 0, (struct sockaddr *)&from, &fromLen);
#elif defined(ITRON)
	T_IPV4EP remoteHost;
	recvLen = udp_rcv_dat(sock->id, &remoteHost, recvBuf, sizeof(recvBuf)-1, TMO_FEVR);
#else
	struct sockaddr_storage from;
	socklen_t fromLen = sizeof(from);
	recvLen = recvfrom(sock->id, recvBuf, sizeof(recvBuf)-1, 0, (struct sockaddr *)&from, &fromLen);
#endif

	cg_log_debug_l4("Entering...\n");

	if (recvLen <= 0)
		return 0;
	recvBuf[recvLen] = '\0';
	cg_socket_datagram_packet_setdata(dgmPkt, recvBuf);

	cg_socket_datagram_packet_setlocalport(dgmPkt, cg_socket_getport(sock));
	cg_socket_datagram_packet_setremoteaddress(dgmPkt, "");
	cg_socket_datagram_packet_setremoteport(dgmPkt, 0);

#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	cg_socket_datagram_packet_setlocaladdress(dgmPkt, cg_socket_getaddress(sock));
	cg_socket_datagram_packet_setremoteaddress(dgmPkt, inet_ntoa(from.sin_addr));
	cg_socket_datagram_packet_setremoteport(dgmPkt, ntohl(from.sin_port));
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	cg_socket_datagram_packet_setlocaladdress(dgmPkt, cg_socket_getaddress(sock));
	ka_tfInetToAscii((unsigned long)from.sin_addr.s_addr, remoteAddr);
	cg_socket_datagram_packet_setremoteaddress(dgmPkt, remoteAddr);
	cg_socket_datagram_packet_setremoteport(dgmPkt, ka_ntohl(from.sin_port));
#elif defined(ITRON)
	cg_socket_datagram_packet_setlocaladdress(dgmPkt, cg_socket_getaddress(sock));
	ipaddr_to_ascii(remoteAddr, remoteHost.ipaddr);
	cg_socket_datagram_packet_setremoteaddress(dgmPkt, remoteAddr);
	cg_socket_datagram_packet_setremoteport(dgmPkt, ntohs(remoteHost.portno));
#else

	if (getnameinfo((struct sockaddr *)&from, fromLen, remoteAddr, sizeof(remoteAddr), remotePort, sizeof(remotePort), NI_NUMERICHOST | NI_NUMERICSERV) == 0) {
		cg_socket_datagram_packet_setremoteaddress(dgmPkt, remoteAddr);
		cg_socket_datagram_packet_setremoteport(dgmPkt, atol(remotePort));
	}

	cg_log_debug_s("From pointer %p\n", &from);
	localAddr = cg_net_selectaddr((struct sockaddr *)&from);
	cg_socket_datagram_packet_setlocaladdress(dgmPkt, localAddr);
	free(localAddr);

#endif

	cg_log_debug_l4("Leaving...\n");

	return recvLen;
}

/****************************************
* cg_socket_setreuseaddress
****************************************/

BOOL cg_socket_setreuseaddress(CgSocket *sock, BOOL flag)
{
	int sockOptRet;
#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	B optval
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	int optval;
#elif defined (WIN32)
	BOOL optval;
#else
	int optval;
#endif

	cg_log_debug_l4("Entering...\n");

#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	optval = (flag == TRUE) ? 1 : 0;
	sockOptRet = so_setsockopt(sock->id, SOL_SOCKET, SO_REUSEADDR, (B *)&optval, sizeof(optval));
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	optval = (flag == TRUE) ? 1 : 0;
	sockOptRet = ka_setsockopt(sock->id, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof(optval));
#elif defined (ITRON)
	/**** Not Implemented for NORTi ***/
	sockOptRet = -1;
#elif defined (WIN32)
	optval = (flag == TRUE) ? 1 : 0;
	sockOptRet = setsockopt(sock->id, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof(optval));
#else
	optval = (flag == TRUE) ? 1 : 0;
	sockOptRet = setsockopt(sock->id, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof(optval));
	#if defined(USE_SO_REUSEPORT) || defined(TARGET_OS_MAC) || defined(TARGET_OS_IPHONE)
	setsockopt(sock->id, SOL_SOCKET, SO_REUSEPORT, (const char *)&optval, sizeof(optval));
	#endif
#endif
	cg_log_debug_l4("Leaving...\n");

	return (sockOptRet == 0) ? TRUE : FALSE;
}

/****************************************
* cg_socket_setmulticastttl
****************************************/

BOOL cg_socket_setmulticastttl(CgSocket *sock, int ttl)
{
	int sockOptRet;
	int ttl_;
	unsigned int len=0;
	
	cg_log_debug_l4("Entering...\n");

#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	sockOptRet = so_setsockopt(sock->id, IPPROTO_IP, IP_MULTICAST_TTL, (B *)&ttl, sizeof(ttl));
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	sockOptRet = ka_setsockopt(sock->id, IPPROTO_IP, IP_MULTICAST_TTL, (const char *)&ttl, sizeof(ttl));
#elif defined (ITRON)
	/**** Not Implemented for NORTi ***/
	sockOptRet = -1;
#elif defined (WIN32)
	sockOptRet = setsockopt(sock->id, IPPROTO_IP, IP_MULTICAST_TTL, (const char *)&ttl, sizeof(ttl));
#else
	cg_log_debug("Setting multicast time to live to %d\n", ttl);
	sockOptRet = setsockopt(sock->id, IPPROTO_IP, IP_MULTICAST_TTL, (const unsigned char *)&ttl, sizeof(ttl));
	if (sockOptRet != 0)
	{
		cg_log_debug("setsockopt() failed with errno %d: %s, fd:%d\n", errno, strerror(errno),sock->id);
	}
	else
	{
		len = sizeof(ttl_);
		getsockopt(sock->id, IPPROTO_IP, IP_MULTICAST_TTL, &ttl_, (socklen_t*)&len);
		cg_log_debug("Multicast time to live is %i\n", ttl_);
	}
#endif
	cg_log_debug_l4("Leaving...\n");

	return (sockOptRet == 0) ? TRUE : FALSE;
}

/****************************************
* cg_socket_settimeout
****************************************/

BOOL cg_socket_settimeout(CgSocket *sock, int sec)
{
	int sockOptRet;
#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
	sockOptRet = -1; /* TODO: Implement this */
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	sockOptRet = -1; /* TODO: Implement this */
#elif defined (ITRON)
	/**** Not Implemented for NORTi ***/
	sockOptRet = -1;
#elif defined (WIN32)
	struct timeval timeout;
	timeout.tv_sec = sec;
	timeout.tv_usec = 0;

	cg_log_debug_l4("Entering...\n");

	sockOptRet = setsockopt(sock->id, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
	if (sockOptRet == 0)
		sockOptRet = setsockopt(sock->id, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout));
#else
	struct timeval timeout;
	timeout.tv_sec = (time_t)sec;
	timeout.tv_usec = 0;

	cg_log_debug_l4("Entering...\n");

	sockOptRet = setsockopt(sock->id, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
	if (sockOptRet == 0)
		sockOptRet = setsockopt(sock->id, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout));
#endif

	cg_log_debug_l4("Leaving...\n");

	return (sockOptRet == 0) ? TRUE : FALSE;
}

/****************************************
* cg_socket_joingroup
****************************************/

#if defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)

BOOL cg_socket_joingroup(CgSocket *sock, char *mcastAddr, char *ifAddr)
{
	struct ip_mreq ipmr;
	u_long ifInetAddr = ka_inet_addr(ifAddr);
	
	BOOL joinSuccess;
	int sockOptRetCode;
	
	cg_log_debug_l4("Entering...\n");

	joinSuccess = TRUE;
	
	ka_inet_pton( AF_INET, mcastAddr, &(ipmr.imr_multiaddr) );
	memcpy(&ipmr.imr_interface, &ifInetAddr, sizeof(struct in_addr));
	sockOptRetCode = ka_setsockopt(sock->id, IP_PROTOIP, IPO_ADD_MEMBERSHIP, (char *)&ipmr, sizeof(ipmr));

	if (sockOptRetCode != 0)
		joinSuccess = FALSE;

	cg_string_setvalue(sock->ipaddr, (joinSuccess == TRUE) ? ifAddr : NULL);

	cg_log_debug_l4("Leaving...\n");

	return joinSuccess;
}

#elif defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))

BOOL cg_socket_joingroup(CgSocket *sock, char *mcastAddr, char *ifAddr)
{
	cg_log_debug_l4("Entering...\n");

	cg_log_debug_l4("Leaving...\n");

	/**** Not Suppoted ****/
	return TRUE;

}

#elif defined(ITRON)

BOOL cg_socket_joingroup(CgSocket *sock, char *mcastAddr, char *ifAddr)
{
	cg_log_debug_l4("Entering...\n");

	UW optval;
	ER ret;

	optval = ascii_to_ipaddr(mcastAddr);
	ret = udp_set_opt(sock->id, IP_ADD_MEMBERSHIP, &optval, sizeof(optval));

	cg_log_debug_l4("Leaving...\n");

	return (ret == E_OK) ? TRUE : FALSE;
}
#else

BOOL cg_socket_joingroup(CgSocket *sock, char *mcastAddr, char *ifAddr)
{
	struct addrinfo hints;
	struct addrinfo *mcastAddrInfo, *ifAddrInfo;
	
	/**** for IPv6 ****/
	struct ipv6_mreq ipv6mr;
	struct sockaddr_in6 toaddr6, ifaddr6;
	int scopeID;
	
	/**** for IPv4 ****/	
	struct ip_mreq ipmr;
	struct sockaddr_in toaddr, ifaddr;
	
	BOOL joinSuccess;
	int sockOptRetCode;
	
	cg_log_debug_l4("Entering...\n");

	memset(&hints, 0, sizeof(hints));
	hints.ai_flags= AI_NUMERICHOST | AI_PASSIVE;

	if (getaddrinfo(mcastAddr, NULL, &hints, &mcastAddrInfo) != 0) 
		return FALSE;

	if (getaddrinfo(ifAddr, NULL, &hints, &ifAddrInfo) != 0) {
		freeaddrinfo(mcastAddrInfo);
		return FALSE;
	}

	joinSuccess = TRUE;
	
	if (cg_net_isipv6address(mcastAddr) == TRUE) {
		memcpy(&toaddr6, mcastAddrInfo->ai_addr, sizeof(struct sockaddr_in6));
		memcpy(&ifaddr6, ifAddrInfo->ai_addr, sizeof(struct sockaddr_in6));
		ipv6mr.ipv6mr_multiaddr = toaddr6.sin6_addr;	
		scopeID = cg_net_getipv6scopeid(ifAddr);
		ipv6mr.ipv6mr_interface = scopeID /*if_nametoindex*/;
		
		sockOptRetCode = setsockopt(sock->id, IPPROTO_IPV6, IPV6_MULTICAST_IF, (char *)&scopeID, sizeof(scopeID));

		if (sockOptRetCode != 0)
			joinSuccess = FALSE;

		sockOptRetCode = setsockopt(sock->id, IPPROTO_IPV6, IPV6_JOIN_GROUP, (char *)&ipv6mr, sizeof(ipv6mr));

		if (sockOptRetCode != 0)
			joinSuccess = FALSE;

	}
	else {
		memcpy(&toaddr, mcastAddrInfo->ai_addr, sizeof(struct sockaddr_in));
		memcpy(&ifaddr, ifAddrInfo->ai_addr, sizeof(struct sockaddr_in));
		memcpy(&ipmr.imr_multiaddr.s_addr, &toaddr.sin_addr, sizeof(struct in_addr));
		memcpy(&ipmr.imr_interface.s_addr, &ifaddr.sin_addr, sizeof(struct in_addr));
		sockOptRetCode = setsockopt(sock->id, IPPROTO_IP, IP_MULTICAST_IF, (char *)&ipmr.imr_interface.s_addr, sizeof(struct in_addr));
		if (sockOptRetCode != 0)
			joinSuccess = FALSE;
		sockOptRetCode = setsockopt(sock->id, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&ipmr, sizeof(ipmr));
		
		if (sockOptRetCode != 0)
			joinSuccess = FALSE;
	}

	freeaddrinfo(mcastAddrInfo);
	freeaddrinfo(ifAddrInfo);

	cg_log_debug_l4("Leaving...\n");

	return joinSuccess;
}

#endif

/****************************************
* cg_socket_tosockaddrin
****************************************/

#if !defined(ITRON)

BOOL cg_socket_tosockaddrin(char *addr, int port, struct sockaddr_in *sockaddr, BOOL isBindAddr)
{
	cg_log_debug_l4("Entering...\n");

	cg_socket_startup();

	memset(sockaddr, 0, sizeof(struct sockaddr_in));

#if defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	sockaddr->sin_family = AF_INET;
	sockaddr->sin_addr.s_addr = ka_htonl(INADDR_ANY);
	sockaddr->sin_port = ka_htons((unsigned short)port);
#else
	sockaddr->sin_family = AF_INET;
	sockaddr->sin_addr.s_addr = htonl(INADDR_ANY);
	sockaddr->sin_port = htons((unsigned short)port);
#endif

	if (isBindAddr == TRUE) {
#if defined(BTRON) || (defined(TENGINE) && !defined(CG_TENGINE_NET_KASAGO))
		sockaddr->sin_addr.s_addr = inet_addr(addr);
		if (sockaddr->sin_addr.s_addr == -1 /*INADDR_NONE*/) {
			struct hostent hent;
			B hostBuf[HBUFLEN];
			if (so_gethostbyname((B*)addr, &hent, hostBuf) != 0)
				return FALSE;
			memcpy(&(sockaddr->sin_addr), hent.h_addr, hent.h_length);
		}
#elif defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
		sockaddr->sin_addr.s_addr = ka_inet_addr(addr);
#else
		sockaddr->sin_addr.s_addr = inet_addr(addr);
		if (sockaddr->sin_addr.s_addr == INADDR_NONE) {
			struct hostent *hent = gethostbyname(addr);
			if (hent == NULL)
				return FALSE;
			memcpy(&(sockaddr->sin_addr), hent->h_addr, hent->h_length);
		}
#endif
	}

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}
#endif

/****************************************
* cg_socket_tosockaddrinfo
****************************************/

#if !defined(BTRON) && !defined(ITRON) && !defined(TENGINE)

BOOL cg_socket_tosockaddrinfo(int sockType, char *addr, int port, struct addrinfo **addrInfo, BOOL isBindAddr)
{
#if defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	struct addrinfo hints;
	char portStr[32];
#else
	struct addrinfo hints;
	char portStr[32];
	int errorn;
#endif

	cg_log_debug_l4("Entering...\n");

	cg_socket_startup();

#if defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = sockType;
	hints.ai_flags= 0; /*AI_NUMERICHOST | AI_PASSIVE*/;
	sprintf(portStr, "%d", port);
	if (ka_getaddrinfo(addr, portStr, &hints, addrInfo) != 0)
		return FALSE;
	if (isBindAddr == TRUE)
		return TRUE;
	hints.ai_family = (*addrInfo)->ai_family;
	ka_freeaddrinfo(*addrInfo);
	if (ka_getaddrinfo(NULL, portStr, &hints, addrInfo) != 0)
		return FALSE;
	return TRUE;
#else
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = sockType;
	hints.ai_flags= /*AI_NUMERICHOST | */AI_PASSIVE;
	sprintf(portStr, "%d", port);
	cg_log_debug("Address: %s, port: %s\n", addr, portStr);
	if ( (errorn = getaddrinfo(addr, portStr, &hints, addrInfo)) != 0) {
#if !defined(WINCE)
		cg_log_debug_s("ERROR: %s\n", gai_strerror(errorn));
		cg_log_debug_s("SERROR: %s\n", strerror(errno));
#endif
		return FALSE;
	}
	if (isBindAddr == TRUE)
		return TRUE;
	hints.ai_family = (*addrInfo)->ai_family;
	freeaddrinfo(*addrInfo);
	if ((errorn = getaddrinfo(NULL, portStr, &hints, addrInfo)) != 0) {
#if !defined(WINCE)
		cg_log_debug_s("ERROR: %s\n", gai_strerror(errorn));
		cg_log_debug_s("SERROR: %s\n", strerror(errno));
#endif
		return FALSE;
	}
	return TRUE;
#endif
 }

#endif

/****************************************
* cg_socket_setmulticastinterface
****************************************/

#if defined(TENGINE) && defined(CG_TENGINE_NET_KASAGO)

BOOL cg_socket_setmulticastinterface(CgSocket *sock, char *ifaddr)
{
	struct sockaddr_in sockaddr;
	BOOL sockAddrSuccess;
	int optSuccess;
	CgNetworkInterfaceList *netIfList;
	CgNetworkInterface *netIf;
	int netIfCnt;

	cg_log_debug_l4("Entering...\n");

	netIfList = NULL;
	if (cg_strlen(ifaddr) <= 0) {
		netIfList = cg_net_interfacelist_new();
		netIfCnt = cg_net_gethostinterfaces(netIfList);
		if (netIfCnt <= 0) {
			cg_net_interfacelist_delete(netIfList);
			return FALSE;
		}
		netIf = cg_net_interfacelist_gets(netIfList);
		ifaddr = cg_net_interface_getaddress(netIf);
	}

	sockAddrSuccess = cg_socket_tosockaddrin(ifaddr, 0, &sockaddr, TRUE);
	if (netIfList != NULL)
		cg_net_interfacelist_delete(netIfList);
	if (sockAddrSuccess == FALSE)
		return FALSE;

	optSuccess = ka_setsockopt(sock->id, IP_PROTOIP, IPO_MULTICAST_IF, (const char *)&sockaddr.sin_addr, sizeof(sockaddr.sin_addr));
	if (optSuccess != 0)
		return FALSE;

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

#endif

/****************************************
* cg_socket_getavailableid
****************************************/

#if defined(CG_NET_USE_SOCKET_LIST)

static int cg_socket_getavailableid(int type)
{
	CgSocket *sock;
	int id;
	BOOL isIDUsed;

	cg_log_debug_l4("Entering...\n");

	id = 0;
	do {
		id++;
		isIDUsed = FALSE;
		for (sock = cg_socketlist_gets(socketList); sock != NULL; sock = cg_socket_next(sock)) {
			if (cg_socket_gettype(sock) != type)
				continue;
			if (cg_socket_getid(sock) == id) {
				isIDUsed = TRUE;
				break;
			}
		}
	} while (isIDUsed != FALSE);

	cg_log_debug_l4("Leaving...\n");

	return id;
}

#endif

/****************************************
* cg_socket_getavailableid
****************************************/

#if defined(CG_NET_USE_SOCKET_LIST)

#define CG_NET_SOCKET_MIN_SOCKET_PORT 50000

static int cg_socket_getavailableport()
{
	CgSocket *sock;
	int port;
	BOOL isPortUsed;

	cg_log_debug_l4("Entering...\n");

	port = CG_NET_SOCKET_MIN_SOCKET_PORT - 1;
	do {
		port++;
		isPortUsed = FALSE;
		for (sock = cg_socketlist_gets(socketList); sock != NULL; sock = cg_socket_next(sock)) {
			if (cg_socket_getport(sock) == port) {
				isPortUsed = TRUE;
				break;
			}
		}
	} while (isPortUsed != FALSE);

	cg_log_debug_l4("Leaving...\n");

	return port;
}

#endif

/****************************************
* cg_socket_*windowbuffer (ITRON)
****************************************/

#if defined(ITRON)

BOOL cg_socket_initwindowbuffer(CgSocket *sock)
{
	cg_log_debug_l4("Entering...\n");

	if (sock->sendWinBuf == NULL)
		sock->sendWinBuf = (char *)malloc(sizeof(UH) * CG_NET_SOCKET_WINDOW_BUFSIZE);
	if (sock->sendWinBuf == NULL)
		sock->recvWinBuf = (char *)malloc(sizeof(UH) * CG_NET_SOCKET_WINDOW_BUFSIZE);
	
	cg_log_debug_l4("Leaving...\n");

	if ( ( NULL == sock->sendWinBuf ) || ( NULL == sock->sendWinBuf ) )
	{
		cg_log_debug_s("Memory allocation failure!\n");
		return FALSE;
	}
	else
		return TRUE;
}

BOOL cg_socket_freewindowbuffer(CgSocket *sock)
{
	cg_log_debug_l4("Entering...\n");

	if (sock->sendWinBuf != NULL)
		free(sock->sendWinBuf);
	if (sock->sendWinBuf != NULL)
		free(sock->recvWinBuf);

	cg_log_debug_l4("Leaving...\n");
}

#endif

/****************************************
* cg_socket_*_callback (ITRON)
****************************************/

#if defined(ITRON)

static ER cg_socket_udp_callback(ID cepid, FN fncd, VP parblk)
{
	cg_log_debug_l4("Entering...\n");

    return E_OK;

	cg_log_debug_l4("Leaving...\n");
}

static ER cg_socket_tcp_callback(ID cepid, FN fncd, VP parblk)
{
	cg_log_debug_l4("Entering...\n");

    return E_OK;

	cg_log_debug_l4("Leaving...\n");
}

#endif

/****************************************
* cg_socket_getavailablelocaladdress (ITRON)
****************************************/

#if defined(ITRON)

static BOOL cg_socket_getavailablelocaladdress(T_IPV4EP *localAddr)
{
	ER ret;
	char *ifAddr;
	int localPort;

	cg_log_debug_l4("Entering...\n");

	CgNetworkInterfaceList *netIfList;
	CgNetworkInterface *netIf;
	int netIfCnt;
	netIfList = cg_net_interfacelist_new();
	netIfCnt = cg_net_gethostinterfaces(netIfList);
	if (netIfCnt <= 0) {
		cg_net_interfacelist_delete(netIfList);
		return FALSE;
	}
	netIf = cg_net_interfacelist_gets(netIfList);
	ifAddr = cg_net_interface_getaddress(netIf);
	localPort = cg_socket_getavailableport();
	localAddr->ipaddr = ascii_to_ipaddr(ifAddr);
	localAddr->portno = htons(localPort);
	cg_net_interfacelist_delete(netIfList);

	return FALSE;
}
#endif

#if defined (WIN32)
/****************************************
* cg_socket_getlasterror (WIN32)
****************************************/
int cg_socket_getlasterror()
{
	return WSAGetLastError();
};
#endif
