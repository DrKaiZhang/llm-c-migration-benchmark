/* Mosquitto :: src/topic_tok.c
   modern revision: 83f2620f0b955b89aadce32cceb96a8e81950f5e
   domain: industrial-messaging
*/
#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for config.h */
#ifndef STUB_CONFIG_H_03461DB8
#define STUB_CONFIG_H_03461DB8
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_config_h_03461db8;
#endif
/* stub for memory_mosq.h */
#ifndef STUB_MEMORY_MOSQ_H_B83B737B
#define STUB_MEMORY_MOSQ_H_B83B737B
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_memory_mosq_h_b83b737b;
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
/* stub for util_mosq.h */
#ifndef STUB_UTIL_MOSQ_H_CF5382EA
#define STUB_UTIL_MOSQ_H_CF5382EA
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_util_mosq_h_cf5382ea;
#endif
/* stub for utlist.h */
#ifndef STUB_UTLIST_H_6AA2FA60
#define STUB_UTLIST_H_6AA2FA60
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_utlist_h_6aa2fa60;
#endif
#endif

/*
Copyright (c) 2010-2021 Roger Light <roger@atchoo.org>

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
*/


#include <stdio.h>
#include <string.h>




static char *strtok_hier(char *str, char **saveptr)
{
	char *c;

	if(str != NULL){
		*saveptr = str;
	}

	if(*saveptr == NULL){
		return NULL;
	}

	c = strchr(*saveptr, '/');
	if(c){
		str = *saveptr;
		*saveptr = c+1;
		c[0] = '\0';
	}else if(*saveptr){
		/* No match, but surplus string */
		str = *saveptr;
		*saveptr = NULL;
	}
	return str;
}


int sub__topic_tokenise(const char *subtopic, char **local_sub, char ***topics, const char **sharename)
{
	char *saveptr = NULL;
	char *token;
	int count;
	int topic_index = 0;
	size_t len;

	if(!subtopic){
		return MOSQ_ERR_INVAL;
	}

	len = strlen(subtopic);
	if(len == 0){
		return MOSQ_ERR_INVAL;
	}

	*local_sub = mosquitto_strdup(subtopic);
	if((*local_sub) == NULL){
		return MOSQ_ERR_NOMEM;
	}

	count = 0;
	saveptr = *local_sub;
	while(saveptr){
		saveptr = strchr(&saveptr[1], '/');
		count++;
	}
	*topics = mosquitto_calloc((size_t)(count+3) /* 3=$shared,sharename,NULL */, sizeof(char *));
	if((*topics) == NULL){
		mosquitto_FREE(*local_sub);
		return MOSQ_ERR_NOMEM;
	}

	if((*local_sub)[0] != '$'){
		(*topics)[topic_index] = "";
		topic_index++;
	}

	token = strtok_hier((*local_sub), &saveptr);
	while(token){
		(*topics)[topic_index] = token;
		topic_index++;
		token = strtok_hier(NULL, &saveptr);
	}

	if(!strcmp((*topics)[0], "$share")){
		if(count < 3 || (count == 3 && strlen((*topics)[2]) == 0)){
			mosquitto_FREE(*local_sub);
			mosquitto_FREE(*topics);
			return MOSQ_ERR_PROTOCOL;
		}

		if(sharename){
			if(strpbrk((*topics)[1], "+#")){
				mosquitto_FREE(*local_sub);
				mosquitto_FREE(*topics);
				return MOSQ_ERR_PROTOCOL;
			}
			(*sharename) = (*topics)[1];
		}

		for(int i=1; i<count-1; i++){
			(*topics)[i] = (*topics)[i+1];
		}
		(*topics)[0] = "";
		(*topics)[count-1] = NULL;
	}
	return MOSQ_ERR_SUCCESS;
}
