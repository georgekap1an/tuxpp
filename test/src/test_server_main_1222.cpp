#include <stdio.h>
#include <xa.h>
#include <atmi.h>

#if defined(__cplusplus)
extern "C" {
#endif
extern int _tmrunserver _((int));
extern void BAD_SVC _((TPSVCINFO *));
extern void CALC _((TPSVCINFO *));
extern void ECHO_CLIENTID _((TPSVCINFO *));
extern void ECHO_XML _((TPSVCINFO *));
extern void FORWARDING_SVC _((TPSVCINFO *));
extern void FORWARD_TARGET _((TPSVCINFO *));
extern void HIDE_SECRET _((TPSVCINFO *));
extern void NOTIFY _((TPSVCINFO *));
extern void NO_REPLY_SVC _((TPSVCINFO *));
extern void REVEAL_SECRET _((TPSVCINFO *));
extern void REVERSE _((TPSVCINFO *));
extern void SECRET_SVC _((TPSVCINFO *));
extern void SLOW_TOUPPER _((TPSVCINFO *));
extern void TOUPPER _((TPSVCINFO *));
extern void TRIGGER_BROADCAST _((TPSVCINFO *));
extern void TRIGGER_NOTIFY _((TPSVCINFO *));
extern void TRIGGER_NOTIFY_TWICE _((TPSVCINFO *));
extern void VERY_SLOW_SVC _((TPSVCINFO *));
#if defined(__cplusplus)
}
#endif

static struct tmdsptchtbl_t _tmdsptchtbl[] = {
	{ (char*)"BAD_SVC", (char*)"BAD_SVC", (void (*) _((TPSVCINFO *))) BAD_SVC, 0, 0 },
	{ (char*)"CALC", (char*)"CALC", (void (*) _((TPSVCINFO *))) CALC, 1, 0 },
	{ (char*)"ECHO_CLIENTID", (char*)"ECHO_CLIENTID", (void (*) _((TPSVCINFO *))) ECHO_CLIENTID, 2, 0 },
	{ (char*)"ECHO_XML", (char*)"ECHO_XML", (void (*) _((TPSVCINFO *))) ECHO_XML, 3, 0 },
	{ (char*)"FORWARDING_SVC", (char*)"FORWARDING_SVC", (void (*) _((TPSVCINFO *))) FORWARDING_SVC, 4, 0 },
	{ (char*)"FORWARD_TARGET", (char*)"FORWARD_TARGET", (void (*) _((TPSVCINFO *))) FORWARD_TARGET, 5, 0 },
	{ (char*)"HIDE_SECRET", (char*)"HIDE_SECRET", (void (*) _((TPSVCINFO *))) HIDE_SECRET, 6, 0 },
	{ (char*)"", (char*)"NOTIFY", (void (*) _((TPSVCINFO *))) NOTIFY, 7, 0 },
	{ (char*)"NO_REPLY_SVC", (char*)"NO_REPLY_SVC", (void (*) _((TPSVCINFO *))) NO_REPLY_SVC, 8, 0 },
	{ (char*)"REVEAL_SECRET", (char*)"REVEAL_SECRET", (void (*) _((TPSVCINFO *))) REVEAL_SECRET, 9, 0 },
	{ (char*)"REVERSE", (char*)"REVERSE", (void (*) _((TPSVCINFO *))) REVERSE, 10, 0 },
	{ (char*)"", (char*)"SECRET_SVC", (void (*) _((TPSVCINFO *))) SECRET_SVC, 11, 0 },
	{ (char*)"SLOW_TOUPPER", (char*)"SLOW_TOUPPER", (void (*) _((TPSVCINFO *))) SLOW_TOUPPER, 12, 0 },
	{ (char*)"TOUPPER", (char*)"TOUPPER", (void (*) _((TPSVCINFO *))) TOUPPER, 13, 0 },
	{ (char*)"TRIGGER_BROADCAST", (char*)"TRIGGER_BROADCAST", (void (*) _((TPSVCINFO *))) TRIGGER_BROADCAST, 14, 0 },
	{ (char*)"TRIGGER_NOTIFY", (char*)"TRIGGER_NOTIFY", (void (*) _((TPSVCINFO *))) TRIGGER_NOTIFY, 15, 0 },
	{ (char*)"TRIGGER_NOTIFY_TWICE", (char*)"TRIGGER_NOTIFY_TWICE", (void (*) _((TPSVCINFO *))) TRIGGER_NOTIFY_TWICE, 16, 0 },
	{ (char*)"VERY_SLOW_SVC", (char*)"VERY_SLOW_SVC", (void (*) _((TPSVCINFO *))) VERY_SLOW_SVC, 17, 0 },
	{ NULL, NULL, NULL, 0, 0 }
};

#ifndef _TMDLLIMPORT
#define _TMDLLIMPORT
#endif

#if defined(__cplusplus)
extern "C" {
#endif
_TMDLLIMPORT extern struct xa_switch_t tmnull_switch;
#if defined(__cplusplus)
}
#endif

typedef void (*tmp_void_cast)(void);
typedef void (*tmp_voidvoid_cast)(void);
typedef int (*tmp_intchar_cast)(int, char **); 
typedef int (*tmp_int_cast)(int);
typedef int (*tmp_charvoid_cast)(char*, void *); 
static struct tmsvrargs_t tmsvrargs = {
	NULL,
	&_tmdsptchtbl[0],
	0,
	(tmp_intchar_cast)tpsvrinit,
	(tmp_voidvoid_cast)tpsvrdone,
	(tmp_int_cast)_tmrunserver,	/* PRIVATE  */
	NULL,			/* RESERVED */
	NULL,			/* RESERVED */
	NULL,			/* RESERVED */
	NULL,			/* RESERVED */
	(tmp_charvoid_cast)tprminit,
	(tmp_intchar_cast)tpsvrthrinit,
	(tmp_voidvoid_cast)tpsvrthrdone
};

struct tmsvrargs_t *
#ifdef _TMPROTOTYPES
_tmgetsvrargs(void)
#else
_tmgetsvrargs()
#endif
{
	tmsvrargs.reserved1 = NULL;
	tmsvrargs.reserved2 = NULL;
	tmsvrargs.xa_switch = &tmnull_switch;
	return(&tmsvrargs);
}

int
#ifdef _TMPROTOTYPES
main(int argc, char **argv)
#else
main(argc,argv)
int argc;
char **argv;
#endif
{
#ifdef TMMAINEXIT
#include "mainexit.h"
#endif

	return( _tmstartserver( argc, argv, _tmgetsvrargs()));
}

