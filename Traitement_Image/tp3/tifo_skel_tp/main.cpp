#include <iostream>
#include "image.hh"
#include "image_io.hh"
#include "histogram.hh"



int main(void )
{
    tifo::rgb24_image* im_read = tifo::load_image("../tp1/20140326_124555.tga");
    /* Equalise histogram for gray images
    tifo::gray8_image imGray = tifo::rgb2gray(*im_read);
    tifo::gray8_image refectiGray =  tifo::equalize_histogram_gray(imGray);
    tifo::rgb24_image imrgb = tifo::gray2rgb(refectiGray);
     bool save_image = tifo::save_image(imrgb, "../tp1/gray_20140326_124555.tga");

    // Equalise histogram for rgb images
    //tifo::rgb24_image imrgb = tifo::equalize_histogram_rgb(*im_read);
    */
    // Equalize histogram for hsv Images on V
    //tifo::rgb24_image imrgb = tifo::equalize_histogram_hsv_in_V(*im_read);
    tifo::rgb24_image imrgb = tifo::saturation_modifier(*im_read, 0.8);
    bool save_image = tifo::save_image(imrgb, "../tp1/saturation_mod_20140326_124555.tga");
    if (save_image)
    {
        std::cout << "image save \n";
    }


    //tifo::histogram_1d histo = tifo::give_histogram(*im_read);
    //tifo::plot_histogram(histo);

}


