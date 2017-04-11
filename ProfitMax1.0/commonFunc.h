#pragma once

/// Log information
template <typename _Ty>
static inline void LogInfo(_Ty val)
{
	std::cout << val << std::endl;
}

template <typename _Ty>
static inline void LogInfo(std::string title, _Ty val)
{
	std::cout << title << ": " << val << std::endl;
}

/// Math
static inline double pow2(double t)
{
	return t * t;
}

static inline double log2(int n)
{
	return log(n) / log(2);
}

static inline double logcnk(int n, int k)
{
	k = min(k, n - k);
	double res = 0;
	for (int i = 1; i <= k; i++) res += log(double(n - k + i) / i);
	return res;
}

static inline int gen_random_node_by_weight(const int numV, const double* pAccumWeight = nullptr)
{
	if (pAccumWeight == nullptr) return dsfmt_gv_genrand_uint32_range(numV);
	double weight = dsfmt_gv_genrand_open_close();
	int minIdx = 0, maxIdx = numV - 1, meanIdx;
	if (weight < pAccumWeight[0]) return 0;
	while (maxIdx > minIdx)
	{
		meanIdx = (minIdx + maxIdx) / 2;
		auto meanWeight = pAccumWeight[meanIdx];
		if (weight <= meanWeight) maxIdx = meanIdx;
		else minIdx = meanIdx + 1;
	}
	return maxIdx;
}

///For LT method
static inline int gen_random_node_by_weight_LT(const Edgelist& edges)
{
	double weight = dsfmt_gv_genrand_open_close();
	int minIdx = 0, maxIdx = (int)edges.size() - 1, meanIdx;
	if (weight < edges.front().second) return 0; // First element
	if (weight > edges.back().second) return -1; // No element
	while (maxIdx > minIdx)
	{
		meanIdx = (minIdx + maxIdx) / 2;
		auto meanWeight = edges[meanIdx].second;
		if (weight <= meanWeight) maxIdx = meanIdx;
		else minIdx = meanIdx + 1;
	}
	return maxIdx;
}

static inline void to_normal_accum_prob(Graph& vecGraph)
{
	for (auto& nbrs : vecGraph)
	{
		float accumVal = float(0.0);
		for (auto& nbr : nbrs)
		{
			accumVal += nbr.second;
			nbr.second = accumVal;
		}
		// Normalization
		for (auto& nbr : nbrs)
		{
			nbr.second /= accumVal;
		}
	}
}

static inline double to_normal_accum_weight(double* pWeight, int numV)
{
	double accumVal = 0;
	for (int i = 0; i < numV; i++)
	{
		accumVal += pWeight[i];
		pWeight[i] = accumVal;
	}
	// Normalization
	for (int i = 0; i < numV; i++)
	{
		pWeight[i] /= accumVal;
	}
	return accumVal;
}
