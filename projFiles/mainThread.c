#include "main.h"
#include "mainThread.h"
#include "functions.h"

void mainThreadLoop()
{
    while (TRUE)
    {
        changeState(STATE_WAITING_FOR_COSTUME);

        requestCostume();
        waitForCostumeAcknowledgements();
        sleep(1);
        waitUntilCostumeIsAvailable();

        changeState(STATE_WAITING_FOR_BOAT);

        sleep(2);

        int boatId = -1;
        int boatTaken = FALSE;
        int previousPriority = -1;
        while (!boatTaken)
        {
            boatId = waitForAvailableBoat();
            previousPriority = requestBoat(boatId, previousPriority);
            boatTaken = waitForPermissionAndTryToGetOnBoat(boatId);
        }

        changeState(STATE_ON_A_BOAT);

        waitForCruise(boatId);

        changeState(STATE_ON_A_CRUISE);

        println("GREETINGS FROM CRUISE");
        sleep(TIME_ON_CRUISE);

        changeState(STATE_AFTER_A_CRUISE);

        releaseCostume();
        releaseBoat(boatId);

        sleep(rand() % 10);
    }
}

void requestCostume()
{
    pthread_mutex_lock(&mutexCostumes);
    increaseClockValue();
    int currentClock = clockValue;

    for (int i = 0; i < size; i++)
    {
        Message costumeMessage;
        costumeMessage.senderId = tId;
        costumeMessage.timestamp = currentClock;
        costumeMessage.priority = currentClock;
        costumeMessage.type = REQ_C;
        sendMessage(&costumeMessage, i);
    }
    pthread_mutex_unlock(&mutexCostumes);
}

void waitForCostumeAcknowledgements()
{
    pthread_mutex_lock(&mutexCostumes);
    int permissionGranted = FALSE;
    while (!permissionGranted)
    {
        if (numberOfReceivedCostumePermissions == size)
        {
            permissionGranted = TRUE;
            numberOfReceivedCostumePermissions = 0;
        }
        else
        {
            pthread_cond_wait(&condition_ACK_C, &mutexCostumes);
        }
    }
    pthread_mutex_unlock(&mutexCostumes);
}

void waitUntilCostumeIsAvailable()
{
    pthread_mutex_lock(&mutexCostumes);
    int costumeTaken = FALSE;
    while (!costumeTaken)
    {
        int numOfProcesses = numOfProcessesBeforeMeInCostumesQueue();
        if (numOfProcesses + 1 <= costumesPool->availableCostumes)
        {
            costumeTaken = TRUE;
            println("I got a costume, let's find a boat");
        }
        else
        {
            println("Didn't get a costume, waiting...");
            pthread_cond_wait(&condition_REL_C, &mutexCostumes);
        }
    }
    pthread_mutex_unlock(&mutexCostumes);
}

int waitForAvailableBoat()
{
    int boatId = -1;
    pthread_mutex_lock(&mutexBoats);
    int availableBoatFound = FALSE;
    while (!availableBoatFound)
    {
        boatId = getBoatWithTheBiggestAvailableCapacityThatCanFitMe();
        if (boatId != -1)
        {
            availableBoatFound = TRUE;
        }
        else
        {
            println("All boats are on a cruise or full, waiting...");
            pthread_cond_wait(&condition_REL_B, &mutexBoats);
        }
    }
    pthread_mutex_unlock(&mutexBoats);
    println("Trying to get on a boat %d", boatId);
    return boatId;
}

int requestBoat(int boatId, int previousPriority)
{
    pthread_mutex_lock(&mutexBoats);
    increaseClockValue();
    int currentClock = clockValue;

    for (int i = 0; i < size; i++)
    {
        Message boatMessage;
        boatMessage.senderId = tId;
        boatMessage.timestamp = currentClock;
        boatMessage.priority = previousPriority == -1 ? currentClock : previousPriority;
        boatMessage.boatId = boatId;
        boatMessage.senderWeight = weight;
        boatMessage.type = REQ_B;
        sendMessage(&boatMessage, i);
    }
    pthread_mutex_unlock(&mutexBoats);
    return currentClock;
}

int waitForPermissionAndTryToGetOnBoat(int boatId)
{
    pthread_mutex_lock(&mutexBoats);
    int boatTaken = FALSE;
    int permissionGranted = FALSE;
    while (!permissionGranted)
    {
        if (numberOfReceivedBoatPermissions == size)
        {
            permissionGranted = TRUE;
            numberOfReceivedBoatPermissions = 0;

            int weightOfPassengers = weightOfPassengersBeforeMeInBoatsQueue(boatId);
            if (!boats[boatId].isOnACruise)
            {
                if (weightOfPassengers + weight <= boats[boatId].capacity)
                {
                    boatTaken = TRUE;
                    println("Found a place on a boat %d", boatId);
                    if (lastInTheQueue(boatId))
                    {
                        sendCruiseMessage(boatId);
                    }
                }
                else
                {
                    println("There was not enough space for me");
                    sendResignMessage(boatId);
                }
            }
            else
            {
                println("Too late, this boat is already on a cruise");
                sendResignMessage(boatId);
            }
        }
        else
        {
            pthread_cond_wait(&condition_ACK_B, &mutexBoats);
        }
    }
    pthread_mutex_unlock(&mutexBoats);
    return boatTaken;
}

void waitForCruise(int boatId)
{
    pthread_mutex_lock(&mutexBoats);
    while (!boats[boatId].isOnACruise)
    {
        pthread_cond_wait(&condition_CRUISE, &mutexBoats);
    }
    pthread_mutex_unlock(&mutexBoats);
}

void sendCruiseMessage(int boatId)
{
    increaseClockValue();
    int currentClock = clockValue;

    for (int i = 0; i < size; i++)
    {
        Message cruiseMessage;
        cruiseMessage.senderId = tId;
        cruiseMessage.timestamp = currentClock;
        cruiseMessage.boatId = boatId;
        cruiseMessage.type = CRUISE;
        sendMessage(&cruiseMessage, i);
    }
}

void sendResignMessage(int boatId)
{
    increaseClockValue();
    for (int i = 0; i < size; i++)
    {
        Message resignMessage;
        resignMessage.senderId = tId;
        resignMessage.timestamp = clockValue;
        resignMessage.type = RESIGN_B;
        resignMessage.boatId = boatId;
        sendMessage(&resignMessage, i);
    }
}

void releaseBoat(int boatId)
{
    increaseClockValue();
    for (int i = 0; i < size; i++)
    {
        Message releaseBoatMessage;
        releaseBoatMessage.senderId = tId;
        releaseBoatMessage.timestamp = clockValue;
        releaseBoatMessage.type = REL_B;
        releaseBoatMessage.boatId = boatId;
        sendMessage(&releaseBoatMessage, i);
    }
}

void releaseCostume()
{
    increaseClockValue();
    for (int i = 0; i < size; i++)
    {
        Message releaseCostumeMessage;
        releaseCostumeMessage.senderId = tId;
        releaseCostumeMessage.timestamp = clockValue;
        releaseCostumeMessage.type = REL_C;
        sendMessage(&releaseCostumeMessage, i);
    }
}
