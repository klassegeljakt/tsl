/******************************************************************************
*     File Name           :     tsl.h                                         *
*     Created By          :     Klas                                          *
*     Creation Date       :     [2016-06-15 18:05]                            *
*     Last Modified       :     [2016-06-15 21:37]                            *
*     Description         :     tsl finds info on SL departures and arrivals. *
******************************************************************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "nxjson/nxjson.h"
/****************************************************************************/
/* Connection */
#define SL_IP "194.68.78.66" /* IP of server */
#define HOST_NAME "api.sl.se" /* Name of server */
#define PORT 80 /* Port to connect */
/* HTTP GET request */
#define FORMAT "json" /* Requested format, can either be json or xml */
#define API_KEY "<your API key>"
#define MIN_CHG "0" /* Min */
#define ORIGIN_ID "Duvbo" /* Default origin station */
#define DEST_ID "Universitetet" /* Default target station */
/* Buffers */
#define MESSAGE_SIZE 2000 /* Size of buffer storing http get request */
#define RESPONSE_SIZE 200000 /* Size of buffer storing response */
/*****************************************************************************/
typedef struct trip trip;
typedef struct edge edge;
typedef struct station station;
/*****************************************************************************/
enum _tsl_error {
	E_UNKNOWN = -1,
	E_CONNECT = -2,
	E_RECEIVE
};
/******************************************************************************
 * Function: main                                                             *
 * --------------                                                             *
 *   Main function of the program.                                            *
 *                                                                            *
 *   Input parameters:                                                        *
 *     Required: <Origin> <Destination>                                       *
 *     Optional:                                                              *
 *****************************************************************************/
int main(int argc, char *argv[]);
/******************************************************************************
 * Function: get_request                                                      *
 * ---------------------                                                      *
 *   Sends a http GET request to the SL server to determine the               *
 *   travel path.                                                             *
 *                                                                            *
 *   js: Pointer to buffer where response is stored.                          *
 *   origin: Station where travel starts.                                     *
 *   dest: Station where travel ends.                                         *
 *                                                                            *
 *   Returns: The number of bytes received.                                   *
 *            E_CONNECT when connect fails.                                   *
 *            E_RECEIVE when recv fails.                                      *
 *****************************************************************************/
int get_request(char **js, char *origin, char *dest);
/******************************************************************************
 * Function: extract_js                                                       *
 * --------------------                                                       *
 *   Extracts json code from a string.                                        *
 *                                                                            *
 *   js: Pointer to buffer where input and output string are stored.          *
 *   len: Length of the input string.                                         *
 *                                                                            *
 *   Returns: E_SUCCESS if succesful.                                         *
 *****************************************************************************/
int extract_js(char **js, int len);
/******************************************************************************
 * Function: extract                                                          *
 * -----------------                                                          *
 *   Extracts data of trips from a nx_json hashmap.                           *
 *                                                                            *
 *   jsmap: Pointer to hashmap of json data.                                  *
 *   trips: Pointer to pointer of array of trips, is initially unallocated.   *
 *                                                                            *
 *   Returns: Number of trips that were found in the json.                    *
 *****************************************************************************/
int extract_trips(const nx_json *jsmap, trip **trip_head);
/******************************************************************************
 * Function: extract_trip                                                     *
 * ----------------------                                                     *
 *   Extract data of a trip from a nx_json hashmap.                           *
 *                                                                            *
 *   js_trip: Pointer to hashmap of json data.                                *
 *   trip: Pointer to struct where trip data i stored.                        *
 *                                                                            *
 *   Returns: Number of edges that were found in the trip.                    *
 *****************************************************************************/
int extract_trip(const nx_json *js_trip, trip *new_trip);
/******************************************************************************
 * Function: extract_edge                                                     *
 * ----------------------                                                     *
 *   Extract data of an edge from a nx_json hashmap.                          *
 *                                                                            *
 *   js_edge: Pointer to hashmap of json data.                                *
 *   trip: Pointer to struct where edge data is stored.                       *
 *                                                                            *
 *   Returns: E_SUCCESS if successful.                                        *
 *****************************************************************************/
int extract_edge(const nx_json *js_edge, edge *new_edge);
/******************************************************************************
 * Function: extract_station                                                  *
 * -------------------------                                                  *
 *   Extracts data of a station from a nx_jsonn hashmap.                      *
 *                                                                            *
 *   js_station: Pointer to hashmap of json data.                             *
 *   trip: Pointer to struct where station data is stored.                    *
 *                                                                            *
 *   Returns: E_SUCCESS if successful.                                        *
 *****************************************************************************/
int extract_station(const nx_json *js_station, station *new_station);
/******************************************************************************
 * Function: print_trips                                                      *
 * ---------------------                                                      *
 *   Prints data of trips.                                                    *
 *                                                                            *
 *   trips: Pointer to array of trips.                                        *
 *   num_trips: Size of the trips array.                                      *
 *                                                                            *
 *   Returns: E_SUCCESS if successful.                                        *
 *****************************************************************************/
int print_trips(trip *trips, int num_trips);
/*****************************************************************************/
