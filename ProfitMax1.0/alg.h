#pragma once

class Alg
{
private:
	int __numV, __numRRsets;
	std::vector<int> __vecSeed, __vecLowerLattice, __vecSearchLattice;
	std::vector<NodeEleType> __vecSeedEle, __vecExtraLatticeEle;
	const double* __pCost;
	PHyperGraphRef __pHyperG;
	PResult __pRes;

	///Refresh all the parameters
	void refresh_params();
	/// A simple pruning
	void simple_prune();
	/// Iterative pruning
	void iter_prune(std::vector<bool>& vecBoolVst, std::vector<int>& vecIntVst);
	/// Build a maximum heap in terms of profit generated of each node
	void build_heap(PriorityQueue profitHeap) const;
	/// Sub-gradient, a tight modular lower bound of submodular
	void subgradient();
	/// Type-1 super-gradient via full node set (B*), a tight modular upper bound of submodular
	void supergradient1();
	/// Type-2 super-gradient via empty node set (A*), a tight modular upper bound of submodular
	void supergradient2();
	/// IMM step 1, refer to Tang et al. SIGMOD 2016
	double IMM_step1(int targetSize, double epsilon, int order);
	/// IMM step 2, refer to Tang et al. SIGMOD 2016
	double IMM_step2(int targetSize, double epsilon, int order, double OPTprime);
public:
	Alg(const double* pCost, const PHyperGraphRef& pHyperG, const PResult& pRes) : __pCost(pCost), __pHyperG(pHyperG), __pRes(pRes)
	{
		__numV = __pHyperG->get_nodes();
		__numRRsets = __pHyperG->get_RRsets_size();
	}

	/// Build a set of n RR sets
	void build_n_RRsets(int64 numRRsets);
	/// Find the top-k seeds
	double top_k_seed(int targetSize);
	/// Find the top-k seeds using lazy forward technique
	double top_k_seed_heap(int targetSize);
	/// Simple greedy algorithm
	void simple_greedy(bool isPerfectPrune = false);
	/// Simple greedy algorithm using lazy forward technique
	void simple_greedy_heap(bool isPerfectPrune = false);
	/// Double greedy algorithm
	void double_greedy(bool isPerfectPrune = false, bool isDeter = true);
	/// Upper bounds on the optimum
	void upper_bound();
	/// Find a serial upper bounds via the type-1 super-gradient
	std::vector<double> all_upper_bound_B(double* pCost, std::vector<int>& vecNonOptimal, int sizeResult = -1);
	/// Find a serial upper bounds via the type-2 super-gradient
	std::vector<double> all_upper_bound_A(double* pCost, std::vector<int>& vecNonOptimal, int sizeResult = -1);
	/// IMM, refer to Tang et al. SIGMOD 2016
	double IMM(const int targetSize, const double epsilon, const int order);
	/// BCT, refer to Nguyen et al. INFOCOM 2016
	double BCT(const int targetSize, const double epsilon, const int order);
	/// Random, a baseline algorithm. It randomly selects some nodes.
	void baseline_random() const;
	/// High-degree, a baseline algorithm. It selects high-degree nodes.
	void baseline_highdegree() const;
	/// Influence maximization algorithm, a baseline algorithm. Including IMM and BCT.
	void baseline_infmax(const double epsilon, const int order, const bool isUniBen = true);
	/// Greedy algorithm, default is double greedy.
	void greedy(const int numRRsets, const int greedyType = 1, const bool isPerfectPrune = false);
};

using TAlg = Alg;
using PAlg = std::shared_ptr<TAlg>;
