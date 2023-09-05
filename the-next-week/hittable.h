#ifndef HITTABLE_H
#define HITTABLE_H

#include "rtweekend.h"
#include "aabb.h"

class material;

class hit_record
{
public:
    point3 p;
    vec3 normal;
    shared_ptr<material> mat;
    double t;
    double u;
    double v;

    bool front_face;
    void set_face_normal(const ray &r, const vec3 &outward_normal)
    {   
        // note: the parameter outward_normal is assumed to be a unit vector.
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable
{
public:
    virtual ~hittable() = default;

    virtual bool hit(const ray &r, interval ray_t , hit_record &rec) const = 0;
    virtual aabb bounding_box() const = 0;
};
class translate : public hittable
{
public:
    translate(shared_ptr<hittable> p, const vec3& displacement) : object(p), offset(displacement)
    {
        bbox = object->bounding_box() + offset;
    }
    bool hit(const ray &r, interval ray_t , hit_record &rec) const override
    {
        // Move the ray backwards by the offset
        ray offset_r(r.origin() - offset, r.direction(), r.time());
        //determine if the ray hits the object
        if(!object->hit(offset_r, ray_t, rec))
            return false;

        // Move the hit point forwards by the offset
        rec.p += offset;
        return true;
    }
    aabb bounding_box() const override{return bbox; }
private:
    shared_ptr <hittable> object;
    vec3 offset;
    aabb bbox;
};
#endif