#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "queue.h"

struct QNode* newNode(char message [])
{
    struct QNode *temp = (struct QNode*)malloc(sizeof(struct QNode));
    memcpy(temp->message, message, strlen(message));
    temp->next = NULL;
    return temp;
}

/*function to create an empty queue */
struct Queue *createQueue()
{
    struct Queue *q = (struct Queue*)malloc(sizeof(struct Queue));
    q->front = q->rear = NULL;
    return q;
}

/* function to add a node into queue */
void enQueue(struct Queue *q, char message[])
{
    struct QNode *temp = newNode(message);
 
    if (q->rear == NULL)
    {
       q->front = q->rear = temp;
       return;
    }

    q->rear->next = temp;
    q->rear = temp;
}

/* function to pop from queu */
struct QNode *deQueue(struct Queue *q)
{
    // If queue is empty, return NULL.
    if (q->front == NULL)
       return NULL;
 
    // Store previous front and move front one node ahead
    struct QNode *temp = q->front;
    q->front = q->front->next;
 
    // If front becomes NULL, then change rear also as NULL
    if (q->front == NULL)
       q->rear = NULL;
    return temp;
}
