#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cctype>

// Struct to store word and its frequency for sorting
struct WordCount {
    std::string word;
    int count;
};

// Comparator function to sort words by frequency
bool compare(const WordCount &a, const WordCount &b) {
    return a.count > b.count;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: ./word_frequency <path_to_textfile> <output_file>" << std::endl;
        return 1;
    }

    std::string input_filename = argv[1];  // Get input file path from command-line argument
    std::string output_filename = argv[2]; // Get output file path from command-line argument
    std::ifstream file(input_filename);
    
    if (!file.is_open()) {
        std::cerr << "Error opening input file!" << std::endl;
        return 1;
    }

    std::unordered_map<std::string, int> wordFreq;
    std::string word;

    // Read the file word by word
    while (file >> word) {
        // Convert the word to lowercase to avoid case-sensitivity
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);

        // Remove punctuation from the word
        word.erase(std::remove_if(word.begin(), word.end(), ::ispunct), word.end());

        // Only process words that are at least 3 characters long
        if (word.length() >= 3) {
            wordFreq[word]++;
        }
    }

    file.close(); // Close the input file after reading

    // Open the output file for writing
    std::ofstream output_file(output_filename);
    if (!output_file.is_open()) {
        std::cerr << "Error opening output file!" << std::endl;
        return 1;
    }

    // Write words that occur at least 2 times and are at least 3 letters long to the output file
    for (const auto &entry : wordFreq) {
        if (entry.second >= 2) {
            output_file << entry.first << std::endl;
        }
    }

    output_file.close(); // Close the output file

    std::cout << "Word frequency analysis complete. Results saved to " << output_filename << std::endl;

    return 0;
}

