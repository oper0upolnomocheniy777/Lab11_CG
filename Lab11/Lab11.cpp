#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;     // позиция вершины
    layout (location = 1) in vec3 aColor;   // цвет вершины (НОВОЕ!)
    
    out vec3 vertexColor;  // передаем цвет во фрагментный шейдер
    
    void main() {
        gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
        vertexColor = aColor;  // передаем цвет вершины
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 vertexColor;      // получаем интерполированный цвет
    out vec4 FragColor;
    
    void main() {
        FragColor = vec4(vertexColor, 1.0);  // ГРАДИЕНТНОЕ ЗАКРАШИВАНИЕ
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

// Структура для хранения вершин с позицией и цветом
struct Vertex {
    float x, y;       // позиция
    float r, g, b;    // цвет
};

// четырехугольник с разными цветами для вершин
Vertex* createQuadVertices(int& vertexCount) {
    static Vertex vertices[] = {
        // левый верхний треугольник
        {-0.5f,  0.5f,   1.0f, 0.0f, 0.0f},  // красный
        {-0.5f, -0.5f,   0.0f, 1.0f, 0.0f},  // зеленый
        { 0.5f, -0.5f,   0.0f, 0.0f, 1.0f},  // синий

        // правый верхний треугольник
        {-0.5f,  0.5f,   1.0f, 0.0f, 0.0f},  // красный
        { 0.5f, -0.5f,   0.0f, 0.0f, 1.0f},  // синий
        { 0.5f,  0.5f,   1.0f, 1.0f, 0.0f}   // желтый
    };
    vertexCount = 6;
    return vertices;
}

// веер с цветовым градиентом
Vertex* createFanVertices(int& vertexCount) {
    static Vertex vertices[8 * 3];
    float centerX = 0.0f;
    float centerY = 0.0f;
    float radius = 0.7f;
    int triangles = 8;

    for (int i = 0; i < triangles; i++) {
        float angle1 = 3.14159f * 2.0f * i / triangles;
        float angle2 = 3.14159f * 2.0f * (i + 1) / triangles;

        // Центральная вершина - белый
        vertices[i * 3].x = centerX;
        vertices[i * 3].y = centerY;
        vertices[i * 3].r = 1.0f;
        vertices[i * 3].g = 1.0f;
        vertices[i * 3].b = 1.0f;

        // Первая точка на окружности - меняется по цвету
        vertices[i * 3 + 1].x = centerX + radius * cos(angle1);
        vertices[i * 3 + 1].y = centerY + radius * sin(angle1);
        vertices[i * 3 + 1].r = (float)i / triangles;           // R меняется
        vertices[i * 3 + 1].g = 1.0f - (float)i / triangles;    // G меняется
        vertices[i * 3 + 1].b = 0.5f;

        // Вторая точка на окружности - меняется по цвету
        vertices[i * 3 + 2].x = centerX + radius * cos(angle2);
        vertices[i * 3 + 2].y = centerY + radius * sin(angle2);
        vertices[i * 3 + 2].r = (float)(i + 1) / triangles;     // R меняется
        vertices[i * 3 + 2].g = 1.0f - (float)(i + 1) / triangles; // G меняется
        vertices[i * 3 + 2].b = 0.5f;
    }
    vertexCount = triangles * 3;
    return vertices;
}

// пятиугольник с цветовым градиентом
Vertex* createPentagonVertices(int& vertexCount) {
    static Vertex vertices[5 * 3];
    float centerX = 0.0f;
    float centerY = 0.0f;
    float radius = 0.5f;
    int triangles = 5;

    for (int i = 0; i < triangles; i++) {
        float angle1 = 3.14159f * 2.0f * i / triangles;
        float angle2 = 3.14159f * 2.0f * (i + 1) / triangles;

        // Центральная вершина - фиолетовый
        vertices[i * 3].x = centerX;
        vertices[i * 3].y = centerY;
        vertices[i * 3].r = 0.8f;
        vertices[i * 3].g = 0.2f;
        vertices[i * 3].b = 0.8f;

        // Первая точка - теплый цвет
        vertices[i * 3 + 1].x = centerX + radius * cos(angle1);
        vertices[i * 3 + 1].y = centerY + radius * sin(angle1);
        vertices[i * 3 + 1].r = 1.0f;
        vertices[i * 3 + 1].g = 0.5f + 0.5f * sin(angle1);
        vertices[i * 3 + 1].b = 0.2f;

        // Вторая точка - холодный цвет
        vertices[i * 3 + 2].x = centerX + radius * cos(angle2);
        vertices[i * 3 + 2].y = centerY + radius * sin(angle2);
        vertices[i * 3 + 2].r = 0.2f;
        vertices[i * 3 + 2].g = 0.7f;
        vertices[i * 3 + 2].b = 0.9f + 0.1f * cos(angle2);
    }
    vertexCount = triangles * 3;
    return vertices;
}

void drawShape(Vertex* vertices, int vertexCount) {
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), vertices, GL_STATIC_DRAW);

    // Устанавливаем атрибуты:
    // 0 - позиция (2 float)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 1 - цвет (3 float, смещение на 2 float от начала)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

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

    GLFWwindow* window = glfwCreateWindow(800, 600, "Three Shapes - Gradient Shading", NULL, NULL);
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
        "QUADRILATERAL - Gradient: Red/Green/Blue/Yellow",
        "FAN - Gradient: White to Color Wheel",
        "PENTAGON - Gradient: Purple/Warm/Cool"
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
        Vertex* vertices = nullptr;

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
            drawShape(vertices, vertexCount);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}