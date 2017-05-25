#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>


struct AdjListNode
{
	int dest;
	int weight;
	struct AdjListNode* next;
};


struct AdjList
{
	struct AdjListNode *head; 
};


struct Graph
{
	int V;
	struct AdjList* array;
};


struct AdjListNode* newAdjListNode(int dest, int weight)
{
	struct AdjListNode* newNode =
			(struct AdjListNode*) malloc(sizeof(struct AdjListNode));
	newNode->dest = dest;
	newNode->weight = weight;
	newNode->next = NULL;
	return newNode;
}


struct Graph* createGraph(int V)
{
	struct Graph* graph = (struct Graph*) malloc(sizeof(struct Graph));
	graph->V = V;

	graph->array = (struct AdjList*) malloc(V * sizeof(struct AdjList));

	for (int i = 0; i < V; ++i)
		graph->array[i].head = NULL;

	return graph;
}


void addEdge(struct Graph* graph, int src, int dest, int weight)
{
	struct AdjListNode* newNode = newAdjListNode(dest, weight);
	newNode->next = graph->array[src].head;
	graph->array[src].head = newNode;

	newNode = newAdjListNode(src, weight);
	newNode->next = graph->array[dest].head;
	graph->array[dest].head = newNode;
}

struct MinHeapNode
{
	int v;
	int dist;
};

struct MinHeap
{
	int size;	 
	int capacity; 
	int *pos;	 
	struct MinHeapNode **array;
};

struct MinHeapNode* newMinHeapNode(int v, int dist)
{
	struct MinHeapNode* minHeapNode =
		(struct MinHeapNode*) malloc(sizeof(struct MinHeapNode));
	minHeapNode->v = v;
	minHeapNode->dist = dist;
	return minHeapNode;
}

struct MinHeap* createMinHeap(int capacity)
{
	struct MinHeap* minHeap =
		(struct MinHeap*) malloc(sizeof(struct MinHeap));
	minHeap->pos = (int *)malloc(capacity * sizeof(int));
	minHeap->size = 0;
	minHeap->capacity = capacity;
	minHeap->array =
		(struct MinHeapNode**) malloc(capacity * sizeof(struct MinHeapNode*));
	return minHeap;
}

void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b)
{
	struct MinHeapNode* t = *a;
	*a = *b;
	*b = t;
}

void minHeapify(struct MinHeap* minHeap, int idx)
{
	int smallest, left, right;
	smallest = idx;
	left = 2 * idx + 1;
	right = 2 * idx + 2;

	if (left < minHeap->size &&
		minHeap->array[left]->dist < minHeap->array[smallest]->dist )
	smallest = left;

	if (right < minHeap->size &&
		minHeap->array[right]->dist < minHeap->array[smallest]->dist )
	smallest = right;

	if (smallest != idx)
	{
		struct MinHeapNode *smallestNode = minHeap->array[smallest];
		struct MinHeapNode *idxNode = minHeap->array[idx];

		minHeap->pos[smallestNode->v] = idx;
		minHeap->pos[idxNode->v] = smallest;

		swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);

		minHeapify(minHeap, smallest);
	}
}

int isEmpty(struct MinHeap* minHeap)
{
	return minHeap->size == 0;
}

struct MinHeapNode* extractMin(struct MinHeap* minHeap)
{
	if (isEmpty(minHeap))
		return NULL;

	struct MinHeapNode* root = minHeap->array[0];

	struct MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
	minHeap->array[0] = lastNode;

	minHeap->pos[root->v] = minHeap->size-1;
	minHeap->pos[lastNode->v] = 0;

	--minHeap->size;
	minHeapify(minHeap, 0);

	return root;
}

void decreaseKey(struct MinHeap* minHeap, int v, int dist)
{

	int i = minHeap->pos[v];

	minHeap->array[i]->dist = dist;

	while (i && minHeap->array[i]->dist < minHeap->array[(i - 1) / 2]->dist)
	{
		minHeap->pos[minHeap->array[i]->v] = (i-1)/2;
		minHeap->pos[minHeap->array[(i-1)/2]->v] = i;
		swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);

		i = (i - 1) / 2;
	}
}


void printPath(int parent[], int j)
{
    if (parent[j]==-1)
        return;
 
    printPath(parent, parent[j]);
 
    printf("%d ", j);
}

void printSolution(int dist[], int n, int parent[])
{
    int src = 0;
    printf("Vertex\t\t Distance\tPath");
    for (int i = 1; i < 21 ; i++)
    {
        printf("\n%d -> %d \t\t %d\t\t%d ", src, i, dist[i], src);
        printPath(parent, i);
    }
    printf("\n");
}


bool isInMinHeap(struct MinHeap *minHeap, int v)
{
if (minHeap->pos[v] < minHeap->size)
	return true;
return false;
}


void dijkstra(struct Graph* graph, int src)
{
	int V = graph->V;
	int dist[V];	 

	struct MinHeap* minHeap = createMinHeap(V);
    int parent[V];

	for (int v = 0; v < V; ++v)
	{
		parent[0] = -1;
		dist[v] = INT_MAX;
		minHeap->array[v] = newMinHeapNode(v, dist[v]);
		minHeap->pos[v] = v;
	}

	minHeap->array[src] = newMinHeapNode(src, dist[src]);
	minHeap->pos[src] = src;
	dist[src] = 0;
	decreaseKey(minHeap, src, dist[src]);

	minHeap->size = V;

	while (!isEmpty(minHeap))
	{
		struct MinHeapNode* minHeapNode = extractMin(minHeap);
		int u = minHeapNode->v; 

		struct AdjListNode* pCrawl = graph->array[u].head;
		while (pCrawl != NULL)
		{
			int v = pCrawl->dest;

			if (isInMinHeap(minHeap, v) && dist[u] != INT_MAX && 
										pCrawl->weight + dist[u] < dist[v])
			{
				parent[v]  = u;
				dist[v] = dist[u] + pCrawl->weight;

				decreaseKey(minHeap, v, dist[v]);
			}
			pCrawl = pCrawl->next;
		}
	}

	printSolution(dist, V, parent);
}

int main()
{
	
	int V = 21;
	struct Graph* graph = createGraph(V);
	addEdge(graph, 0, 1, 6);
	addEdge(graph, 0, 2, 2);
	addEdge(graph, 1, 3, 1);
	addEdge(graph, 2, 4, 7);
	addEdge(graph, 3, 4, 5);
	addEdge(graph, 4, 5, 10);
	addEdge(graph, 4, 6, 8);
	addEdge(graph, 6, 7, 1);
	addEdge(graph, 5, 8, 3);
	addEdge(graph, 7, 9, 5);
	addEdge(graph, 7, 10, 3);
	addEdge(graph, 8, 11, 2);
	addEdge(graph, 8, 12, 4);
	addEdge(graph, 9, 13, 1);
	addEdge(graph, 10, 13, 4);
	addEdge(graph, 11, 12, 7);
	addEdge(graph, 11, 14, 2);
	addEdge(graph, 12, 14, 5);
	addEdge(graph, 13, 15, 1);
	addEdge(graph, 14, 15, 6);
	addEdge(graph, 15, 16, 8);
	addEdge(graph, 15, 17, 11);
	addEdge(graph, 14, 17, 8);
	addEdge(graph, 16, 18, 2);
	addEdge(graph, 17, 19, 5);
	addEdge(graph, 18, 20, 4);
	addEdge(graph, 19, 20, 1);

	dijkstra(graph, 0);

	return 0;
}

