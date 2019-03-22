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

#include <pthread.h>
#include <limits.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <jansson.h>
#include "mqtt/mqtt_client.h"
#include "mqttsn/mqtt_sn_client.h"
#include "coap/coap_client.h"
#include "amqp/amqp_client.h"
#include "account.h"

#define MAXBUF 1024
#define DELIM ":"
#define COMMENT '#'
#define FILENAME "./config_file"
#define FILENAMETEMP "/sys/class/thermal/thermal_zone0/temp"

//enum Protocol current_protocol = 0;
struct MqttListener * mqtt_listener;
static struct Account * account;
const char * topic_name = NULL;
int message_qos = 0;
int message_retain = 0;
int message_dup = 0;

int period_message_resend = 60;
static pthread_t worker_thread;

static void connection_success(void);
static int open_tcp_connection(void);
static void load_config(void);
static void connection_unsuccessful(int cause);
static char * get_str_from_value(char * value);

void * send_message_task(void *threadid)
{
	while (1) {
		struct timeval  cur_time;
		gettimeofday(&cur_time, NULL);
		char time_string[128];
		strftime(time_string, 80, "%Y-%m-%d %H:%M:%S", localtime(&cur_time.tv_sec));
		//get temperature
		char temp_line[7];
		FILE *file = fopen (FILENAMETEMP, "r");
		float temperature = 0.0;
		if (file != NULL)
		{
			while(fgets(temp_line, sizeof(temp_line), file) != NULL)
			{
				temperature = atoi(temp_line)/1000;
				break;
			}
			fclose(file);
		}
		json_t *root = json_object();
		json_object_set_new( root, "time", json_string( time_string ) );
		json_object_set_new( root, "temperature", json_real( temperature ) );
		mqtt_listener->send_message(json_dumps(root, JSON_COMPACT), topic_name, message_qos, message_retain, message_dup);
		sleep(period_message_resend);
	}
	return 0;
}

int main (int argc, char **argv)
{
	load_config();

	mqtt_listener = malloc (sizeof (struct MqttListener));
	mqtt_listener->cs_pt = connection_success;
	mqtt_listener->cu_pt = connection_unsuccessful;
	LOOP:
	if(open_tcp_connection()) {
		mqtt_listener->send_connect(account);
	} else {
		sleep(5);
		goto LOOP;
	}

	while(1) {
		sleep(10);
	}

  return 0;
}

static int open_tcp_connection() {

	int is_succesful = 0;
	switch (account->protocol) {
			case MQTT : {
				if (init_mqtt_client(account, mqtt_listener) != 0) {
					printf("MQTT client : connection failed!!!\n");
				} else {
					is_succesful = 1;
				}

				break;
			}
			case MQTT_SN : {
				if (init_mqtt_sn_client(account, mqtt_listener) != 0) {
					printf("MQTT-SN client : connection failed!!!\n");
				} else {
					is_succesful = 1;
				}
				break;
			}
			case COAP: {
				if (init_coap_client(account, mqtt_listener) != 0) {
					printf("COAP client : connection failed!!!\n");
				} else {
					is_succesful = 1;
				}
				break;
			}
			case AMQP : {
				if (init_amqp_client(account, mqtt_listener) != 0) {
					printf("AMQP client : connection failed!!!\n");
				} else {
					is_succesful = 1;
				}
				break;
			}
			case WEBSOCKETS : {
				if (init_mqtt_client(account, mqtt_listener) != 0) {
					printf("MQTT-WS client : connection failed!!!\n");
				} else {
					is_succesful = 1;
				}
				break;
			}
			default : {
				printf("Unsupported protocol : %i \n", account->protocol);
				exit(1);
			}
		}
	return is_succesful;
}

static void load_config() {

	account = malloc (sizeof (struct Account));
	account->protocol = 0;
	account->username = NULL;
	account->password = NULL;
	account->client_id = NULL;
	account->server_host = NULL;
	account->server_port = 0;
	account->keep_alive = 0;
	account->will = NULL;
	account->will_topic = NULL;
	account->is_retain = 0;
	account->qos = 0;
	account->is_secure = 0;
	account->certificate = NULL;
	account->certificate_password = NULL;

	FILE *file = fopen (FILENAME, "r");

	if (file != NULL)
	{
		char line[MAXBUF];
		char _value[MAXBUF];
		char _key[MAXBUF];

		while(fgets(line, sizeof(line), file) != NULL)
		{
			if(line[0] == COMMENT || line[0] == 10 || line[0] == 32)
				continue;

			strcpy(_value, line);
			strcpy(_key, line);
			char *value;
			char *key;
			key = strtok((char *)_key,DELIM);
			value = strstr((char *)_value, DELIM);
			value = value + strlen(DELIM);
			//printf("%s = %s\n", key, value);
			if (strncasecmp(key, "protocol",8) == 0)
			{
				account->protocol = atoi(value);
			}
			else if (strncasecmp(key, "username",8) == 0 && value[0] != 10)
			{
				account->username = malloc(strlen(value)*sizeof(char));
				char * tmp_str = get_str_from_value(value);
				memcpy((char*)account->username,tmp_str,strlen(value));
				free(tmp_str);
			}
			else if (strncasecmp(key, "password",8) == 0 && value[0] != 10)
			{
				account->password = malloc(strlen(value)*sizeof(char));
				char * tmp_str = get_str_from_value(value);
				memcpy((char*)account->password,tmp_str,strlen(value));
				free(tmp_str);
			}
			else if (strncasecmp(key, "clientId",8) == 0)
			{
				account->client_id = malloc(strlen(value)*sizeof(char));
				char * tmp_str = get_str_from_value(value);
				memcpy((char*)account->client_id,tmp_str,strlen(value));
				free(tmp_str);
			}
			else if (strncasecmp(key, "host",4) == 0)
			{
				account->server_host = malloc(strlen(value)*sizeof(char));
				char * tmp_str = get_str_from_value(value);
				memcpy((char*)account->server_host,tmp_str,strlen(value));
				free(tmp_str);
			}
			else if (strncasecmp(key, "port",4) == 0)
			{
				account->server_port = atoi(value);
			}
			else if (strncasecmp(key, "cleanSession",12) == 0)
			{
				char * value_str = get_str_from_value(value);
				if(strncasecmp(value_str, "true",4) == 0)
					account->clean_session = 1;
				else
					account->clean_session = 0;
				free(value_str);
			}
			else if (strncasecmp(key, "keepAlive",9) == 0)
			{
				account->keep_alive = atoi(value);
			}
			else if (strncasecmp(key, "will",4) == 0 && value[0] != 10)
			{
				account->will = malloc(strlen(value)*sizeof(char));
				char * tmp_str = get_str_from_value(value);
				memcpy((char*)account->will,tmp_str,strlen(value));
				free(tmp_str);
			}
			else if (strncasecmp(key, "willTopic",9) == 0&& value[0] != 10)
			{
				account->will_topic = malloc(strlen(value)*sizeof(char));
				char * tmp_str = get_str_from_value(value);
				memcpy((char*)account->will_topic,tmp_str,strlen(value));
				free(tmp_str);
			}
			else if (strncasecmp(key, "retain",6) == 0)
			{
				char * value_str = get_str_from_value(value);
				if(strncasecmp(value_str, "true",4) == 0)
					account->is_retain = 1;
				else
					account->is_retain = 0;
				free(value_str);
			}
			else if (strncasecmp(key, "qos",3) == 0)
			{
				account->qos = atoi(value);
			}
			else if (strncasecmp(key, "isSecure",8) == 0)
			{
				char * value_str = get_str_from_value(value);
				if(strncasecmp(value_str, "true",4) == 0)
					account->is_secure = 1;
				else
					account->is_secure = 0;
				free(value_str);
			}
			else if (strncasecmp(key, "certKeyPath",11) == 0 && value[0] != 10)
			{
				account->certificate = malloc(strlen(value)*sizeof(char));
				char * tmp_str = get_str_from_value(value);
				memcpy((char*)account->certificate,tmp_str,strlen(value));
				free(tmp_str);

			}
			else if (strncasecmp(key, "certPassword",12) == 0 && value[0] != 10)
			{
				account->certificate_password = malloc(strlen(value)*sizeof(char));
				char * tmp_str  = get_str_from_value(value);
				memcpy((char*)account->certificate_password,tmp_str,strlen(value));
				free(tmp_str);
			}
			else if (strncasecmp(key, "topicName",9) == 0)
			{
				topic_name = malloc(strlen(value)*sizeof(char));
				char * tmp_str = get_str_from_value(value);
				memcpy((char*)topic_name,tmp_str,strlen(value));
				free(tmp_str);
			}
			else if (strncasecmp(key, "period",6) == 0)
			{
			  period_message_resend = atoi(value);
			}
			else if (strncasecmp(key, "messageQos",10) == 0)
			{
				message_qos = atoi(value);
			}
			else if (strncasecmp(key, "messageRetain",13) == 0)
			{
				char * value_str = get_str_from_value(value);
				if(strncasecmp(value_str, "true",4) == 0)
					message_retain = 1;
				else
					message_retain = 0;
				free(value_str);
			}
			else if (strncasecmp(key, "messageDup",10) == 0)
			{
				char * value_str = get_str_from_value(value);
				if(strncasecmp(value_str, "true",4) == 0)
					message_dup = 1;
				else
					message_dup = 0;
				free(value_str);
			}
			else
			{
				if(value[0] != 10)
					printf("I don't know this property : %s\n", key);
			}
		}
		fclose(file);
		//validation

		if(account->keep_alive < 0 || account->keep_alive > USHRT_MAX) {
			printf("keepalive must be >= 0 and < 65535\n");
			exit(1);
		}
		if((account->will == NULL && account->will_topic != NULL) || (account->will != NULL && account->will_topic == NULL)) {
			printf("Will and will topic both MUST be present or absent\n");
			exit(1);
		}
		if(account->server_port < 1 || account->server_port > USHRT_MAX) {
			printf("Port must be > 0 and < 65535\n");
			exit(1);
		}

	} else {
		printf("Cannot find file and start application. Application terminated\n");
		exit(1);
	}
}

static char * get_str_from_value(char * value) {
	char * tmp_str = malloc(strlen(value)*sizeof(char));
	memcpy(tmp_str,value,(strlen(value)-1));
	tmp_str[strlen(value)] = '\0';
	return tmp_str;

}

static void connection_success() {

	long t = 1000;
	int rc = pthread_create(&worker_thread, NULL, send_message_task, (void *)t);
	if (rc) {
		printf("ERROR; return code from pthread_create() is %d\n", rc);
		return ;
	 }
}

static void connection_unsuccessful(int cause) {

	printf("IoT connection unsuccessful with error : %i . The program terminated\n", cause);
	exit(1);
}


