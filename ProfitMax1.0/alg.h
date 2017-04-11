#pragma once

class Alg
{
private:
	int __numV, __numRRsets;
	std::vector<int> __vecSeed, __vecLowerLattice, __vecSearchLattice;
	std::vector<NodeEleType> __vecSeedEle, __vecExtraLatticeEle;
	//PriorityQueue __pHeap;
	const double* __pCost;
	PHyperGraphRef __pHyperG;
	PResult __pRes;

	void reflesh_params();
	void simple_prune();
	void iter_prune(std::vector<bool>& vecBoolVst, std::vector<int>& vecIntVst);
	void build_heap(PriorityQueue profitHeap) const;
	void subgradient();
	void supergradient1();
	void supergradient2();
	double IMM_step1(int targetSize, double epsilon, int order);
	double IMM_step2(int targetSize, double epsilon, int order, double OPTprime);
public:
	Alg(const double* pCost, const PHyperGraphRef& pHyperG, const PResult& pRes) : __pCost(pCost), __pHyperG(pHyperG), __pRes(pRes)
	{
		__numV = __pHyperG->get_nodes();
		__numRRsets = __pHyperG->get_RRsets_size();
	}

	void build_n_RRsets(int64 numHyperE);
	double top_k_seed(int targetSize);
	double top_k_seed_heap(int targetSize);
	void simple_greedy(bool isPerfectPrune = false);
	void simple_greedy_heap(bool isPerfectPrune = false);
	void double_greedy(bool isPerfectPrune = false, bool isDeter = true);
	void upper_bound();
	std::vector<int> get_non_selected_nodes();
	std::vector<double> all_upper_bound_B(double* pCost, std::vector<int>& vecNonOptimal, int sizeResult = -1);
	std::vector<double> all_upper_bound_A(double* pCost, std::vector<int>& vecNonOptimal, int sizeResult = -1);
	double IMM(const int targetSize, const double epsilon, const int order);
	double BCT(const int targetSize, const double epsilon, const int order);
	void baseline_random() const;
	void baseline_highdegree() const;
	void baseline_infmax(const double epsilon, const int order, const bool isUniBen = true);
	/// Greedy algorithm, default is double greedy
	void greedy(const int numRRsets, const int greedyType = 1, const bool isPerfectPrune = false);
};

using TAlg = Alg;
using PAlg = std::shared_ptr<TAlg>;
