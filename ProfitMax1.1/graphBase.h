#pragma once

class GraphBase
{
public:
	static void check_graph(std::string filename)
	{
		Graph vecG;
		TIO::load_file(filename, vecG);
		int nodeId = 0;
		for (auto& nbrs : vecG)
		{
			std::cout << nodeId++ << ": ";
			for (auto nbr : nbrs)
			{
				std::cout << "(" << nbr.first << ", " << nbr.second << ") ";
			}
			std::cout << std::endl;
		}
	}

	/// Format the input for future computing, which is much faster for loading. Vector serialization is used.
	static void format_graph(const std::string filename, const std::string mode)
	{
		char m0 = tolower(mode[0]), m1 = tolower(mode[1]), m2 = tolower(mode[2]);
		bool cond = (m0 == 'e' || m0 == 'a') && (m1 == 'w' || m1 == 'g') && (m2 == 'r' || m2 == 'f' || m2 == 'b');
		if (!cond)
		{
			LogInfo("The input mode is not supported:", mode);
			LogInfo("The supported mode include 3 letters");
			LogInfo("1 -> e: edge file (default), a: adjacent vector (not support yet)");
			LogInfo("2 -> g: graph only (default, WC cascade model), w: with edge property");
			LogInfo("3 -> r: reverse (default), f: forward, b: bilateral");
			LogInfo("e.g., -mode=ewr is acceptable");
		}
		if (m0 == 'e')
		{
			int numV, numE;
			int srcId, dstId;
			float weight = 0.0;
			std::ifstream infile(filename);
			if (!infile.is_open())
			{
				std::cout << "The file \"" + filename + "\" can NOT be opened\n";
				return;
			}
			infile >> numV >> numE;

			// FILE* fin;
			// errno_t err = fopen_s(&fin, filename.c_str(), "r");
			// if (err != 0)
			// {
			//	 std::cout << "The file" + filename + " can NOT be opened\n";
			//	 return;
			// }
			// fscanf_s(fin, "%d%d", &numV, &numE);
			Graph vecG(numV), vecGRev(numV);
			std::vector<int> vecOutDeg(numV), vecInDeg(numV);

			for (int i = numE; i--;)
			{
				if (m1 == 'w')
				{
					infile >> srcId >> dstId >> weight;
					// fscanf_s(fin, "%d%d%f", &srcId, &dstId, &weight);
				}
				else
				{
					infile >> srcId >> dstId;
					// fscanf_s(fin, "%d%d", &srcId, &dstId);
				}
				vecOutDeg[srcId]++;
				vecInDeg[dstId]++;

				if (m2 == 'r' || m2 == 'b')
				{
					vecGRev[dstId].push_back(Edge(srcId, weight));
				}
				if (m2 == 'f' || m2 == 'b')
				{
					vecG[srcId].push_back(Edge(dstId, weight));
				}
			}
			infile.close();
			if (m2 == 'r' || m2 == 'b')
			{
				if (m1 == 'g')
				{
					for (auto& nbrsRev : vecGRev)
					{
						auto inDeg = nbrsRev.size();
						for (auto& nbrRev : nbrsRev)
						{
							nbrRev.second = (float)1.0 / inDeg;
						}
					}
				}
				TIO::save_graph_struct(filename, vecGRev, true);
			}
			if (m2 == 'f' || m2 == 'b')
			{
				if (m1 == 'g')
				{
					for (auto& nbrs : vecG)
					{
						for (auto& nbr : nbrs)
						{
							nbr.second = (float)1.0 / vecInDeg[nbr.first];
						}
					}
				}
				TIO::save_graph_struct(filename, vecG, false);
			}
			std::ofstream outFileCost(filename + ".degree.cost");
			for (auto deg : vecOutDeg)
			{
				outFileCost << std::fixed << 1.0 * deg * numV / numE << '\n';
			}
			outFileCost.close();
		}
		// TODO: adjacent file is not supported currently
		else
		{
			LogInfo("Adjacent format is not supported currently, please use edgelist format.");
			LogInfo("Press [Enter] Key To Exit...");
			getchar();
			exit(-1);
		}
		std::cout << "The graph is formatted!" << std::endl;
	}

	/// Load graph via vector deserialization.
	static Graph load_graph(const std::string graphName, const bool isReverse, const std::string probDist = "load")
	{
		Graph graph;
		TIO::load_graph_struct(graphName, graph, isReverse);

		// Trivalency cascade
		if (tolower(probDist[0]) == 't')
		{// TODO: be careful when original graph and reversed graph generate probability distribution simultaneously for TR model
			dsfmt_gv_init_gen_rand(0);
			float pProb[3] = {float(0.001), float(0.01), float(0.1)};
			for (auto& nbrs : graph)
			{
				for (auto& nbr : nbrs)
				{
					nbr.second = pProb[dsfmt_gv_genrand_uint32_range(3)];
				}
			}
			dsfmt_gv_init_gen_rand(static_cast<uint32_t>(time(nullptr)));
		}
		return graph;
	}
};
