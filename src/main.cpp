#include <iostream>
#include <fstream>
#include <string>
#include "encryption.h"

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <mode> <input_file> <output_file> <password>" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    std::string input_file = argv[2];
    std::string output_file = argv[3];
    std::string password = argv[4];

    if (mode != "encrypt" && mode != "decrypt") {
        std::cerr << "Mode must be either 'encrypt' or 'decrypt'" << std::endl;
        return 1;
    }

    std::ifstream input(input_file, std::ios::binary);
    if (!input) {
        std::cerr << "Error opening input file" << std::endl;
        return 1;
    }

    std::ofstream output(output_file, std::ios::binary);
    if (!output) {
        std::cerr << "Error opening output file" << std::endl;
        return 1;
    }

    std::string content((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    input.close();

    std::string result;
    if (mode == "encrypt") {
        result = encrypt(content, password);
    } else {
        result = decrypt(content, password);
    }

    output.write(result.c_str(), result.length());
    output.close();

    std::cout << "Operation completed successfully." << std::endl;
    return 0;
}