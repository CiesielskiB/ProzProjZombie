#include "main.h"
#include "communicationThread.h"

void *communicationThreadLoop(void *ptr)
{
    MPI_Status status;
    Message message;
    while (TRUE)
    {
        MPI_Recv(&message, 1, MPI_Msg, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        switch (status.MPI_TAG)
        {
        case REQ_C:
            printf("Received REQ_C from %d\n", message.senderId);

            pthread_mutex_lock(&mutexCostumes);
            costumesPool->availableCostumes--;
            addProcessToACostumesQueue(message.senderId, message.priority);
            sendMessage(0, message.senderId, ACK_C);
            pthread_mutex_unlock(&mutexCostumes);

            break;
        case REL_C:
            printf("Received REL_C from %d\n", message.senderId);

            pthread_mutex_lock(&mutexCostumes);
            costumesPool->availableCostumes++;
            removeProcessFromACostumesQueue(message.senderId);
            pthread_mutex_unlock(&mutexCostumes);

            break;
        case REQ_B:
            printf("Received REQ_B from %d\n", message.senderId);

            pthread_mutex_lock(&mutexBoats);
            boats[message.boatId].capacity -= message.senderWeight;
            addProcessToABoatsQueue(message.senderId, message.boatId, message.priority, message.senderWeight);
            sendMessage(0, message.senderId, ACK_B);
            pthread_mutex_unlock(&mutexBoats);

            break;
        case REL_B:
            printf("Received REL_B from %d\n", message.senderId);

            pthread_mutex_lock(&mutexBoats);
            boats[message.boatId].capacity += message.senderWeight;
            removeProcessFromABoatsQueue(message.senderId, message.boatId);
            pthread_mutex_unlock(&mutexBoats);

            break;
        case RESIGN_B:
            printf("Received RESIGN_B from %d\n", message.senderId);

            pthread_mutex_lock(&mutexBoats);
            boats[message.boatId].capacity += message.senderWeight;
            removeProcessFromABoatsQueue(message.senderId, message.boatId);
            pthread_mutex_unlock(&mutexBoats);

            break;
        case CRUISE:
            printf("Received CRUISE from %d\n", message.senderId);

            pthread_mutex_lock(&mutexBoats);
            boats[message.boatId].isOnACruise = TRUE;
            pthread_mutex_unlock(&mutexBoats);

            break;
        case END_OF_A_CRUISE:
            printf("Received END_OF_A_CRUISE from %d\n", message.senderId);

            pthread_mutex_lock(&mutexBoats);
            boats[message.boatId].isOnACruise = FALSE;
            pthread_mutex_unlock(&mutexBoats);
            break;
        }
    }
}

void addProcessToACostumesQueue(int processId, int priority)
{
    if (processIsNotAlreadyInACostumesQueue(processId))
    {
        //find an index where to insert the process at
        int indexToInsert = 0;
        while (costumesPool->queue[indexToInsert][0] != -1 && costumesPool->queue[indexToInsert][1] <= priority)
            indexToInsert++;

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

int processIsNotAlreadyInACostumesQueue(int processId)
{
    for (int i = 0; i < size; i++)
    {
        if (costumesPool->queue[i][0] == processId)
            return FALSE;
    }
    return TRUE;
}

void addProcessToABoatsQueue(int processId, int boatId, int priority, int weight)
{
    if (processIsNotAlreadyInABoatsQueue(processId, boatId))
    {
        //find an index where to insert the process at
        int indexToInsert = 0;
        while (boats[boatId].queue[indexToInsert][0] != -1 && boats[boatId].queue[indexToInsert][1] <= priority)
            indexToInsert++;

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

int processIsNotAlreadyInABoatsQueue(int processId, int boatId)
{
    for (int i = 0; i < size; i++)
    {
        if (boats[boatId].queue[i][0] == processId)
            return FALSE;
    }
    return TRUE;
}

void removeProcessFromACostumesQueue(int processId)
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
            }

            break;
        }
    }
}

void removeProcessFromABoatsQueue(int processId, int boatId)
{
    for (int i = 0; i < size; i++)
    {
        if (boats[boatId].queue[i][0] == processId)
        {
            boats[boatId].queue[i][0] = -1;
            boats[boatId].queue[i][1] = -1;
            while (i < size - 1 && boats[boatId].queue[i + 1][0] != -1)
            {
                boats[boatId].queue[i][0] = boats[boatId].queue[i + 1][0];
                boats[boatId].queue[i][1] = boats[boatId].queue[i + 1][1];
            }

            break;
        }
    }
}