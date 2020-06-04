#include <iostream>
#include <string>
#include <vector>
#include <fstream>

class InputParser {
public:
	InputParser(int& argc, char** argv) {
		for (int i = 1; i < argc; ++i)
			this->tokens.push_back(std::string(argv[i]));
	}
	/// @author iain
	const std::string& getCmdOption(const std::string& option) const {
		std::vector<std::string>::const_iterator itr;
		itr = std::find(this->tokens.begin(), this->tokens.end(), option);
		if (itr != this->tokens.end() && ++itr != this->tokens.end()) {
			return *itr;
		}
		static const std::string empty_string("");
		return empty_string;
	}
	/// @author iain
	bool cmdOptionExists(const std::string& option) const {
		return std::find(this->tokens.begin(), this->tokens.end(), option)
			!= this->tokens.end();
	}
	std::string getToken(int id) {
		return tokens.at(id);
	}
	int getTokenSize() {
		return tokens.size();
	}

private:
	std::vector<std::string> tokens;
};

inline bool ifFileExists(const std::string& name) {
	std::ifstream f(name.c_str());
	return f.good();
}

void insertString(
	std::vector < std::pair<std::string, std::vector<std::pair<std::string, int>>>>& dictionary,
	std::string str,
	std::string next
) {
	auto it = std::find_if(dictionary.begin(), dictionary.end(),
		[&](std::pair<std::string, std::vector<std::pair<std::string, int>>> arg)->bool {
			return arg.first == str;
		});

	if (it == dictionary.end()) {
		//Not found
		std::vector<std::pair<std::string, int>> v = { std::make_pair(next, 1) };
		dictionary.push_back(std::make_pair(str, v));
		return;
	}
	//Found string
	//Search if next string is found
	auto endVectorIt = (*it).second.end();
	auto it2 = std::find_if((*it).second.begin(), endVectorIt, [&](std::pair<std::string, int> arg)->bool {
		return next == arg.first;
		});
	if (it2 == endVectorIt) {
		//Not found
		(*it).second.push_back(std::make_pair(next, 1));
		return;
	}
	//Second string exists
	(*it2).second++;
}

int main(int argc, char* argv[]) {
	InputParser inputParser(argc, argv);
	if (inputParser.cmdOptionExists("-h")) {
		std::cout << "Help Options" << std::endl;
		std::cout << "Program looks for file named first argument and imports contents if file is found." << std::endl;
		std::cout << "If file is not found the arguments are used as input." << std::endl;
		std::cout << "You can use the -h flag to print this help page." << std::endl;
		std::cout << "Use the -o option to specify the JSON output of the program." << std::endl;
		//Print bunch of help options
		return 0;
	}

	std::string outFileName = inputParser.getCmdOption("-o");
	if (outFileName == "") {
		outFileName = "outFile";
	}
	std::ofstream outFile;
	outFile.open(outFileName.c_str());

	if (!outFile) {
		std::cout << "File " + outFileName + "couldn't be opened." << std::endl;
		return 0;
	}

	std::vector < std::pair<std::string, std::vector<std::pair<std::string, int>>>> dictionary;

	//Control flow according to the flag and options provided
	if (ifFileExists(inputParser.getToken(0))) {
		//File exists
		std::fstream infile;
		infile.open(inputParser.getToken(0));
		std::string word, previous;

		infile >> word;
		previous = word;
		while (infile >> word) {
			//Setup filters here
			//if (word == ".") continue;
			insertString(dictionary, previous, word);
			previous = word;
		}
	}
	else {
		//File doesn't exist
		for (int i = 0; i < inputParser.getTokenSize() - 1; i++) {
			if (inputParser.getToken(i)[0] == '-') {
				i++;
				continue;
			}
			insertString(dictionary, inputParser.getToken(i), inputParser.getToken(i + 1));
		}
	}

	//Write to JSON
	outFile << "{";
	outFile << "\n";

	for (auto v : dictionary) {
		outFile << "\"";
		outFile << v.first;
		outFile << "\"";
		outFile << ":";
		outFile << "{";
		for (auto u : v.second) {
			outFile << "\"";
			outFile << u.first;
			outFile << "\"";
			outFile << ":";
			outFile << u.second;
			outFile << ",";
		}
		outFile.seekp(-1, std::ios_base::end);
		outFile << "}";
		outFile << ",";
		outFile << "\n";
	}
	outFile.seekp(-3, std::ios_base::end);
	outFile << "\n";
	outFile << "}";
	

	//Text generation
	while (true) {
		std::cout << "Generate a sentence? ( Y or N )" << std::endl;
		std::cin >> std::ws;
		char res = std::cin.get();

		if (res == 'n' || res == 'N')
			break;

		std::cout << "Select a length" << std::endl;
		int n = 0;
		std::cin >> std::ws;
		std::cin >> n;

		int index = rand() % dictionary.size();
		std::cout << dictionary[index].first + " ";

		for (int t = 0; t < n; t++) {
			int count = 0;
			for (auto u : dictionary[index].second) {
				count += u.second;
			}
			int secondaryIndex = rand() % count;

			std::string nextWord;
			count = 0;
			for (auto u : dictionary[index].second) {
				count += u.second;
				if (count >= secondaryIndex) {
					nextWord = u.first;
					std::cout << nextWord + " ";
					break;
				}
			}
			auto it = std::find_if(dictionary.begin(), dictionary.end(), [&]
			(std::pair<std::string, std::vector<std::pair<std::string, int>>> arg)->bool
				{
					return arg.first == nextWord;
				}
			);
			if (it == dictionary.end()) {
				break;
			}
			index = std::distance(dictionary.begin(), it);
		}

	}

	return 0;
}