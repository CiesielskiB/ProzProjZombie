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

#define BOAT_CAPACITY_MIN 10
#define BOAT_CAPACITY_MAX 20

#define MIN_NUMBER_OF_COSTUMES 3
#define MAX_NUMBER_OF_COSTUMES 6 //no more than the number of running processes

#define PASSENGER_MIN_WEIGHT 2
#define PASSENGER_MAX_WEIGHT 8

typedef struct
{
  int senderId;
  int timestamp; //Lamport clock
  int priority;
  int type;
  int data;
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

/* Typy wiadomości - STRÓJ */

#define INIT_C 10
#define REQ_C 11
#define ACK_C 12
#define REL_C 13

/* Typy wiadomości - ŁÓDŹ */

#define INIT_B 20
#define REQ_B 21
#define ACK_B 22
#define REL_B 23
#define RESIGN_B 24
#define CRUISE 25
#define END_OF_A_CRUISE 26

/* Stany procesow */

#define STATE_INIT 30
#define STATE_WAITING_FOR_COSTUME 31
#define STATE_WAITING_FOR_BOAT 32
#define STATE_ON_A_BOAT 33
#define STATE_ON_A_CRUISE 34
#define STATE_AFTER_A_CRUISE 35

/**
 * Inicjalizacja środowiska
**/
void initMPI();

/**
 * Metoda tworzy customowy typ wiadomosci wysyłanej przez MPI
**/
void createMessageType();

/**
  * Tworzenie łodzi i losowanie ich pojemności
**/
void initBoats(int tId, int size);

/**
  * Losowanie ilości dostępnych kostiumow
**/
void initCostumes(int tId, int size);

/**
 * Losowanie pojemności zajmowanej przez pasazera na łodzi
**/
void generatePassengerWeight(int tId);

/**
 * Czysczenie zasobow, czekanie na zakończenie innych watkow
**/
void cleanUp();
#endif