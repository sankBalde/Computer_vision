//************************************************
//*                                              *
//*   TP 1&2    (c) 2017 J. FABRIZIO             *
//*                                              *
//*                               LRDE EPITA     *
//*                                              *
//************************************************

#ifndef HISTOGRAM_HH
#define	HISTOGRAM_HH

#include "image.hh"

#include <iostream>


namespace tifo {

  typedef struct { unsigned int histogram[IMAGE_NB_LEVELS]; } histogram_1d;

  histogram_1d give_histogram(rgb24_image image);
  histogram_1d give_histogram(gray8_image image);
  void plot_histogram(const histogram_1d& histogram);
  rgb24_image gray2rgb(const gray8_image& imGray);
  gray8_image rgb2gray(const rgb24_image& imRgb);

  gray8_image equalize_histogram_gray(gray8_image& image);
  rgb24_image equalize_histogram_rgb(rgb24_image& image);
  rgb24_image equalize_histogram_hsv_in_V(rgb24_image& image);
  std::vector<gray8_image> split_rgbImage(rgb24_image image);
  rgb24_image merge_Rgb(std::vector<gray8_image> rgb_vector);
  std::vector<float> color_rgb2hsv(std::vector<uint8_t> rgb);
  rgb24_image convert_hsv_rgb(hsv_image image);
  hsv_image convert_rgb_hsv(rgb24_image image);

  std::vector<uint8_t> color_hsv2rgb(std::vector<float> hsv);

  rgb24_image saturation_modifier(rgb24_image& image, float parameter);
  void apply_convolution(rgb24_image& image, const mask& m);
  void apply_convolution(gray8_image& image, const mask& m);
  void ameliore_nettete(gray8_image& image);
  void ameliore_nettete(rgb24_image& image);
  mask create_gaussian_blur_mask(float sigma);
  void nlmeans_denoising(rgb24_image& image, int search_window = 2, int patch_size = 2);
  void create_noise(rgb24_image& image, int k);

}

#endif
