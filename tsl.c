/******************************************************************************
*     File Name           :     tsl.h                                         *
*     Created By          :     Klas Segeljakt                                *
*     Creation Date       :     [2016-06-14 23:41]                            *
*     Last Modified       :     [2016-06-15 21:37]                            *
*     Description         :     tsl finds info on SL departures and arrivals. *
******************************************************************************/
#include "tsl.h"
/*****************************************************************************/
struct station {
	char *name;
	char *time;
};
struct edge {
	char *name;
	station origin;
	station destination;
};
struct trip {
	char *duration;
	int edges_len;
	edge *edges;
};
/*****************************************************************************/
int main(int argc, char *argv[]) {
	char *js;
	int retval;
	trip *trips;
	const nx_json *jsmap;

	if(argc < 2) {
		printf("tsl <Origin> <Destination>\n");
		return -1;
	}
	/* Get data from server */
	retval = get_request(&js, argv[1], argv[2]);
	/* Extract json from data */
	extract_js(&js, retval);
	/* Parse json */
	jsmap = nx_json_parse(js, 0);
	/* Free js since it will no longer be used */
	free(js);
	/* Extract properties */
	retval = extract_trips(jsmap, &trips);
	/* Print properties */
	print_trips(trips, retval);

	return 0;
}
/*****************************************************************************/
int get_request(char **js, char *origin, char *dest) {
	int sock_fd;
	struct sockaddr_in serv_addr;
	char *http_get = (char*) malloc(sizeof(char)*MESSAGE_SIZE);
	*js = (char*) malloc(sizeof(char)*RESPONSE_SIZE);
	//struct hostent *server = gethostbyname(HOST_NAME);

	sprintf(http_get,
			"GET http://api.sl.se/api2/travelplannerv2/trip.%s?"
			"key=%s&"		// API KEY
			"originId=%s&"	// originId
			"destId=%s "	// destId
			"HTTP/1.1\r\n"
			"Host: api.sl.se\r\n"
			"Connection: close\r\n"
			"\r\n",
			FORMAT, API_KEY, origin, dest);

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(SL_IP);
	serv_addr.sin_port = htons(PORT);
//	bzero(&(socket_details.sin_zero), 8);
	int recv_bytes = 0;
	if(connect(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) {
		perror("Error");
		free(http_get);
		free(*js);
		return E_CONNECT;
	}
	int sent_bytes;
	if((sent_bytes = write(sock_fd, http_get, strlen(http_get))) > 0) {
		int retval;
		do {
			retval = read(sock_fd, *js+recv_bytes, RESPONSE_SIZE-recv_bytes);
			if(retval == 0) {
				break;
			} else if(retval < 0) {
				perror("Error when receiving data");
				free(http_get);
				free(*js);
				return E_RECEIVE;
			} else {
				recv_bytes += retval;
			}
		} while(recv_bytes < RESPONSE_SIZE);
	} else {
		perror("Error");
	}

	free(http_get);
	close(sock_fd);

	return recv_bytes;
}
/*****************************************************************************/
int extract_js(char **js, int len) {
	int i;

	for(i = len-1;;i--) {
		if((*js)[i] == '}') {
			(*js)[i+1] = '\0';
			break;
		}
	}
	for(i = 0;;i++) {
		if((*js)[i] == '{') {
			memmove(*js, *js+i, strlen(*js));
			break;
		}
	}
	return 0;
}
/*****************************************************************************/
int extract_trips(const nx_json *jsmap, trip **trips) {
	int len;
	const nx_json *js_trip_list = nx_json_get(jsmap, "TripList");
	const nx_json *js_trip_array = nx_json_get(js_trip_list, "Trip");
	if(js_trip_array->type == NX_JSON_ARRAY) {
		len = js_trip_array->length;
		*trips = malloc(sizeof(trip)*len);
		int i;
		for(i = 0; i < len; i++) {
			const nx_json *js_trip = nx_json_item(js_trip_array, i);
			extract_trip(js_trip, &(*trips)[i]);
		}
	} else {
		len = 1;
		*trips = malloc(sizeof(trip));
		extract_trip(js_trip_array, &(*trips)[0]);
	}
	return len;
}
/*****************************************************************************/
int extract_trip(const nx_json *js_trip, trip *new_trip) {
	int len;
	const nx_json *js_edge_list = nx_json_get(js_trip, "LegList");
	const nx_json *js_edge_array = nx_json_get(js_edge_list, "Leg");
	new_trip->duration = strdup(nx_json_get(js_trip, "dur")->text_value);
	if(js_edge_array->type == NX_JSON_ARRAY) {
		len = js_edge_array->length;
		new_trip->edges = malloc(sizeof(edge)*len);
		int i;
		for(i = 0; i < len; i++) {
			const nx_json *js_edge = nx_json_item(js_edge_array, i);
			extract_edge(js_edge, &(new_trip->edges[i]));
		}
	} else {
		len = 1;
		new_trip->edges = malloc(sizeof(edge));
		extract_edge(js_edge_array, &(new_trip->edges[0]));
	}
	new_trip->edges_len = len;
	return len;
}
/*****************************************************************************/
int extract_edge(const nx_json *js_edge, edge *new_edge) {
	new_edge->name = strdup(nx_json_get(js_edge, "name")->text_value);
	const nx_json *js_origin_station = nx_json_get(js_edge, "Origin");
	const nx_json *js_dest_station = nx_json_get(js_edge, "Destination");
	extract_station(js_origin_station, &(new_edge->origin));
	extract_station(js_dest_station, &(new_edge->destination));
	return 0;
}
/*****************************************************************************/
int extract_station(const nx_json *js_station, station *new_station) {
	new_station->name = strdup(nx_json_get(js_station, "name")->text_value);
	new_station->time = strdup(nx_json_get(js_station, "time")->text_value);
	return 0;
}
/*****************************************************************************/
int print_trips(trip *trips, int num_trips) {
	int i, j/*, max, len*/;
/*	for(i = 0; i < num_trips; i++) {
		for(j = 0; j < trips[i].edges_len; j++) {
			if((len = strlen(trips[i].edges[j].origin.name)) > max) {
				max = len;
			}
			if((len = strlen(trips[i].edges[j].destination.name)) > max) {
				max = len;
			}
		}
	}*/
	for(i = 0; i < num_trips; i++) {
		printf("(%s min)\n", trips[i].duration);
		for(j = 0; j < trips[i].edges_len; j++) {
			station origin = trips[i].edges[j].origin;
			station destination = trips[i].edges[j].destination;

			int origin_len = strlen(origin.name),
				destination_len = strlen(destination.name);

			printf("  [%s : %s] ---{%s}---> [%s : %s]\n",
							origin.name, origin.time,
							trips[i].edges[j].name,
							destination.name, destination.time);
		}
	}
	return 0;
}
/*****************************************************************************/
