#ifndef QUAD_H
#define QUAD_H
#include "rtweekend.h"
#include "hittable.h"
class quad : public hittable
{
public:
    quad(const point3 &_Q, const vec3 &_u, const vec3 &_v, shared_ptr<material> m)
        : Q(_Q), u(_u), v(_v), mat(m)
    {
        auto n = cross(u, v);
        normal = unit_vector(n);
        D = dot(normal, Q);
        set_bounding_box();
    }
    virtual void set_bounding_box()
    {
        bbox = aabb(Q, Q + u + v);
    }
    aabb bounding_box() const override
    {
        return bbox;
    }
    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        auto denom = dot(normal, r.direction());
        if(fabs(denom) < 1e-8)
            return false;
        auto t = (D - dot(normal, r.origin())) / denom;
        if(!ray_t.contains(t))
            return false;
        auto intersection = r.at(t);

        rec.t = t;
        rec.p = intersection;
        rec.mat = mat;
        rec.set_face_normal(r, normal);

        return true;
    }

private:
    point3 Q;
    vec3 u, v;
    shared_ptr<material> mat;
    aabb bbox;

    vec3 normal;
    double D;
};
#endif