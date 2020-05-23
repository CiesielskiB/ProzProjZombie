/**
 * Main thread loop
**/
void mainThreadLoop();

/**
 * Sends a REQ_C message to all processes
**/
void requestCostume();

/**
 * Waits for ACK messages from all processes
**/
void waitForCostumeAcknowledgements();

/**
 * Checks if there are enough costumes for me and waits othwerwise
**/
void waitUntilCostumeIsAvailable();

/**
 * Waits until a boat is available
**/
int waitForAvailableBoat();

/**
 * Sends a REQ_B message to all processes
**/
int requestBoat(int boatId, int previousPriority);

/**
 * Waits for ACK messages from all processes and checks if there is enough space for me,
 * resigns otherwise.
**/
int waitForPermissionAndTryToGetOnBoat(int boatId);

/**
 * Waits for a CRUISE message
**/
void waitForCruise(int boatId);

/**
 * Sends a CRUISE message to everyone
**/
void sendCruiseMessage(int boatId);

/**
 * Sends a RESIGN message to everyone
**/
void sendResignMessage(int boatId);

/**
 * Sends a REL_B message to everyone
**/
void releaseBoat(int boatId);

/**
 * Sends a REL_C message to everyone
**/
void releaseCostume();
