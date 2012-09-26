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
*	File: return "chttp.cpp
*
*	Revision:
*
*	01/25/05
*		- first revision
*
******************************************************************/

#include <cybergarage/http/chttp.h>
#include <cybergarage/util/clog.h>

static const char CG_HTTP_REASON_PHRASE_100[] = { CG_HTTP_REASON_100 };
static const char CG_HTTP_REASON_PHRASE_101[] = { CG_HTTP_REASON_101 };
static const char CG_HTTP_REASON_PHRASE_200[] = { CG_HTTP_REASON_200 };
static const char CG_HTTP_REASON_PHRASE_201[] = { CG_HTTP_REASON_201 };
static const char CG_HTTP_REASON_PHRASE_202[] = { CG_HTTP_REASON_202 };
static const char CG_HTTP_REASON_PHRASE_203[] = { CG_HTTP_REASON_203 };
static const char CG_HTTP_REASON_PHRASE_204[] = { CG_HTTP_REASON_204 };
static const char CG_HTTP_REASON_PHRASE_205[] = { CG_HTTP_REASON_205 };
static const char CG_HTTP_REASON_PHRASE_206[] = { CG_HTTP_REASON_206 };
static const char CG_HTTP_REASON_PHRASE_300[] = { CG_HTTP_REASON_300 };
static const char CG_HTTP_REASON_PHRASE_301[] = { CG_HTTP_REASON_301 };
static const char CG_HTTP_REASON_PHRASE_302[] = { CG_HTTP_REASON_302 };
static const char CG_HTTP_REASON_PHRASE_303[] = { CG_HTTP_REASON_303 };
static const char CG_HTTP_REASON_PHRASE_304[] = { CG_HTTP_REASON_304 };
static const char CG_HTTP_REASON_PHRASE_305[] = { CG_HTTP_REASON_305 };
static const char CG_HTTP_REASON_PHRASE_307[] = { CG_HTTP_REASON_307 };
static const char CG_HTTP_REASON_PHRASE_400[] = { CG_HTTP_REASON_400 };
static const char CG_HTTP_REASON_PHRASE_401[] = { CG_HTTP_REASON_401 };
static const char CG_HTTP_REASON_PHRASE_402[] = { CG_HTTP_REASON_402 };
static const char CG_HTTP_REASON_PHRASE_403[] = { CG_HTTP_REASON_403 };
static const char CG_HTTP_REASON_PHRASE_404[] = { CG_HTTP_REASON_404 };
static const char CG_HTTP_REASON_PHRASE_405[] = { CG_HTTP_REASON_405 };
static const char CG_HTTP_REASON_PHRASE_406[] = { CG_HTTP_REASON_406 };
static const char CG_HTTP_REASON_PHRASE_407[] = { CG_HTTP_REASON_407 };
static const char CG_HTTP_REASON_PHRASE_408[] = { CG_HTTP_REASON_408 };
static const char CG_HTTP_REASON_PHRASE_409[] = { CG_HTTP_REASON_409 };
static const char CG_HTTP_REASON_PHRASE_410[] = { CG_HTTP_REASON_410 };
static const char CG_HTTP_REASON_PHRASE_411[] = { CG_HTTP_REASON_411 };
static const char CG_HTTP_REASON_PHRASE_412[] = { CG_HTTP_REASON_412 };
static const char CG_HTTP_REASON_PHRASE_413[] = { CG_HTTP_REASON_413 };
static const char CG_HTTP_REASON_PHRASE_414[] = { CG_HTTP_REASON_414 };
static const char CG_HTTP_REASON_PHRASE_415[] = { CG_HTTP_REASON_415 };
static const char CG_HTTP_REASON_PHRASE_416[] = { CG_HTTP_REASON_416 };
static const char CG_HTTP_REASON_PHRASE_417[] = { CG_HTTP_REASON_417 };
static const char CG_HTTP_REASON_PHRASE_500[] = { CG_HTTP_REASON_500 };
static const char CG_HTTP_REASON_PHRASE_501[] = { CG_HTTP_REASON_501 };
static const char CG_HTTP_REASON_PHRASE_502[] = { CG_HTTP_REASON_502 };
static const char CG_HTTP_REASON_PHRASE_503[] = { CG_HTTP_REASON_503 };
static const char CG_HTTP_REASON_PHRASE_504[] = { CG_HTTP_REASON_504 };
static const char CG_HTTP_REASON_PHRASE_505[] = { CG_HTTP_REASON_505 };
static const char CG_HTTP_REASON_PHRASE_UNKNOWN[] = { "Unknown" };

/**
 * Convert an HTTP status code to a reason phrase (ex. 200 -> "OK")
 *
 * @param code The HTTP status code to convert
 * @return The reason phrase (do not modify the pointer's contents)
 */
const char *cg_http_statuscode2reasonphrase(const int code)
{
	cg_log_debug_l4("Entering...\n");

	switch (code) 
	{
		case 100: return CG_HTTP_REASON_PHRASE_100;
		case 101: return CG_HTTP_REASON_PHRASE_101;
		case 200: return CG_HTTP_REASON_PHRASE_200;
		case 201: return CG_HTTP_REASON_PHRASE_201;
		case 202: return CG_HTTP_REASON_PHRASE_202;
		case 203: return CG_HTTP_REASON_PHRASE_203;
		case 204: return CG_HTTP_REASON_PHRASE_204;
		case 205: return CG_HTTP_REASON_PHRASE_205;
		case 206: return CG_HTTP_REASON_PHRASE_206;
		case 300: return CG_HTTP_REASON_PHRASE_300;
		case 301: return CG_HTTP_REASON_PHRASE_301;
		case 302: return CG_HTTP_REASON_PHRASE_302;
		case 303: return CG_HTTP_REASON_PHRASE_303;
		case 304: return CG_HTTP_REASON_PHRASE_304;
		case 305: return CG_HTTP_REASON_PHRASE_305;
		case 307: return CG_HTTP_REASON_PHRASE_307;
		case 400: return CG_HTTP_REASON_PHRASE_400;
		case 401: return CG_HTTP_REASON_PHRASE_401;
		case 402: return CG_HTTP_REASON_PHRASE_402;
		case 403: return CG_HTTP_REASON_PHRASE_403;
		case 404: return CG_HTTP_REASON_PHRASE_404;
		case 405: return CG_HTTP_REASON_PHRASE_405;
		case 406: return CG_HTTP_REASON_PHRASE_406;
		case 407: return CG_HTTP_REASON_PHRASE_407;
		case 408: return CG_HTTP_REASON_PHRASE_408;
		case 409: return CG_HTTP_REASON_PHRASE_409;
		case 410: return CG_HTTP_REASON_PHRASE_410;
		case 411: return CG_HTTP_REASON_PHRASE_411;
		case 412: return CG_HTTP_REASON_PHRASE_412;
		case 413: return CG_HTTP_REASON_PHRASE_413;
		case 414: return CG_HTTP_REASON_PHRASE_414;
		case 415: return CG_HTTP_REASON_PHRASE_415;
		case 416: return CG_HTTP_REASON_PHRASE_416;
		case 417: return CG_HTTP_REASON_PHRASE_417;
		case 500: return CG_HTTP_REASON_PHRASE_500;
		case 501: return CG_HTTP_REASON_PHRASE_501;
		case 502: return CG_HTTP_REASON_PHRASE_502;
		case 503: return CG_HTTP_REASON_PHRASE_503;
		case 504: return CG_HTTP_REASON_PHRASE_504;
		case 505: return CG_HTTP_REASON_PHRASE_505;
		default: return CG_HTTP_REASON_PHRASE_UNKNOWN;
	}

	cg_log_debug_l4("Leaving...\n");
}
