#pragma once


class Argument
{
public:
	int _func = 1;
	std::string _graphname = "facebook"; // Graph name. Default is "facebook".
	std::string _mode = "egr"; // three letters, 1 -> e: edge file (default), a: adjacent vector, 
	// 2 -> graph only (default, WC cascade model), w: with edge property,
	// 3 -> r: reverse (default), f: forward, b: bilateral
	std::string _dir = "graphInfo"; // Directory
	std::string _outFileName; // File name of the result
	std::string _resultFolder = "result"; // Result folder. Default is "test".
	std::string _algName = "doublegreedy"; // Algorithm. Default is double greedy.
	std::string _probDist = "WC";// Probability distribution for diffusion model. Option: WC, TR. Default is WC.
	std::string _benefitDist = "uniform";// Probability distribution for diffusion model. Option: WC, TR. Default is WC.
	std::string _costDist = "degree"; // Cost type. Default is degree-proportional.
	double _scale = 10; // Scale factor: the ratio between total cost and total benefit. Default is 10.
	double _para = 0.0; // Cost parameter: the fraction of base cost. Default is 0 (pure degree-proportional).
	double _eps = 0.5; // Error allowed for IMM/BCT. Default is 0.5.
	//int _round = 0; // Number of rounds (for random algorithms). Default is 0.
	int _numR = 1000000; // Number of RR sets to be generated. Default is 1000000.
	bool _isIterPrune = false; // Whether pruning is used. Default is false.
	enum CascadeModel
	{
		IC,
		LT
	};

	CascadeModel _model = IC; // Diffusion models: IC, LT. Default is IC.

	Argument(int argc, char* argv[])
	{
		std::string param, value;
		for (int ind = 1; ind < argc; ind++)
		{
			if (argv[ind][0] != '-') break;
			std::stringstream sstr(argv[ind]);
			getline(sstr, param, '=');
			getline(sstr, value, '=');
			if (!param.compare("-func")) _func = stoi(value);
			else if (!param.compare("-gname")) _graphname = value;
			else if (!param.compare("-mode")) _mode = value;
			else if (!param.compare("-dir")) _dir = value;
			else if (!param.compare("-outpath")) _resultFolder = value;
			else if (!param.compare("-alg")) _algName = value;
			else if (!param.compare("-pdist")) _probDist = value;
			else if (!param.compare("-bdist")) _benefitDist = value;
			else if (!param.compare("-cdist")) _costDist = value;
			else if (!param.compare("-scale")) _scale = stod(value);
			else if (!param.compare("-para")) _para = stod(value);
			else if (!param.compare("-eps")) _eps = stod(value);
			else if (!param.compare("-numR")) _numR = stoi(value);
			else if (!param.compare("-ptype")) _isIterPrune = stoi(value) != 0;
			else if (!param.compare("-model")) _model = value == "LT" ? LT : IC;
		}
		_outFileName = TIO::get_out_file_name(_graphname, _costDist, _algName, _scale, _para);
		if (_isIterPrune) _outFileName = "IP_" + _outFileName;
		if (_model == LT) _outFileName = "LT_" + _outFileName;
	}

	std::string get_outfilename_with_alg(const std::string& algName) const
	{
		return TIO::get_out_file_name(_graphname, _costDist, algName, _scale, _para);
	}
};

using TArgument = Argument;
using PArgument = std::shared_ptr<TArgument>;
