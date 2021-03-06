#ifndef __t2u_h__
#define __t2u_h__

/**************************************************************************
 * normal tcp protocol:
 *     c1 ----- tcp ----- s1
 *     c2 ----- tcp ----- s2
 *
 *
 * over t2u mode:
 *     c1.0 ---+
 *             + -- tcp --- local client forwarder ---+
 *     c1.1 ---+                                      |
 *                                                    + one udp socket +
 *     c2.0 ---+                                      |                |
 *             + -- tcp --- local client forwarder ---+                |
 *     c2.1 ---+                                                    internet
 *                                                                     |
 *     s1 ====== tcp ====== local server forwarder ---+                |
 *                                                    + one udp socket +
 *     s2 ====== tcp ====== local server forwarder ---+
 *
 **************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>

#ifdef _MSC_VER
#include <Windows.h>
typedef SOCKET sock_t;
#else
typedef int sock_t;
#endif


/* forward rule mode, client or server */
typedef enum forward_mode_
{
    forward_client_mode,    /* tcp's client should work with a client forwarder together */
    forward_server_mode,    /* server forwarder */
} forward_mode;


/* forward context,
 * one context mapping to one udp socket pair */
typedef void *forward_context;


/* forward rule */
typedef void *forward_rule;


/* create a forward context with the udp socket pair
 * if using this in STUN mode. you need to STUN it by yourself.
 */
forward_context create_forward(sock_t s);


/*
 * destroy the context, and it's rules.
 * udp socket will not be closed, you should manage it by youself.
 */
void free_forward(forward_context c);


// timeout for udp packet wait response(ms). 10 - 30,000. default 500.
#define CTX_UDP_TIMEOUT (0x01)

// retries for resent udp packets. 0 - 20, default 3.
#define CTX_UDP_RETRIES (0x02)

// slide window for udp packets. 1 - 64, default 16.
#define CTX_UDP_SLIDEWINDOW (0x03)

// session timeout in seconds. 10 - 86400, default 900.
#define CTX_SESSION_TIMEOUT (0x04)


// udp debug option: simulate a delay, ms. default: 0.
#define CTX_UDP_DEBUG_DELAY (0xf0)

// udp debug option: simulate a packet loss. 1-10000. default: 0
#define CTX_UDP_DEBUG_PACKET_LOSS (0xf1)

// udp debug option: simulate bandwidth in bps. default: 0
#define CTX_UDP_DEBUG_BANDWIDTH (0xf2)

/*
 * forward context option
 */
void set_context_option(forward_context c, int option, unsigned long value);


/*
 * This function is useful for send data with the socket.
 * as socket it managed by the forward-context, if tou need send some data,
 * the only  way is using forward_send.
 */
int forward_send(sock_t s, const char *buffer, size_t length);


/*
 * unknown packets callback.
 * when context recv some unknown packet, it has a chance to forward to your callbakc handler.
 */
void set_unknown_callback(void(*unknown_cb)(forward_context c, const char *buffer, size_t length));

/*
 * error callback functions
 * when error ocupy, callback is called
 */
void set_error_callback(void(*error_cb)(forward_context c, forward_rule rule, int error_code, char *error_message));

/* log callback */
/* level: 0: debug, 1: info, 2: warning, 3: error */
void set_log_callback(void(*cb)(int level, const char *mess));

/* add a forward rule, return NULL if failed. */
forward_rule add_forward_rule(forward_context c,       /* context */
    forward_mode mode,       /* mode: client or server */
    const char *service,     /* server name, for identify the different tcp server on server side */
    const char *addr,        /* address, listen for client mode, forward address for client mode */
    unsigned short port);    /* port, listen for client mode, forward port for client mode */


/* remove a forward rule */
void del_forward_rule(forward_rule r);


/* debug current internal variables */
void debug_dump(FILE *fp);

#ifdef __cplusplus
};
#endif

#endif /* __t2u_h__ */
