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
Copyright (c) 2010-2019 Roger Light <roger@atchoo.org>

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




static struct sub__token *sub__topic_append(struct sub__token **tail, struct sub__token **topics, char *topic)
{
	struct sub__token *new_topic;

	if(!topic){
		return NULL;
	}
	new_topic = mosquitto__malloc(sizeof(struct sub__token));
	if(!new_topic){
		return NULL;
	}
	new_topic->next = NULL;
	new_topic->topic_len = strlen(topic);
	new_topic->topic = mosquitto__malloc(new_topic->topic_len+1);
	if(!new_topic->topic){
		mosquitto__free(new_topic);
		return NULL;
	}
	strncpy(new_topic->topic, topic, new_topic->topic_len+1);

	if(*tail){
		(*tail)->next = new_topic;
		*tail = (*tail)->next;
	}else{
		*topics = new_topic;
		*tail = new_topic;
	}
	return new_topic;
}


int sub__topic_tokenise(const char *subtopic, struct sub__token **topics)
{
	struct sub__token *new_topic, *tail = NULL;
	int len;
	int start, stop, tlen;
	int i;
	char *topic;

	assert(subtopic);
	assert(topics);

	if(subtopic[0] != '$'){
		new_topic = sub__topic_append(&tail, topics, "");
		if(!new_topic) goto cleanup;
	}

	len = strlen(subtopic);

	if(subtopic[0] == '/'){
		new_topic = sub__topic_append(&tail, topics, "");
		if(!new_topic) goto cleanup;

		start = 1;
	}else{
		start = 0;
	}

	stop = 0;
	for(i=start; i<len+1; i++){
		if(subtopic[i] == '/' || subtopic[i] == '\0'){
			stop = i;

			if(start != stop){
				tlen = stop-start;

				topic = mosquitto__malloc(tlen+1);
				if(!topic) goto cleanup;
				memcpy(topic, &subtopic[start], tlen);
				topic[tlen] = '\0';
				new_topic = sub__topic_append(&tail, topics, topic);
				mosquitto__free(topic);
			}else{
				new_topic = sub__topic_append(&tail, topics, "");
			}
			if(!new_topic) goto cleanup;
			start = i+1;
		}
	}

	return MOSQ_ERR_SUCCESS;

cleanup:
	tail = *topics;
	*topics = NULL;
	while(tail){
		mosquitto__free(tail->topic);
		new_topic = tail->next;
		mosquitto__free(tail);
		tail = new_topic;
	}
	return 1;
}


void sub__topic_tokens_free(struct sub__token *tokens)
{
	struct sub__token *tail;

	while(tokens){
		tail = tokens->next;
		mosquitto__free(tokens->topic);
		mosquitto__free(tokens);
		tokens = tail;
	}
}

