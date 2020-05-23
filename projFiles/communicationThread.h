/**
 * Communication thread: receving and reacting to messages
**/
void *communicationThreadLoop(void *ptr);

/**
 * Sends back an ACK_C message
**/
void respondWithACK_C(int destination);

/**
 * Sends back an ACK_B message
**/
void respondWithACK_B(int destination);