#pragma once
#include <cstddef>
#include <vector>

struct Vector {
    float x, y;

    Vector(float x, float y);

    float magnitude() const;
    Vector normalize() const;

    Vector operator+(const Vector& other) const;
    Vector operator-(const Vector& other) const;
    Vector operator*(const float other) const;
    Vector operator/(const float other) const;

    void operator+=(const Vector& other);
    void operator-=(const Vector& other);
    void operator*=(const float other);
    void operator/=(const float other);
};

class Particle {
public:
    Particle(float x, float y, float mass, bool locked);

    void applyForce(const Vector& force);
    void update();
    const Vector& pos() const;
    const Vector& accel() const;

private:
    Vector position;
    Vector velocity;
    Vector acceleration;
    float mass;
    bool locked;
};

class Spring {
public:
    Spring(Particle& a, Particle& b, const float k, const float restLength);
    Spring(Particle&& a, Particle&& b, const float k, const float restLength) = delete;

    void update();

private:
    const float k;
    const float restLength;
    Particle& a;
    Particle& b;
};

class SpringArray {
public:
    SpringArray(const size_t nodes, const float mass, const float k, const float length);

    void applySample(const float force);
    void update();
    
    float getSample();
private:
    std::vector<Particle> particles;
    std::vector<Spring> springs;

    Particle *start, *end;
};
