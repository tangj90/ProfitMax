#pragma once

class ResultInfo
{
private:
	double __RunningTime, __Profit, __ProfitOrg, __ProfitLowerLattice, __ProfitUpperLattice, __TotalCost;
	int __SeedSize, __SimpleSearchLatticeSize, __LowerLatticeSize, __SearchLatticeSize, __EstNodeSize, __RRsetsSize;
	std::vector<int> __VecSeed, __VecLowerLattice, __VecSearchLattice;
	std::vector<double> __VecProfitBound, __VecInfluence;
public:
	ResultInfo()
	{
		reflesh();
	}

	~ResultInfo()
	{
	}

	void reflesh()
	{
		__RunningTime = -1.0;
		__Profit = -1.0;
		__ProfitOrg = -1.0;
		__ProfitLowerLattice = 0.0;
		__ProfitUpperLattice = -1.0;
		__TotalCost = -1.0;
		__SeedSize = 0;
		__SimpleSearchLatticeSize = -1;
		__LowerLatticeSize = -1;
		__SearchLatticeSize = -1;
		__EstNodeSize = 0;
		__RRsetsSize = -1;
		__VecSeed.clear();
		__VecLowerLattice.clear();
		__VecSearchLattice.clear();
		__VecProfitBound = std::vector<double>(5, -1);
		__VecInfluence = std::vector<double>(5);
	}

	double get_running_time() const
	{
		return __RunningTime;
	}

	double get_profit() const
	{
		return __Profit;
	}

	double get_profit_org() const
	{
		return __ProfitOrg;
	}

	double get_profit_lower_lattice() const
	{
		return __ProfitLowerLattice;
	}

	double get_profit_upper_lattice() const
	{
		return __ProfitUpperLattice;
	}

	double get_total_cost() const
	{
		return __TotalCost;
	}

	int get_seed_size() const
	{
		return __SeedSize;
	}

	int get_simple_search_lattice_size() const
	{
		return __SimpleSearchLatticeSize;
	}

	int get_lower_lattice_size() const
	{
		return __LowerLatticeSize;
	}

	int get_search_lattice_size() const
	{
		return __SearchLatticeSize;
	}

	int get_estimated_node_size() const
	{
		return __EstNodeSize;
	}

	int get_RRsets_size() const
	{
		return __RRsetsSize;
	}

	std::vector<int> get_seed_vec() const
	{
		return __VecSeed;
	}

	std::vector<int> get_lower_lattice_vec() const
	{
		return __VecLowerLattice;
	}

	std::vector<int> get_search_lattice_vec() const
	{
		return __VecSearchLattice;
	}

	std::vector<double> get_profit_bound_vector() const
	{
		return __VecProfitBound;
	}

	std::vector<double> get_influence_vector() const
	{
		return __VecInfluence;
	}

	void set_running_time(double value)
	{
		__RunningTime = value;
	}

	void set_profit(double value)
	{
		__Profit = value;
	}

	void set_profit_org(double value)
	{
		__ProfitOrg = value;
	}

	void set_profit_lower_lattice(double value)
	{
		__ProfitLowerLattice = value;
	}

	void set_profit_upper_lattice(double value)
	{
		__ProfitUpperLattice = value;
	}

	void set_total_cost(double value)
	{
		__TotalCost = value;
	}

	void set_seed_size(int value)
	{
		__SeedSize = value;
	}

	void set_simple_search_lattice_size(int value)
	{
		__SimpleSearchLatticeSize = value;
	}

	void set_lower_lattice_size(int value)
	{
		__LowerLatticeSize = value;
	}

	void set_search_lattice_size(int value)
	{
		__SearchLatticeSize = value;
	}

	void set_estimated_node_size(int value)
	{
		__EstNodeSize = value;
	}

	void set_RRsets_size(int value)
	{
		__RRsetsSize = value;
	}

	void inc_estimated_node_size(int value)
	{
		__EstNodeSize += value;
	}

	void set_seed_vec(std::vector<int>& vecSeed)
	{
		__VecSeed = vecSeed;
		set_seed_size((int)vecSeed.size());
	}

	void set_lower_lattice_vec(std::vector<int>& vecLowerLattice)
	{
		__VecLowerLattice = vecLowerLattice;
		set_lower_lattice_size((int)vecLowerLattice.size());
	}

	void set_search_lattice_vec(std::vector<int>& vecSearchLattice)
	{
		__VecSearchLattice = vecSearchLattice;
		set_search_lattice_size((int)vecSearchLattice.size());
	}

	void set_profit_bound_idx(double value, int idx = 0)
	{
		assert(idx < __VecProfitBound.size());
		__VecProfitBound[idx] = value;
	}

	void set_profit_bound_vector(std::vector<double>& vecProfit)
	{
		__VecProfitBound = vecProfit;
	}

	void set_influence_idx(double value, int idx = 0)
	{
		assert(idx < __VecInfluence.size());
		__VecInfluence[idx] = value;
	}

	void set_influence_vector(std::vector<double>& vecInfluence)
	{
		__VecInfluence = vecInfluence;
	}

	void set_profit_bound_double_greedy()
	{
		set_profit_bound_idx(3 * __ProfitOrg - (__ProfitLowerLattice + __ProfitUpperLattice), 4);
	}
};

using TResult = ResultInfo;
using PResult = std::shared_ptr<ResultInfo>;
