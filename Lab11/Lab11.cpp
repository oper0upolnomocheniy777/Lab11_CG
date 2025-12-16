#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    void main() {
        gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    uniform vec4 shapeColor; // uniform-переменная для цвета (плоское закрашивание)
    void main() {
        FragColor = shapeColor; // все пиксели фигуры получат одинаковый цвет
    }
)";

unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "Shader compilation error:\n" << infoLog << std::endl;
        return 0;
    }
    return shader;
}

unsigned int createShaderProgram() {
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    if (!vertexShader || !fragmentShader) {
        return 0;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Program linking error:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

// четырехугольник
float* createQuadVertices(int& vertexCount) {
    static float vertices[] = {
        -0.5f,  0.5f,  // левый верхний
        -0.5f, -0.5f,  // левый нижний
         0.5f, -0.5f,  // правый нижний

         -0.5f,  0.5f,  // левый верхний
          0.5f, -0.5f,  // правый нижний
          0.5f,  0.5f   // правый верхний
    };
    vertexCount = 6;
    return vertices;
}

// веер
float* createFanVertices(int& vertexCount) {
    static float vertices[8 * 3 * 2];
    float centerX = 0.0f;
    float centerY = 0.0f;
    float radius = 0.7f;
    int triangles = 8;

    for (int i = 0; i < triangles; i++) {
        float angle1 = 3.14159f * 2.0f * i / triangles;
        float angle2 = 3.14159f * 2.0f * (i + 1) / triangles;

        // Центральная вершина 
        vertices[i * 6] = centerX;
        vertices[i * 6 + 1] = centerY;

        // Первая точка на окружности
        vertices[i * 6 + 2] = centerX + radius * cos(angle1);
        vertices[i * 6 + 3] = centerY + radius * sin(angle1);

        // Вторая точка на окружности
        vertices[i * 6 + 4] = centerX + radius * cos(angle2);
        vertices[i * 6 + 5] = centerY + radius * sin(angle2);
    }
    vertexCount = triangles * 3;
    return vertices;
}

// пятиугольник
float* createPentagonVertices(int& vertexCount) {
    static float vertices[5 * 3 * 2];
    float centerX = 0.0f;
    float centerY = 0.0f;
    float radius = 0.5f;
    int triangles = 5;

    for (int i = 0; i < triangles; i++) {
        float angle1 = 3.14159f * 2.0f * i / triangles;
        float angle2 = 3.14159f * 2.0f * (i + 1) / triangles;

        // Центральная 
        vertices[i * 6] = centerX;
        vertices[i * 6 + 1] = centerY;

        // Первая точка
        vertices[i * 6 + 2] = centerX + radius * cos(angle1);
        vertices[i * 6 + 3] = centerY + radius * sin(angle1);

        // Вторая точка
        vertices[i * 6 + 4] = centerX + radius * cos(angle2);
        vertices[i * 6 + 5] = centerY + radius * sin(angle2);
    }
    vertexCount = triangles * 3;
    return vertices;
}

void drawShape(float* vertices, int vertexCount, unsigned int shaderProgram, float r, float g, float b) {
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 2 * sizeof(float), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Получаем location uniform-переменной для цвета
    int colorLocation = glGetUniformLocation(shaderProgram, "shapeColor");
    // Устанавливаем цвет для плоского закрашивания
    glUniform4f(colorLocation, r, g, b, 1.0f); // alpha = 1.0

    glDrawArrays(GL_TRIANGLES, 0, vertexCount);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

int main() {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Three Shapes - Flat Shading with Uniforms", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    unsigned int shaderProgram = createShaderProgram();
    if (!shaderProgram) {
        return -1;
    }

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    int shapeType = 0;
    float lastTime = glfwGetTime();

    const char* shapeNames[] = {
        "QUADRILATERAL (2 triangles) - Flat shading: BLUE",
        "FAN (8 triangles) - Flat shading: GREEN",
        "PENTAGON (5 triangles) - Flat shading: RED"
    };

    // Цвета для плоского закрашивания каждой фигуры
    // Каждая фигура имеет свой равномерный цвет
    float shapeColors[3][3] = {
        {0.2f, 0.4f, 0.9f},  // Синий для четырехугольника
        {0.3f, 0.8f, 0.4f},  // Зеленый для веера
        {0.9f, 0.3f, 0.3f}   // Красный для пятиугольника
    };

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        float currentTime = glfwGetTime();
        if (currentTime - lastTime > 3.0f) {
            shapeType = (shapeType + 1) % 3;
            lastTime = currentTime;
            std::cout << "Switching to: " << shapeNames[shapeType] << std::endl;
        }

        glUseProgram(shaderProgram);

        int vertexCount;
        float* vertices = nullptr;

        switch (shapeType) {
        case 0:
            vertices = createQuadVertices(vertexCount);
            break;
        case 1:
            vertices = createFanVertices(vertexCount);
            break;
        case 2:
            vertices = createPentagonVertices(vertexCount);
            break;
        }

        if (vertices != nullptr) {
            // Передаем цвет для текущей фигуры через uniform
            drawShape(vertices, vertexCount, shaderProgram,
                shapeColors[shapeType][0],
                shapeColors[shapeType][1],
                shapeColors[shapeType][2]);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}