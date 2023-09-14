#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

#include "color.h"
#include "hittable.h"
#include "material.h"
#include "image.h"
#include <iomanip>
#include <chrono>
#include <iostream>

class camera
{
public:
    double aspect_ratio = 1.0;  // Ratio of image width over height
    int image_width = 100;      // Rendered image width in pixel count
    int samples_per_pixel = 10; // Number of samples per pixel
    int max_depth = 10;         // Maximum ray bounce depth

    color background; // Scene background color

    double vfov = 90.0; // Vertical field-of-view in degrees
    point3 lookfrom = point3(0, 0, -1);
    point3 lookat = point3(0, 0, 0);
    vec3 vup = vec3(0, 1, 0);

    double defocus_angle = 0; // Variation angle of rays through each pixel
    double focus_dist = 10;   // Distance from camera lookfrom point to plane of perfect focus

    std::string outputfile = "image.bmp";

    ~camera(){delete img;}

    void render(const hittable &world)
    {
        initialize();

        std::cout << "P3\n"
                  << image_width << ' ' << image_height << "\n255\n";
        auto start_time = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < image_height; ++j)
        {
            for (int i = 0; i < image_width; ++i)
            {
                // TODO: 美观地输出进度和剩余时间
                double progress = (double)(j * image_width + i) / (image_width * image_height);
                auto current_time = std::chrono::system_clock::now();
                // double
                auto elapsed_time = std::chrono::duration_cast<std::chrono::duration<double>>(current_time - start_time);
                auto remaining_time = std::chrono::duration_cast<std::chrono::duration<double>>(elapsed_time / progress - elapsed_time);

                std::clog << "\rProgress: " << std::fixed << std::setprecision(2) << progress * 100 << "%"
                          << " Elapsed time: " << std::fixed << std::setprecision(0) << elapsed_time.count() << "s"
                            << " Remaining time: " << std::fixed << std::setprecision(0) << remaining_time.count() << "s"
                          << std::flush;
                
                color pixel_color(0, 0, 0);
                for(int s_j = 0; s_j < sqrt_spp; ++s_j)
                {
                    for(int s_i = 0;s_i<sqrt_spp;++s_i)
                    {
                        ray r = get_ray(i, j, s_i, s_j);
                        pixel_color += ray_color(r, max_depth, world);
                    }
                }
                write_color(std::cout, pixel_color, samples_per_pixel);
            }
        }
        img->save_bmp(outputfile.c_str());
        std::clog << "\rDone.                 \n";
    }

private:
    int image_height;    // Rendered image height
    int sqrt_spp;
    double recip_sqrt_spp;

    point3 center;       // Camera center
    point3 pixel00_loc;  // Location of pixel 0, 0
    vec3 pixel_delta_u;  // Offset to pixel to the right
    vec3 pixel_delta_v;  // Offset to pixel below
    vec3 u, v, w;        // Camera basis vectors
    vec3 defocus_disk_u; // Defocus disk horizontal radius
    vec3 defocus_disk_v; // Defocus disk vertical radius

    image *img;

    void initialize()
    {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        img = new image(image_width, image_height);

        center = lookfrom;

        // Determine viewport dimensions.
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2);
        auto viewport_height = 2.0 * h * focus_dist;
        auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

        sqrt_spp = static_cast<int>(sqrt(samples_per_pixel));
        recip_sqrt_spp = 1.0 / sqrt_spp;   

        // Calculate the u, v, and w basis vectors.
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        auto viewport_u = viewport_width * u;
        auto viewport_v = viewport_height * -v;

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left =
            center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    point3 defocus_disk_sample() const
    {
        // Returns a random point in the camera defocus disk.
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    color ray_color(const ray &r, int depth, const hittable &world) const
    {
        hit_record rec;

        if (depth <= 0)
            return color(0, 0, 0);
        // if ray hits nothing
        if (!world.hit(r, interval(1e-3, infinity), rec))
            return background;

        ray scattered;
        color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);
        double pdf;
        color albedo;

        if(!rec.mat->scatter(r, rec, albedo, scattered, pdf))
            return color_from_emission;
        
        color color_from_scatter = albedo * rec.mat->scattering_pdf(r, rec, scattered) 
                                          * ray_color(scattered, depth - 1, world) / pdf;

        return color_from_emission + color_from_scatter;
    }

    ray get_ray(int i, int j, int s_i, int s_j) const
    {
        // Get a randomly sampled camera ray for the pixel at location i,j.
        auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        auto pixel_sample = pixel_center + pixel_sample_square(s_i, s_j);

        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;

        auto ray_time = random_double();
        return ray(ray_origin, ray_direction, ray_time);
    }

    vec3 pixel_sample_square(int s_i, int s_j) const
    {
        // Returns a random point in the square surrounding a pixel at the origin
        // given the two subpixel indices
        auto px = -0.5 + recip_sqrt_spp * (0.5 + random_double());
        auto py = -0.5 + recip_sqrt_spp * (0.5 + random_double());
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }
};

#endif