#include "functions.h"

void increaseClockValue()
{
    pthread_mutex_lock(&mutexClock);
    clockValue++;
    pthread_mutex_unlock(&mutexClock);
}

void updateClockValue(int newValue)
{
    pthread_mutex_lock(&mutexClock);
    clockValue = newValue;
    pthread_mutex_unlock(&mutexClock);
}

void changeState(int newState)
{
    pthread_mutex_lock(&mutexState);
    state = newState;
    pthread_mutex_unlock(&mutexState);
}

void sendMessage(Message *message, int destination)
{
    MPI_Send(message, 1, MPI_Msg, destination, MESSAGE_TAG, MPI_COMM_WORLD);
}

void addProcessToCostumesQueue(int processId, int priority)
{
    if (processIsNotAlreadyInCostumesQueue(processId))
    {
        //find an index where to insert the process at
        int indexToInsert = 0;
        while (costumesPool->queue[indexToInsert][0] != -1 && costumesPool->queue[indexToInsert][1] < priority)
            indexToInsert++;

        //if the priorites are the same, order by ids
        while (costumesPool->queue[indexToInsert][1] == priority)
        {
            if (costumesPool->queue[indexToInsert][0] < processId)
            {
                indexToInsert++;
            }
            else
            {
                break;
            }
        }

        debug("Adding %d to the costumes queue at a position %d with priority %d", processId, indexToInsert, priority);

        //insert at a given position and save previous values
        int previousProcessId = costumesPool->queue[indexToInsert][0];
        int previousPriority = costumesPool->queue[indexToInsert][1];
        costumesPool->queue[indexToInsert][0] = processId;
        costumesPool->queue[indexToInsert][1] = priority;

        //shift the entire queue
        int i = indexToInsert + 1;
        while (previousProcessId != -1 && i < size)
        {
            int tmp1 = costumesPool->queue[i][0];
            int tmp2 = costumesPool->queue[i][1];
            costumesPool->queue[i][0] = previousProcessId;
            costumesPool->queue[i][1] = previousPriority;
            previousProcessId = tmp1;
            previousPriority = tmp2;
            i++;
        }
    }
}

int processIsNotAlreadyInCostumesQueue(int processId)
{
    for (int i = 0; i < size; i++)
    {
        if (costumesPool->queue[i][0] == processId)
            return FALSE;
    }
    return TRUE;
}

void addProcessToBoatsQueue(int processId, int boatId, int priority, int weight)
{
    if (processIsNotAlreadyInBoatsQueue(processId, boatId))
    {
        //find an index where to insert the process at
        int indexToInsert = 0;
        while (boats[boatId].queue[indexToInsert][0] != -1 && boats[boatId].queue[indexToInsert][1] < priority)
            indexToInsert++;

        //if the priorites are the same, order by ids
        while (boats[boatId].queue[indexToInsert][1] == priority)
        {
            if (boats[boatId].queue[indexToInsert][0] < processId)
            {
                indexToInsert++;
            }
            else
            {
                break;
            }
        }

        debug("Adding %d to the boat %d queue at a position %d with priority %d", processId, boatId, indexToInsert, priority);

        //insert at a given position and save previous values
        int previousProcessId = boats[boatId].queue[indexToInsert][0];
        int previousPriority = boats[boatId].queue[indexToInsert][1];
        int previousWeight = boats[boatId].queue[indexToInsert][2];
        boats[boatId].queue[indexToInsert][0] = processId;
        boats[boatId].queue[indexToInsert][1] = priority;
        boats[boatId].queue[indexToInsert][2] = weight;

        //shift the entire queue
        int i = indexToInsert + 1;
        while (previousProcessId != -1 && i < size)
        {
            int tmp1 = boats[boatId].queue[i][0];
            int tmp2 = boats[boatId].queue[i][1];
            int tmp3 = boats[boatId].queue[i][2];
            boats[boatId].queue[i][0] = previousProcessId;
            boats[boatId].queue[i][1] = previousPriority;
            boats[boatId].queue[i][2] = previousWeight;
            previousProcessId = tmp1;
            previousPriority = tmp2;
            previousWeight = tmp3;
            i++;
        }
    }
}

int processIsNotAlreadyInBoatsQueue(int processId, int boatId)
{
    for (int i = 0; i < size; i++)
    {
        if (boats[boatId].queue[i][0] == processId)
            return FALSE;
    }
    return TRUE;
}

void removeProcessFromCostumesQueue(int processId)
{
    for (int i = 0; i < size; i++)
    {
        if (costumesPool->queue[i][0] == processId)
        {
            costumesPool->queue[i][0] = -1;
            costumesPool->queue[i][1] = -1;
            while (i < size - 1 && costumesPool->queue[i + 1][0] != -1)
            {
                costumesPool->queue[i][0] = costumesPool->queue[i + 1][0];
                costumesPool->queue[i][1] = costumesPool->queue[i + 1][1];
                costumesPool->queue[i + 1][0] = -1;
                costumesPool->queue[i + 1][1] = -1;
                i++;
            }

            break;
        }
    }
}

void removeProcessFromBoatsQueue(int processId, int boatId)
{
    for (int i = 0; i < size; i++)
    {
        if (boats[boatId].queue[i][0] == processId)
        {
            boats[boatId].queue[i][0] = -1;
            boats[boatId].queue[i][1] = -1;
            boats[boatId].queue[i][2] = -1;
            while (i < size - 1 && boats[boatId].queue[i + 1][0] != -1)
            {
                boats[boatId].queue[i][0] = boats[boatId].queue[i + 1][0];
                boats[boatId].queue[i][1] = boats[boatId].queue[i + 1][1];
                boats[boatId].queue[i][2] = boats[boatId].queue[i + 1][2];
                boats[boatId].queue[i + 1][0] = -1;
                boats[boatId].queue[i + 1][1] = -1;
                boats[boatId].queue[i + 1][2] = -1;
                i++;
            }

            break;
        }
    }
}

int numOfProcessesBeforeMeInCostumesQueue()
{
    //check my current position in the queue
    int myIndex = -1;
    for (int i = 0; i < size; i++)
    {
        if (costumesPool->queue[i][0] == tId)
        {
            myIndex = i;
            break;
        }
    }

    return myIndex;
}

int weightOfPassengersBeforeMeInBoatsQueue(int boatId)
{
    //check my current position in the queue
    int myIndex = -1;
    for (int i = 0; i < size; i++)
    {
        if (boats[boatId].queue[i][0] == tId)
        {
            myIndex = i;
            break;
        }
    }

    //calculate the weight of passengers before me
    int sumOfWeight = 0;
    for (int i = myIndex - 1; i >= 0; i--)
    {
        sumOfWeight += boats[boatId].queue[i][2];
    }

    return sumOfWeight;
}

int getBoatWithTheBiggestAvailableCapacityThatCanFitMe()
{
    int biggestCapacity = -1;
    int boatWithBiggestCapacity = -1;

    for (int boatId = 0; boatId < BOATS_COUNT; boatId++)
    {
        if (!boats[boatId].isOnACruise)
        {
            int occupiedCapacity = 0;
            for (int i = 0; i < size; i++)
            {
                int passengerWeight = boats[boatId].queue[i][2];
                if (passengerWeight != -1)
                    occupiedCapacity += passengerWeight;
            }
            int availableCapacity = boats[boatId].capacity - occupiedCapacity;
            if (availableCapacity > biggestCapacity && availableCapacity >= weight)
            {
                biggestCapacity = availableCapacity;
                boatWithBiggestCapacity = boatId;
            }
        }
    }

    return boatWithBiggestCapacity;
}

int lastInTheQueue(int boatId)
{
    //find my current position in the queue and the occupied capacity so far
    int occupied = 0;
    int myIndex = 0;
    for (int i = 0; i < size; i++)
    {
        occupied += boats[boatId].queue[i][2];
        if (boats[boatId].queue[i][0] == tId)
        {
            myIndex = i;
            break;
        }
    }

    //if I am the last one in the queue, return true
    if (myIndex == size - 1)
        return TRUE;

    //if the next passenger cannot fit, it means that I am the last
    if (boats[boatId].queue[myIndex + 1][2] == -1 || occupied + boats[boatId].queue[myIndex + 1][2] > boats[boatId].capacity)
        return TRUE;

    return FALSE;
}