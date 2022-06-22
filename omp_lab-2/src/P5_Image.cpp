#include <fstream>
#include "P5_Image.h"


P5_Image read_P5_image_from_file(std::string filename) {
    std::ifstream fin(filename);
    std::string tmp;
    P5_Image read_image;

    if (!fin.is_open())
        ::report_failure("Unable to open input file");
    
    fin >> tmp;
    if (tmp.compare("P5") != 0)
        ::report_failure("Invalid file format");
    
    fin >> read_image.width >> read_image.height >> read_image.max_val >> std::ws;

    if (read_image.max_val > 255)
        ::report_failure("Invalid max value");

    std::uint8_t data_byte;
    fin.unsetf(std::ios_base::skipws);
    read_image.data.reserve(read_image.width * read_image.height);

    for (int i = 0; i < read_image.width * read_image.height; i++) {
        fin >> data_byte;
        read_image.data.emplace_back(data_byte);
    }

    fin.close();
    
    return read_image;
}
