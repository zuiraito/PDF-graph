#include <poppler-document.h>
#include <poppler-page.h>
#include <iostream>
#include <string>
#include <memory>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <iterator>
#include <cctype>

// Function to trim leading and trailing whitespace
std::string trim_whitespace(const std::string &str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    if (start == std::string::npos || end == std::string::npos)
        return ""; // Return empty string if no non-whitespace character found
    return str.substr(start, end - start + 1);
}

// Function to sanitize text by replacing non-alphabet characters with spaces
std::string sanitize_text(const std::string &text) {
    std::string sanitized;
    for (char c : text) {
        if (std::isalpha(static_cast<unsigned char>(c))) {
            sanitized += c; // Keep alphabetic characters
        } else {
            sanitized += ' '; // Replace non-alphabetic characters with a space
        }
    }
    return sanitized;
}

// Function to convert a string to lowercase
std::string to_lowercase(const std::string &text) {
    std::string lowercased;
    lowercased.reserve(text.size());
    for (char c : text) {
        lowercased += std::tolower(static_cast<unsigned char>(c));
    }
    return lowercased;
}

// Function to split text into words
std::vector<std::string> tokenize(const std::string &text) {
    std::istringstream stream(text);
    std::vector<std::string> words;
    std::copy(std::istream_iterator<std::string>(stream),
              std::istream_iterator<std::string>(),
              std::back_inserter(words));
    return words;
}

// Function to load ignore words from a file into a set
std::unordered_set<std::string> load_ignore_words(const std::string &file_path) {
    std::unordered_set<std::string> ignore_words;
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open ignore words file: " << file_path << std::endl;
        return ignore_words;
    }

    std::string word;
    while (std::getline(file, word)) {
        ignore_words.insert(to_lowercase(trim_whitespace(word)));
    }
    return ignore_words;
}

// Function to filter ignored words from a list of tokens
// Function to filter ignored words from a list of tokens
std::vector<std::string> filter_ignore_words(const std::vector<std::string> &tokens, const std::unordered_set<std::string> &ignore_words) {
    std::vector<std::string> filtered;
    for (const auto &word : tokens) {
        // Ignore words that are in the ignore_words set or are exactly 2 characters long
        if (ignore_words.find(word) == ignore_words.end() && word.size() != 2) {
            filtered.push_back(word);
        }
    }
    return filtered;
}

// Function to extract the title of a page
std::string extract_pdf_title(const std::string &page_text) {
    std::stringstream ss(page_text);
    std::string line;
    int line_count = 0;

    // Skip the first 3 header lines
    while (line_count < 3 && std::getline(ss, line)) {
        line_count++;
    }

    // Skip empty lines after the header
    while (std::getline(ss, line) && line.empty()) {
        // Continue until we find a non-empty line
    }

    // The next non-empty line will be the title
    if (!line.empty()) {
        return trim_whitespace(line);
    }

    return ""; // Return empty if no title found
}

// Function to extract the contents (words) of a page
std::vector<std::string> extract_pdf_contents(const std::string &page_text, const std::unordered_set<std::string> &ignore_words) {
    std::string sanitized_text = sanitize_text(page_text);
    std::string lowercased_text = to_lowercase(sanitized_text);
    auto tokens = tokenize(lowercased_text);
    return filter_ignore_words(tokens, ignore_words);
}

// Function to extract titles and contents from a PDF and populate JSON
void extract_pdf_data(const std::string &pdf_path, const std::unordered_set<std::string> &ignore_words, nlohmann::json &output_json) {
    // Open the PDF document
    std::unique_ptr<poppler::document> doc(poppler::document::load_from_file(pdf_path));
    if (!doc) {
        std::cerr << "Error: Unable to open PDF file: " << pdf_path << std::endl;
        return;
    }

    // Iterate through the pages
    int total_pages = doc->pages();
    for (int i = 0; i < total_pages; ++i) {
        std::unique_ptr<poppler::page> page(doc->create_page(i));
        if (!page) {
            std::cerr << "Error: Unable to access page " << i + 1 << std::endl;
            continue;
        }

        // Extract text from the page
        std::string page_text = page->text().to_latin1();

        // Extract title and contents
        std::string title = extract_pdf_title(page_text);
        std::vector<std::string> contents = extract_pdf_contents(page_text, ignore_words);
        // Add data to JSON
        output_json["nodes"].push_back({
            {"id", title},
            {"page", i + 1},
            {"connections", contents}
        });
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " input.pdf ignore_words output.json" << std::endl;
        return 1;
    }

    std::string pdf_path = argv[1];
    std::string ignore_words_path = argv[2];
    std::string json_path = argv[3];
    nlohmann::json output_json;

    // Load ignore words
    auto ignore_words = load_ignore_words(ignore_words_path);

    // Initialize the "nodes" array
    output_json["nodes"] = nlohmann::json::array();

    // Extract data from the PDF
    extract_pdf_data(pdf_path, ignore_words, output_json);

    // Write the result to output.json
    std::ofstream out_file(json_path);
    out_file << output_json.dump(4); // Pretty print with an indent of 4 spaces
    out_file.close();

    std::cout << "JSON output written to " << json_path << std::endl;
    return 0;
}
