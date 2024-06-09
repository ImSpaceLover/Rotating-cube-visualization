#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "ShaderInit.cpp"
#include "GUI.cpp"

bool windowSelected = true;
bool closePressed = false;
float mousePos[] = { 0, 0 };

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_ESCAPE) windowSelected = !windowSelected;
    }
    return;
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    mousePos[0] = xpos;
    mousePos[1] = ypos;
}

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    int width{ 1920 };
    int height{ 1080 };
    float time{ 0 };
    float prevTime{ 0 };
    float deltaTime{ 0 };

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    window = glfwCreateWindow(width, height, "Cube renderer", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);    
    if (glewInit() != GLEW_OK) std::cout << "Error while initializing GLEW" << '\n';

    glfwSetKeyCallback(window, key_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    unsigned int buffer{ 0 };
    unsigned int indexbuffer{ 0 };

    float positions[] = { 
        -1.0f, -1.0f,
        -1.0f, 1.0f,
        1.0f, -1.0f,
        1.0f, 1.0f
    };

    unsigned int indicies[] = {
        0, 1, 2,
        1, 2, 3
    };

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    glGenBuffers(1, &indexbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

    std::string vertexSource = ParseShader("res/shaders/Vertex.glsl");
    std::string fragmentSource = ParseShader("res/shaders/Fragment.glsl");

    unsigned int shader = CreateShader(vertexSource, fragmentSource);
    glUseProgram(shader);

    float mousePosLast[2] = { 0, 0 };

    GUI gui = GUI();
    int timeLoc = glGetUniformLocation(shader, "u_time");
    int sizeLoc = glGetUniformLocation(shader, "u_resolution");
    int mouseLoc = glGetUniformLocation(shader, "u_mouse");
    int angleLoc = glGetUniformLocation(shader, "u_rotation");


    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        time = glfwGetTime();
        deltaTime = time - prevTime;
        prevTime = time;
      
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT);

        if (!windowSelected) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else { 
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            mousePosLast[0] = mousePos[0];
            mousePosLast[1] = mousePos[1];
        }

        gui.addAngle(deltaTime);

        glUniform1f(timeLoc, time);
        glUniform2f(sizeLoc, width, height);
        glUniform2f(mouseLoc, mousePosLast[0], mousePosLast[1]);
        glUniform3f(angleLoc, gui.getX(), gui.getY(), gui.getZ());
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const void*)indexbuffer);

        std::cout << 1/deltaTime << '\n';

        gui.ShowWindow();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}