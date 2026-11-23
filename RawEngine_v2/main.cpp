#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "core/mesh.h"
#include "core/assimpLoader.h"
#include "core/texture.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

//#define MAC_CLION
#define VSTUDIO

#ifdef MAC_CLION
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#endif

#ifdef VSTUDIO
// Note: install imgui with:
//     ./vcpkg.exe install imgui[glfw-binding,opengl3-binding]
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#endif

int g_width = 800;
int g_height = 600;

float c_SpeedZ = 0.15f;
float c_SpeedX = 0.15f;
float c_SpeedY = 0.15f;

float rotationSpeed = 0.3f;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraDirection = glm::normalize(cameraTarget - cameraPos);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);


void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        // cameraPos.z = cameraPos.z - c_SpeedZ;
        // cameraTarget.z = cameraTarget.z - c_SpeedZ;
        cameraPos = cameraPos + cameraDirection * c_SpeedZ;
        cameraTarget = cameraTarget + cameraDirection * c_SpeedZ;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos = cameraPos - cameraDirection * c_SpeedZ;
        cameraTarget = cameraTarget - cameraDirection * c_SpeedZ;
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPos = cameraPos + cameraRight * c_SpeedX;
        cameraTarget = cameraTarget + cameraRight * c_SpeedX;
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        // cameraPos.x = cameraPos.x + c_SpeedX;
        // cameraTarget.x = cameraTarget.x + c_SpeedX;
        cameraPos = cameraPos - cameraRight * c_SpeedX;
        cameraTarget = cameraTarget - cameraRight * c_SpeedX;
    }
    else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        cameraPos = cameraPos + cameraUp * c_SpeedY;
        cameraTarget = cameraTarget + cameraUp * c_SpeedY;
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        cameraPos = cameraPos - cameraUp * c_SpeedY;
        cameraTarget = cameraTarget - cameraUp * c_SpeedY;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        cameraTarget = cameraTarget - cameraPos;
        cameraTarget = glm::rotate(cameraTarget, glm::radians(1.0f), cameraUp);
        cameraTarget = cameraTarget + cameraPos;

        cameraDirection = glm::normalize(cameraTarget - cameraPos);
        cameraRight = glm::normalize(glm::cross(cameraUp, cameraDirection));
    }
    else if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        cameraTarget = cameraTarget - cameraPos;
        cameraTarget = glm::rotate(cameraTarget, glm::radians(-1.0f), cameraUp);
        cameraTarget = cameraTarget + cameraPos;

        cameraDirection = glm::normalize(cameraTarget - cameraPos);
        cameraRight = glm::normalize(glm::cross(cameraUp, cameraDirection));
    }
}

void framebufferSizeCallback(GLFWwindow *window,
                             int width, int height) {
    g_width = width;
    g_height = height;
    glViewport(0, 0, width, height);
}

std::string readFileToString(const std::string &filePath) {
    std::ifstream fileStream(filePath, std::ios::in);
    if (!fileStream.is_open()) {
        printf("Could not open file: %s\n", filePath.c_str());
        return "";
    }
    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    return buffer.str();
}

GLuint generateShader(const std::string &shaderPath, GLuint shaderType) {
    printf("Loading shader: %s\n", shaderPath.c_str());
    const std::string shaderText = readFileToString(shaderPath);
    const GLuint shader = glCreateShader(shaderType);
    const char *s_str = shaderText.c_str();
    glShaderSource(shader, 1, &s_str, nullptr);
    glCompileShader(shader);
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("Error! Shader issue [%s]: %s\n", shaderPath.c_str(), infoLog);
    }
    return shader;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(g_width, g_height, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    //Setup platforms
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 400");

    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const GLuint modelVertexShader = generateShader("shaders/modelVertex.vs", GL_VERTEX_SHADER);
    const GLuint fragmentShader = generateShader("shaders/fragment.fs", GL_FRAGMENT_SHADER);
    const GLuint textureShader = generateShader("shaders/texture.fs", GL_FRAGMENT_SHADER);
    const GLuint postProcessShader = generateShader("shaders/Post-Processing.fs", GL_FRAGMENT_SHADER);

    int success;
    char infoLog[512];
    const unsigned int modelShaderProgram = glCreateProgram();
    glAttachShader(modelShaderProgram, modelVertexShader);
    glAttachShader(modelShaderProgram, fragmentShader);
    glLinkProgram(modelShaderProgram);
    glGetProgramiv(modelShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(modelShaderProgram, 512, NULL, infoLog);
        printf("Error! Making Shader Program: %s\n", infoLog);
    }
    const unsigned int textureShaderProgram = glCreateProgram();
    glAttachShader(textureShaderProgram, modelVertexShader);
    glAttachShader(textureShaderProgram, textureShader);
    glLinkProgram(textureShaderProgram);
    glGetProgramiv(textureShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(textureShaderProgram, 512, NULL, infoLog);
        printf("Error! Making Shader Program: %s\n", infoLog);
    }
    const unsigned int postProcessingProgram = glCreateProgram();
    glAttachShader(postProcessingProgram, modelVertexShader);
    glAttachShader(postProcessingProgram, postProcessShader);
    glLinkProgram(postProcessingProgram);
    glGetProgramiv(postProcessingProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(postProcessingProgram, 512, NULL, infoLog);
        printf("Error! Making Shader Program: %s\n", infoLog);
    }

    glDeleteShader(modelVertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(textureShader);

    core::Mesh quad = core::Mesh::generateQuad();
    core::Model quadModel({quad});
    quadModel.translate(glm::vec3(0,0,-2.5));
    quadModel.scale(glm::vec3(5, 5, 1));

    core::Model suzanne = core::AssimpLoader::loadModel("models/nonormalmonkey.obj");
    core::Texture Wall_Facade_basecolor("textures/WallWithWindow/Wall_Facade_basecolor.png");
    core::Texture Wall_Facade_normal("textures/WallWithWindow/Wall_Facade_normal.png");

    glm::vec4 clearColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    glClearColor(clearColor.r,
                 clearColor.g, clearColor.b, clearColor.a);


    GLint mvpMatrixUniform = glGetUniformLocation(modelShaderProgram, "mvpMatrix");
    GLint textureModelUniform = glGetUniformLocation(textureShaderProgram, "mvpMatrix");
    GLint postProcessModelUniform = glGetUniformLocation(postProcessingProgram, "mvpMatrix");
    GLint textureUniform = glGetUniformLocation(textureShaderProgram, "text");
    GLint textureNormalUniform = glGetUniformLocation(textureShaderProgram, "textNormal");
    GLint ambientColorUniform = glGetUniformLocation(modelShaderProgram, "ambientColor");
    GLint ambientIntensityUniform = glGetUniformLocation(modelShaderProgram, "ambientIntensity");
    GLint diffuseColorUniform = glGetUniformLocation(modelShaderProgram, "diffuseColor");
    GLint lightDirectionUniform = glGetUniformLocation(modelShaderProgram, "lightDirection");
    GLint modelUniform = glGetUniformLocation(modelShaderProgram, "model");
    GLint c1Uniform = glGetUniformLocation(modelShaderProgram, "c1");
    GLint c2Uniform = glGetUniformLocation(modelShaderProgram, "c2");
    GLint c3Uniform = glGetUniformLocation(modelShaderProgram, "c3");
    GLint shininessUniform = glGetUniformLocation(modelShaderProgram, "shininess");
    GLint camPosUniform = glGetUniformLocation(modelShaderProgram, "camPos");
    GLint specularColorUniform = glGetUniformLocation(modelShaderProgram, "specularColor");

    //We initialize a frame buffer by requesting OpenGL to generate a frame buffer via an ID
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    //o bind a texture to a frame buffer, we must first create a texture
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

    //Apply the texture parameters of choice
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_width, g_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Attach the texture to the Framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    //Using a Framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, g_width, g_height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT |
    GL_DEPTH_BUFFER_BIT);

    //Unbind the Framebuffer after
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    double currentTime = glfwGetTime();
    double finishFrameTime = 0.0;
    float deltaTime = 0.0f;
    float rotationStrength = 100.0f;
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Raw Engine v2");
        ImGui::Text("Hello :) %f %f %f", cameraPos.x, cameraPos.y, cameraPos.z);
        ImGui::End();

        processInput(window);

        //VP
        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(g_width) / static_cast<float>(g_height), 0.1f, 100.0f);

        suzanne.rotate(glm::vec3(1.0f, 1.0f, 0.0f), glm::radians(rotationStrength) * static_cast<float>(deltaTime));
        //glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(modelShaderProgram);
        glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, glm::value_ptr(projection * view * suzanne.getModelMatrix()));
        glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(projection * view * suzanne.getModelMatrix()));
        glUniform3f(ambientColorUniform, 0.24, 0.81, 0.75);
        glUniform1f(ambientIntensityUniform, 0.6);
        glUniform3f(diffuseColorUniform, 1.0, 1.0, 1.0);
        glUniform3f(lightDirectionUniform, 0.0, 2.0, 0.0);
        glUniform1f(c1Uniform, 1);
        glUniform1f(c2Uniform, 0.2);
        glUniform1f(c3Uniform, 0.05);
        glUniform1f(shininessUniform, 128);
        glUniform3f(camPosUniform, cameraPos.x, cameraPos.y, cameraPos.z);
        glUniform3f(specularColorUniform, 1, 1, 1);
        suzanne.render();
        glBindVertexArray(0);

        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glUseProgram(textureShaderProgram);
        glUniformMatrix4fv(textureModelUniform, 1, GL_FALSE, glm::value_ptr(projection * view * quadModel.getModelMatrix()));
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(textureUniform, 0);
        glBindTexture(GL_TEXTURE_2D, Wall_Facade_basecolor.getId());
        glActiveTexture(GL_TEXTURE1);
        glUniform1i(textureNormalUniform, 1);
        glBindTexture(GL_TEXTURE_2D, Wall_Facade_normal.getId());
        quadModel.render();
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        //HELLO :)!!!!

        glfwSwapBuffers(window);
        glfwPollEvents();
        finishFrameTime = glfwGetTime();
        deltaTime = static_cast<float>(finishFrameTime - currentTime);
        currentTime = finishFrameTime;
    }

    glDeleteProgram(modelShaderProgram);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}