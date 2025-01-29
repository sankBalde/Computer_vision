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
  // Constructor: creates a mask with the given width and height.
  mask(int width, int height) : width(width), height(height) {
   values.resize(width * height);
  }

  // Gets the value at a specific position in the mask.
  float get_value(int x, int y) const {
   return values[y * width + x];
  }

  // Sets the value at a specific position in the mask.
  void set_value(int x, int y, float value) {
   values[y * width + x] = value;
  }

 public:
  int width, height; // Dimensions of the mask.
  std::vector<float> values; // Values of the mask.

 };

}

}
#endif	/* IMAGE_HH */
