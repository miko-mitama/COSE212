#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Structure for an EDGE
typedef struct edge {
   struct vertex *dest; // pointer to destination vertex
   struct edge *next;
} EDGE;

// Structure for a Vertex
typedef struct vertex {
   int id;
   int visited; // visited flag for graph traversal
   EDGE *edges; // head of adjacency list
} VERTEX;

// Structure for a Graph
typedef struct {
   int numVertices; // number of vertices in graph
   VERTEX *vertices; // array to store vertices
} GRAPH;

// Stack structure for DFS
typedef struct {
   int *data; // array to store elements
   int top; // index of the top element
   int capacity;
} Stack;

// Queue structure for BFS
typedef struct {
   int *data; // array to store elements
   int front; // index of the front element
   int rear; // index of the rear element
   int capacity;
   int size; // current size of the queue
} Queue;

GRAPH *createGraph(int numVertices);
void destroyGraph(GRAPH *graph);
void addEdge(GRAPH *graph,int src,int dest);
void resetVisitedFlag(GRAPH *graph);


////////////////////////////////////////////////////////////////////////////////
// Function to create a stack
Stack *createStack(int capacity) {
   Stack *stack = (Stack*)malloc(sizeof(Stack));
   stack->data = (int*)malloc(capacity  *sizeof(int));
   stack->top = -1;
   stack->capacity = capacity;
   return stack;
}

////////////////////////////////////////////////////////////////////////////////
// Push an element onto the stack
void push(Stack *stack, int value) {
   if (stack->top == stack->capacity - 1) {
      printf("Stack overflow\n");
      return;
   }
   stack->data[++stack->top] = value;
}

////////////////////////////////////////////////////////////////////////////////
// Pop an element from the stack
int pop(Stack *stack) {
   if (stack->top == -1) {
      printf("Stack underflow\n");
      return -1;
   }
   return stack->data[stack->top--];
}

////////////////////////////////////////////////////////////////////////////////
// Check if the stack is empty
int isEmptyStack(Stack *stack) {
   return stack->top == -1;
}

////////////////////////////////////////////////////////////////////////////////
// Free the stack
void freeStack(Stack *stack) {
   free(stack->data);
   free(stack);
}

////////////////////////////////////////////////////////////////////////////////
// Function to create a queue
Queue *createQueue(int capacity) {
   Queue *queue = (Queue *)malloc(sizeof(Queue));
   queue->data = (int *)malloc(capacity * sizeof(int));
   queue->front = 0;
   queue->rear = -1;
   queue->capacity = capacity;
   queue->size = 0;
   return queue;
}

////////////////////////////////////////////////////////////////////////////////
// Function to check if the queue is empty
int isEmptyQueue(Queue *queue) {
   return queue->size == 0;
}

////////////////////////////////////////////////////////////////////////////////
// Function to check if the queue is full
int isFullQueue(Queue *queue) {
   return queue->size == queue->capacity;
}

////////////////////////////////////////////////////////////////////////////////
// Function to add an element to the queue (Enqueue)
void enqueue(Queue *queue, int value) {
   if (isFullQueue(queue)) {
      printf("Queue overflow\n");
      return;
   }
   queue->rear = (queue->rear + 1) % queue->capacity; // Circular increment
   queue->data[queue->rear] = value;
   queue->size++;
}

////////////////////////////////////////////////////////////////////////////////
// Function to remove an element from the queue (Dequeue)
int dequeue(Queue *queue) {
   if (isEmptyQueue(queue)) {
      printf("Queue underflow\n");
      return -1;
   }
   int value = queue->data[queue->front];
   queue->front = (queue->front + 1) % queue->capacity; // Circular increment
   queue->size--;
   return value;
}

////////////////////////////////////////////////////////////////////////////////
// Free the stack
void freeQueue(Queue *queue) {
   free(queue->data);
   free(queue);
}

////////////////////////////////////////////////////////////////////////////////
void printGraph(GRAPH *graph) {

   for (int i = 0; i < graph->numVertices; i++) {
      
      printf("vertex %d:", i+1);

      EDGE *edge = graph->vertices[i].edges;
      
      while(edge) {
         printf("->%d", edge->dest->id+1);
         edge = edge->next;
      }
      printf("\n");
   }
}


// Function to parse the .net file and build the graph
GRAPH *parseNetFile(const char *fileName){
   FILE *fp=fopen(fileName,"r");
   if(!fp) return NULL;   
   char line[256];
   int num=0;
   while(fgets(line,sizeof(line),fp)){
      if (strncmp(line,"*Vertices",9)==0){
         sscanf(line,"*Vertices %d",&num);
         break;
      }
   }
   if (num==0){
      fclose(fp);
      return NULL;
   }
   GRAPH *new=createGraph(num);
   if(!new){
      fclose(fp);
      return NULL;
   }
   while (fgets(line,sizeof(line),fp)){
      if(line[0]=='*') break;
   }
   int in,out;
	while (fgets(line, sizeof(line), fp)) {
		if (sscanf(line, "%d %d", &in, &out) == 2) {
		addEdge(new, in-1,out-1);
		addEdge(new, out-1,in-1);
		}
	}
   fclose(fp);
   return new;
}

// Function to create a graph with disjoint vertices
GRAPH *createGraph(int numVertices){
   GRAPH *new=(GRAPH*)malloc(sizeof(GRAPH));
   if (!new) return NULL;
   new->numVertices=numVertices;
   new->vertices=(VERTEX*)malloc(sizeof(VERTEX)*numVertices);
   if (!new->vertices){
      free(new);
      return NULL;
   }
   int i;
   for(i=0;i<numVertices;i++){
      new->vertices[i].id=i;
      new->vertices[i].visited=0;
      new->vertices[i].edges=NULL;
   }
   return new;
}
   

// Function to add an edge to the graph (insertion in an ordered list) 
void addEdge(GRAPH *graph, int src, int dest){
   EDGE *edge=(EDGE*)malloc(sizeof(EDGE));
   edge->next=NULL;
   edge->dest=&graph->vertices[dest];
   EDGE **current=&graph->vertices[src].edges;
   while (*current&&(*current)->dest->id <dest){
      current=&((*current)->next);
   }
   edge->next=*current;
   *current=edge;
}

void destroyGraph(GRAPH *graph){
   if (!graph) return;
   int i;
   for (i=0;i<graph->numVertices;i++){
      EDGE *del=graph->vertices[i].edges;
      while (del){
         EDGE *temp=del;
         del=del->next;
         free(temp);
      }
   }
   free(graph->vertices);
   free(graph);
}

void DFS(GRAPH *graph) {
    if (!graph) return;
	Stack *stack = createStack(graph->numVertices);
    resetVisitedFlag(graph);
	int i;
    for (i=0;i<graph->numVertices;i++) {
        if (!graph->vertices[i].visited) {
            printf("Depth-First Traversal starting from vertex %d:\n",i+1);
            graph->vertices[i].visited=1;
            push(stack,i);
            while (!isEmptyStack(stack)){
                int j=pop(stack);
                printf("%d ",j+1);
                for (EDGE *edge=graph->vertices[j].edges;edge;edge=edge->next) {
                    int idd=edge->dest->id;
                    if (!graph->vertices[idd].visited) {
                        graph->vertices[idd].visited=1;
                        push(stack,idd);
                    }
                }
            }
            printf("\n");
        }
    }
    freeStack(stack);
}


// BFS using a queue
void BFS(GRAPH *graph){
   if (!graph) return;
   Queue *queue=createQueue(graph->numVertices);
   resetVisitedFlag(graph);
   int i;
   for (i=0;i<graph->numVertices;i++) {
      if (!graph->vertices[i].visited) {
         printf("Breadth-First Traversal starting from vertex %d:\n",i+1);
         enqueue(queue,i);
         while (!isEmptyQueue(queue)) {
            int current = dequeue(queue);
            if (!graph->vertices[current].visited) {
               graph->vertices[current].visited=1;
               printf("%d ",current+1);
               EDGE *edge=graph->vertices[current].edges;
               while (edge){
                  int neighbor=edge->dest->id;
                  if (!graph->vertices[neighbor].visited) {
                     enqueue(queue,neighbor);
                  }
                  edge=edge->next;
               }
            }
         }
         printf("\n");
      }
   }
   freeQueue(queue);
}

void resetVisitedFlag(GRAPH *graph){
	int i;
	for (i=0;i<graph->numVertices;i++){
		graph->vertices[i].visited=0;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Main function
int main(int argc, char **argv) {
   
   if (argc != 2) {
      fprintf(stderr, "Usage: %s .net_file\n", argv[0]);
      return 1;
   }
   
   GRAPH *graph = parseNetFile(argv[1]);

   printGraph(graph);

   // Perform DFS starting from the first vertex
   DFS(graph);

   // Perform BFS starting from the first vertex
   BFS(graph);
   
   destroyGraph(graph);

   return 0;
}