#pragma once


struct HeapStruct
{
	int Capacity;
	int Size;
	NodeEleType* Elements;
};

typedef struct HeapStruct* PriorityQueue;

class Heap
{
public:
	static PriorityQueue initialize_heap(int MaxElements);
	static void destroy(PriorityQueue H);
	static void make_empty(PriorityQueue H);
	static void insert(NodeEleType X, PriorityQueue H);
	static NodeEleType delete_max(PriorityQueue H);
	static NodeEleType find_max(PriorityQueue H);
	static int is_empty(PriorityQueue H);
	static int is_full(PriorityQueue H);
	static void replace_value(int nodeId, double newValue, PriorityQueue H);
	static void print(PriorityQueue H);
	static void min_heap_insert(NodeEleType X, PriorityQueue H);
	static NodeEleType delete_min(PriorityQueue H);
	static NodeEleType find_min(PriorityQueue H);
	static void min_heap_replace_value(int nodeId, double newValue, PriorityQueue H);
	static NodeEleType find_element(int nodeId, PriorityQueue H);
};

using THeap = Heap;
using PHeap = std::shared_ptr<THeap>;
