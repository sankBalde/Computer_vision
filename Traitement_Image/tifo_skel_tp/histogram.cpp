//************************************************
//*                                              *
//*   TP 1&2    (c) 2017 J. FABRIZIO             *
//*                                              *
//*                               LRDE EPITA     *
//*                                              *
//************************************************

#include "histogram.hh"
#include <iostream>
#include <cmath>
#include <vector>
#include <random>

namespace tifo {
    rgb24_image gray2rgb(const gray8_image& imGray)
    {
        rgb24_image imRgb{imGray.sx, imGray.sy};
        int j = 0;
        for (int i = 0; i < imGray.sy * imGray.sx; i++)
        {
            imRgb.pixels[j] = imGray.pixels[i];
            imRgb.pixels[j+1] = imGray.pixels[i];
            imRgb.pixels[j+2] = imGray.pixels[i];
            j +=3;

        }
        return imRgb;
    }
    gray8_image rgb2gray(const rgb24_image& imRgb)
    {
        gray8_image imGray{imRgb.sx, imRgb.sy};
        int j = 0;
        for (int i = 0; i < imRgb.sx * imRgb.sy; i +=1)
        {
            imGray.pixels[i] = static_cast<int>((imRgb.pixels[j] + imRgb.pixels[j + 1] + imRgb.pixels[j + 2]) / 3);
            j += 3;
        }
        return imGray;
    }
    float max_tuple(float a, float b, float c)
    {
        float max = a;
        if (max < b)
            max = b;
        if (max < c)
            max = c;
        return  max;
    }
    float min_tuple(float a, float b, float c)
    {
        float min = a;
        if (min > b)
            min = b;
        if (min > c)
            min = c;
        return  min;
    }

    std::vector<float> color_rgb2hsv(std::vector<uint8_t> rgb)
    {
        float H, S, V;

        float R = rgb[0] / 255.0;


        float G = rgb[1] / 255.0;
        float B = rgb[2] / 255.0;

        float cmax = max_tuple(R, G, B);
        float cmin = min_tuple(R, G, B);
        float delta = cmax - cmin;
        // composante H
        if (delta == 0)
            H = 0; // H
        else
        {
            if (cmax == R)
                H = 60 * ((static_cast<int>((G - B)/ delta)) % 6);
            if (cmax == G)
                H = 60 * (((B - R)/ delta) + 2);
            if (cmax == B)
                H = 60 * (((R - G)/ delta) + 4);
        }
        // composante S
        if (cmax == 0)
            S = 0; // S
        else
            S = delta / cmax;
        // composante V
        V = cmax;

        std::vector<float> hsv{H, S, V};
        return hsv;
    }
    histogram_1d give_histogram(rgb24_image image)
    {
        histogram_1d histogram = give_histogram(rgb2gray(image));
        return histogram;
    }

    histogram_1d give_histogram(gray8_image image)
    {
        histogram_1d histogram;
        std::fill(std::begin(histogram.histogram), std::end(histogram.histogram), 0);

        const GRAY8& pixels = image.get_buffer();
        const int total_pixels = image.sx * image.sy;

        for (int i = 0; i < total_pixels; ++i) {
            histogram.histogram[pixels[i]]++;
        }

        return histogram;
    }

    std::vector<gray8_image> split_rgbImage(rgb24_image image)
    {
        gray8_image red {image.sx, image.sy};
        gray8_image green {image.sx, image.sy};
        gray8_image bleu {image.sx, image.sy};
        int j = 0;
        for (int i = 0; i < image.sx * image.sy; i++){
            red.pixels[i] = image.pixels[j];
            green.pixels[i] = image.pixels[j + 1];
            bleu.pixels[i] = image.pixels[j + 2];
            j += 3;
        }
        std::vector<gray8_image> rgb_vector{red, green, bleu};
        return rgb_vector;
    }

    std::vector<hsv_image> split_hsvImage(hsv_image image)
    {
        hsv_image H {image.sx, image.sy};
        hsv_image S {image.sx, image.sy};
        hsv_image V {image.sx, image.sy};
        int j = 0;
        for (int i = 0; i < image.sx * image.sy; i++){
            H.pixels[i] = image.pixels[j];
            S.pixels[i] = image.pixels[j + 1];
            V.pixels[i] = image.pixels[j + 2];
            j += 3;
        }
        std::vector<hsv_image> hsv_vector{H, S, V};
        return hsv_vector;
    }



    rgb24_image merge_Rgb(std::vector<gray8_image> rgb_vector)
    {
        rgb24_image imRgb{rgb_vector[0].sx, rgb_vector[0].sy};

        gray8_image red = rgb_vector[0];
        gray8_image green = rgb_vector[1];
        gray8_image bleu = rgb_vector[2];
        int j = 0;
        for (int i = 0; i < rgb_vector[0].sx * rgb_vector[0].sy; i++)
        {
            imRgb.pixels[j] = red.pixels[i];
            imRgb.pixels[j + 1] = green.pixels[i];
            imRgb.pixels[j + 2] = bleu.pixels[i];
            j += 3;
        }
        return imRgb;
    }

    hsv_image merge_hsv(std::vector<hsv_image> hsv_vector)
    {
        hsv_image imHsv{hsv_vector[0].sx, hsv_vector[0].sy};

        hsv_image H = hsv_vector[0];
        hsv_image S = hsv_vector[1];
        hsv_image V = hsv_vector[2];
        int j = 0;
        for (int i = 0; i < hsv_vector[0].sx * hsv_vector[0].sy; i++)
        {
            imHsv.pixels[j] = H.pixels[i];
            imHsv.pixels[j + 1] = S.pixels[i];
            imHsv.pixels[j + 2] = V.pixels[i];
            j += 3;
        }
        return imHsv;
    }

    gray8_image equalize_histogram_gray(gray8_image& image) {
        gray8_image imageNew{image.sx, image.sy};
        // Calcul de l'histogramme de l'image d'entrée
        histogram_1d histogram = give_histogram(image);

        // Calcul de la fonction de transformation cumulative
        int total_pixels = image.sx * image.sy;
        float cumulative_distribution[IMAGE_NB_LEVELS] = {0};
        cumulative_distribution[0] = (float)histogram.histogram[0] / total_pixels;
        for (int i = 1; i < IMAGE_NB_LEVELS; ++i) {
            cumulative_distribution[i] = cumulative_distribution[i - 1] + (float)histogram.histogram[i] / total_pixels;
        }

        // Application de la transformation à chaque pixel de l'image
        GRAY8 pixels = image.get_buffer();
        for (int i = 0; i < total_pixels; ++i) {
            imageNew.pixels[i] = (unsigned char)(cumulative_distribution[pixels[i]] * IMAGE_MAX_LEVEL);
        }
        return imageNew;
    }







    std::vector<uint8_t> color_hsv2rgb(std::vector<float> hsv)
    {
        float H = hsv[0];
        float S = hsv[1]; // Normalisation de la saturation à la plage [0, 1]
        float V = hsv[2]; // Normalisation de la valeur à la plage [0, 1]

        float C = V * S;
        float X = C * (1 - std::abs(std::fmod(H / 60, 2) - 1));
        float m = V - C;
        float R, G, B;

        if (H >= 0 && H < 60)
        {
            R = C;
            G = X;
            B = 0;
        }
        else if (H >= 60 && H < 120)
        {
            R = X;
            G = C;
            B = 0;
        }
        else if (H >= 120 && H < 180)
        {
            R = 0;
            G = C;
            B = X;
        }
        else if (H >= 180 && H < 240)
        {
            R = 0;
            G = X;
            B = C;
        }
        else if (H >= 240 && H < 300)
        {
            R = X;
            G = 0;
            B = C;
        }
        else if (H >= 300 && H < 360)
        {
            R = C;
            G = 0;
            B = X;
        }
        else
        {
            R = 0;
            G = 0;
            B = 0;
        }

        // Convertir R, G, B de la plage [0, 1] à la plage [0, 255]
        uint8_t R_int = ((R + m) * 255);
        uint8_t G_int = ((G + m) * 255);
        uint8_t B_int = ((B + m) * 255);

        std::vector<uint8_t> rgb_vec{R_int, G_int, B_int};
        return rgb_vec;
    }


    rgb24_image convert_hsv_rgb(hsv_image image)
    {
        rgb24_image imageRgb{image.sx, image.sy};

        for (int i = 0; i < image.sx * image.sy * 3; i+=3)
        {

            std::vector<float> hsv_color{image.pixels[i], image.pixels[i + 1], image.pixels[i + 2]};
            std::vector<uint8_t> rgb_color = color_hsv2rgb(hsv_color);
            imageRgb.pixels[i] = rgb_color[0];
            imageRgb.pixels[i + 1] = rgb_color[1];
            imageRgb.pixels[i + 2] = rgb_color[2];

        }
        return imageRgb;
    }


    hsv_image convert_rgb_hsv(rgb24_image image)
    {
        hsv_image imageHsv{image.sx, image.sy};

        for (int i = 0; i < image.sx * image.sy * 3; i+=3)
        {

            std::vector<uint8_t> rgb_color{image.pixels[i], image.pixels[i + 1], image.pixels[i + 2]};
            std::vector<float> hsv_color = color_rgb2hsv(rgb_color);

            imageHsv.pixels[i] = hsv_color[0];
            imageHsv.pixels[i + 1] = hsv_color[1];
            imageHsv.pixels[i + 2] = hsv_color[2];
        }
        return imageHsv;
    }

    gray8_image convert_One_canal_HSV2GRAY(hsv_image image)
    {
        gray8_image imGray{image.sx, image.sy};

        for (int i = 0; i < image.sx * image.sy; i +=1)
        {
            imGray.pixels[i] = image.pixels[i] * 255.0;
        }
        return imGray;
    }

    hsv_image convert_One_canal_GRAY2HSV(gray8_image image)
    {
        hsv_image imhsv{image.sx, image.sy};
        for (int i = 0; i < image.sx * image.sy; i +=1)
        {
            imhsv.pixels[i] = image.pixels[i] / 255.0;
        }
        return imhsv;
    }

    rgb24_image equalize_histogram_hsv_in_V(rgb24_image& image)
    {
        hsv_image imageHsv = convert_rgb_hsv(image);
        std::vector<hsv_image> hsv_vector = split_hsvImage(imageHsv);


        gray8_image grayONV = convert_One_canal_HSV2GRAY(hsv_vector[2]);
        gray8_image equV = equalize_histogram_gray(grayONV);

        hsv_image rev_V = convert_One_canal_GRAY2HSV(equV);


        std::vector<hsv_image> hsv_equ{hsv_vector[0], hsv_vector[1], rev_V};

        hsv_image final_hsv_eq = merge_hsv(hsv_equ);

        rgb24_image final_rgb = convert_hsv_rgb(final_hsv_eq);

        return final_rgb;
    }

    rgb24_image saturation_modifier(rgb24_image& image, float parameter)
    {
        hsv_image imageHsv = convert_rgb_hsv(image);
        std::vector<hsv_image> hsv_vector = split_hsvImage(imageHsv);
        for (int i = 0; i < hsv_vector[1].sx * hsv_vector[1].sy; i +=1)
        {
            hsv_vector[1].pixels[i] = hsv_vector[1].pixels[i] * parameter;
            if (hsv_vector[1].pixels[i] > 1)
                hsv_vector[1].pixels[i] = 1;
            if (hsv_vector[1].pixels[i] < 0)
                hsv_vector[1].pixels[i] = 0;
        }
        hsv_image final_hsv_eq = merge_hsv(hsv_vector);

        rgb24_image final_rgb = convert_hsv_rgb(final_hsv_eq);
        return final_rgb;
    }


    rgb24_image equalize_histogram_rgb(rgb24_image& image)
    {
        std::vector<gray8_image> rgb_vector = split_rgbImage(image);
        gray8_image red = equalize_histogram_gray(rgb_vector[0]);
        gray8_image green = equalize_histogram_gray(rgb_vector[1]);
        gray8_image bleu = equalize_histogram_gray(rgb_vector[2]);

        std::vector<gray8_image> rgb_equ{red, green, bleu};
        rgb24_image final_img = merge_Rgb(rgb_equ);
        return final_img;
    }

    mask create_laplacian_mask() {
        mask laplacian_mask(3, 3);
        laplacian_mask.set_value(0, 0, 0);
        laplacian_mask.set_value(1, 0, -1);
        laplacian_mask.set_value(2, 0, 0);
        laplacian_mask.set_value(0, 1, -1);
        laplacian_mask.set_value(1, 1, 5);
        laplacian_mask.set_value(2, 1, -1);
        laplacian_mask.set_value(0, 2, 0);
        laplacian_mask.set_value(1, 2, -1);
        laplacian_mask.set_value(2, 2, 0);

        return laplacian_mask;
    }

    mask create_gaussian_blur_mask(float sigma) {
        int size = 3 * sigma;
        mask gaussian_mask(size, size);

        float sum = 0.0;
        int half_size = size / 2;

        for (int y = -half_size; y <= half_size; ++y) {
            for (int x = -half_size; x <= half_size; ++x) {
                float value = std::exp(-1 * (x * x + y * y) / (2 * sigma * sigma)) ;// / (2 * 3.14 * sigma * sigma)
                gaussian_mask.set_value(x + half_size, y + half_size, value);
                sum += value;
            }
        }

        for (int i = 0; i < size * size; ++i) {
            gaussian_mask.values[i] /= sum;
        }

        return gaussian_mask;
    }

    void create_noise(rgb24_image& image, int k) {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_real_distribution<double> dis(-k, k);
        for (int i = 0; i < image.sx * image.sy * 3; i +=1) {
            double random_number = dis(gen);
            int noise_value = static_cast<int>(random_number);
            int new_value = image.pixels[i] + noise_value;
            if (new_value < 0)
                new_value = 0;
            else if (new_value > 255)
                new_value = 255;

            image.pixels[i] = static_cast<unsigned char>(new_value);
            //std::cout << static_cast<unsigned char>(new_value) << '\t' << image.pixels[i] << "\n";
        }
    }

    void nlmeans_denoising(rgb24_image& image, int search_window, int patch_size) {

        float h = search_window * patch_size;

    rgb24_image denoised_image(image.sx, image.sy);

    int hw = search_window / 2;
    int hp = patch_size / 2;


    for (int y = hp; y < image.sy - hp; ++y) {
        for (int x = hp; x < image.sx - hp; ++x) {
            float sumR = 0.0, sumG = 0.0, sumB = 0.0;
            float total_weight = 0.0;

            for (int py = -hp; py <= hp; ++py) {
                for (int px = -hp; px <= hp; ++px) {
                    float weight = 0.0;

                    for (int ry = -hw; ry <= hw; ++ry) {
                        for (int rx = -hw; rx <= hw; ++rx) {
                            int ix = x + px + rx;
                            int iy = y + py + ry;

                            if (ix >= 0 && ix < image.sx && iy >= 0 && iy < image.sy) {

                                unsigned char pixelR = image.pixels[iy * image.sx * 3 + ix * 3];     // R
                                unsigned char pixelG = image.pixels[iy * image.sx * 3 + ix * 3 + 1]; // G
                                unsigned char pixelB = image.pixels[iy * image.sx * 3 + ix * 3 + 2]; // B


                                float diffR = static_cast<float>(pixelR) - static_cast<float>(image.pixels[y * image.sx * 3 + x * 3]);
                                float diffG = static_cast<float>(pixelG) - static_cast<float>(image.pixels[y * image.sx * 3 + x * 3 + 1]);
                                float diffB = static_cast<float>(pixelB) - static_cast<float>(image.pixels[y * image.sx * 3 + x * 3 + 2]);


                                weight += std::exp(-(diffR * diffR + diffG * diffG + diffB * diffB) / (h * h));
                            }
                        }
                    }


                    total_weight += weight;
                    sumR += image.pixels[(y + py) * image.sx * 3 + (x + px) * 3] * weight;
                    sumG += image.pixels[(y + py) * image.sx * 3 + (x + px) * 3 + 1] * weight;
                    sumB += image.pixels[(y + py) * image.sx * 3 + (x + px) * 3 + 2] * weight;
                }
            }


            denoised_image.pixels[y * image.sx * 3 + x * 3] = std::min(std::max(int(sumR / total_weight), 0), 255);
            denoised_image.pixels[y * image.sx * 3 + x * 3 + 1] = std::min(std::max(int(sumG / total_weight), 0), 255);
            denoised_image.pixels[y * image.sx * 3 + x * 3 + 2] = std::min(std::max(int(sumB / total_weight), 0), 255);
        }
    }

    // Remplacer l'image actuelle par l'image débruitée
    std::swap(image.pixels, denoised_image.pixels);
}


    void ameliore_nettete(gray8_image& image) {
        mask laplacien = create_laplacian_mask();
        apply_convolution(image, laplacien);
    }

    void ameliore_nettete(rgb24_image& image) {
        mask laplacien = create_laplacian_mask();
        apply_convolution(image, laplacien);
    }
    void apply_convolution(gray8_image& image, const mask& m) {
        // Créer une nouvelle image pour stocker le résultat.
        gray8_image result(image.sx, image.sy);

        for (int y = 0; y < image.sy; ++y) {
            for (int x = 0; x < image.sx; ++x) {
                float sum = 0.0;

                // Appliquer le masque.
                for (int my = 0; my < m.height; ++my) {
                    for (int mx = 0; mx < m.width; ++mx) {
                        int ix = x + mx - m.width / 2;
                        int iy = y + my - m.height / 2;

                        if (ix >= 0 && ix < image.sx && iy >= 0 && iy < image.sy) {
                            GRAY8 pixel = &image.pixels[iy * image.sx + ix];
                            sum += pixel[0] * m.get_value(mx, my);
                        }
                    }
                }

                // Définir le pixel résultant.
                GRAY8 resultPixel = &result.pixels[y * image.sx + x];
                resultPixel[0] = std::min(std::max(int(sum), 0), 255);
            }
        }

        // Remplacer l'image actuelle par le résultat.
        std::swap(image.pixels, result.pixels);
    }

    void apply_convolution(rgb24_image& image, const mask& m) {
        rgb24_image result(image.sx, image.sy);

        for (int y = 0; y < image.sy; ++y) {
            for (int x = 0; x < image.sx; ++x) {
                float sumR = 0.0, sumG = 0.0, sumB = 0.0;

                for (int my = 0; my < m.height; ++my) {
                    for (int mx = 0; mx < m.width; ++mx) {
                        int ix = x + mx - m.width / 2;
                        int iy = y + my - m.height / 2;

                        if (ix >= 0 && ix < image.sx && iy >= 0 && iy < image.sy) {
                            RGB8 pixel = &image.pixels[(iy * image.sx + ix) * 3];
                            sumR += pixel[0] * m.get_value(mx, my);
                            sumG += pixel[1] * m.get_value(mx, my);
                            sumB += pixel[2] * m.get_value(mx, my);
                        }
                    }
                }

                RGB8 resultPixel = &result.pixels[(y * image.sx + x) * 3];
                resultPixel[0] = std::min(std::max(int(sumR), 0), 255);
                resultPixel[1] = std::min(std::max(int(sumG), 0), 255);
                resultPixel[2] = std::min(std::max(int(sumB), 0), 255);
            }
        }

        std::swap(image.pixels, result.pixels);
    }



    void plot_histogram(const histogram_1d& histogram) {
        int max_frequency = 0;
        for (int i = 0; i < IMAGE_NB_LEVELS; ++i) {
            if (histogram.histogram[i] > max_frequency) {
                max_frequency = histogram.histogram[i];
            }
        }

        const int max_height = 20; // Hauteur maximale de l'histogramme en lignes ASCII
        for (int row = max_height; row >= 0; --row) {
            for (int col = 0; col < IMAGE_NB_LEVELS; ++col) {
                if ((histogram.histogram[col] * max_height) / max_frequency > row) {
                    std::cout << "#" << " ";
                } else {
                    std::cout << " ";
                }
            }
            std::cout << std::endl;
        }

        // Affichage des niveaux de gris en bas de l'histogramme
        for (int i = 0; i < IMAGE_NB_LEVELS; ++i) {
            std::cout << "-";
        }
        std::cout << std::endl;
        for (int i = 0; i < IMAGE_NB_LEVELS; ++i) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }
}
