/**
* Mobius Software LTD
* Copyright 2015-2018, Mobius Software LTD
*
* This is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License as
* published by the Free Software Foundation; either version 2.1 of
* the License, or (at your option) any later version.
*
* This software is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this software; if not, write to the Free
* Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
* 02110-1301 USA, or see the FSF site: http://www.fsf.org.
*/
#include <stdlib.h>
#include <stdio.h>
#include "../net/lws_net_client.h"
#include "../account.h"
#include "../tcp_listener.h"
#include "../mqtt_listener.h"
#include "../map/map.h"
#include "../mqtt/mqtt_timers.h"
#include "../mqtt/packets/connack.h"
#include "../mqtt/packets/connect.h"
#include "../mqtt/packets/puback.h"
#include "../mqtt/packets/pubcomp.h"
#include "../mqtt/packets/publish.h"
#include "../mqtt/packets/pubrec.h"
#include "../mqtt/packets/pubrel.h"
#include "../mqtt/packets/suback.h"
#include "../mqtt/packets/subscribe.h"
#include "../mqtt/packets/unsuback.h"
#include "../mqtt/packets/unsubscribe.h"
#include "../mqtt/parser.h"
#include "../ws/ws_parser.h"

void process_rx(char * data, int length);
void send_ping(void);
void send_connect(struct Account * account);
void send_subscribe(const char * topic_name, int qos);
void send_unsubscribe(const char * topic_name);
void send_disconnect(void);
void send_publish(const char * content, const char * topic_name, int qos, int retain, int dup);
void mqtt_data_received(char * buf, int readable_bytes);

static struct MqttListener * mqtt_listener = NULL;
static struct Account * account = NULL;
static int current_packet_number = 0;
struct TcpListener * tcp_listener;
unsigned int delay_in_seconds = 10;

void encode_and_fire(struct Message * message) {

	if(account->protocol == MQTT) {
		int length = get_length(message);
		int total_length = length + 2;
		char * buf = encode(message, length);
		raw_fire(buf,total_length);
	} else {
		char * s = ws_encode(message);
		fire(s);
	}
}

int init_mqtt_client(struct Account * acc, struct MqttListener * listener) {

	account = acc;
	mqtt_listener = listener;
	mqtt_listener->send_connect = send_connect;
	mqtt_listener->send_sub = send_subscribe;
	mqtt_listener->send_disconnect = send_disconnect;
	mqtt_listener->send_message = send_publish;
	mqtt_listener->send_unsubscribe = send_unsubscribe;
	tcp_listener = malloc (sizeof (struct TcpListener));
	tcp_listener->prd_pt = mqtt_data_received;
	int is_successful = open_lws_net_connection(acc->server_host, acc->server_port, tcp_listener, acc->is_secure, acc->certificate, acc->certificate_password, acc->protocol);

	if (is_successful >= 0)
		printf("MQTT client successfully connected with transport %s\n", acc->protocol == MQTT ? "TCP" : "WEBSOCKETS" );
	else
		printf("MQTT client NOT connected with transport %s\n", acc->protocol == MQTT ? "TCP" : "WEBSOCKETS" );
	return is_successful;

}

void send_connect(struct Account * account) {
	//prepare connect packet
	delay_in_seconds = account->keep_alive;
	struct Connect * connect_packet = malloc (sizeof (struct Connect));
	connect_packet->username = account->username;
	connect_packet->password = account->password;
	connect_packet->client_id = account->client_id;
	connect_packet->clean_session = account->clean_session;
	connect_packet->keepalive = account->keep_alive;
	connect_packet->protocol_level = 4;
	if(account->will != NULL && account->will_topic != NULL)
		connect_packet->will_flag = 1;
	else
		connect_packet->will_flag = 0;
	connect_packet->will.topic.topic_name = account->will_topic;
	connect_packet->will.topic.qos = account->qos;
	connect_packet->will.content = account->will;
	connect_packet->will.retain = account->is_retain;
	struct Message * message = malloc (sizeof (struct Message));
	message->message_type = 1;
	message->packet = connect_packet;

	encode_and_fire(message);
	start_connect_timer();

	add_message_in_map(message);

}

void send_subscribe(const char * topic_name, int qos) {

	struct Subscribe * subscribe = NULL;
	subscribe = malloc (sizeof (struct Subscribe));
	subscribe->topics = malloc (sizeof (struct Topic)*1);
	subscribe->topics->qos = qos;
	subscribe->topics->topic_name = malloc (sizeof (char)*strlen(topic_name));
	subscribe->topics->topic_name = topic_name;
	subscribe->packet_id = ++current_packet_number;
	subscribe->topics_number = 1;//currently support only one topic
	struct Message * message = malloc (sizeof (struct Message));
	message->message_type = 8;
	message->packet = subscribe;

	add_message_in_map(message);

	encode_and_fire(message);

}

void send_unsubscribe(const char * topic_name) {

	struct Unsubscribe * unsubscribe = malloc (sizeof (struct Unsubscribe));
	unsubscribe->topics = malloc (sizeof (struct Topic)*1);
	unsubscribe->topics->topic_name = malloc (sizeof (char)*strlen(topic_name));
	unsubscribe->topics->topic_name = topic_name;

	unsubscribe->packet_id = ++current_packet_number;
	unsubscribe->topics_number = 1;

	struct Message * message = malloc (sizeof (struct Message));
	message->message_type = 10;
	message->packet = unsubscribe;

	add_message_in_map(message);

	encode_and_fire(message);

}

void send_publish(const char * content, const char * topic_name, int qos, int retain, int dup) {

	struct Publish * publish = malloc (sizeof (struct Publish));
	publish->content = content;
	publish->topic.topic_name = topic_name;
	publish->topic.qos = qos;
	publish->dup = dup;
	publish->retain = retain;

	if(qos != AT_MOST_ONCE)
		publish->packet_id = ++current_packet_number;
	else
		publish->packet_id = 0;
	struct Message * message = malloc (sizeof (struct Message));
	message->message_type = PUBLISH;
	message->packet = publish;
	encode_and_fire(message);
	add_message_in_map(message);

}

void send_puback(int packet_id) {

	struct Puback * pa = malloc (sizeof (struct Puback));
	pa->packet_id = packet_id;

	struct Message * message = malloc (sizeof (struct Message));
	message->message_type = 4;
	message->packet = pa;

	encode_and_fire(message);
}

void send_pubrec(int message_id) {

	struct Pubrec * pr = malloc (sizeof (struct Pubrec));
	pr->packet_id = message_id;
	struct Message * message = malloc (sizeof (struct Message));
	message->message_type = 5;
	message->packet = pr;

	encode_and_fire(message);

}

void send_pubcomp(int message_id) {

	struct Pubcomp * pc = malloc (sizeof (struct Pubcomp));
	pc->packet_id = message_id;
	struct Message * message = malloc (sizeof (struct Message));
	message->message_type = 7;
	message->packet = pc;

	encode_and_fire(message);

}

static void send_pubrel(int message_id) {

	struct Pubrel * pr = malloc (sizeof (struct Pubrel));
	pr->packet_id = message_id;
	struct Message * message = malloc (sizeof (struct Message));
	message->message_type = 6;
	message->packet = pr;

	encode_and_fire(message);
}

void send_disconnect() {

	stop_all_timers();

	struct Message * message = malloc (sizeof (struct Message));
	message->message_type = 14;
	encode_and_fire(message);
	lws_close_tcp_connection();
}

void send_ping() {

	struct Message * message = malloc (sizeof (struct Message));
	message->message_type = 12;
	encode_and_fire(message);
}

void mqtt_data_received(char * buf, int readable_bytes) {
	int i = 0;
	int length = buf[1]+2;
	if(length < readable_bytes && account->protocol == MQTT) {
		do {
			char * next_bytes = malloc(length * sizeof(char));
			memcpy(next_bytes,&(buf[i]),length);
			process_rx(next_bytes, length);
			i += length;
			length = buf[i+1] + 2;
			} while (i < readable_bytes);
	} else {
		process_rx(buf, readable_bytes);
	}
}

void process_rx(char * data, int length) {

	struct Message * message = NULL;
	if(account->protocol == MQTT)
		message = decode(data);
	else
		message = ws_decode(data);

	enum MessageType type = message->message_type;
	switch(type) {

		case CONNACK: {
			stop_connect_timer();
			remove_message_from_map(0);
			struct Connack * ca = (struct Connack*) message->packet;
			if(ca->return_code == ACCEPTED) {
				//open new window
				mqtt_listener->cs_pt();
				start_mqtt_ping_timer((unsigned int)delay_in_seconds);
			} else {
				//connection unsuccessful
				mqtt_listener->cu_pt(ca->return_code);
				printf("MQTT client have got CONNACK with error : %i \n", ca->return_code);
			}

			break;
		}
		case PUBLISH: {
			break;
		}
		case PUBACK: {
			break;
		}
		case PUBREC: {
			break;
		}
		case PUBREL: {
			break;
		}
		case PUBCOMP: {
			break;
		}
		case SUBACK: {
			break;
		}
		case UNSUBACK: {
			break;
		}
		case PINGRESP:
			break;

		//if got unacceptable request/response close connection
		case CONNECT:
		case SUBSCRIBE:
		case UNSUBSCRIBE:
		case PINGREQ:
		case DISCONNECT: {

			printf("ERROR : MQTT client have got unacceptable packet : %i \n",type);
			send_disconnect();
			exit(-1);
		}
	}
}

void fin_mqtt_client() {

}


