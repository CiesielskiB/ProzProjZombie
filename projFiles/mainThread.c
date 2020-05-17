#include "main.h"
#include "mainThread.h"

void mainThreadLoop()
{
    Message *testMessage = malloc(sizeof(Message));
    testMessage->senderId = tId;
    testMessage->type = REQ_B;
    testMessage->priority = 5;
    testMessage->boatId = 1;
    int destination = (tId + 1) % size;
    MPI_Send(testMessage, 1, MPI_Msg, destination, REQ_B, MPI_COMM_WORLD);

    free(testMessage);
}