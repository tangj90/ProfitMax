#pragma once

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#endif

class IOcontroller
{
public:
	static void mkdir_absence(const char* outFolder)
	{
#if defined(_WIN32)
		CreateDirectoryA(outFolder, nullptr); // can be used on Windows
#else
		mkdir(outFolder, 0733); // can be used on non-Windows
#endif
	}

	/// Save a serialized file
	template <class T>
	static void save_file(std::string filename, const T& output)
	{
		std::ofstream outfile(filename, std::ios::binary);
		if (!outfile.eof() && !outfile.fail())
		{
			StreamType res;
			serialize(output, res);
			outfile.write(reinterpret_cast<char*>(&res[0]), res.size());
			outfile.close();
			res.clear();
			std::cout << "Save file successfully: " << filename << '\n';
		}
		else
		{
			std::cout << "Save file failed: " + filename << '\n';
			exit(1);
		}
	}

	/// Load a serialized file
	template <class T>
	static void load_file(std::string filename, T& input)
	{
		std::ifstream infile(filename, std::ios::binary);
		if (!infile.eof() && !infile.fail())
		{
			infile.seekg(0, std::ios_base::end);
			std::streampos fileSize = infile.tellg();
			infile.seekg(0, std::ios_base::beg);
			std::vector<uint8_t> res(fileSize);
			infile.read(reinterpret_cast<char*>(&res[0]), fileSize);
			infile.close();
			input.clear();
			auto it = res.cbegin();
			input = deserialize<T>(it, res.cend());
			res.clear();
		}
		else
		{
			std::cout << "Cannot open file: " + filename << '\n';
			exit(1);
		}
	}

	/// Save graph structure to a file
	static void save_graph_struct(std::string graphName, const Graph& vecGraph, const bool isReverse)
	{
		std::string postfix = ".vec.graph";
		if (isReverse) postfix = ".vec.rvs.graph";
		std::string filename = graphName + postfix;
		save_file(filename, vecGraph);
	}

	/// Load graph structure from a file
	static void load_graph_struct(std::string graphName, Graph& vecGraph, const bool isReverse)
	{
		std::string postfix = ".vec.graph";
		if (isReverse) postfix = ".vec.rvs.graph";
		std::string filename = graphName + postfix;
		load_file(filename, vecGraph);
	}

	/// Load cost of each node from a file
	static double* read_cost(std::string graphName, int numV, std::string costType, double scale, double base)
	{
		std::string fullName = graphName + "." + costType + ".cost";
		double *cost, val;
		cost = (double *)malloc(sizeof(double) * numV);
		memset(cost, 0, sizeof(double) * numV);
		std::ifstream costfile(fullName);
		base = scale * base;
		scale -= base;
		if (costfile.is_open())
		{
			for (int i = 0; i < numV; i++)
			{
				costfile >> val;
				cost[i] = scale * val + base;
			}
			costfile.close();
		}
		else
		{
			std::cout << "Cannot open file: " + fullName << '\n';
			exit(0);
		}
		return cost;
	}

	/// Get out-file name
	static std::string get_out_file_name(std::string graphName, std::string costType, std::string algName, double scale, double para)
	{
		std::ostringstream strs1, strs2;
		strs1 << scale;
		std::string strScale = strs1.str();
		strs2 << para;
		std::string strPara = strs2.str();
		return graphName + "_" + algName + "_" + costType + "_s" + strScale + "b" + strPara;
	}

	/// Print the results
	static void write_result(std::string outFileName, PResult resultObj, std::string outFolder)
	{
		double runTime = resultObj->get_running_time();
		double profit = resultObj->get_profit();
		double profitLowerLattice = resultObj->get_profit_lower_lattice(), profitUpperLattice = resultObj->get_profit_upper_lattice();
		std::vector<double> vecBound(resultObj->get_profit_bound_vector());
		std::vector<double> vecInf(resultObj->get_influence_vector());
		double totalCost = resultObj->get_total_cost();
		int seedSize = resultObj->get_seed_size(), lowerLatticeSize = resultObj->get_lower_lattice_size();
		int searchLatticeSize = resultObj->get_search_lattice_size();
		int RRsetsSize = resultObj->get_RRsets_size();
		auto upperLatticeSize = lowerLatticeSize + searchLatticeSize;
		upperLatticeSize = -1 > upperLatticeSize ? -1 : upperLatticeSize;
		std::cout << "   --------------------" << std::endl;
		std::cout << "  |Time (sec): " << runTime << std::endl;
		std::cout << "  |Profit: " << profit << std::endl;
		std::cout << "  |Upper bound mu(S_g) via A*: " << vecBound[3] << ", mu(A*): " << vecBound[0] << std::endl;
		std::cout << "  |Upper bound mu(S_g) via B*: " << vecBound[2] << ", mu(B*): " << vecBound[1] << std::endl;
		std::cout << "  |Upper bound mu_1 via USM: " << vecBound[4] << std::endl;
		std::cout << "  |Cost: " << totalCost << std::endl;
		std::cout << "  |#Seeds: " << seedSize << std::endl;
		std::cout << "  ||A*|: " << lowerLatticeSize << ", |B*|: " << upperLatticeSize << ", |B*\\A*|: " << searchLatticeSize << std::endl;
		std::cout << "  |profit(A*)+profit(B*): " << profitLowerLattice + profitUpperLattice << std::endl;
		std::cout << "  |#RR sets: " << RRsetsSize << std::endl;
		std::cout << "   --------------------" << std::endl;
		mkdir_absence(outFolder.c_str());
		std::ofstream outFileNew(outFolder + "/" + "performance_" + outFileName);
		if (outFileNew.is_open())
		{
			outFileNew << "time (sec): " << runTime << '\n';
			outFileNew << "profit: " << profit << '\n';
			outFileNew << "upper bound mu(S_g) via A*: " << vecBound[3] << '\n';
			outFileNew << "upper bound mu(S_g) via B*: " << vecBound[2] << '\n';
			outFileNew << "Upper bound mu_1 via USM: " << vecBound[4] << '\n';
			outFileNew << "upper bound mu(A*): " << vecBound[0] << '\n';
			outFileNew << "upper bound mu(B*): " << vecBound[1] << '\n';
			outFileNew << "total cost: " << totalCost << '\n';
			outFileNew << "seed size: " << seedSize << '\n';
			outFileNew << "|A*|: " << lowerLatticeSize << '\n';
			outFileNew << "|B*|: " << upperLatticeSize << '\n';
			outFileNew << "|B*\\A*|: " << searchLatticeSize << '\n';
			outFileNew << "profit(A*)+profit(B*): " << profitLowerLattice + profitUpperLattice << '\n';
			outFileNew << "#RR sets: " << RRsetsSize << '\n';
			outFileNew.close();
		}
	}

	/// Print the seeds
	static void write_order_seeds(std::string outFileName, PResult resultObj, std::string outFolder)
	{
		mkdir_absence(outFolder.c_str());
		auto vecSeed = resultObj->get_seed_vec();
		std::ofstream outFile(outFolder + "/seed_" + outFileName);
		for (auto i = 0; i < vecSeed.size(); i++)
		{
			outFile << vecSeed[i] << '\n';
		}
		outFile.close();
	}
};

using TIO = IOcontroller;
using PIO = std::shared_ptr<IOcontroller>;
