#include "Reverb.h"
#include <cmath>
#include <cassert>
#include <cstdio>
#include <algorithm>

Vector::Vector(double x, double y)
    : x(x), y(y) {}

double Vector::magnitude() const
{
    return sqrt(x*x + y*y);
}

Vector Vector::normalize() const
{
    return *this / magnitude();
}

Vector Vector::operator+(const Vector& other) const
{
    return Vector(x + other.x, y + other.y);
}

void Vector::operator+=(const Vector& other)
{
    x += other.x;
    y += other.y;
}

Vector Vector::operator-(const Vector& other) const
{
    return Vector(x - other.x, y - other.y);
}

void Vector::operator-=(const Vector& other)
{
    x -= other.x;
    y -= other.y;
}

Vector Vector::operator*(const double other) const
{
    return Vector(x * other, y * other);
}

void Vector::operator*=(const double other)
{
    x *= other;
    y *= other;
}

Vector Vector::operator/(const double other) const
{
    return Vector(x / other, y / other);
}

void Vector::operator/=(const double other)
{
    x /= other;
    y /= other;
}

Particle::Particle(double x, double y, double mass=1.f, bool locked=false)
    : position {x, y},
      velocity {0.f, 0.f},
      acceleration {0.f, 0.f},
      mass(mass),
      locked(locked)
{
}

void Particle::applyForce(const Vector& force)
{
    acceleration += force / mass;
}

void Particle::update()
{
    if (locked)
        return;
    velocity *= 0.99;
    velocity += acceleration;
    position += velocity;
    acceleration *= 0.0;
}

const Vector& Particle::pos() const
{
    return position;
}

// TODO: remove
const Vector& Particle::accel() const
{
    return acceleration;
}

Spring::Spring(Particle& a,
               Particle& b,
               const double k,
               const double restLength)
    : a(a),
      b(b),
      k(k),
      restLength(restLength)
{
}

void Spring::update()
{
    Vector dist = b.pos() - a.pos();
    double x = dist.magnitude() - restLength;
    Vector f = dist.normalize() * x * k;
    //printf("dist (%f %f), x %f, f (%f %f)\n", dist.x, dist.y, x, f.x, f.y);

    a.applyForce(f);
    b.applyForce(f * -1.f);
}

SpringArray::SpringArray(const size_t nodes,
                         const double mass,
                         const double k,
                         const double length)
    : particles(),
      springs(),
      start(NULL),
      end(NULL)
{
    // temporary
    assert(nodes >= 2);
    const double node_mass = mass / (nodes + 2);
    const double restLength = length / (nodes + 1);

    particles.reserve(nodes + 2);
    springs.reserve(nodes + 1);

    particles.emplace_back(0.f, 0.f, node_mass, true);

    for (size_t i = 1; i <= nodes; ++i) {
        particles.emplace_back(restLength * i, 0.f, node_mass);
    }
    particles.emplace_back(length, 0.f, node_mass, true);
    // TODO: change this (temporary)
    start = &particles.at(3);
    end = &particles.at(particles.size() - 4);

    assert(particles.size() == nodes + 2);

    for (size_t i = 0; i < particles.size() - 1; ++i) {
        springs.emplace_back(particles[i], particles[i+1], k, restLength);
    }

    assert(springs.size() == nodes + 1);
}

void SpringArray::applySample(const double force)
{
    double ratio = 0.7f;
    Vector forceVec = Vector(0.0f, force * 0.1f);
    start->applyForce(forceVec);
}

void SpringArray::update()
{
    for (Spring& spring : springs)
        spring.update();

    for (Particle& particle : particles)
        particle.update();
}

double SpringArray::getSample()
{
    double sample = end->pos().y * 1.0f; // 100.f; 
    // printf("start x: %f, y: %f\n", end->pos().x, end->pos().y);
    // return std::clamp(sample, -1., 1.);
    return sample;
}

const std::vector<Particle>& SpringArray::getParticles() const
{
    return particles;
}

const std::vector<Spring>& SpringArray::getSprings() const
{
    return springs;
}
