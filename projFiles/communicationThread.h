/**
 * Wątek komunikacyjny: odbieranie i reagowanie na wiadomości
**/
void *communicationThreadLoop(void *ptr);

void addProcessToACostumesQueue(int processId, int priority);
int processIsNotAlreadyInACostumesQueue(int processId);

void addProcessToABoatsQueue(int processId, int boatId, int priority, int weight);
int processIsNotAlreadyInABoatsQueue(int processId, int boatId);

void removeProcessFromACostumesQueue(int processId);
void removeProcessFromABoatsQueue(int processId, int boatId);