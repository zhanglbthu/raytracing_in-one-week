#ifndef INTERVAL_H
#define INTERVAL_H

#include <cmath>

class interval {
  public:
    double min, max;

    interval() : min(+INFINITY), max(-INFINITY) {} // Default interval is empty

    interval(double _min, double _max) : min(_min), max(_max) {}

    interval(const interval &a, const interval &b) : min(fmin(a.min, b.min)), max(fmax(a.max, b.max)) {}

    bool contains(double x) const {
        return min <= x && x <= max;
    }

    bool surrounds(double x) const {
        return min < x && x < max;
    }

    double clamp(double x) const {
        if ( x < min ) return min;
        if ( x > max ) return max;
        return x;
    }

    double size() const {
        return max - min;
    }
    
    interval expand(double delta) const {
        return interval(min - delta, max + delta);
    }

    static const interval empty, universe;
};

const static interval empty   (+INFINITY, -INFINITY);
const static interval universe(-INFINITY, +INFINITY);

#endif