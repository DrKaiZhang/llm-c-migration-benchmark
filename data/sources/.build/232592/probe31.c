#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for config.h */
#ifndef STUB_CONFIG_H_03461DB8
#define STUB_CONFIG_H_03461DB8
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_config_h_03461db8;
#endif
/* stub for mosquitto.h */
#ifndef STUB_MOSQUITTO_H_D4898EFF
#define STUB_MOSQUITTO_H_D4898EFF
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_mosquitto_h_d4898eff;
#endif
/* stub for systemd/sd-daemon.h */
#ifndef STUB_SYSTEMD_SD_DAEMON_H_627E2EE0
#define STUB_SYSTEMD_SD_DAEMON_H_627E2EE0
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_systemd_sd_daemon_h_627e2ee0;
#endif
#endif

/*
Copyright (c) 2009-2020 Roger Light <roger@atchoo.org>

All rights reserved. This program and the accompanying materials
are made available under the terms of the Eclipse Public License 2.0
and Eclipse Distribution License v1.0 which accompany this distribution.

The Eclipse Public License is available at
   https://www.eclipse.org/legal/epl-2.0/
and the Eclipse Distribution License is available at
  http://www.eclipse.org/org/documents/edl-v10.php.

SPDX-License-Identifier: EPL-2.0 OR BSD-3-Clause

Contributors:
   Roger Light - initial implementation and documentation.
   Tatsuzo Osawa - Add epoll.
*/


#include <time.h>

#ifdef WITH_SYSTEMD
#endif

#ifdef WITH_SYSTEMD
static time_t next_ping = 0;
static time_t ping_sec = 0;
#endif

void watchdog__init(void)
{
#ifdef WITH_SYSTEMD
	char *watchdog_usec = getenv("WATCHDOG_USEC");
	time_t next_ping = mosquitto_time();
	time_t ping_sec = 0;

	if(watchdog_usec){
		char *endptr = NULL;
		long usec = strtol(watchdog_usec, &endptr, 10);
		if(watchdog_usec[0] != '\0' && endptr[0] == '\0' && usec > 0){
			ping_sec = (usec / 1000000) / 2;
		}
		next_ping = mosquito_time();
	}
#endif
}

void watchdog__check(void)
{
#ifdef WITH_SYSTEMD
	if(ping_sec){
		time_t now = mosquitto_time();
		if(now > next_ping){
			sd_notify(0, "WATCHDOG=1");
			next_ping = now + ping_sec;
		}
	}
#endif
}
