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
	static PriorityQueue initialize_heap(int MaxElements)
	{
		PriorityQueue H = (PriorityQueue)(malloc(sizeof(struct HeapStruct)));

		// Allocate the array plus one extra for sentinel
		H->Elements = (NodeEleType *)(malloc((MaxElements + 1) * sizeof(NodeEleType)));

		H->Capacity = MaxElements;
		H->Size = 0;
		H->Elements[0].id = -1;
		H->Elements[0].value = INFINITY;
		return H;
	}

	/// Destroy a heap
	static void destroy(PriorityQueue H)
	{
		free(H->Elements);
		free(H);
	}

	/// Clear a heap
	static void make_empty(PriorityQueue H)
	{
		H->Size = 0;
	}

	/// Check whether the heap is empty. Return true if empty.
	static int is_empty(PriorityQueue H)
	{
		return H->Size == 0;
	}

	/// Check whether the heap is full. Return true if full.
	static int is_full(PriorityQueue H)
	{
		return H->Size == H->Capacity;
	}

	/// Insert a element X to a heap H
	static void insert(NodeEleType X, PriorityQueue H)
	{// H->Elements[0] is a sentinel
		int i;

		for (i = ++H->Size; (H->Elements[i / 2].value < X.value) && (i != 1); i /= 2) // The new element is percolated up the heap
			H->Elements[i] = H->Elements[i / 2]; // until the correct location is found
		H->Elements[i] = X;
	}

	/// Remove the maximum element
	static NodeEleType delete_max(PriorityQueue H)
	{
		int i, Child;
		NodeEleType MaxElement, LastElement;

		MaxElement = H->Elements[1];
		LastElement = H->Elements[H->Size--];

		for (i = 1; i * 2 <= H->Size; i = Child)
		{
			// Find larger child
			Child = i * 2;
			if (Child != H->Size && H->Elements[Child + 1].value > H->Elements[Child].value)
				Child++;

			// Percolate one level
			if (LastElement.value < H->Elements[Child].value)
				H->Elements[i] = H->Elements[Child];
			else
				break;
		}
		H->Elements[i] = LastElement;
		return MaxElement;
	}

	/// Get the maximum element
	static NodeEleType find_max(PriorityQueue H)
	{
		if (!is_empty(H))
			return H->Elements[1];
		return H->Elements[0];
	}

	/// Update the value of a element 
	static void replace_value(int nodeId, double newValue, PriorityQueue H)
	{
		NodeEleType X = {};
		int nodeIndex = 0;
		for (auto index = 1; index <= H->Size; index++)
		{
			X = H->Elements[index];
			if (X.id == nodeId)
			{
				nodeIndex = index;
				break;
			}
		}
		if (nodeIndex == 0)
		{
			std::cout << "The node does not exist." << std::endl;
		}
		if (newValue > X.value)
		{
			X.value = newValue;
			int i;
			for (i = nodeIndex; (H->Elements[i / 2].value < X.value) && (i != 1); i /= 2) // The new element is percolated up the heap
				H->Elements[i] = H->Elements[i / 2]; // until the correct location is found
			H->Elements[i] = X;
		}
		else if (newValue < X.value)
		{
			X.value = newValue;
			int i, Child;
			for (i = nodeIndex; i * 2 <= H->Size; i = Child)
			{
				/* Find larger child */
				Child = i * 2;
				if (Child != H->Size && H->Elements[Child + 1].value > H->Elements[Child].value)
					Child++;
				/* Percolate one level */
				if (X.value < H->Elements[Child].value)
					H->Elements[i] = H->Elements[Child];
				else
					break;
			}
			H->Elements[i] = X;
		}
	}

	/// Print the heap
	static void print(PriorityQueue H)
	{
		std::cout << "Heap: ";
		for (int i = 0; i < H->Size + 1; i++)
			std::cout << H->Elements[i].value << ' ';
		std::cout << "\n";
	}

	/// For min-heap, insert a element
	static void min_heap_insert(NodeEleType X, PriorityQueue H)
	{
		X.value = -X.value;
		insert(X, H);
	}

	/// For min-heap, remove the minimum element
	static NodeEleType delete_min(PriorityQueue H)
	{
		NodeEleType X = delete_max(H);
		X.value = -X.value;
		return X;
	}

	/// For min-heap, get the minimum element
	static NodeEleType find_min(PriorityQueue H)
	{
		NodeEleType X = find_max(H);
		X.value = -X.value;
		return X;
	}

	/// For min-heap, update the value of a element
	static void min_heap_replace_value(int nodeId, double newValue, PriorityQueue H)
	{
		replace_value(nodeId, -newValue, H);
	}

	/// For min-heap, find the element in the heap by its id
	static NodeEleType find_element(int nodeId, PriorityQueue H)
	{
		NodeEleType X;
		for (auto index = 1; index <= H->Size; index++)
		{
			X = H->Elements[index];
			if (X.id == nodeId) return X;
		}
		return {};
	}
};

using THeap = Heap;
using PHeap = std::shared_ptr<THeap>;
