// A linked list (LL) node to store a queue entry
struct QNode
{
    char message[500];
    struct QNode *next;
};

// The queue, front stores the front node of LL and rear stores ths
// last node of LL
struct Queue
{
    struct QNode *front, *rear;
};

struct QNode* newNode(char message[]);

struct Queue *createQueue();

void enQueue(struct Queue *q, char message []);

struct QNode *deQueue(struct Queue *q);

