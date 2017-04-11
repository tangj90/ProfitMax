#pragma once

class HyperGraphRef
{
private:
	/// __numV: number of nodes in the graph.
	int __numV = 0;
	/// __numE: number of edges in the graph.
	int __numE = 0;
	/// __RRsetsSize: number of RR sets.
	int __numRRsets = 0;
	std::deque<int> __que;
	std::vector<bool> __vecVisitBool;
	std::vector<int> __vecVisitNode;
	const double* __pAccumWeight;
	bool __isBilateral = false;

	void init_hypergraph()
	{
		__numV = static_cast<int>(_vecGRev.size());
		for (auto& nbrs : _vecGRev) __numE += static_cast<int>(nbrs.size());
		_vecRRsets = RRsets(__numV);
		__vecVisitBool = std::vector<bool>(__numV);
		__vecVisitNode = std::vector<int>(__numV);
	}

public:
	/// _vecGRev: reverse graph
	const Graph& _vecGRev;
	/// _vecRRsets: RR sets
	RRsets _vecRRsets;
	/// _vecRRsetsRev: reverse RR sets
	RRsets _vecRRsetsRev;

	/// InfluModel: IC (independent cascade) and LT (liner threshold).
	enum CascadeModel
	{
		IC,
		LT
	};

	/// _cascadeModel: the cascade model, default is IC
	CascadeModel _cascadeModel = IC;

	explicit HyperGraphRef(const Graph& vecGRev, const double* pAccumWeight = nullptr)
		: __pAccumWeight(pAccumWeight), _vecGRev(vecGRev)
	{
		init_hypergraph();
	}

	void set_cascade_model(CascadeModel model)
	{
		_cascadeModel = model;
	}

	void set_hyper_graph_mode(bool val)
	{
		__isBilateral = val;
	}

	/// Returns the number of nodes in the graph.
	int get_nodes() const
	{
		return __numV;
	}

	/// Returns the number of edges in the graph.
	int get_edges() const
	{
		return __numE;
	}

	/// Returns the number of RR sets in the graph.
	int get_RRsets_size() const
	{
		return __numRRsets;
	}

	/// Get out degree
	std::vector<int> get_out_degree() const
	{
		std::vector<int> outDeg(__numV);
		for (auto& nbrs : _vecGRev)
		{
			for (auto& nbr : nbrs)
			{
				outDeg[nbr.first]++;
			}
		}
		return outDeg;
	}

	void build_n_RRsets(const int64 numSamples)
	{
		if (numSamples > INT_MAX)
		{
			std::cout << "Error:R too large" << std::endl;
			exit(1);
		}
		int prevSize = __numRRsets;
		__numRRsets = max(__numRRsets, (int)numSamples);
		for (auto i = prevSize; i < numSamples; i++)
		{
			build_one_RRset(gen_random_node_by_weight(__numV, __pAccumWeight), i);
		}
	}


	void build_one_RRset(const int uStart, const int hyperIdx)
	{
		//LogInfo(uStart);
		int numVisitNode = 0;
		__que.clear();
		__que.push_back(uStart);
		_vecRRsets[uStart].push_back(hyperIdx);
		assert(numVisitNode < __numV);
		__vecVisitNode[numVisitNode++] = uStart;
		__vecVisitBool[uStart] = true;
		while (!__que.empty())
		{
			int expand = __que.front();
			__que.pop_front();
			if (_cascadeModel == IC)
			{
				for (int idx = (int)_vecGRev[expand].size(); idx--;)
				{
					auto nbr = _vecGRev[expand][idx];
					int nbrId = nbr.first;
					if (__vecVisitBool[nbrId])
						continue;
					double randDouble = dsfmt_gv_genrand_open_close();
					if (randDouble > nbr.second)
						continue;
					assert(numVisitNode < __numV);
					__vecVisitNode[numVisitNode++] = nbrId;
					__vecVisitBool[nbrId] = true;
					__que.push_back(nbrId);
					assert((int)_vecHyperGRev.size() > hyperIdx);
					_vecRRsets[nbrId].push_back(hyperIdx);
				}
			}
			else if (_cascadeModel == LT)
			{
				if (_vecGRev[expand].size() == 0)
					continue;
				assert(_vecGRev[expand].size() > 0);
				int nextNbrIdx = gen_random_node_by_weight_LT(_vecGRev[expand]);
				//LogInfo(nextNbrIdx);
				if (nextNbrIdx < 0) break; // No element activated
				int nbrId = _vecGRev[expand][nextNbrIdx].first;
				if (__vecVisitBool[nbrId]) break; // Stop, no further node activated
				__vecVisitNode[numVisitNode++] = nbrId;
				__vecVisitBool[nbrId] = true;
				__que.push_back(nbrId);
				assert((int)_vecHyperGRev.size() > hyperIdx);
				_vecRRsets[nbrId].push_back(hyperIdx);
			}
		}
		for (int i = 0; i < numVisitNode; i++) __vecVisitBool[__vecVisitNode[i]] = false;
		//LogInfo(numVisitNode);
		if (__isBilateral) _vecRRsetsRev.push_back(std::vector<int>(__vecVisitNode.begin(), __vecVisitNode.begin() + numVisitNode));
	}


	double eval_inf_spread(const std::vector<int>& vecSeed)
	{
		std::vector<bool> vecBoolVst;
		std::vector<bool> vecBoolSeed(__numV);
		for (auto seed : vecSeed) vecBoolSeed[seed] = true;
		double inf;
		vecBoolVst = std::vector<bool>(__numRRsets);
		for (auto seed : vecSeed)
		{
			for (auto node : _vecRRsets[seed])
			{
				vecBoolVst[node] = true;
			}
		}
		inf = (double)count(vecBoolVst.begin(), vecBoolVst.end(), true) * __numV / __numRRsets;
		return inf;
	}


	double effic_inf_valid_algo(std::vector<int>& vecSeed, double delta = 1e-6, double eps = 0.01)
	{
		double c = 2 * (exp(1) - 2);
		double LambdaL = 1 + 2 * c * (1 + eps) * log(2 / delta) / (eps * eps);
		int numHyperEdge = 0;
		int numCoverd = 0;
		std::vector<bool> vecBoolSeed(__numV);
		for (auto seed : vecSeed) vecBoolSeed[seed] = true;

		while (numCoverd < LambdaL)
		{
			numHyperEdge++;
			int numVisitNode = 0;
			int uStart = gen_random_node_by_weight(__numV, __pAccumWeight);
			if (vecBoolSeed[uStart])
			{// Stop, this sample is covered
				numCoverd++;
				continue;
			}
			__que.clear();
			__que.push_back(uStart);
			__vecVisitNode[numVisitNode++] = uStart;
			__vecVisitBool[uStart] = true;
			while (!__que.empty())
			{
				int expand = __que.front();
				__que.pop_front();
				if (_cascadeModel == IC)
				{
					for (int idx = (int)_vecGRev[expand].size(); idx--;)
					{
						auto nbr = _vecGRev[expand][idx];
						int nbrId = nbr.first;
						if (__vecVisitBool[nbrId])
							continue;
						double randDouble = dsfmt_gv_genrand_open_close();
						if (randDouble > nbr.second)
							continue;
						if (vecBoolSeed[nbrId])
						{// Stop, this sample is covered
							numCoverd++;
							goto postProcess;
						}
						__vecVisitNode[numVisitNode++] = nbrId;
						__vecVisitBool[nbrId] = true;
						__que.push_back(nbrId);
					}
				}
				else if (_cascadeModel == LT)
				{
					if (_vecGRev[expand].size() == 0)
						continue;
					int nextNbrIdx = gen_random_node_by_weight_LT(_vecGRev[expand]);
					if (nextNbrIdx < 0) break; // No element activated
					auto nbrId = _vecGRev[expand][nextNbrIdx].first;
					if (__vecVisitBool[nbrId]) break; // Stop, no further node activated
					if (vecBoolSeed[nbrId])
					{// Stop, this sample is covered
						numCoverd++;
						goto postProcess;
					}
					__vecVisitNode[numVisitNode++] = nbrId;
					__vecVisitBool[nbrId] = true;
					__que.push_back(nbrId);
				}
			}
		postProcess:
			for (int i = 0; i < numVisitNode; i++)
				__vecVisitBool[__vecVisitNode[i]] = false;
		}
		double inf = (double)numCoverd * __numV / numHyperEdge;
		return inf;
	}


	double inf_valid_algo(std::vector<int>& vecSeed, int numSamples)
	{
		int numHyperEdge = 0;
		int numCoverd = 0;
		std::vector<bool> vecBoolSeed(__numV);
		for (auto seed : vecSeed) vecBoolSeed[seed] = true;
		while (++numHyperEdge < numSamples)
		{
			int numVisitNode = 0;
			int uStart = gen_random_node_by_weight(__numV, __pAccumWeight);
			if (vecBoolSeed[uStart])
			{// Stop, this sample is covered
				numCoverd++;
				continue;
			}
			__que.clear();
			__que.push_back(uStart);
			__vecVisitNode[numVisitNode++] = uStart;
			__vecVisitBool[uStart] = true;
			while (!__que.empty())
			{
				int expand = __que.front();
				__que.pop_front();
				if (_cascadeModel == IC)
				{
					for (int idx = (int)_vecGRev[expand].size(); idx--;)
					{
						auto nbr = _vecGRev[expand][idx];
						int nbrId = nbr.first;
						if (__vecVisitBool[nbrId])
							continue;
						double randDouble = dsfmt_gv_genrand_open_close();
						if (randDouble > nbr.second)
							continue;
						if (vecBoolSeed[nbrId])
						{// Stop, this sample is covered
							numCoverd++;
							goto postProcess;
						}
						__vecVisitNode[numVisitNode++] = nbrId;
						__vecVisitBool[nbrId] = true;
						__que.push_back(nbrId);
					}
				}
				else if (_cascadeModel == LT)
				{
					if (_vecGRev[expand].size() == 0)
						continue;
					int nextNbrIdx = gen_random_node_by_weight_LT(_vecGRev[expand]);
					if (nextNbrIdx < 0) break; // No element activated
					int nbrId = _vecGRev[expand][nextNbrIdx].first;
					if (__vecVisitBool[nbrId]) break; // Stop, no further node activated
					if (vecBoolSeed[nbrId])
					{// Stop, this sample is covered
						numCoverd++;
						goto postProcess;
					}
					__vecVisitNode[numVisitNode++] = nbrId;
					__vecVisitBool[nbrId] = true;
					__que.push_back(nbrId);
				}
			}
		postProcess:
			for (int i = 0; i < numVisitNode; i++)
				__vecVisitBool[__vecVisitNode[i]] = false;
		}
		double inf = (double)numCoverd * __numV / numHyperEdge;
		return inf;
	}


	void reflesh_hypergraph()
	{
		if (__isBilateral)
		{
			for (int i = 0; i < __numRRsets; i++)
			{
				std::vector<int>().swap(_vecRRsetsRev[i]);
			}
			std::vector<std::vector<int>>().swap(_vecRRsetsRev);
		}
		for (int i = 0; i < __numV; i++)
		{
			std::vector<int>().swap(_vecRRsets[i]);
		}
		__numRRsets = 0;
	}


	void release_memory()
	{
		reflesh_hypergraph();
		if (__isBilateral)
		{
			std::vector<std::vector<int>>().swap(_vecRRsetsRev);
		}
		std::vector<bool>().swap(__vecVisitBool);
		std::vector<int>().swap(__vecVisitNode);
		std::vector<std::vector<int>>().swap(_vecRRsets);
	}
};

using THyperGraphRef = HyperGraphRef;
using PHyperGraphRef = std::shared_ptr<THyperGraphRef>;
