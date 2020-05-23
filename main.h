#ifndef GLOBALH
#define GLOBALH

#define _GNU_SOURCE
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

/* Helpers */

#define TRUE 1
#define FALSE 0

#define ROOT 0

#define println(FORMAT, ...) printf("%c[%d;%dm[ID: %d] [CLOCK: %d]: " FORMAT "%c[%d;%dm\n", 27, (1 + (tId / 7)) % 2, 31 + (6 + tId) % 7, tId, clockValue, ##__VA_ARGS__, 27, 0, 37);

#ifdef DEBUG
#define debug(FORMAT, ...) printf("%c[%d;%dm[ID: %d] [CLOCK: %d]: " FORMAT "%c[%d;%dm\n", 27, (1 + (tId / 7)) % 2, 31 + (6 + tId) % 7, tId, clockValue, ##__VA_ARGS__, 27, 0, 37);
#else
#define debug(...) ;
#endif

/* Constants */

#define BOAT_CAPACITY_MIN 5
#define BOAT_CAPACITY_MAX 10

#define PASSENGER_MIN_WEIGHT 2
#define PASSENGER_MAX_WEIGHT 8

#define MESSAGE_TAG 5

#define TIME_ON_CRUISE 4

/* Messages types - COSTUME */

#define REQ_C 10
#define ACK_C 11
#define REL_C 12

/* Messages types- BOAT */

#define REQ_B 20
#define ACK_B 21
#define REL_B 22
#define RESIGN_B 23
#define CRUISE 24
#define END_OF_A_CRUISE 25

/* Process state */

#define STATE_INIT 30
#define STATE_WAITING_FOR_COSTUME 31
#define STATE_WAITING_FOR_BOAT 32
#define STATE_ON_A_BOAT 33
#define STATE_ON_A_CRUISE 34
#define STATE_AFTER_A_CRUISE 35

/* Structs */

typedef struct
{
  int senderId;
  int timestamp; //clock value
  int priority;
  int type;
  int boatId;
  int senderWeight;
} Message;

typedef struct
{
  int **queue;
  int capacity;
  int isOnACruise;
} Boat;

typedef struct
{
  int **queue;
  int availableCostumes;
} CostumesPool;

/**
 * Datatype of a message that is used for communication between processes
**/
extern MPI_Datatype MPI_Msg;

/**
 * Currect clock value
**/
extern int clockValue;

/**
 * Structure containg information about available boats and a queue for each one of them
 * containing processes trying to get on a boat
**/
extern Boat *boats;

/**
 * Structure containg information about available costumes and a queue
 * of processes trying to get one
**/
extern CostumesPool *costumesPool;

/**
 * Number of available boats
**/
extern int BOATS_COUNT;

/**
 * Number of available costumes
**/
extern int COSTUMES_COUNT;

/**
 * The capacity that a passenger occupies on a boat
**/
extern int weight;

/**
 * The id of a process
**/
extern int tId;

/**
 * The amount of running processes
**/
extern int size;

/**
 * Current state of a process
**/
extern int state;

/**
 * Current state of a process
**/
extern int numberOfReceivedCostumePermissions;

/**
 * Current state of a process
**/
extern int numberOfReceivedBoatPermissions;

/**
 * Additional thread used for receiving incoming messages
**/
extern pthread_t commThread;

/**
 * Mutex used for protecting the act of changing the current state
**/
extern pthread_mutex_t mutexState;

/**
 * Mutex protecting boat related operations
**/
extern pthread_mutex_t mutexBoats;

/**
 * Mutex protecting costume related operations
**/
extern pthread_mutex_t mutexCostumes;

/**
 * Mutex protecting clock related operations
**/
extern pthread_mutex_t mutexClock;

/**
 * Condition that is signaled when permission for costume from all processes has been granted
**/
extern pthread_cond_t condition_ACK_C;

/**
 * Condition that is signaled when permission for boat from all processes has been granted
**/
extern pthread_cond_t condition_ACK_B;

/**
 * Condition that is signaled when a costume is returned to the costumes pool
**/
extern pthread_cond_t condition_REL_C;

/**
 * Condition that is signaled when a boat returns from a cruise
**/
extern pthread_cond_t condition_REL_B;

/**
 * Condition that is signaled when some boat goes to cruise
**/
extern pthread_cond_t condition_CRUISE;

/**
 * Initalizing passed arguments
**/
void initArguments(int argc, char **argv);

/**
 * Initializing the MPI environment
**/
void initMPI(int *argc, char ***argv);

/**
 * Creating a custom data type used by MPI
**/
void createMessageType();

/**
  * Creating boats and generating random capacities
**/
void initBoats(int tId, int size);

/**
  * Generating a number of available costumes
**/
void initCostumes(int tId, int size);

/**
 * Generating an occupied capacity on a boat by a process (passenger)
**/
void generatePassengerWeight(int tId);

/**
 * Cleaning used resources
**/
void cleanUp();
#endif