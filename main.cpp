#include "Shape.h"
#include "vertex.h"
#include "glad/glad.h"
#include "vertexGradient.h"
#include "fragmentShader.h"
#include "fragmentGradient.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

int scene = 0;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS) return;
    if (key == GLFW_KEY_SPACE) { scene = (scene + 1) % 3; }
}

float angle = 0.0f;
float angleSpeed = 0.0025f;
float maxAngle = 0.4f;

glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
glm::mat4 rotationS = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));

vector<float> makeThickLine(const vector<float>& points, int count, float width) {
    vector<float> result;
    float half = width / 2.0f;

    for (int i = 0; i < count; ++i) {
        float x = points[i * 3];
        float y = points[i * 3 + 1];

        float dx, dy;
        if (i == 0) {
            dx = points[3] - points[0];
            dy = points[4] - points[1];
        }
        else if (i == count - 1) {
            dx = points[i * 3] - points[(i - 1) * 3];
            dy = points[i * 3 + 1] - points[(i - 1) * 3 + 1];
        }
        else {
            dx = points[(i + 1) * 3] - points[(i - 1) * 3];
            dy = points[(i + 1) * 3 + 1] - points[(i - 1) * 3 + 1];
        }

        float len = sqrtf(dx * dx + dy * dy);
        if (len < 0.0001f) len = 1.0f;
        dx /= len;
        dy /= len;

        float nx = -dy;
        float ny = dx;

        result.push_back(x + nx * half);
        result.push_back(y + ny * half);
        result.push_back(0.0f);

        result.push_back(x - nx * half);
        result.push_back(y - ny * half);
        result.push_back(0.0f);
    }

    return result;
}

unsigned int createProgram(const char* vsSource, const char* fsSource) {
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsSource, NULL);
    glCompileShader(vs);
    int success;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(vs, 512, NULL, log);
        cout << "VERTEX FAILED:\n" << log << "\n";
    }

    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsSource, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(fs, 512, NULL, log);
        cout << "FRAGMENT FAILED:\n" << log << "\n";
    }

    unsigned int prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(prog, 512, NULL, log);
        cout << "LINK FAILED:\n" << log << "\n";
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

int main()
{
    srand((unsigned)time(0));
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* myWin = glfwCreateWindow(1080, 1080, "window", nullptr, nullptr);
    if (!myWin) {
        cout << "GL initialization failed";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(myWin);
    glfwSetKeyCallback(myWin, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Error with load GLAD";
        return -1;
    }
    cout << "OpenGL version: " << glGetString(GL_VERSION) << "\n";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infolog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infolog);
        cout << "VERTEX SHADER FAILED:\n" << infolog << "\n";
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infolog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infolog);
        cout << "FRAGMENT SHADER FAILED:\n" << infolog << "\n";
    }

    unsigned int programUniform = createProgram(vertexShaderSource, fragmentShaderSource);
    unsigned int programGradient = createProgram(vertexGradientSource, fragmentGradientSource);

    int colorLoc = glGetUniformLocation(programUniform, "uColor");
    int rotLoc = glGetUniformLocation(programUniform, "uRotation");
    int rotLocGrad = glGetUniformLocation(programGradient, "uRotation");

    Shape stri1 = createShape(tri1, 3, GL_TRIANGLES, 0.6f, 0.6f, 0.6f);
    Shape stri2 = createShape(tri2, 3, GL_TRIANGLES, 0.6f, 0.6f, 0.6f);

    Shape circl1 = createCircleGradient(0.063f, 0.185f, 0.035f, 64, 0.97f, 0.74f, 0.74f, 0.4f, 0.4f, 0.4f);
    Shape circl2 = createCircle(0.07f, 0.2f, 0.07f, 64, 0.6f, 0.6f, 0.6f);

    Shape circl4 = createCircleGradient(-0.185f, 0.0f, 0.035f, 64, 0.97f, 0.74f, 0.74f, 0.4f, 0.4f, 0.4f);
    Shape circl5 = createCircle(-0.2f, 0.0f, 0.07f, 64, 0.6f, 0.6f, 0.6f);

    Shape circl3 = createCircle(0.3f, -0.22f, 0.01f, 64, 0.0f, 0.0f, 0.0f);
    Shape line1 = createShape(lines1, 4, GL_TRIANGLE_STRIP, 0.0f, 0.0f, 0.0f);
    Shape line2 = createShape(lines2, 4, GL_TRIANGLE_STRIP, 0.0f, 0.0f, 0.0f);

    Shape circl6 = createCircle(0.0f, 0.0f, 0.009f, 64, 0.0f, 0.0f, 0.0f);
    Shape circl7 = createCircle(0.08f, 0.05f, 0.009f, 64, 0.0f, 0.0f, 0.0f);

    vector<float> sineVerts;

    const float PI = 3.14159265358979323846f;
    int points = 200;

    for (int i = 0; i < points; ++i) {
        float t = i / (float)(points - 1);
        float angle = PI / 2.0f + t * (3.0f * PI / 2.0f);
        float x = -0.45f + t * 0.4f;
        float y = 0.2667f * sinf(angle) - 0.35f;
        sineVerts.push_back(x);
        sineVerts.push_back(y);
        sineVerts.push_back(0.0f);
    }

    Shape sSine = createShape(sineVerts.data(), (int)sineVerts.size() / 3, GL_LINE_STRIP, 0.0f, 0.0f, 0.0f);

    const int pointsTwoEx = 400;
    float spiralScale = 0.018f;

    float widths[3] = { 0.005f, 0.01f, 0.02f };

    vector<vector<float>> colors = {
        {1.0f, 0.0f, 0.0f, 1.0f},
        {0.0f, 1.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f, 1.0f}
    };

    Shape spirals[3];

    for (int idx = 0; idx < 3; ++idx) {
        float a = idx + 1;

        vector<float> verts;
        float tMax = 6.0f * PI;

        for (int i = 0; i < pointsTwoEx; ++i) {
            float t = tMax * i / (float)(pointsTwoEx - 1);

            float x = a * t * sinf(t);
            float y = t * cosf(t);

            verts.push_back(x * spiralScale);
            verts.push_back(y * spiralScale);
            verts.push_back(0.0f);
        }
        vector<float> thickVerts = makeThickLine(verts, pointsTwoEx, widths[idx]);

        spirals[idx] = createShape(thickVerts.data(),
            (int)thickVerts.size() / 3,
            GL_TRIANGLE_STRIP,
            colors[idx][0], colors[idx][1], colors[idx][2]);
    }

    vector<float> gridVerts;
    float offset = -(pointsTwoEx - 1) * spiralScale / 2.0f;

    for (int i = 0; i < pointsTwoEx; ++i) {
        float y = offset + i * spiralScale;

        gridVerts.push_back(offset);
        gridVerts.push_back(y);
        gridVerts.push_back(0.0f);

        gridVerts.push_back(offset + (pointsTwoEx - 1) * spiralScale);
        gridVerts.push_back(y);
        gridVerts.push_back(0.0f);
    }

    for (int j = 0; j < pointsTwoEx; ++j) {
        float x = offset + j * spiralScale;

        gridVerts.push_back(x);
        gridVerts.push_back(offset);
        gridVerts.push_back(0.0f);

        gridVerts.push_back(x);
        gridVerts.push_back(offset + (pointsTwoEx - 1) * spiralScale);
        gridVerts.push_back(0.0f);
    }

    Shape grid = createShape(gridVerts.data(),
        (int)gridVerts.size() / 3,
        GL_LINES,
        0.5f, 0.5f, 0.5f);

    vector<float> sierpinskiVerts;

    float vx[3] = { 0.0f,  0.8f, -0.8f };
    float vy[3] = { 0.8f, -0.6f, -0.6f };

    float mx = 0.0f;
    float my = 0.0f;

    int iterations = 500000;

    for (int i = 0; i < iterations; ++i) {
        int v = rand() % 3;

        mx = (mx + vx[v]) * 0.5f;
        my = (my + vy[v]) * 0.5f;

        if (i > 20) {
            sierpinskiVerts.push_back(mx);
            sierpinskiVerts.push_back(my);
            sierpinskiVerts.push_back(0.0f);
        }
    }

    Shape sierpinski = createShape(sierpinskiVerts.data(),
        (int)sierpinskiVerts.size() / 3,
        GL_POINTS,
        1.0f, 1.0f, 1.0f);

    cout << "stri1.VAO = " << stri1.VAO << "\n";
    cout << "stri1.vertexCount = " << stri1.vertexCount << "\n";
    cout << "AFTER CREATE SHAPE" << "\n";

    while (!glfwWindowShouldClose(myWin)) {
        glClearColor(0.95f, 0.95f, 0.95f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (scene == 0) {
            glUseProgram(programUniform);

            glUniform4f(colorLoc, stri1.r, stri1.g, stri1.b, 1.0);
            glBindVertexArray(stri1.VAO);
            glDrawArrays(stri1.mode, 0, stri1.vertexCount);

            angle += angleSpeed;
            if (angle > maxAngle) {
                angle = maxAngle;
                angleSpeed = -angleSpeed;
            }
            if (angle < -maxAngle) {
                angle = -maxAngle;
                angleSpeed = -angleSpeed;
            }
            glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 0, 1));

            glUseProgram(programUniform);
            glUniformMatrix4fv(rotLoc, 1, GL_FALSE, glm::value_ptr(rotation));

            glUniform4f(colorLoc, stri2.r, stri2.g, stri2.b, 1.0f);
            glBindVertexArray(stri2.VAO);
            glDrawArrays(stri2.mode, 0, stri2.vertexCount);

            glUniform4f(colorLoc, circl2.r, circl2.g, circl2.b, 1.0);
            glBindVertexArray(circl2.VAO);
            glDrawArrays(circl2.mode, 0, circl2.vertexCount);

            glUniform4f(colorLoc, circl1.r, circl1.g, circl1.b, 1.0);
            glBindVertexArray(circl1.VAO);
            glDrawArrays(circl1.mode, 0, circl1.vertexCount);

            glUniform4f(colorLoc, circl5.r, circl5.g, circl5.b, 1.0);
            glBindVertexArray(circl5.VAO);
            glDrawArrays(circl5.mode, 0, circl5.vertexCount);

            glUniform4f(colorLoc, circl6.r, circl6.g, circl6.b, 1.0);
            glBindVertexArray(circl6.VAO);
            glDrawArrays(circl6.mode, 0, circl6.vertexCount);

            glUniform4f(colorLoc, circl7.r, circl7.g, circl7.b, 1.0);
            glBindVertexArray(circl7.VAO);
            glDrawArrays(circl7.mode, 0, circl7.vertexCount);

            glUniform4f(colorLoc, circl3.r, circl3.g, circl3.b, 1.0);
            glBindVertexArray(circl3.VAO);
            glDrawArrays(circl3.mode, 0, circl3.vertexCount);

            glUniform4f(colorLoc, line1.r, line1.g, line1.b, 1.0);
            glBindVertexArray(line1.VAO);
            glDrawArrays(line1.mode, 0, line1.vertexCount);

            glUniform4f(colorLoc, line2.r, line2.g, line2.b, 1.0);
            glBindVertexArray(line2.VAO);
            glDrawArrays(line2.mode, 0, line2.vertexCount);

            glm::mat4 identity = glm::mat4(1.0f);
            glUniformMatrix4fv(rotLoc, 1, GL_FALSE, glm::value_ptr(identity));

            glm::mat4 rotationS = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));
            glUniformMatrix4fv(rotLoc, 1, GL_FALSE, glm::value_ptr(rotationS));

            glUniform4f(colorLoc, sSine.r, sSine.g, sSine.b, 1.0f);
            glBindVertexArray(sSine.VAO);
            glDrawArrays(sSine.mode, 0, sSine.vertexCount);

            glm::mat4 identityS = glm::mat4(1.0f);
            glUniformMatrix4fv(rotLoc, 1, GL_FALSE, glm::value_ptr(identity));

            glUseProgram(programGradient);

            glUniformMatrix4fv(rotLocGrad, 1, GL_FALSE, glm::value_ptr(rotation));
            glBindVertexArray(circl4.VAO);
            glDrawArrays(circl4.mode, 0, circl4.vertexCount);

            glBindVertexArray(circl1.VAO);
            glDrawArrays(circl1.mode, 0, circl1.vertexCount);
        }
        else if (scene == 1) {
            glUseProgram(programUniform);

            glUniform4f(colorLoc, grid.r, grid.g, grid.b, 1.0f);
            glBindVertexArray(grid.VAO);
            glDrawArrays(grid.mode, 0, grid.vertexCount);

            for (int i = 0; i < 3; ++i) {
                glUniform4f(colorLoc, spirals[i].r, spirals[i].g, spirals[i].b, 1.0f);
                glBindVertexArray(spirals[i].VAO);
                glDrawArrays(spirals[i].mode, 0, spirals[i].vertexCount);
            }
        }
        else if (scene == 2) {
            glUseProgram(programUniform);

            glClearColor(0.627f, 0.282f, 0.275f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glUniform4f(colorLoc, sierpinski.r, sierpinski.g, sierpinski.b, 1.0f);
            glBindVertexArray(sierpinski.VAO);
            glDrawArrays(sierpinski.mode, 0, sierpinski.vertexCount);
        }

        glfwSwapBuffers(myWin);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
