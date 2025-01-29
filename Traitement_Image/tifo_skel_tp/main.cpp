#include <iostream>
#include "image.hh"
#include "image_io.hh"
#include "histogram.hh"
#include "read_raw.hh"

int main() {

    tifo::RawIMAGE image(3280, 2464);

    try {
        tifo::decode_raw_image("../tifo_skel_tp/image.raw", image);
        std::cout << "Image decoded successfully." << std::endl;

        double rmin, gmin, bmin;
        image.detect_black_level(rmin, gmin, bmin);
        std::cout << "Detected black level from image:" << rmin << ", G: " << gmin << ", B: " << bmin << std::endl;

        image.subtract_black_level(rmin, gmin, bmin);
        std::cout << "Subtracted black level from image." << std::endl;

        auto debayered_image = image.debayer();
        std::cout << "Image debayered successfully." << std::endl;


        debayered_image.save_to_tga("../tifo_skel_tp/output_image.tga");
        std::cout << "Image saved to output_image.tga." << std::endl;



    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }



    return 0;
}




/*int main(void )
{
    tifo::rgb24_image* im_read = tifo::load_image("../tifo_skel_tp/ray_marcher.tga");
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
    //tifo::rgb24_image imrgb = tifo::saturation_modifier(*im_read, 0.8);

    /*tifo::mask conv_mask{4, 4};
    for (int x = 0; x < conv_mask.width; x++) {
        for (int y = 0; y < conv_mask.width; y++) {
            conv_mask.set_value(x, y, 0.25);
        }
    }

    tifo::apply_convolution(*im_read, conv_mask);
    //tifo::gray8_image im_gray = tifo::rgb2gray(*im_read);
    //tifo::ameliore_nettete(*im_read);
    //tifo::rgb24_image im_rgb = tifo::gray2rgb(im_gray);

    //tifo::mask gaussian_flur = tifo::create_gaussian_blur_mask(1);
    //gaussian_flur.print_as_matrix();
    //tifo::apply_convolution(*im_read, gaussian_flur);
    tifo::create_noise(*im_read, 15);
    tifo::nlmeans_denoising(*im_read,2, 2);
    bool save_image = tifo::save_image(*im_read, "../tifo_skel_tp/rayMarcherdenoise.tga");
    if (save_image)
    {
        std::cout << "image save \n";
    }


    //tifo::histogram_1d histo = tifo::give_histogram(*im_read);
    //tifo::plot_histogram(histo);

}
*/

