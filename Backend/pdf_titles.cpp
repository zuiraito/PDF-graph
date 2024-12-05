#include <poppler-document.h>
#include <poppler-page.h>
#include <iostream>
#include <string>
#include <memory>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>
#include <algorithm>
#include <iterator>

// Function to trim leading and trailing whitespace
std::string trim_whitespace(const std::string &str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    if (start == std::string::npos || end == std::string::npos)
        return ""; // Return empty string if no non-whitespace character found
    return str.substr(start, end - start + 1);
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

void extract_pdf_titles(const std::string &pdf_path, nlohmann::json &output_json) {
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

        // Tokenize the entire page text into words
        std::vector<std::string> words = tokenize(page_text);

        // Use stringstream to split text into lines
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
            // Trim any leading or trailing spaces
            std::string title = trim_whitespace(line);

            // Add title, page number, and contents to the output JSON
            output_json["nodes"].push_back({
                {"id", title},
                {"page", i + 1},
                {"contents", words}
            });
        } else {
            std::cout << "Page " << i + 1 << ": No title found" << std::endl;

            // Add only contents if no title is found
            output_json["nodes"].push_back({
                {"id", ""},
                {"page", i + 1},
                {"contents", words}
            });
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " path/to/pdf" << std::endl;
        return 1;
    }

    std::string pdf_path = argv[1];
    nlohmann::json output_json;

    // Initialize the "nodes" array
    output_json["nodes"] = nlohmann::json::array();

    extract_pdf_titles(pdf_path, output_json);

    // Write the result to output.json
    std::ofstream out_file("output.json");
    out_file << output_json.dump(4); // Pretty print with an indent of 4 spaces
    out_file.close();

    std::cout << "JSON output written to output.json" << std::endl;
    return 0;
}

