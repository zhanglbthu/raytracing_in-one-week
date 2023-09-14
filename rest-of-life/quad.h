#ifndef QUAD_H
#define QUAD_H
#include "rtweekend.h"
#include "hittable.h"
#include "hittable_list.h"

class quad : public hittable
{
public:
    quad(const point3 &_Q, const vec3 &_u, const vec3 &_v, shared_ptr<material> m)
        : Q(_Q), u(_u), v(_v), mat(m)
    {
        auto n = cross(u, v);
        normal = unit_vector(n);
        D = dot(normal, Q);
        w = n / dot(n, n);

        area = n.length();

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
        if (fabs(denom) < 1e-8)
            return false;
        auto t = (D - dot(normal, r.origin())) / denom;
        if (!ray_t.contains(t))
            return false;
        auto intersection = r.at(t);

        // determine if the intersection is inside the quad
        vec3 planar_hitpt_vector = intersection - Q;
        auto alpha = dot(w, cross(planar_hitpt_vector, v));
        auto beta = dot(w, cross(u, planar_hitpt_vector));
        if (!is_interior(alpha, beta, rec))
            return false;

        rec.t = t;
        rec.p = intersection;
        rec.mat = mat;
        rec.set_face_normal(r, normal);

        return true;
    }

    virtual bool is_interior(double a, double b, hit_record &rec) const
    {
        if ((a < 0) || (1 < a) || (b < 0) || (1 < b))
            return false;

        rec.u = a;
        rec.v = b;
        return true;
    }
    double pdf_value(const point3 &origin, const vec3 &v) const override
    {
        hit_record rec;
        if (!this->hit(ray(origin, v), interval(0.001, infinity), rec))
            return 0;

        auto distance_squared = rec.t * rec.t * v.length_squared();
        auto cosine = fabs(dot(v, rec.normal) / v.length());

        return distance_squared / (cosine * area);
    }

    vec3 random(const point3 &origin) const override
    {
        auto random_point = Q + random_double() * u + random_double() * v;
        return random_point - origin;
    }

private:
    point3 Q;
    vec3 u, v;
    shared_ptr<material> mat;
    aabb bbox;

    vec3 normal;
    double D;

    vec3 w;
    double area;
};

inline shared_ptr<hittable_list> box(const point3 &a, const point3 &b, shared_ptr<material> mat)
{
    // Returns the 3D box (six sides) that contains the two opposite vertices a & b.

    auto sides = make_shared<hittable_list>();

    // Construct the two opposite vertices with the minimum and maximum coordinates.
    auto min = point3(fmin(a.x(), b.x()), fmin(a.y(), b.y()), fmin(a.z(), b.z()));
    auto max = point3(fmax(a.x(), b.x()), fmax(a.y(), b.y()), fmax(a.z(), b.z()));

    auto dx = vec3(max.x() - min.x(), 0, 0);
    auto dy = vec3(0, max.y() - min.y(), 0);
    auto dz = vec3(0, 0, max.z() - min.z());

    sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()), dx, dy, mat));  // front
    sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz, dy, mat)); // right
    sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx, dy, mat)); // back
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dz, dy, mat));  // left
    sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()), dx, -dz, mat)); // top
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dx, dz, mat));  // bottom

    return sides;
}
#endif