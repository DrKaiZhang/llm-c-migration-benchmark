#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for config.h */
#ifndef STUB_CONFIG_H_03461DB8
#define STUB_CONFIG_H_03461DB8
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_config_h_03461db8;
#endif
/* stub for mosquitto_broker_internal.h */
#ifndef STUB_MOSQUITTO_BROKER_INTERNAL_H_4F1724E7
#define STUB_MOSQUITTO_BROKER_INTERNAL_H_4F1724E7
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_mosquitto_broker_internal_h_4f1724e7;
#endif
/* stub for mqtt_protocol.h */
#ifndef STUB_MQTT_PROTOCOL_H_A954DA7A
#define STUB_MQTT_PROTOCOL_H_A954DA7A
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_mqtt_protocol_h_a954da7a;
#endif
/* stub for property_mosq.h */
#ifndef STUB_PROPERTY_MOSQ_H_4CFBC606
#define STUB_PROPERTY_MOSQ_H_4CFBC606
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_property_mosq_h_4cfbc606;
#endif
#endif

/*
Copyright (c) 2018 Roger Light <roger@atchoo.org>

All rights reserved. This program and the accompanying materials
are made available under the terms of the Eclipse Public License v1.0
and Eclipse Distribution License v1.0 which accompany this distribution.
 
The Eclipse Public License is available at
   http://www.eclipse.org/legal/epl-v10.html
and the Eclipse Distribution License is available at
  http://www.eclipse.org/org/documents/edl-v10.php.
 
Contributors:
   Roger Light - initial implementation and documentation.
*/


#include <assert.h>
#include <stdio.h>
#include <string.h>


/* Process the incoming properties, we should be able to assume that only valid
 * properties for CONNECT are present here. */
int property__process_connect(struct mosquitto *context, mosquitto_property *props)
{
	mosquitto_property *p;

	p = props;

	while(p){
		if(p->identifier == MQTT_PROP_SESSION_EXPIRY_INTERVAL){
			context->session_expiry_interval = p->value.i32;
		}
		p = p->next;
	}

	return MOSQ_ERR_SUCCESS;
}

/* Process the incoming properties, we should be able to assume that only valid
 * properties for DISCONNECT are present here. */
int property__process_disconnect(struct mosquitto *context, mosquitto_property *props)
{
	mosquitto_property *p;

	p = props;

	while(p){
		if(p->identifier == MQTT_PROP_SESSION_EXPIRY_INTERVAL){
			if(context->session_expiry_interval == 0 && p->value.i32 != 0){
				return MOSQ_ERR_PROTOCOL;
			}
			context->session_expiry_interval = p->value.i32;
		}
		p = p->next;
	}
	return MOSQ_ERR_SUCCESS;
}

