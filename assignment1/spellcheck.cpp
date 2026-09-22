#include "hash.h"

#include <iostream>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <ctime>

bool is_valid_char(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '\'';
}

void check_curr_word(char c, std::string& current_word, std::ofstream& output_file, hashTable& hashtable, int line_num) {

    if(is_valid_char(c)) {
        current_word += c;
    }
    
    else {
        if(!current_word.empty()) {
            if(current_word.size() >= 21) {
                output_file << "Long word at line " << line_num << ", starts: " << current_word.substr(0, 20) << "\n";
                current_word = current_word.substr(0, 20);
            }

            else if(!hashtable.contains(current_word) && !std::any_of(current_word.begin(), current_word.end(), ::isdigit)) {
                output_file << "Unknown word at line " << line_num << ": " << current_word << "\n";
            }
        }

        current_word.clear();
    }
}

int main(void) {
    std::string dictionary_filename;
    std::string input_filename;
    std::string output_filename;


    std::cout << "Input the filename of the dictionary" << std::endl;

    std::cin >> dictionary_filename;

    std::cout << "Input the filename of the file to spell check" << std::endl;

    std::cin >> input_filename;

    std::cout << "Input the filename of the file to output to" << std::endl;

    std::cin >> output_filename;

    std::ifstream dictionary_file(dictionary_filename);

    if(!dictionary_file.is_open()) {
        std::cerr << "error: could not open " << dictionary_filename << std::endl;
        return 1;
    }

    
    hashTable hashtable;

    std::clock_t dictionary_load_start = clock();

    std::string line;
    while(std::getline(dictionary_file, line)) {
        std::transform(line.begin(), line.end(), line.begin(), [](unsigned char c) {
            return std::tolower(c);
        });

        hashtable.insert(line);

    }

    std::clock_t dictionary_load_end = clock();

    double dict_load_cpu_time = static_cast<double>(dictionary_load_end - dictionary_load_start)/CLOCKS_PER_SEC;

    std::cout << "Total time (in seconds) to load dictionary: " << dict_load_cpu_time << std::endl;

    std::ifstream input_file(input_filename);

    std::ofstream output_file(output_filename);

    if(!input_file.is_open()) {
        std::cerr << "error: could not open " << input_filename << std::endl;
        return 1;
    }

    if(!output_file.is_open()) {
        std::cerr << "error: could not open " << output_filename << std::endl;
        return 1;
    }

    std::clock_t check_start = clock();

    int line_num = 1;
    
    while(std::getline(input_file, line)) {
        std::transform(line.begin(), line.end(), line.begin(), [](unsigned char c) {
            return std::tolower(c);
        });
        
        std::string current_word = "";

        
        for(char c : line) {
            check_curr_word(c, current_word, output_file, hashtable, line_num);
        }

        check_curr_word('\n', current_word, output_file, hashtable, line_num);

        line_num++;
    }

    std::clock_t check_end = clock();

    double check_cpu_time = static_cast<double>(check_end - check_start)/CLOCKS_PER_SEC;

    std::cout << "Total time (in seconds) to check document: " << check_cpu_time << std::endl;

    return 0;
}