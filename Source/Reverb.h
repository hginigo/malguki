#pragma once
#include <cstddef>
#include <vector>

struct Vector {
    double x, y;

    Vector(double x, double y);

    double magnitude() const;
    Vector normalize() const;

    Vector operator+(const Vector& other) const;
    Vector operator-(const Vector& other) const;
    Vector operator*(const double other) const;
    Vector operator/(const double other) const;

    void operator+=(const Vector& other);
    void operator-=(const Vector& other);
    void operator*=(const double other);
    void operator/=(const double other);
};

class Particle {
public:
    Particle(double x, double y, double mass, bool locked);

    void applyForce(const Vector& force);
    void update();
    const Vector& pos() const;
    const Vector& accel() const;

private:
    Vector position;
    Vector velocity;
    Vector acceleration;
    double mass;
    bool locked;
};

class Spring {
public:
    Spring(Particle& a, Particle& b, const double k, const double restLength);
    Spring(Particle&& a, Particle&& b, const double k, const double restLength) = delete;

    void update();

private:
    const double k;
    const double restLength;
    Particle& a;
    Particle& b;
};

class SpringArray {
public:
    SpringArray(const size_t nodes, const double mass, const double k, const double length);

    void applySample(const double force);
    void update();
    
    double getSample();

    const std::vector<Particle>& getParticles() const;
    const std::vector<Spring>& getSprings() const;
private:
    std::vector<Particle> particles;
    std::vector<Spring> springs;

    Particle *start, *end;
};
