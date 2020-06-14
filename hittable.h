#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

// hit_record
// ???
struct hit_record{
    point3 p;
    vec3 normal;
    double t;
    bool front_face;

    // set_face normal
    // ???
    inline void set_face_normal(const ray& r, const vec3& outward_normal){
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal :-outward_normal;
    }
};

// hittable
// 
class hittable{
    public:
        // hit
        // Takes a ray, and bounds on t
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};

#endif