#include "stdafx.h"

void Alg::refresh_params()
{
	__vecSeed.clear();
	__vecLowerLattice.clear();
	__vecSearchLattice.clear();
	__vecSeedEle.clear();
	__vecExtraLatticeEle.clear();
}


void Alg::simple_prune()
{
	double incProfit;
	NodeEleType nodeEle;
	for (int i = 0; i < __numV; i++)
	{// Compute the maximum possible marginal profit of each node
		incProfit = (int)__pHyperG->_vecCover[i].size() - __pCost[i] * __numRRsets / __numV;
		if (incProfit > 0)
		{// Add the node to a max-heap if its maximum possible profit is positive
			nodeEle.id = i;
			nodeEle.value = incProfit;
			__vecExtraLatticeEle.push_back(nodeEle);
		}
	}
	__pRes->set_estimated_node_size(__numV);
	__pRes->set_simple_search_lattice_size((int)__vecExtraLatticeEle.size());
}


void Alg::iter_prune(std::vector<bool>& vecBoolVst, std::vector<int>& vecIntVst)
{
	simple_prune();
	for (auto iter : __vecExtraLatticeEle)
	{// Compute the coverage of each node
		for (auto idx : __pHyperG->_vecCover[iter.id])
		{
			++vecIntVst[idx];
		}
	}
	bool isLog = false;
	__vecSeed.clear();
	int nodeId;
	double incProfit;
	NodeEleType nodeEle;
	std::vector<NodeEleType> vecNodeEleTmp;
	vecNodeEleTmp.clear();
	bool flagS = false, flagT = true;
	while (flagS || flagT)
	{
		if (flagS)
		{// Based on A, compute B
			flagS = false;
			__vecExtraLatticeEle.clear();
			for (auto iter : vecNodeEleTmp)
			{// Use f(v|A) as an upper bound since f(v|A)>=f(v|S)
				nodeId = iter.id;
				incProfit = -__pCost[nodeId] * __numRRsets / __numV;
				for (auto idx : __pHyperG->_vecCover[nodeId])
				{
					if (!vecBoolVst[idx]) incProfit++;
				}
				if (incProfit <= 0)
				{
					for (auto idx : __pHyperG->_vecCover[nodeId]) --vecIntVst[idx];
					flagT = true;
				}
				else
				{
					nodeEle.id = nodeId;
					nodeEle.value = incProfit;
					__vecExtraLatticeEle.push_back(nodeEle);
				}
			}
			__pRes->inc_estimated_node_size((int)vecNodeEleTmp.size());
			if (isLog) std::cout << "  ---Remove non-optimal, before: " << vecNodeEleTmp.size() << ", after: " << __vecExtraLatticeEle.size() << std::endl;
		}
		if (flagT)
		{// Based on B, compute A
			flagT = false;
			vecNodeEleTmp.clear();
			for (auto iter : __vecExtraLatticeEle)
			{// Use f(v|B) as a lower bound since f(v|B)<=f(v|S)
				nodeId = iter.id;
				incProfit = __pCost[nodeId] * __numRRsets / __numV;
				for (auto idx : __pHyperG->_vecCover[nodeId])
				{
					if (vecIntVst[idx] == 1) incProfit--;
				}
				if (incProfit <= 0)
				{
					__vecSeed.push_back(nodeId);
					for (auto idx : __pHyperG->_vecCover[nodeId]) vecBoolVst[idx] = true;
					flagS = true;
				}
				else
				{
					nodeEle.id = nodeId;
					nodeEle.value = incProfit;
					vecNodeEleTmp.push_back(nodeEle);
				}
			}
			__pRes->inc_estimated_node_size((int)__vecExtraLatticeEle.size());
			if (isLog) std::cout << "  +++Add to optimal, before: " << __vecExtraLatticeEle.size() << ", after: " << vecNodeEleTmp.size() << std::endl;
		}
	}
	// Saving profit of A* and B* after pruning
	double sumProfit, totalCost = 0;
	for (auto seed : __vecSeed) totalCost += __pCost[seed];
	sumProfit = (double)count(vecBoolVst.begin(), vecBoolVst.end(), true) * __numV / __numRRsets - totalCost;
	__pRes->set_profit_lower_lattice(sumProfit);
	for (auto validNodeEle : __vecExtraLatticeEle) totalCost += __pCost[validNodeEle.id];
	sumProfit = __numV - (double)count(vecIntVst.begin(), vecIntVst.end(), 0) * __numV / __numRRsets - totalCost;
	__pRes->set_profit_upper_lattice(sumProfit);
	// Saving two upper bounds, mu(A*) and mu(B*)
	double upperBound = __pRes->get_profit_lower_lattice();
	__vecSearchLattice.clear();
	for (auto node : __vecExtraLatticeEle)
	{
		__vecSearchLattice.push_back(node.id);
		upperBound += node.value * __numV / __numRRsets;
	}
	__pRes->set_profit_bound_idx(upperBound, 0);
	upperBound = __pRes->get_profit_upper_lattice();
	for (auto node : vecNodeEleTmp) upperBound += node.value * __numV / __numRRsets;
	__pRes->set_profit_bound_idx(upperBound, 1);
	// Saving A* and B*
	__pRes->set_lower_lattice_vec(__vecSeed);
	__pRes->set_search_lattice_vec(__vecSearchLattice);
	std::vector<NodeEleType>().swap(vecNodeEleTmp);
}


void Alg::build_heap(PriorityQueue profitHeap) const
{
	double incProfit;
	NodeEleType nodeEle;
	for (int i = 0; i < __numV; i++)
	{// Build a heap according to each node's marginal profit
		incProfit = (int)__pHyperG->_vecCover[i].size() - __pCost[i] * __numRRsets / __numV;
		if (incProfit > 0)
		{
			nodeEle.id = i;
			nodeEle.value = incProfit;
			Heap::insert(nodeEle, profitHeap);
		}
	}
}


void Alg::subgradient()
{// The node sequence is ordered by seeds priori to non-seeds
	std::vector<bool> vecBoolVst(__numRRsets);
	for (auto& nodeEle : __vecSeedEle)
	{// Compute the marginal profits for every seed
		nodeEle.value = -__pCost[nodeEle.id] * __numRRsets / __numV;
		for (auto nbr : __pHyperG->_vecCover[nodeEle.id])
		{
			if (vecBoolVst[nbr]) continue;
			nodeEle.value++;
			vecBoolVst[nbr] = true;
		}
	}
	for (auto& nodeEle : __vecExtraLatticeEle)
	{// Compute the marginal profits for every non-seed
		nodeEle.value = -__pCost[nodeEle.id] * __numRRsets / __numV;
		for (auto nbr : __pHyperG->_vecCover[nodeEle.id])
		{
			if (vecBoolVst[nbr]) continue;
			nodeEle.value++;
			vecBoolVst[nbr] = true;
		}
	}
}


void Alg::supergradient1()
{
	std::vector<bool> vecBoolVst(__numRRsets);
	std::vector<int> vecIntVst(__numRRsets);
	// initialize
	for (auto seed : __vecSeed)
	{
		for (auto idx : __pHyperG->_vecCover[seed])
		{
			vecBoolVst[idx] = true;
			++vecIntVst[idx];
		}
	}
	for (auto iter : __vecExtraLatticeEle)
	{
		for (auto idx : __pHyperG->_vecCover[iter.id]) ++vecIntVst[idx];
	}
	// find the upper bound
	double incProfit, maxBound = 0;
	for (auto seed : __vecSeed)
	{
		incProfit = __pCost[seed] * __numRRsets / __numV;
		for (auto idx : __pHyperG->_vecCover[seed])
		{
			if (vecIntVst[idx] == 1) incProfit--;
		}
		maxBound += 0 > incProfit ? 0 : incProfit;
	}
	for (auto iter : __vecExtraLatticeEle)
	{
		incProfit = -__pCost[iter.id] * __numRRsets / __numV;
		for (auto idx : __pHyperG->_vecCover[iter.id])
		{
			if (!vecBoolVst[idx]) incProfit++;
		}
		maxBound += 0 > incProfit ? 0 : incProfit;
	}
	maxBound = maxBound * __numV / __numRRsets + __pRes->get_profit_org();
	__pRes->set_profit_bound_idx(maxBound, 2);
	std::cout << "  ***Super-gradient 1 (via B*): " << maxBound << std::endl;
}


void Alg::supergradient2()
{
	std::vector<bool> vecBoolVst(__numRRsets);
	std::vector<int> vecIntVst(__numRRsets);
	// initialize
	for (auto seed : __pRes->get_lower_lattice_vec())
	{
		for (auto idx : __pHyperG->_vecCover[seed]) vecBoolVst[idx] = true;
	}
	for (auto seed : __vecSeed)
	{
		for (auto idx : __pHyperG->_vecCover[seed]) ++vecIntVst[idx];
	}
	// find the upper bound
	double incProfit, maxBound = 0;
	for (auto seed : __vecSeed)
	{
		incProfit = __pCost[seed] * __numRRsets / __numV;;
		for (auto idx : __pHyperG->_vecCover[seed])
		{
			if (vecIntVst[idx] == 1) incProfit--;
		}
		maxBound += 0 > incProfit ? 0 : incProfit;
	}
	for (auto iter : __vecExtraLatticeEle)
	{
		incProfit = -__pCost[iter.id] * __numRRsets / __numV;;
		for (auto idx : __pHyperG->_vecCover[iter.id])
		{
			if (!vecBoolVst[idx]) incProfit++;
		}
		maxBound += 0 > incProfit ? 0 : incProfit;
	}
	maxBound = maxBound * __numV / __numRRsets + __pRes->get_profit_org();
	__pRes->set_profit_bound_idx(maxBound, 3);
	std::cout << "  ***Super-gradient 2 (via A*): " << maxBound << std::endl;
}


double Alg::IMM_step1(int targetSize, double epsilon, int order)
{
	double epsilonPrime = epsilon * sqrt(2);
	int64 lambdaPrime = int64((2 + 2 / 3 * epsilonPrime) * (logcnk(__numV, targetSize) + order * log(__numV) + log(log2(__numV))) / pow2(epsilonPrime));
	double threshold = __numV;
	while (true)
	{
		threshold /= 2.0;
		lambdaPrime <<= 1;
		__pHyperG->build_n_RRsets(lambdaPrime);
		__numRRsets = __pHyperG->get_RRsets_size();
		double ept = top_k_seed_heap(targetSize);
		if (ept >= (1 + epsilonPrime) * threshold)
		{// Return OPTprime=ept/(1+epsilonPrime) as a lower bound of the optimum with high probability
			double OPTprime = ept / (1 + epsilonPrime);
			return OPTprime;
		}
	}
}


double Alg::IMM_step2(int targetSize, double epsilon, int order, double OPTprime)
{
	double e = exp(1);
	double alpha = sqrt(log(__numV) + log(2));
	double beta = sqrt((1 - 1 / e) * (logcnk(__numV, targetSize) + order * log(__numV) + log(2)));
	int64 R = int64(2.0 * __numV * pow2((1 - 1 / e) * alpha + beta) / pow2(epsilon) / OPTprime);
	__pHyperG->build_n_RRsets(R);
	__numRRsets = __pHyperG->get_RRsets_size();
	double opt = top_k_seed_heap(targetSize);
	return opt;
}


void Alg::build_n_RRsets(int64 numRRsets)
{
	__pHyperG->build_n_RRsets(numRRsets);
	__numRRsets = __pHyperG->get_RRsets_size();
}


double Alg::top_k_seed(int targetSize)
{
	std::vector<int> degree;
	std::vector<bool> visitLocal(__numRRsets);
	__vecSeed.clear();
	for (int i = 0; i < __numV; i++)
	{
		degree.push_back((int)__pHyperG->_vecCover[i].size());
	}
	std::vector<int> vecMargin(targetSize);
	int sumInf = 0;
	for (int i = 0; i < targetSize; i++)
	{// Find the largest marginal influence gain and remove the corresponding hyper-edge
		auto t = max_element(degree.begin(), degree.end());
		int id = int(t - degree.begin());
		sumInf += degree[id];
		__vecSeed.push_back(id);
		degree[id] = -1;
		for (int i1 : __pHyperG->_vecCover[id])
		{
			if (!visitLocal[i1])
			{
				visitLocal[i1] = true;
				for (int node : __pHyperG->_vecCoverRev[i1])
				{
					degree[node]--;
				}
			}
		}
	}
	return 1.0 * sumInf * __numV / __numRRsets;
}


double Alg::top_k_seed_heap(int targetSize)
{
	PriorityQueue infHeap = Heap::initialize_heap(__numV);
	NodeEleType nodeEle;
	for (int i = 0; i < __numV; i++)
	{
		nodeEle.id = i;
		nodeEle.value = (int)__pHyperG->_vecCover[i].size();
		Heap::insert(nodeEle, infHeap);
	}
	int roundIndex = 0;
	std::vector<bool> vecBoolVst(__numRRsets);
	std::vector<int> comp(__numV);
	double incInf, sumInf = 0;
	__vecSeed.clear();
	while (roundIndex < targetSize)
	{// Find the largest marginal influence gain and recompute the out-date information using lazy forward approach
		nodeEle = Heap::delete_max(infHeap);
		if (comp[nodeEle.id] == roundIndex)
		{
			__vecSeed.push_back(nodeEle.id);
			sumInf += nodeEle.value;
			roundIndex++;
			for (auto idx : __pHyperG->_vecCover[nodeEle.id]) vecBoolVst[idx] = true;
		}
		else
		{
			incInf = 0;
			for (auto idx : __pHyperG->_vecCover[nodeEle.id])
			{
				if (!vecBoolVst[idx]) incInf++;
			}
			nodeEle.value = incInf;
			Heap::insert(nodeEle, infHeap);
			comp[nodeEle.id] = roundIndex;
		}
	}
	Heap::destroy(infHeap);
	return sumInf * __numV / __numRRsets;
}


void Alg::simple_greedy(bool isPerfectPrune)
{
	refresh_params();
	double sumCost = 0, sumProfit = 0, incProfit;
	std::vector<int> vecSeed;
	std::vector<double> profitVec(__numV);
	std::vector<bool> visit_local(__numRRsets);
	for (int i = 0; i < __numV; i++)
	{
		profitVec[i] = (int)__pHyperG->_vecCover[i].size() - __pCost[i] * __numRRsets / __numV;
	}
	__pRes->set_estimated_node_size(__numV);
	while (true)
	{// Find the largest marginal influence gain and remove the corresponding hyper-edge
		int id = (int)distance(profitVec.begin(), max_element(profitVec.begin(), profitVec.end()));
		incProfit = profitVec[id];
		if (incProfit <= 0) break;
		vecSeed.push_back(id);
		sumProfit += incProfit;
		sumCost += __pCost[id];
		profitVec[id] = -1;
		for (auto idx : __pHyperG->_vecCover[id])
		{
			if (!visit_local[idx])
			{
				visit_local[idx] = true;
				for (int node : __pHyperG->_vecCoverRev[idx])
				{
					profitVec[node]--;
				}
			}
		}
	}
	__pRes->set_profit_org(sumProfit * __numV / __numRRsets);
	__pRes->set_total_cost(sumCost);
	__pRes->set_seed_vec(vecSeed);
}


void Alg::simple_greedy_heap(bool isPerfectPrune)
{
	refresh_params();
	int *comp, roundIndex = 0;
	double sumCost = 0, sumProfit = 0, incProfit;
	comp = (int *)calloc(__numV, sizeof(int));
	std::vector<bool> vecBoolVst(__numRRsets);
	PriorityQueue profitHeap = Heap::initialize_heap(__numV);
	NodeEleType nodeU;
	if (isPerfectPrune)
	{// Iterative pruning
		std::vector<int> vecIntVst(__numRRsets);
		iter_prune(vecBoolVst, vecIntVst);
		for (auto nodeEle : __vecExtraLatticeEle) Heap::insert(nodeEle, profitHeap);
		sumProfit = __pRes->get_profit_lower_lattice() * __numRRsets / __numV;
		for (auto seed : __vecSeed) sumCost += __pCost[seed];
		__vecExtraLatticeEle.clear();
	}
	else
	{
		build_heap(profitHeap);
		__pRes->set_estimated_node_size(__numV);
		__pRes->set_simple_search_lattice_size((int)profitHeap->Size);
		__pRes->set_search_lattice_size((int)profitHeap->Size);
	}

	while (!Heap::is_empty(profitHeap))
	{// Find the largest marginal profit gain and recompute the out-date information using lazy forward approach
		nodeU = Heap::delete_max(profitHeap);
		if (nodeU.value <= 0)
		{
			__vecExtraLatticeEle.push_back(nodeU);
			continue;
		}
		if (comp[nodeU.id] == roundIndex)
		{
			__vecSeed.push_back(nodeU.id);
			sumProfit += nodeU.value;
			sumCost += __pCost[nodeU.id];
			roundIndex++;
			for (auto idx : __pHyperG->_vecCover[nodeU.id]) vecBoolVst[idx] = true;
			__vecSeedEle.push_back(nodeU);
		}
		else
		{
			__pRes->inc_estimated_node_size(1);
			incProfit = -__pCost[nodeU.id] * __numRRsets / __numV;
			for (auto idx : __pHyperG->_vecCover[nodeU.id])
			{
				if (!vecBoolVst[idx]) incProfit++;
			}
			if (incProfit <= 0)
			{
				__vecExtraLatticeEle.push_back(nodeU);
				continue;
			}
			nodeU.value = incProfit;
			Heap::insert(nodeU, profitHeap);
			comp[nodeU.id] = roundIndex;
		}
	}
	__pRes->set_profit_org(sumProfit * __numV / __numRRsets);
	__pRes->set_total_cost(sumCost);
	__pRes->set_seed_vec(__vecSeed);
	free(comp);
	Heap::destroy(profitHeap);
}


void Alg::double_greedy(bool isPerfectPrune, bool isDeter)
{
	refresh_params();
	int nodeId;
	double sumCostS = 0, sumProfitS, incProfitS, sumCostT = 0, sumProfitT, incProfitT, prob;
	bool cond;
	std::vector<bool> vecBoolVst(__numRRsets);
	std::vector<int> vecIntVst(__numRRsets);
	std::vector<NodeEleType> profitVec;
	if (isPerfectPrune)
	{// Iterative pruning
		iter_prune(vecBoolVst, vecIntVst);
		sumProfitS = __pRes->get_profit_lower_lattice() * __numRRsets / __numV;
		sumProfitT = __pRes->get_profit_upper_lattice();
		for (auto seed : __vecSeed) sumCostS += __pCost[seed];
		profitVec.swap(__vecExtraLatticeEle);
	}
	else
	{
		for (int i = 0; i < __numRRsets; i++) vecIntVst[i] = (int)__pHyperG->_vecCoverRev[i].size();
		NodeEleType nodeEle;
		for (int i = 0; i < __numV; i++)
		{
			sumCostT += __pCost[i];
			nodeEle.id = i;
			nodeEle.value = -1;
			profitVec.push_back(nodeEle);
		}
		sumProfitS = 0;
		sumProfitT = __numV - sumCostT;
		__pRes->set_profit_upper_lattice(sumProfitT);
		__pRes->set_lower_lattice_size((int)__vecSeed.size());
		__pRes->set_search_lattice_size((int)profitVec.size());
	}
	__pRes->inc_estimated_node_size(__numV + 2 * (int)profitVec.size());
	//random_shuffle(profitVec.begin(), profitVec.end());
	sumProfitT *= 1.0 * __numRRsets / __numV;
	for (auto iter : profitVec)
	{
		nodeId = iter.id;
		// Compute f(v|S)
		incProfitS = -__pCost[nodeId] * __numRRsets / __numV;
		// Compute -f(v|T\{v})
		incProfitT = __pCost[nodeId] * __numRRsets / __numV;
		for (auto idx : __pHyperG->_vecCover[nodeId])
		{
			if (!vecBoolVst[idx]) incProfitS++;
			if (vecIntVst[idx] == 1) incProfitT--;
		}
		if (isDeter)
		{
			cond = (incProfitS >= incProfitT);
		}
		else
		{
			incProfitS = 0 > incProfitS ? 0 : incProfitS;
			incProfitT = 0 > incProfitT ? 0 : incProfitT;
			if (incProfitS + incProfitT < 1e-8)
			{
				prob = 1;
			}
			else
			{
				prob = incProfitS / (incProfitS + incProfitT);
			}
			cond = dsfmt_gv_genrand_open_close() <= prob;
		}
		if (cond)
		{// Add the node to S
			sumProfitS += incProfitS;
			sumCostS += __pCost[nodeId];
			__vecSeed.push_back(nodeId);
			for (auto idx : __pHyperG->_vecCover[nodeId]) vecBoolVst[idx] = true;
			__vecSeedEle.push_back(iter);
		}
		else
		{// Remove the node from T
			sumProfitT += incProfitT;
			for (auto idx : __pHyperG->_vecCover[nodeId]) --vecIntVst[idx];
			__vecExtraLatticeEle.push_back(iter);
		}
	}
	assert(abs(sumProfitS- sumProfitT) < 1);
	__pRes->set_profit_org(sumProfitS * __numV / __numRRsets);
	__pRes->set_total_cost(sumCostS);
	__pRes->set_seed_vec(__vecSeed);
	__pRes->set_profit_bound_double_greedy();
}


void Alg::upper_bound()
{
	supergradient1();
	supergradient2();
}


std::vector<double> Alg::all_upper_bound_B(double* pCost, std::vector<int>& vecNonOptimal, int sizeResult)
{
	auto additionalSeedSize = (int)__vecExtraLatticeEle.size();
	if (sizeResult < 0 || sizeResult > additionalSeedSize) sizeResult = additionalSeedSize;
	auto step = additionalSeedSize / sizeResult + 1;
	auto remainder = additionalSeedSize % sizeResult;
	std::vector<double> vecResult(sizeResult + 1);
	std::vector<bool> vecBoolVst(__numRRsets);
	std::vector<int> vecIntVst(__numRRsets);
	// initialize
	for (auto seed : __vecSeed)
	{
		for (auto idx : __pHyperG->_vecCover[seed])
		{
			vecBoolVst[idx] = true;
			++vecIntVst[idx];
		}
	}
	for (auto iter : __vecExtraLatticeEle)
	{
		for (auto idx : __pHyperG->_vecCover[iter.id]) ++vecIntVst[idx];
	}
	for (auto node : vecNonOptimal)
	{
		for (auto idx : __pHyperG->_vecCover[node]) ++vecIntVst[idx];
	}
	// Build the incremental profit to quit from B
	double incProfit, baseMaxBound = 0;
	std::vector<double> vecIncProfitB(__numV);
	for (auto node : __vecSeed)
	{
		incProfit = pCost[node] * __numRRsets / __numV;
		for (auto idx : __pHyperG->_vecCover[node])
		{
			if (vecIntVst[idx] == 1) incProfit--;
		}
		vecIncProfitB[node] = incProfit;
	}
	for (auto iter : __vecExtraLatticeEle)
	{
		auto node = iter.id;
		incProfit = pCost[node] * __numRRsets / __numV;
		for (auto idx : __pHyperG->_vecCover[node])
		{
			if (vecIntVst[idx] == 1) incProfit--;
		}
		vecIncProfitB[node] = incProfit;
	}
	// Find the optimum
	for (auto seed : __vecSeed)
	{
		baseMaxBound += 0 > vecIncProfitB[seed] ? 0 : vecIncProfitB[seed];
	}
	baseMaxBound += __pRes->get_profit_org() * __numRRsets / __numV;
	int counter = 0, currSize = 0;
	for (int extraSeedIdx = (int)__vecExtraLatticeEle.size() + 1; extraSeedIdx--;)
	{
		if (counter % step == 0)
		{
			if (currSize % 20 == 0) LogInfo("TotalB", currSize);
			auto maxBound = baseMaxBound;
			for (int idx = 0; idx < extraSeedIdx; idx++)
			{
				auto nodeId = __vecExtraLatticeEle[idx].id;
				incProfit = -pCost[nodeId] * __numRRsets / __numV;
				for (auto sampleIdx : __pHyperG->_vecCover[nodeId])
				{
					if (!vecBoolVst[sampleIdx]) incProfit++;
				}
				maxBound += 0 > incProfit ? 0 : incProfit;
			}
			vecResult[sizeResult - currSize] = maxBound * __numV / __numRRsets;
			if (currSize == remainder) step--;
			currSize++;
			counter = 0;
		}
		if (extraSeedIdx > 0)
		{
			auto seed = __vecExtraLatticeEle[extraSeedIdx - 1].id;
			incProfit = -pCost[seed] * __numRRsets / __numV;
			for (auto sampleIdx : __pHyperG->_vecCover[seed])
			{
				if (!vecBoolVst[sampleIdx])
				{
					incProfit++;
					vecBoolVst[sampleIdx] = true;
				}
			}
			baseMaxBound += incProfit; // The incremental profit of the last node 
			baseMaxBound += 0 > vecIncProfitB[seed] ? 0 : vecIncProfitB[seed]; // The incremental profit of quiting from B
			counter++;
		}
	}
	return vecResult;
}


std::vector<double> Alg::all_upper_bound_A(double* pCost, std::vector<int>& vecNonOptimal, int sizeResult)
{
	int sizeA = (int)__pRes->get_lower_lattice_vec().size();
	auto additionalSeedSize = (int)__vecSeed.size() - sizeA;
	if (sizeResult < 0 || sizeResult > additionalSeedSize) sizeResult = additionalSeedSize;
	auto step = additionalSeedSize / sizeResult + 1;
	auto remainder = additionalSeedSize % sizeResult;
	std::vector<double> vecResult(sizeResult + 1);
	std::vector<bool> vecBoolVst(__numRRsets);
	std::vector<int> vecIntVst(__numRRsets);
	// initialize
	if (vecNonOptimal.empty()) // From A to B if vecNonOptimal is empty, otherwise from empty to V
	{
		for (auto seed : __pRes->get_lower_lattice_vec())
		{
			for (auto idx : __pHyperG->_vecCover[seed]) vecBoolVst[idx] = true;
		}
	}
	for (auto seed : __vecSeed)
	{
		for (auto idx : __pHyperG->_vecCover[seed]) ++vecIntVst[idx];
	}
	// Build the incremental profit to quit from B
	double incProfit, baseMaxBound = 0;
	std::vector<double> vecIncProfitA(__numV);
	for (int node = 0; node < __numV; node++)
	{
		incProfit = -pCost[node] * __numRRsets / __numV;;
		for (auto idx : __pHyperG->_vecCover[node])
		{
			if (!vecBoolVst[idx]) incProfit++;
		}
		vecIncProfitA[node] = incProfit;
	}
	// find the upper bound
	for (auto iter : __vecExtraLatticeEle)
	{
		auto node = iter.id;
		baseMaxBound += 0 > vecIncProfitA[node] ? 0 : vecIncProfitA[node];
	}
	for (auto node : vecNonOptimal)
	{
		baseMaxBound += 0 > vecIncProfitA[node] ? 0 : vecIncProfitA[node];
	}
	baseMaxBound += __pRes->get_profit_org() * __numRRsets / __numV;

	int counter = 0, currSize = 0;
	for (int seedSize = (int)__vecSeed.size(); seedSize >= sizeA; seedSize--)
	{
		if (counter % step == 0)
		{
			if (currSize % 20 == 0) LogInfo("TotalA", currSize);
			auto maxBound = baseMaxBound;
			for (auto seedIdx = 0; seedIdx < seedSize; seedIdx++)
			{
				auto seed = __vecSeed[seedIdx];
				incProfit = pCost[seed] * __numRRsets / __numV;;
				for (auto idx : __pHyperG->_vecCover[seed])
				{
					if (vecIntVst[idx] == 1) incProfit--;
				}
				maxBound += 0 > incProfit ? 0 : incProfit;
			}
			//LogInfo(maxBound * __numV / __RRsetsSize);
			vecResult[sizeResult - currSize] = maxBound * __numV / __numRRsets;
			if (currSize == remainder) step--;
			currSize++;
			counter = 0;
		}
		if (seedSize > sizeA)
		{
			auto seed = __vecSeed[seedSize - 1];
			incProfit = pCost[seed] * __numRRsets / __numV;
			for (auto idx : __pHyperG->_vecCover[seed])
			{
				if (vecIntVst[idx] == 1) incProfit--;
				--vecIntVst[idx];
			}
			baseMaxBound += incProfit; // The incremental profit of the last node 
			baseMaxBound += 0 > vecIncProfitA[seed] ? 0 : vecIncProfitA[seed]; // The incremental profit of adding into A
			counter++;
		}
	}
	return vecResult;
}


double Alg::IMM(int targetSize, double epsilon, int order)
{
	dsfmt_gv_init_gen_rand(0);
	double OPTprime = IMM_step1(targetSize, epsilon, order);
	double optFinal = IMM_step2(targetSize, epsilon, order, OPTprime);
	return optFinal;
}


double Alg::BCT(int targetSize, double epsilon, int order)
{
	double e = exp(1);
	double c = 2 * (e - 2);
	double gamma = 8 * c * pow2(1 - 1 / (2 * e)) * (logcnk(__numV, targetSize) + order * log(__numV) + 2.0 / __numV) / pow2(epsilon);
	int64 lambda = int64((1 + e * epsilon / (2 * e - 1)) * gamma);
	int64 R = lambda;
	while (true)
	{
		build_n_RRsets(R);
		top_k_seed_heap(targetSize);
		double opt = __pHyperG->eval_inf_spread(__vecSeed) * __numRRsets / __numV;
		std::cout << opt << std::endl;
		if (opt > lambda)
		{
			return opt;
		}
		R *= 2;
	}
}


void Alg::baseline_random() const
{
	Timer timerRD("random");
	std::vector<int> vecNode(__numV);
	iota(vecNode.begin(), vecNode.end(), 0);
	double maxProfit = -INFINITY, maxTotalCost = -1.0;
	int seedSize = -1;
	for (int i = 0; i < 11; i++)
	{// Search the best seed size from n/2^i for i=0,1,...,10
		double avgProfit = 0.0, avgTotalCost = 0.0;
		int numSeed = __numV >> i;
		for (int j = 0; j < 10; j++)
		{// Repeat 10 times and take the average
			random_shuffle(vecNode.begin(), vecNode.end());
			std::vector<int> vecSeed(vecNode.begin(), vecNode.begin() + numSeed);
			double totalCost = 0;
			for (auto seed : vecSeed)
			{
				totalCost += __pCost[seed];
			}
			//auto inf = __pHyperG->effic_inf_valid_algo(vecSeed, 0.001, 0.01);
			auto inf = __pHyperG->inf_valid_algo(vecSeed, 100000);
			auto profit = inf - totalCost;
			avgProfit += profit;
			avgTotalCost += totalCost;
		}
		std::cout << "  round " << i + 1 << " of 11, profit: " << avgProfit / 10.0 << ", seed size: " << numSeed << '\n';
		if (avgProfit / 10.0 > maxProfit)
		{
			maxProfit = avgProfit;
			maxTotalCost = avgTotalCost;
			seedSize = numSeed;
		}
	}
	__pRes->set_running_time(timerRD.get_total_time());
	__pRes->set_profit(maxProfit);
	__pRes->set_total_cost(maxTotalCost);
	__pRes->set_seed_size(seedSize);
}


void Alg::baseline_highdegree() const
{
	Timer timerHD("high-degree");
	auto outDeg = __pHyperG->get_out_degree();
	std::vector<NodeEleType> vecEle(__numV);
	for (int i = 0; i < __numV; i++)
	{
		vecEle[i].id = i;
		vecEle[i].value = (double)outDeg[i];
	}
	// Sort the node by its degree
	sort(vecEle.begin(), vecEle.end(), greater());
	std::vector<int> vecNode(__numV);
	for (int i = 0; i < __numV; i++) vecNode[i] = vecEle[i].id;
	double totalCost, profit;
	double maxProfit = -INFINITY, maxTotalCost = -1.0;
	std::vector<int> maxVecSeed;
	for (int i = 0; i < 11; i++)
	{// Search the best seed size from n/2^i for i=0,1,...,10
		int numSeed = __numV >> i;
		std::vector<int> vecSeed(vecNode.begin(), vecNode.begin() + numSeed);
		totalCost = 0.0;
		for (auto seed : vecSeed)
		{
			totalCost += __pCost[seed];
		}
		//auto inf = __pHyperG->effic_inf_valid_algo(vecSeed, 1e-3, 0.01);
		auto inf = __pHyperG->inf_valid_algo(vecSeed, 100000);
		profit = inf - totalCost;
		std::cout << "  round " << i + 1 << " of 11, profit: " << profit << ", seed size: " << numSeed << '\n';
		if (profit > maxProfit)
		{
			maxProfit = profit;
			maxTotalCost = totalCost;
			maxVecSeed = vecSeed;
		}
	}
	__pRes->set_running_time(timerHD.get_total_time());
	__pRes->set_profit(maxProfit);
	__pRes->set_total_cost(maxTotalCost);
	__pRes->set_seed_vec(maxVecSeed);
}


void Alg::baseline_infmax(const double epsilon, const int order, const bool isUniBen)
{
	Timer timerIM("IM");
	double totalCost, profit;
	double maxProfit = -INFINITY, maxTotalCost = -1.0;
	std::vector<int> maxVecSeed;
	int maxRR = 0;
	for (int i = 0; i < 11; i++)
	{// Search the best seed size from n/2^i for i=0,1,...,10
		int numSeed = __numV >> i;
		if (isUniBen) IMM(numSeed, epsilon, order);
		else BCT(numSeed, epsilon, order);
		totalCost = 0.0;
		for (auto seed : __vecSeed)
		{
			totalCost += __pCost[seed];
		}
		//auto inf = __pHyperG->effic_inf_valid_algo(__vecSeed, 1e-3, 0.01);
		auto inf = __pHyperG->inf_valid_algo(__vecSeed, 100000);
		profit = inf - totalCost;
		std::cout << "  round " << i + 1 << " of 11, profit: " << profit << ", seed size: " << numSeed << '\n';
		if (profit > maxProfit)
		{
			maxProfit = profit;
			maxTotalCost = totalCost;
			maxVecSeed = __vecSeed;
		}
		auto numRR = __pHyperG->get_RRsets_size();
		maxRR = maxRR > (int)numRR ? maxRR : (int)numRR;
		__pHyperG->refresh_hypergraph();
	}
	__pRes->set_running_time(timerIM.get_total_time());
	__pRes->set_profit(maxProfit);
	__pRes->set_total_cost(maxTotalCost);
	__pRes->set_seed_vec(maxVecSeed);
	__pRes->set_RRsets_size(maxRR);
}


void Alg::greedy(const int numRRsets, const int greedyType, const bool isPerfectPrune)
{// greedyTyepe, 0->simple greedy, 1->double greedy (default), 2->randomized double greedy,  
	Timer timerGreedy("greedy");
	build_n_RRsets(numRRsets);
	if (greedyType == 0) simple_greedy_heap(isPerfectPrune);
	else if (greedyType == 2) double_greedy(isPerfectPrune, false);
	else double_greedy(isPerfectPrune, true);
	__pRes->set_running_time(timerGreedy.get_total_time());
	upper_bound();
	//auto infReal = __pHyperG->effic_inf_valid_algo(__vecSeed, 1e-3, 0.01);
	auto infReal = __pHyperG->inf_valid_algo(__vecSeed, 100000);
	auto profit = infReal - __pRes->get_total_cost();
	__pRes->set_profit(profit);
	__pRes->set_RRsets_size(numRRsets);
}
