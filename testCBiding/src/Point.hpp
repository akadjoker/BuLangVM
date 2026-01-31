#pragma once

/**
 * Simple point struct to demonstrate basic binding
 */
struct Point {
    float x;
    float y;
    
    // Constructor
    Point() : x(0.0f), y(0.0f) {}
    Point(float x, float y) : x(x), y(y) {}
    
    // Helper methods
    float distance() const {
        return sqrt(x * x + y * y);
    }
    
    void translate(float dx, float dy) {
        x += dx;
        y += dy;
    }
};
