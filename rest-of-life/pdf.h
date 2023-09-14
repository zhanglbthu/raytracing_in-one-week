#ifndef PDF_H
#define PDF_H

#include "rtweekend.h"

#include "onb.h"
#include "hittable_list.h"
class pdf
{
public:
    virtual ~pdf() {}

    virtual double value(const vec3 &direction) const = 0;
    virtual vec3 generate() const = 0;
};

class sphere_pdf : public pdf
{
public:
    sphere_pdf() {}

    double value(const vec3 &direction) const override
    {
        return 1 / (4 * pi);
    }

    vec3 generate() const override
    {
        return random_unit_vector();
    }
};

class cosine_pdf : public pdf
{
public:
    cosine_pdf(const vec3 &w) { uvw.build_from_w(w); }

    double value(const vec3 &direction) const override
    {
        auto cosine_theta = dot(unit_vector(direction), uvw.w());
        return fmax(0, cosine_theta / pi);
    }

    vec3 generate() const override
    {
        return uvw.local(random_cosine_direction());
    }

private:
    onb uvw;
};
class hittable_pdf : public pdf
{
public:
    hittable_pdf(const hittable &_objects, const point3 &_origin)
        : objects(_objects), origin(_origin)
    {
    }

    double value(const vec3 &direction) const override
    {
        return objects.pdf_value(origin, direction);
    }

    vec3 generate() const override
    {
        return objects.random(origin);
    }

private:
    const hittable &objects;
    point3 origin;
};
#endif