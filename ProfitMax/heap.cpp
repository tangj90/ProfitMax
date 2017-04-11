#include "stdafx.h"

PriorityQueue Heap::initialize_heap(int MaxElements)
{
	PriorityQueue H = (PriorityQueue)(malloc(sizeof(struct HeapStruct)));

	/* Allocate the array plus one extra for sentinel */
	H->Elements = (NodeEleType *)(malloc((MaxElements + 1) * sizeof(NodeEleType)));

	H->Capacity = MaxElements;
	H->Size = 0;
	H->Elements[0].id = -1;
	H->Elements[0].value = INFINITY;
	return H;
}

/* H->Elements[0] is a sentinel */
void Heap::insert(NodeEleType X, PriorityQueue H)
{
	int i;

	for (i = ++H->Size; (H->Elements[i / 2].value < X.value) && (i != 1); i /= 2) /* The new element is percolated up the heap  */
		H->Elements[i] = H->Elements[i / 2]; /* until the correct location is found */
	H->Elements[i] = X;
}


NodeEleType Heap::delete_max(PriorityQueue H)
{
	int i, Child;
	NodeEleType MaxElement, LastElement;

	MaxElement = H->Elements[1];
	LastElement = H->Elements[H->Size--];

	for (i = 1; i * 2 <= H->Size; i = Child)
	{
		/* Find larger child */
		Child = i * 2;
		if (Child != H->Size && H->Elements[Child + 1].value > H->Elements[Child].value)
			Child++;

		/* Percolate one level */
		if (LastElement.value < H->Elements[Child].value)
			H->Elements[i] = H->Elements[Child];
		else
			break;
	}
	H->Elements[i] = LastElement;
	return MaxElement;
}


void Heap::make_empty(PriorityQueue H)
{
	H->Size = 0;
}


NodeEleType Heap::find_max(PriorityQueue H)
{
	if (!Heap::is_empty(H))
		return H->Elements[1];
	return H->Elements[0];
}


int Heap::is_empty(PriorityQueue H)
{
	return H->Size == 0;
}


int Heap::is_full(PriorityQueue H)
{
	return H->Size == H->Capacity;
}


void Heap::destroy(PriorityQueue H)
{
	free(H->Elements);
	free(H);
}


void Heap::replace_value(int nodeId, double newValue, PriorityQueue H)
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
		for (i = nodeIndex; (H->Elements[i / 2].value < X.value) && (i != 1); i /= 2) /* The new element is percolated up the heap  */
			H->Elements[i] = H->Elements[i / 2]; /* until the correct location is found */
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


void Heap::print(PriorityQueue H)
{
	printf("Heap: ");
	for (int i = 0; i < H->Size + 1; i++)
		printf("%f ", H->Elements[i].value);
	printf("\n");
}


void Heap::min_heap_insert(NodeEleType X, PriorityQueue H)
{
	X.value = -X.value;
	Heap::insert(X, H);
}


NodeEleType Heap::delete_min(PriorityQueue H)
{
	NodeEleType X = Heap::delete_max(H);
	X.value = -X.value;
	return X;
}


NodeEleType Heap::find_min(PriorityQueue H)
{
	NodeEleType X = Heap::find_max(H);
	X.value = -X.value;
	return X;
}

void Heap::min_heap_replace_value(int nodeId, double newValue, PriorityQueue H)
{
	replace_value(nodeId, -newValue, H);
}

NodeEleType Heap::find_element(int nodeId, PriorityQueue H)
{
	NodeEleType X;
	for (auto index = 1; index <= H->Size; index++)
	{
		X = H->Elements[index];
		if (X.id == nodeId) return X;
	}
	return {};
}
