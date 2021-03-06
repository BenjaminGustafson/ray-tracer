#include "common.h"
#include "hittable_list.h"
#include "sphere.h"
#include "color.h"
#include "camera.h"
#include "material.h"

#include <iostream>

// ray_color
// if an object is hit
// otherwise return the sky gradient
color ray_color(const ray& r, const hittable& world, int depth){
    hit_record rec;

    if(depth <= 0)
        return color(0,0,0);

    if(world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        color attenuation;
        if(rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth-1);
        return color(0,0,0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    //gradient
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for(int a = -11; a < 11; a++){
        for(int b = -11; b < 11; b++){
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }
    /*
    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));
    */

    return world;
}



int main() {
    //Setup image size
    const auto aspect_ratio = 5.0 / 1.0;
    const int image_width = 5000;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    //Write file heading
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    hittable_list world;

    //auto silver = make_shared<metal>(color(0.5, 0.5, 0.7), 0.0);
    auto gold = make_shared<metal>(color(0.7, 0.6, 0.0), 0.0);
    //ground
    auto gray = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    auto red = make_shared<lambertian>(color(1.0, 0.0, 0.0));
    auto green = make_shared<lambertian>(color(0.0, 1.0, 0.0));
    auto white = make_shared<lambertian>(color(1.0, 1.0, 1.0));
    auto black_hole = make_shared<lambertian>(color(0.0, 0.0, 0.0));
    auto blue = make_shared<lambertian>(color(0.2, 0.0, 0.7));

    
    world.add(make_shared<sphere>(point3(0,-10000,0), 10000, gray));
    //one in focus
    //world.add(make_shared<sphere>(point3(4, 1, 0), 1, gold));

    for(int i = 0; i <= 5; i ++){
        auto mat = make_shared<metal>(color(1.0, 1.0, 1.0), i*0.2);
        world.add(make_shared<sphere>(point3(0, 0.4, i + 2.5), 0.4, mat));
    }
    
    
    //lots of spheres
    //for(int i = -20; i < 7; i++){
    //    for(int j = -10; j < 10; j++){
    //        world.add(make_shared<sphere>(point3(i, 0.2, j), 0.2, white));
    //    }
    //}

    point3 lookfrom(7,1,5);
    point3 lookat(0,0.5,5);
    vec3 vup(0,1,0);
    auto dist_to_focus = 7.0;
    auto aperture = 0.1;

    camera cam(lookfrom, lookat, vup, 10, aspect_ratio, aperture, dist_to_focus);

    //TODO time elapsed
    for(int j = image_height-1; j >= 0; j--){
        double row_percent = 100 - 100*double(j)/image_height;
        std::cerr << "\r Rows: " << row_percent << "% " << std::flush;
        for(int i = 0; i < image_width; i++){
            double col_percent = 100*double(i)/image_width;
            //std::cerr << "\r Rows: " << row_percent << "% Columns: " << col_percent << "%    " << std::flush;
            color pixel_color(0, 0, 0);
            for(int s = 0; s < samples_per_pixel; s++){
                auto u = (i + random_double()) / (image_width-1);
                auto v = (j + random_double()) / (image_height-1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }
    std::cerr << "\nDone.\n";
}