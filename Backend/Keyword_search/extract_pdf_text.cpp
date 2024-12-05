#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./extract_pdf_text <path_to_pdf>" << std::endl;
        return 1;
    }

    // Path to the PDF file
    std::string filePath = argv[1];

    // Load the PDF document
    poppler::document* doc = poppler::document::load_from_file(filePath);

    if (!doc) {
        std::cerr << "Error: Unable to open the PDF file." << std::endl;
        return 1;
    }

    int numPages = doc->pages(); // Get number of pages

    for (int i = 0; i < numPages; ++i) {
        // Extract text from each page
        poppler::page* pg = doc->create_page(i);

        if (pg) {
            poppler::ustring utext = pg->text(); // Get the text from the page
            std::string text = utext.to_latin1(); // Convert poppler::ustring to std::string
            std::cout << "Page " << i + 1 << ":" << std::endl;
            std::cout << text << std::endl;
        }

        delete pg; // Free the page
    }

    delete doc; // Free the document
    return 0;
}

