#include "main.h"
#include "functions.h"
#include "communicationThread.h"

void *communicationThreadLoop(void *ptr)
{
    Message message;
    while (TRUE)
    {
        MPI_Recv(&message, 1, MPI_Msg, MPI_ANY_SOURCE, MESSAGE_TAG, MPI_COMM_WORLD, NULL);

        int newClockValue = MAX(clockValue, message.timestamp) + 1;
        updateClockValue(newClockValue);

        switch (message.type)
        {
        case REQ_C:
            debug("Received REQ_C from %d", message.senderId);

            pthread_mutex_lock(&mutexCostumes);

            addProcessToCostumesQueue(message.senderId, message.priority);
            respondWithACK_C(message.senderId);

            pthread_mutex_unlock(&mutexCostumes);

            break;
        case ACK_C:
            debug("Received ACK_C from %d", message.senderId);

            pthread_mutex_lock(&mutexCostumes);

            numberOfReceivedCostumePermissions++;
            if (numberOfReceivedCostumePermissions == size)
            {
                pthread_cond_signal(&condition_ACK_C);
            }

            pthread_mutex_unlock(&mutexCostumes);

            break;
        case REL_C:
            debug("Received REL_C from %d", message.senderId);

            pthread_mutex_lock(&mutexCostumes);

            removeProcessFromCostumesQueue(message.senderId);
            pthread_cond_signal(&condition_REL_C);

            pthread_mutex_unlock(&mutexCostumes);

            break;
        case REQ_B:
            debug("Received REQ_B from %d", message.senderId);

            pthread_mutex_lock(&mutexBoats);

            addProcessToBoatsQueue(message.senderId, message.boatId, message.priority, message.senderWeight);
            respondWithACK_B(message.senderId);

            pthread_mutex_unlock(&mutexBoats);

            break;
        case ACK_B:
            debug("Received ACK_B from %d", message.senderId);

            pthread_mutex_lock(&mutexBoats);

            numberOfReceivedBoatPermissions++;
            if (numberOfReceivedBoatPermissions == size)
            {
                pthread_cond_signal(&condition_ACK_B);
            }

            pthread_mutex_unlock(&mutexBoats);

            break;
        case REL_B:
            debug("Received REL_B from %d", message.senderId);

            pthread_mutex_lock(&mutexBoats);

            boats[message.boatId].isOnACruise = FALSE;
            removeProcessFromBoatsQueue(message.senderId, message.boatId);
            pthread_cond_signal(&condition_REL_B);

            pthread_mutex_unlock(&mutexBoats);

            break;
        case RESIGN_B:
            debug("Received RESIGN_B from %d", message.senderId);

            pthread_mutex_lock(&mutexBoats);

            removeProcessFromBoatsQueue(message.senderId, message.boatId);

            pthread_mutex_unlock(&mutexBoats);

            break;
        case CRUISE:
            debug("Received CRUISE from %d", message.senderId);

            pthread_mutex_lock(&mutexBoats);

            boats[message.boatId].isOnACruise = TRUE;
            pthread_cond_signal(&condition_CRUISE);

            pthread_mutex_unlock(&mutexBoats);

            break;
        }
    }
}

void respondWithACK_C(int destination)
{
    increaseClockValue();
    Message message;
    message.senderId = tId;
    message.timestamp = clockValue;
    message.type = ACK_C;
    sendMessage(&message, destination);
}

void respondWithACK_B(int destination)
{
    increaseClockValue();
    Message message;
    message.senderId = tId;
    message.timestamp = clockValue;
    message.type = ACK_B;
    sendMessage(&message, destination);
}