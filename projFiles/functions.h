#include "main.h"

#define MAX(a, b) ((a) > (b) ? a : b)

/**
 * Increases the current value of a clock by 1
**/
void increaseClockValue();

/**
 * Updates the current value of a clock to a new value
**/
void updateClockValue(int newValue);

/**
 * Changes the state of a process
**/
void changeState(int newState);

/**
 * Sends a message to another process
**/
void sendMessage(Message *message, int destination);

/**
 * Adds a process to the costumes queue according to its priority
**/
void addProcessToCostumesQueue(int processId, int priority);

/**
 * Checks if the process is not already present in the queue
**/
int processIsNotAlreadyInCostumesQueue(int processId);

/**
 * Adds a process to the boats queue according to its priority
**/
void addProcessToBoatsQueue(int processId, int boatId, int priority, int weight);

/**
 * Checks if the process is not already present in the queue
**/
int processIsNotAlreadyInBoatsQueue(int processId, int boatId);

/**
 * Removes a process from the costumes queue
**/
void removeProcessFromCostumesQueue(int processId);

/**
 * Removes a process from the boats queue
**/
void removeProcessFromBoatsQueue(int processId, int boatId);

/**
 * Returns a number of processes that are before me in the costumes queue
**/
int numOfProcessesBeforeMeInCostumesQueue();

/**
 * Returns a sum of weight of passengers that are before me in the boats queue
**/
int weightOfPassengersBeforeMeInBoatsQueue(int boatId);

/**
 * Returns an id of a boat that can fit me, has the biggest available capacity and is not on a cruise
**/
int getBoatWithTheBiggestAvailableCapacityThatCanFitMe();

/**
 * Checks whether we are last in the boats queue (of those that can enter the boat)
**/
int lastInTheQueue(int boatId);