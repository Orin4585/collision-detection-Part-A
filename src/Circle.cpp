#include "../include/Circle.hpp"
#include <cmath>
#include <random>

Circle::Circle(float x, float y, float radius, float mass) {
    this->position.x = x;
    this->position.y = y;
    this->radius = radius;
    this->mass = mass;
    this->velocity.x = 0;
    this->velocity.y = 0;
    this->restitution = 0.8f;  // Coefficient of restitution
}

void Circle::update(float dt) {
    // Update position based on velocity
    position.x += velocity.x * dt;
    position.y += velocity.y * dt;
}

bool Circle::checkCollision(Circle* other) {
    if (!other) return false;
    
    // Calculate distance between centers
    float dx = position.x - other->position.x;
    float dy = position.y - other->position.y;
    float distance = sqrt(dx*dx + dy*dy);
    
    // If distance is less than sum of radii, collision occurred
    return distance < (radius + other->radius);
}

void Circle::resolveCollision(Circle* other) {
    if (!other) return;
    
    // Calculate collision normal
    float dx = position.x - other->position.x;
    float dy = position.y - other->position.y;
    float distance = sqrt(dx*dx + dy*dy);
    
    // Avoid division by zero
    if (distance == 0) return;
    
    float nx = dx / distance;
    float ny = dy / distance;
    
    // Relative velocity
    float rvx = velocity.x - other->velocity.x;
    float rvy = velocity.y - other->velocity.y;
    
    // Relative velocity along normal
    float velAlongNormal = rvx * nx + rvy * ny;
    
    // Don't resolve if objects are moving apart
    if (velAlongNormal > 0) return;
    
    // Calculate restitution (bounciness)
    float e = restitution;
    
    // Calculate impulse scalar
    float j = -(1.0f + e) * velAlongNormal;
    j /= 1.0f/mass + 1.0f/other->mass;
    
    // Apply impulse
    float impulse_x = j * nx;
    float impulse_y = j * ny;
    
    velocity.x += impulse_x / mass;
    velocity.y += impulse_y / mass;
    other->velocity.x -= impulse_x / other->mass;
    other->velocity.y -= impulse_y / other->mass;
    
    // Positional correction (prevent sinking)
    float percent = 0.2f; // penetration percentage to correct
    float slop = 0.01f; // penetration allowance
    float penetration = radius + other->radius - distance;
    if (penetration > slop) {
        float correction = (penetration * percent) / (1.0f/mass + 1.0f/other->mass);
        position.x += correction * nx / mass;
        position.y += correction * ny / mass;
        other->position.x -= correction * nx / other->mass;
        other->position.y -= correction * ny / other->mass;
    }
}

void Circle::handleBoundaryCollision(float width, float height) {
    // Left and right walls
    if (position.x - radius < 0) {
        position.x = radius;
        velocity.x = -velocity.x * restitution;
    } else if (position.x + radius > width) {
        position.x = width - radius;
        velocity.x = -velocity.x * restitution;
    }
    
    // Top and bottom walls
    if (position.y - radius < 0) {
        position.y = radius;
        velocity.y = -velocity.y * restitution;
    } else if (position.y + radius > height) {
        position.y = height - radius;
        velocity.y = -velocity.y * restitution;
    }
}
