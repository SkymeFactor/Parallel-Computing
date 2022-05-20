#pragma once
#include <vector>
#include <cstdint>
#include "utilities.h"


typedef struct {
    unsigned int width, height, max_val;
    std::vector<std::uint8_t> data;
} P5_Image;

typedef std::vector<std::uint32_t> Image_Hist;

constexpr static auto& save_histogram = write_vector_to_file<std::uint32_t>;
P5_Image read_P5_image_from_file(std::string filename);