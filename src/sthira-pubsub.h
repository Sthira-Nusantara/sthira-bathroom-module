#ifndef STHIRAPUBSUB_H
#define STHIRAPUBSUB_H
#include <PubSubClient.h>

extern PubSubClient client;
extern String getCard;
void pubsub_connect();
bool pubsub_connected();
void pubsub_init();

#endif