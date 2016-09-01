//
//  main.cpp
//  GLFW Test
//
//  Creatsed by Logan Pazol on 8/16/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include <iostream>
#include <vector>
#include <boost/thread.hpp>
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

#include "object.hpp"

std::vector<object*>objects;
object* a;

template <class T>
T* createObject() {
    
    T* newObject = new T();
    
    objects.push_back(newObject);
    
    return newObject;
    
}

double velocity_constant = 5;

void key_release(int key) {
    
    switch (key) {
            
        case GLFW_KEY_W:
            
            a->velocity_y += velocity_constant;
            
            break;
        case GLFW_KEY_S:
            
            a->velocity_y -= velocity_constant;
            
            break;
            
            
        case GLFW_KEY_D:
            
            a->velocity_x -= velocity_constant;
            
            break;
            
        case GLFW_KEY_A:
            
            a->velocity_x += velocity_constant;
            
            break;
            
        case GLFW_KEY_R:
            
            a->scale_driver.reverse();
            
            break;
    }
    
}

void key_press(int key) {
    
    switch (key) {
            
        case GLFW_KEY_W:
            
            a->velocity_y -= velocity_constant;
            
            break;
            
        case GLFW_KEY_S:
            
            a->velocity_y += velocity_constant;
            
            break;
            
            
        case GLFW_KEY_D:
            
            a->velocity_x += velocity_constant;
            
            break;
            
        case GLFW_KEY_A:
            
            a->velocity_x -= velocity_constant;
            
            break;
            
        case GLFW_KEY_R:
            
            a->scale_driver.play();
            
            break;
            
    }
    
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    
    if (action == GLFW_PRESS)
        key_press(key);
    
    if (action == GLFW_RELEASE)
        key_release(key);
    
}

void wait(double time) {
    
    double start = glfwGetTime();
    
    while (glfwGetTime() < start + time) {}
    
}

int main(void) {
    
    GLFWwindow* window;
    
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1280, 800, "Hello World", NULL, NULL);
    if (!window) {
        
        glfwTerminate();
        return -1;
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    glfwSetKeyCallback(window, key_callback);
    
    a = createObject<object>();
    
    double timePerTick = 1.0 / TICKS_PER_SECOND;
    double lastTime = glfwGetTime();
    double loopElapsedTime = 0.0;
    
    int maxUpdateCount = 5;
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        
        double elapsed = glfwGetTime() - lastTime;
        lastTime = glfwGetTime();

        loopElapsedTime += elapsed;
        
        int loops = 0;
        while (loopElapsedTime >= timePerTick && loops < maxUpdateCount) {
            
            for (int i = 0; i < objects.size(); i++)
                objects[i]->update();
            
            /* Poll for and process events */
            glfwPollEvents();
            
            loopElapsedTime -= timePerTick;
            
            loops++;
            
        }
        
        if (loops == maxUpdateCount)
            std::cout << "Cant keep up with " << TICKS_PER_SECOND << "UPS!\n";
        
        double interpolation = loopElapsedTime / timePerTick;
        
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, 1280, 800, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);

        for (int i = 0; i < objects.size(); i++)
            objects[i]->render(interpolation);
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        
    }
    
    glfwTerminate();
    return 0;
}
