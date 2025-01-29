#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cmath>
#include "image.hh"

namespace tifo {

void decode_raw_image(const std::string& file_path, RawIMAGE& image) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Could not open file.");
    }

    // Dimensions de l'image RAW
    const int raw_width = 3280;
    const int raw_height = 2464;
    const int pixel_count = raw_width * raw_height;

    // Buffer pour les données RAW
    std::vector<uint8_t> raw_data((pixel_count * 5) / 4);
    file.read(reinterpret_cast<char*>(raw_data.data()), raw_data.size());
    if (!file) {
        throw std::runtime_error("Could not read file.");
    }

    // Variables pour stocker les 4 pixels et leurs bits de poids faible
    uint8_t pixel1, pixel2, pixel3, pixel4, low_bits;

    // Index dans le buffer RAW
    size_t raw_index = 0;
    for (int y = 0; y < raw_height; ++y) {
        for (int x = 0; x < raw_width; x += 4) {
            // Lire les 4 pixels et les bits de poids faible
            pixel1 = raw_data[raw_index++];
            pixel2 = raw_data[raw_index++];
            pixel3 = raw_data[raw_index++];
            pixel4 = raw_data[raw_index++];
            low_bits = raw_data[raw_index++];

            // Reconstruction des pixels sur 10 bits
            uint16_t pixel1_10 = (pixel1 << 2) | ((low_bits >> 6) & 0x03);
            uint16_t pixel2_10 = (pixel2 << 2) | ((low_bits >> 4) & 0x03);
            uint16_t pixel3_10 = (pixel3 << 2) | ((low_bits >> 2) & 0x03);
            uint16_t pixel4_10 = (pixel4 << 2) | (low_bits & 0x03);

            // Conversion en double
            double p1 = static_cast<double>(pixel1_10) / 1023.0;
            double p2 = static_cast<double>(pixel2_10) / 1023.0;
            double p3 = static_cast<double>(pixel3_10) / 1023.0;
            double p4 = static_cast<double>(pixel4_10) / 1023.0;

            // Attribution des valeurs aux canaux RGB en fonction de la ligne et de la colonne
            if (y % 2 == 0) {
                // Ligne paire: B G B G
                image.set_pixel(x, y, 0.0, 0.0, p1); // B
                image.set_pixel(x + 1, y, 0.0, p2, 0.0); // G
                image.set_pixel(x + 2, y, 0.0, 0.0, p3); // B
                image.set_pixel(x + 3, y, 0.0, p4, 0.0); // G
            } else {
                // Ligne impaire: G R G R
                image.set_pixel(x, y, 0.0, p1, 0.0); // G
                image.set_pixel(x + 1, y, p2, 0.0, 0.0); // R
                image.set_pixel(x + 2, y, 0.0, p3, 0.0); // G
                image.set_pixel(x + 3, y, p4, 0.0, 0.0); // R
            }
        }
    }
}

}