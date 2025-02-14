#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "../include/Circle.hpp"
#include <vector>
#include <random>
#include <ctime>

// Link with OpenGL
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float VELOCITY_SCALE = 400.0f; // Increased velocity scale

std::vector<Circle> circles;
float lastFrameTime = 0;
HWND globalHwnd;  // Store window handle globally

// Random number generator
float random(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

// Add a new circle at a random position
void addNewCircle() {
    float radius = random(20.0f, 40.0f);
    float mass = radius * radius; // Mass proportional to area
    float x = random(radius, WINDOW_WIDTH - radius);
    float y = random(radius, WINDOW_HEIGHT - radius);
    
    Circle circle(x, y, radius, mass);
    // Increased velocity range for more dynamic movement
    circle.setVelocity(
        random(-VELOCITY_SCALE, VELOCITY_SCALE),
        random(-VELOCITY_SCALE, VELOCITY_SCALE)
    );
    circles.push_back(circle);
}

// Initialize OpenGL
void initGL(HWND hWnd) {
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR), 1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0,
        PFD_MAIN_PLANE, 0, 0, 0, 0
    };

    HDC hDC = GetDC(hWnd);
    int pixelFormat = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, pixelFormat, &pfd);
    HGLRC hRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hRC);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Initialize with two circles
void initCircles() {
    // First circle - left side
    Circle circle1(WINDOW_WIDTH * 0.25f, WINDOW_HEIGHT * 0.5f, 30.0f, 900.0f);
    circle1.setVelocity(VELOCITY_SCALE * 0.5f, 0); // Moving right
    circles.push_back(circle1);
    
    // Second circle - right side
    Circle circle2(WINDOW_WIDTH * 0.75f, WINDOW_HEIGHT * 0.5f, 30.0f, 900.0f);
    circle2.setVelocity(-VELOCITY_SCALE * 0.5f, 0); // Moving left
    circles.push_back(circle2);
}

// Draw a circle
void drawCircle(const Circle& circle) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 32; i++) {
        float angle = i * 3.14159f * 2.0f / 32;
        float x = circle.position.x + cos(angle) * circle.radius;
        float y = circle.position.y + sin(angle) * circle.radius;
        glVertex2f(x, y);
    }
    glEnd();
}

// Update simulation
void update() {
    float currentTime = GetTickCount() / 1000.0f;
    float deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;
    
    // Update positions
    for (auto& circle : circles) {
        circle.update(deltaTime);
        circle.handleBoundaryCollision(WINDOW_WIDTH, WINDOW_HEIGHT);
    }
    
    // Check and resolve collisions
    for (size_t i = 0; i < circles.size(); i++) {
        for (size_t j = i + 1; j < circles.size(); j++) {
            if (circles[i].checkCollision(&circles[j])) {
                circles[i].resolveCollision(&circles[j]);
            }
        }
    }
}

// Render frame
void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    glColor3f(1.0f, 1.0f, 1.0f);
    for (const auto& circle : circles) {
        drawCircle(circle);
    }
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_CHAR:
            // Handle keyboard input
            if (wParam == 'c' || wParam == 'C') {
                addNewCircle();
            }
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Main function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register window class
    WNDCLASSEX wc = {
        sizeof(WNDCLASSEX), CS_OWNDC, WndProc, 0, 0,
        GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
        "CircleCollision", NULL
    };
    RegisterClassEx(&wc);
    
    // Create window
    HWND hWnd = CreateWindow(
        "CircleCollision", "Circle Collision Simulation - Press 'C' to add circles",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH + 16, WINDOW_HEIGHT + 39, NULL, NULL, hInstance, NULL
    );
    
    globalHwnd = hWnd;
    
    // Initialize OpenGL and circles
    initGL(hWnd);
    initCircles();
    
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    
    // Message loop
    MSG msg;
    while (true) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        update();
        render();
        SwapBuffers(GetDC(hWnd));
    }
    
    return msg.wParam;
}
