/**
* @file ProfitMax.cpp
* @brief This project is used to demonstrate the experiments for the below paper:
* Jing Tang, Xueyan Tang, Junsong Yuan, "Profit Maximization for Viral Marketing in Online Social Networks," in Proc. IEEE ICNP 2016.
*
* @author Jing Tang (Nanyang Technological University)
* 
* Copyright (C) 2017 Jing Tang and Nanyang Technological University. All rights reserved.
*
*/

#include "stdafx.h"

int main(int argc, char* argv[])
{
	// Randomize the seed for generating random numbers
	dsfmt_gv_init_gen_rand(static_cast<uint32_t>(time(nullptr)));
	TArgument Arg(argc, argv);
	std::string infilename = Arg._dir + "/" + Arg._graphname;
	if (Arg._func == 0 || Arg._func == 2)
	{// Format the graph
		GraphBase::format_graph(infilename, Arg._mode);
		if (Arg._func == 0) return 1;
	}

	std::cout << "---The Begin of " << Arg._outFileName << "---" << std::endl;
	Timer mainTimer("main");
	PResult pResult(new TResult());
	Graph graph = GraphBase::load_graph(infilename, true, Arg._probDist);
	if (Arg._model == TArgument::CascadeModel::LT)
	{// Normalize the propagation probabilities in accumulation format
		to_normal_accum_prob(graph);
	}
	int numV = (int)graph.size();
	double* pCost;
	pCost = TIO::read_cost(infilename, numV, Arg._costDist, Arg._scale, Arg._para);
	double* pAccumWeight = nullptr;
	bool isUniBen = true;
	if (tolower(Arg._benefitDist[0]) == 'n')
	{// Generate benefit weights for each node via normal distribution 
		isUniBen = false;
		/*Weighted benefit distribution*/
		std::default_random_engine generator;
		std::normal_distribution<double> distribution(3.0, 1.0);
		//std::binomial_distribution<int> distribution(100, 0.4);
		//std::poisson_distribution<int> distribution(1.0);
		//std::gamma_distribution<double> distribution(2.0, 2.0);
		//std::exponential_distribution<double> distribution(2.0);
		pAccumWeight = (double *)malloc(numV * sizeof(double));
		for (int i = 0; i < numV; i++)
		{
			pAccumWeight[i] = max(0, distribution(generator));
		}
		to_normal_accum_weight(pAccumWeight, numV);
	}
	PHyperGraphRef pHyperG(new THyperGraphRef(graph, pAccumWeight));
	pHyperG->set_cascade_model(static_cast<THyperGraphRef::CascadeModel>(Arg._model));
	pHyperG->set_hyper_graph_mode(true);
	TAlg tAlg(pCost, pHyperG, pResult);
	std::cout << "  ==>Graph loaded for RIS! total time used (sec): " + std::to_string(mainTimer.get_total_time()) << std::endl;
	pHyperG->refresh_hypergraph();
	dsfmt_gv_init_gen_rand(0);

	switch (tolower(Arg._algName[0]))
	{
	case 'r':
		// Random algorithm
		tAlg.baseline_random();
		break;
	case 'h':
		// High-degree algorithm
		tAlg.baseline_highdegree();
		break;
	case 'i':
		// Influence maximization algorithm
		tAlg.baseline_infmax(Arg._eps, 1, isUniBen);
		break;
	case 'g':
		// Simple greedy algorithm
		tAlg.greedy(Arg._numR, 0, Arg._isIterPrune);
		break;
	case 'd':
		// Double greedy algorithm
		tAlg.greedy(Arg._numR, 1, Arg._isIterPrune);
		break;
	default: ;
	}
	std::cout << ">>>Finished!\n" <<
		"  ==>Time used for finding solution (sec): " + std::to_string(pResult->get_running_time()) << '\n' <<
		"  ==>Total time used (sec): " + std::to_string(mainTimer.get_total_time()) << "\nPrint results:" << std::endl;
	TIO::write_result(Arg._outFileName, pResult, Arg._resultFolder);
	//TIO::write_order_seeds(Arg._outFileName, pResult, Arg._resultFolder);
	std::cout << "---The End of " << Arg._outFileName << "---" << std::endl;
	return 0;
}
