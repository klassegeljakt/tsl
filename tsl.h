/******************************************************************************
*     File Name           :     tsl.h                                         *
*     Created By          :     Klas                                          *
*     Creation Date       :     [2016-06-15 18:05]                            *
*     Last Modified       :     [2016-06-16 14:23]                            *
*     Description         :     tsl finds info on SL departures and arrivals. *
******************************************************************************/
#include <sys/socket.h>         // socket, connect
#include <netinet/in.h>         // struct sockaddr_in, struct sockaddr
#include <string.h>             // strlen, memmove, strdup
#include <stdio.h>              // printf, sprintf
#include <stdlib.h>             // malloc, free
#include <arpa/inet.h>          // inet_addr
#include <unistd.h>             // read, write, close
#include "nxjson/nxjson.h"      // json parser
//#include <netdb.h>            // struct hostent, gethostbyname
/*****************************************************************************/
/* Connection */
#define SL_IP "194.68.78.66" 	// IP of server
#define HOST_NAME "api.sl.se" 	// Name of server
#define PORT 80 				// Port to connect
/* HTTP GET request */
#define FORMAT "json" 			// Requested format, either json or xml
#define API_KEY "<your API key>"// Authentication
#define MIN_CHG "0" 			// Min
#define ORIGIN_ID "Duvbo" 		// Default origin station
#define DEST_ID "Universitetet" // Default target station
/* Buffers */
#define MESSAGE_SIZE 2000 		// Size of buffer storing http get request
#define RESPONSE_SIZE 200000 	// Size of buffer storing response
/*****************************************************************************/
/* Error numbers */
enum tsl_error {
	E_SUCCESS = 0, 				// No errors occurred.
	E_UNKNOWN = -1,			 	// Unknwon error.
	E_CONNECT = -2,				// Could not establish connection to server.
	E_SEND = -3,				// Something went wrong when sending request.
	E_RECEIVE = -4, 			// Error when receiving data.
	E_RESPONSE = -5,			// Response could not fit in buffer.
	E_NOJSON = -6				// No json string found in response.
};
/******************************************************************************
* Struct: station                                                             *
* ---------------                                                             *
*   Stores data of an arrival at a station.                                   *
*                                                                             *
*   name: Name of the station.                                                *
*   time: Time which the train arrives at the station.                        *
******************************************************************************/
typedef struct station {char *name; char *time;} station;
/******************************************************************************
* Struct: edge                                                                *
* ------------                                                                *
*   Stores data of travelling from one station to another.                    *
*                                                                             *
*   type: Type of travel, walk, train, etc.                                   *
*   origin: Station where travel starts.                                      *
*   dest: Station where travel ends.                                          *
******************************************************************************/
typedef struct edge {char *type; station origin; station dest;} edge;
/******************************************************************************
* Struct: trip                                                                *
* ------------                                                                *
*   Stores data of travelling over a set of edges.                            *
*                                                                             *
*   dur: Duration of the travel.                                              *
*   edges_len: Number of edges.                                               *
*   edges: Array of edges.                                                    *
******************************************************************************/
typedef struct trip {char *dur; int edges_len; edge *edges;} trip;
/******************************************************************************
* Function: main                                                              *
* --------------                                                              *
*   Main function of the program.                                             *
*                                                                             *
*   Input parameters:                                                         *
*     Required: <Origin> <Destination>                                        *
*     Optional:                                                               *
******************************************************************************/
int main(int argc, char *argv[]);
/******************************************************************************
* Function: get_request                                                       *
* ---------------------                                                       *
*   Sends a http GET request to the SL server to determine the travel path.   *
*                                                                             *
*   js: Pointer to buffer where response is stored.                           *
*   origin: Station where travel starts.                                      *
*   dest: Station where travel ends.                                          *
*                                                                             *
*   Returns: The number of bytes received.                                    *
*            E_CONNECT when connect fails.                                    *
*            E_RECEIVE when recv fails.                                       *
******************************************************************************/
int get_request(char **js, char *origin, char *dest);
/******************************************************************************
* Function: extract_js                                                        *
* --------------------                                                        *
*   Extracts json code from a string.                                         *
*                                                                             *
*   js: Pointer to buffer where input and output string are stored.           *
*   len: Length of the input string.                                          *
*                                                                             *
*   Returns: The size of the json string.                                     *
******************************************************************************/
int extract_js(char **js, int len);
/******************************************************************************
* Function: extract                                                           *
* -----------------                                                           *
*   Extracts data of trips from a nx_json hashmap.                            *
*                                                                             *
*   jsmap: Pointer to hashmap of json data.                                   *
*   trips: Pointer to pointer of array of trips, is initially unallocated.    *
*                                                                             *
*   Returns: Number of trips that were found in the json.                     *
******************************************************************************/
int extract_trips(const nx_json *jsmap, trip **trips);
/******************************************************************************
* Function: extract_trip                                                      *
* ----------------------                                                      *
*   Extract data of a trip from a nx_json hashmap.                            *
*                                                                             *
*   js_trip: Pointer to hashmap of json data.                                 *
*   trip: Pointer to struct where trip data i stored.                         *
*                                                                             *
*   Returns: Number of edges that were found in the trip.                     *
******************************************************************************/
int extract_trip(const nx_json *js_trip, trip *new_trip);
/******************************************************************************
* Function: extract_edge                                                      *
* ----------------------                                                      *
*   Extract data of an edge from a nx_json hashmap.                           *
*                                                                             *
*   js_edge: Pointer to hashmap of json data.                                 *
*   trip: Pointer to struct where edge data is stored.                        *
*                                                                             *
*   Returns: E_SUCCESS if successful.                                         *
******************************************************************************/
int extract_edge(const nx_json *js_edge, edge *new_edge);
/******************************************************************************
* Function: extract_station                                                   *
* -------------------------                                                   *
*   Extracts data of a station from a nx_jsonn hashmap.                       *
*                                                                             *
*   js_station: Pointer to hashmap of json data.                              *
*   trip: Pointer to struct where station data is stored.                     *
*                                                                             *
*   Returns: E_SUCCESS if successful.                                         *
******************************************************************************/
int extract_station(const nx_json *js_station, station *new_station);
/******************************************************************************
* Function: print_trips                                                       *
* ---------------------                                                       *
*   Prints data of trips.                                                     *
*                                                                             *
*   trips: Pointer to array of trips.                                         *
*   num_trips: Size of the trips array.                                       *
*                                                                             *
*   Returns: E_SUCCESS if successful.                                         *
******************************************************************************/
int print_trips(trip *trips, int num_trips);
/******************************************************************************
* Function: free_trips                                                        *
*   Free memory occupied by trips.                                            *
*                                                                             *
*   trips: Pointer to array of trips.                                         *
*   num_trips: Number of trips in array.                                      *
*                                                                             *
*   Returns: E_SUCCESS if successful.                                         *
******************************************************************************/
int free_trips(trip *trips, int num_trips);
/******************************************************************************
* Function: free_trip                                                         *
*   Free memory occupied by members of a trip struct.                         *
*                                                                             *
*   tr: Trip that should be freed.                                            *
*                                                                             *
*   Returns: E_SUCCESS if successful.                                         *
******************************************************************************/
int free_trip(trip tr);
/******************************************************************************
* Function: free_edge                                                         *
*   Free memory occupied by an edge.                                          *
*                                                                             *
*   ed: Edge that should be freed.                                            *
*                                                                             *
*   Returns: E_SUCCESS if successful.                                         *
******************************************************************************/
int free_edge(edge ed);
/******************************************************************************
* Function: free_station                                                      *
*   Free memory occupied by members of a station struct.                      *
*                                                                             *
*   st: Station that should be freed.                                         *
*                                                                             *
*   Returns: E_SUCCESS if successful.                                         *
******************************************************************************/
int free_station(station st);
/*****************************************************************************/
