#pragma once

struct Vector2 {
    float x;
    float y;
};

class Circle {
public:
    Circle(float x, float y, float radius, float mass);
    
    void update(float dt);
    bool checkCollision(Circle* other);
    void resolveCollision(Circle* other);
    void handleBoundaryCollision(float width, float height);
    
    // Getters
    Vector2 getPosition() const { return position; }
    Vector2 getVelocity() const { return velocity; }
    float getRadius() const { return radius; }
    float getMass() const { return mass; }
    
    // Setters
    void setVelocity(float vx, float vy) { velocity.x = vx; velocity.y = vy; }
    void setPosition(float x, float y) { position.x = x; position.y = y; }

public: // Making these public for simplicity
    Vector2 position;
    Vector2 velocity;
    float radius;
    float mass;
    float restitution;
};
