//************************************************
//*                                              *
//*   TP 1&2    (c) 2017 J. FABRIZIO             *
//*                                              *
//*                               LRDE EPITA     *
//*                                              *
//************************************************


#ifndef IMAGE_HH
#define	IMAGE_HH

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <fstream>

#define IMAGE_NB_LEVELS 256
#define IMAGE_MAX_LEVEL 255
#define TL_IMAGE_ALIGNMENT 64

namespace tifo {

typedef uint8_t* __restrict__ __attribute__((aligned(TL_IMAGE_ALIGNMENT))) GRAY8;
typedef uint8_t* __restrict__ __attribute__((aligned(TL_IMAGE_ALIGNMENT))) RGB8;
typedef float* __restrict__ __attribute__((aligned(TL_IMAGE_ALIGNMENT))) HSV;

/**
 * Gray scale image with pixels on 8 bits.
 * @author J. Fabrizio
*/
class gray8_image {

  public:
            /**
             * Image creation and allocation.
             * @param sx width of the image in pixel
             * @param sy height of the image in pixel
            */
            gray8_image(int sx, int sy);
            ~gray8_image();

            /**
             * Gives the pixel buffer aligned according to TL_IMAGE_ALIGNMENT
             * macro.
             * @return the pixel buffer.
             */
             const GRAY8& get_buffer() const;

            /**
             * Gives the pixel buffer aligned according to TL_IMAGE_ALIGNMENT
             * macro.
             * @return the pixel buffer.
             */
            GRAY8& get_buffer();

  public:
            /**Width of the image in pixels.*/
            int sx;
            /**Height of the image in pixels.*/
            int sy;
            /**Size of the reserved area in bytes.*/
            int length;
            /**Buffer*/
            GRAY8 pixels;
    };



/**
 * Color image with pixels on 3*8 bits.
 * @author J. Fabrizio
*/
class rgb24_image {

        public:
            /**
             * Image creation and allocation.
             * @param sx width of the image in pixel
             * @param sy height of the image in pixel
            */
            rgb24_image(int sx, int sy);
            ~rgb24_image();

            /**
             * Gives the pixel buffer aligned according to TL_IMAGE_ALIGNMENT
             * macro.
             * @return the pixel buffer.
             */
            const RGB8& get_buffer() const;

            /**
             * Gives the pixel buffer aligned according to TL_IMAGE_ALIGNMENT
             * macro.
             * @return the pixel buffer.
             */
            RGB8& get_buffer();

    public:
            /**Width of the image in pixels.*/
            int sx;
            /**Height of the image in pixels.*/
            int sy;
            /**Size of the reserved area in bytes.*/
            int length;
            /**Buffer*/
            RGB8 pixels;
};

    class hsv_image {

    public:
        /**
         * Image creation and allocation.
         * @param sx width of the image in pixel
         * @param sy height of the image in pixel
        */
        hsv_image(int sx, int sy);
        ~hsv_image();


    public:
        /**Width of the image in pixels.*/
        int sx;
        /**Height of the image in pixels.*/
        int sy;
        /**Size of the reserved area in bytes.*/
        int length;
        /**Buffer*/
        HSV pixels;
    };

 class mask {
 public:
  mask(int width, int height) : width(width), height(height) {
   values.resize(width * height);
  }


  float get_value(int x, int y) const {
   return values[y * width + x];
  }


  void set_value(int x, int y, float value) {
   values[y * width + x] = value;
  }

  void print_as_matrix() const {
   for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
     std::cout << get_value(x, y) << "\t";
    }
    std::cout << "\n";

   }
  }

 public:
  int width, height;
  std::vector<float> values;

 };

 class DebayeredImage {
 public:
  DebayeredImage(int width, int height) : width(width), height(height), data(width * height * 3, 0.0) {}

  void set_pixel(int x, int y, double r, double g, double b) {
   int index = (y * width + x) * 3;
   data[index] = r;
   data[index + 1] = g;
   data[index + 2] = b;
  }

  std::tuple<double, double, double> get_pixel(int x, int y) const {
   int index = (y * width + x) * 3;
   return std::make_tuple(data[index], data[index + 1], data[index + 2]);
  }
  double linear_to_srgb(double value) const {
   if (value <= 0.0031308)
    return 12.92 * value;
   else
    return 1.055 * std::pow(value, 1.0 / 2.4) - 0.055;
  }

  void save_to_tga(const std::string& filename) const {
   std::ofstream out(filename, std::ios::binary);
   if (!out) {
    throw std::runtime_error("Unable to open file for writing");
   }


   uint8_t header[18] = {};
   header[2] = 2;
   header[12] = width & 0xFF;
   header[13] = (width >> 8) & 0xFF;
   header[14] = height & 0xFF;
   header[15] = (height >> 8) & 0xFF;
   header[16] = 24; // 24 bits per pixel

   out.write(reinterpret_cast<char*>(header), 18);

   for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
     auto [r, g, b] = get_pixel(x, y);
     uint8_t sr = linear_to_srgb(r) * 255;
     uint8_t sg = linear_to_srgb(g) * 255;
     uint8_t sb = linear_to_srgb(b) * 255;
     out.put(sb); // TGA format stores BGR
     out.put(sg);
     out.put(sr);
    }
   }

   out.close();
  }

  int get_width() const { return width; }
  int get_height() const { return height; }

 private:
  int width, height;
  std::vector<double> data;
 };

 class RawIMAGE {
 public:
  RawIMAGE(int sx, int sy) : sx(sx), sy(sy), pixels(sx * sy * 3) {}

  void set_pixel(int x, int y, double r, double g, double b) {
   int index = (y * sx + x) * 3;
   pixels[index] = r;
   pixels[index + 1] = g;
   pixels[index + 2] = b;
  }

  std::tuple<double, double, double> get_pixel(int x, int y) const {
   int index = (y * sx + x) * 3;
   return std::make_tuple(pixels[index], pixels[index + 1], pixels[index + 2]);
  }

  int get_width() const { return sx; }
  int get_height() const { return sy; }

  void detect_black_level(double& rmin, double& gmin, double& bmin) const {
   rmin = std::numeric_limits<double>::max();
   gmin = std::numeric_limits<double>::max();
   bmin = std::numeric_limits<double>::max();

   for (int y = 0; y < sy; ++y) {
    for (int x = 0; x < sx; ++x) {
     auto [r, g, b] = get_pixel(x, y);
     if (r < rmin) rmin = r;
     if (g < gmin) gmin = g;
     if (b < bmin) bmin = b;
    }
   }
  }

  void subtract_black_level(double rmin, double gmin, double bmin) {
   for (int y = 0; y < sy; ++y) {
    for (int x = 0; x < sx; ++x) {
     auto [r, g, b] = get_pixel(x, y);
     set_pixel(x, y, r - rmin, g - gmin, b - bmin);
    }
   }
  }

  DebayeredImage debayer() const {
        DebayeredImage debayered(sx, sy);

        for (int y = 0; y < sy; ++y) {
            for (int x = 0; x < sx; ++x) {
                auto [r, g, b] = get_pixel(x, y);

                if ((y % 2 == 0) && (x % 2 == 0)) {

                    double r_interp = (get_valid_pixel(x - 1, y) + get_valid_pixel(x + 1, y)) / 2.0;
                    double g_interp = (get_valid_pixel(x, y - 1) + get_valid_pixel(x, y + 1)) / 2.0;
                    debayered.set_pixel(x, y, r_interp, g_interp, b);
                } else if ((y % 2 == 0) && (x % 2 == 1)) {

                    double r_interp = (get_valid_pixel(x - 1, y) + get_valid_pixel(x + 1, y)) / 2.0;
                    double b_interp = (get_valid_pixel(x, y - 1) + get_valid_pixel(x, y + 1)) / 2.0;
                    debayered.set_pixel(x, y, r_interp, g, b_interp);
                } else if ((y % 2 == 1) && (x % 2 == 0)) {

                    double r_interp = (get_valid_pixel(x, y - 1) + get_valid_pixel(x, y + 1)) / 2.0;
                    double b_interp = (get_valid_pixel(x - 1, y) + get_valid_pixel(x + 1, y)) / 2.0;
                    debayered.set_pixel(x, y, r_interp, g, b_interp);
                } else {

                    double g_interp = (get_valid_pixel(x - 1, y) + get_valid_pixel(x + 1, y)) / 2.0;
                    double b_interp = (get_valid_pixel(x, y - 1) + get_valid_pixel(x, y + 1)) / 2.0;
                    debayered.set_pixel(x, y, r, g_interp, b_interp);
                }
            }
        }

        return debayered;
    }


    double get_valid_pixel(int x, int y) const {
        if (x < 0 || x >= sx || y < 0 || y >= sy) {
            return 0.0;
        }
        auto [r, g, b] = get_pixel(x, y);
        return (x % 2 == 0) ? (y % 2 == 0 ? b : g) : (y % 2 == 0 ? g : r);
    }

 private:
  int sx, sy;
  std::vector<double> pixels;
 };

}


#endif	/* IMAGE_HH */
