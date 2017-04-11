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
	/// Initialize a heap
	static PriorityQueue initialize_heap(int MaxElements);
	/// Destroy a heap
	static void destroy(PriorityQueue H);
	/// Clear a heap
	static void make_empty(PriorityQueue H);
	/// Insert a element X to a heap H
	static void insert(NodeEleType X, PriorityQueue H);
	/// Remove the maximum element
	static NodeEleType delete_max(PriorityQueue H);
	/// Get the maximum element
	static NodeEleType find_max(PriorityQueue H);
	/// Check whether the heap is empty. Return true if empty.
	static int is_empty(PriorityQueue H);
	/// Check whether the heap is full. Return true if full.
	static int is_full(PriorityQueue H);
	/// Update the value of a element 
	static void replace_value(int nodeId, double newValue, PriorityQueue H);
	/// Print the heap
	static void print(PriorityQueue H);
	/// For min-heap, insert a element
	static void min_heap_insert(NodeEleType X, PriorityQueue H);
	/// For min-heap, remove the minimum element
	static NodeEleType delete_min(PriorityQueue H);
	/// For min-heap, get the minimum element
	static NodeEleType find_min(PriorityQueue H);
	/// For min-heap, update the value of a element
	static void min_heap_replace_value(int nodeId, double newValue, PriorityQueue H);
	/// For min-heap, find the element in the heap by its id
	static NodeEleType find_element(int nodeId, PriorityQueue H);
};

using THeap = Heap;
using PHeap = std::shared_ptr<THeap>;
