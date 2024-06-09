#include "imgui.h"
#include "math.h"

class GUI {
private:
    float speedX, speedY, speedZ;
    float rotX, rotY, rotZ;
public:
    bool shouldClose = false;
    float getX() { return rotX; }
    float getY() { return rotY; }
    float getZ() { return rotZ; }
    void ShowWindow()
    {
        bool* p_open = 0;
        IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing Dear ImGui context!");

        IMGUI_CHECKVERSION();
        ImGuiWindowFlags window_flags = 0;

        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(550, 320), ImGuiCond_FirstUseEver);

        ImGui::Begin("Cube Rotation", p_open, window_flags);


        ImGui::SliderFloat("X axis", &this->speedX, -60.0f, 60.0f, "rpm = %.3f");
        ImGui::SliderFloat("Y axis", &this->speedY, -60.0f, 60.0f, "rpm = %.3f");
        ImGui::SliderFloat("Z axis", &this->speedZ, -60.0f, 60.0f, "rpm = %.3f");
       
        ImGui::End();
    }
    void addAngle(float deltaTime) {
        rotX = fmodf(rotX + speedX * deltaTime * 6, 360 * 10000000);
        rotY = fmodf(rotY + speedY * deltaTime * 6, 360 * 10000000);
        rotZ = fmodf(rotZ + speedZ * deltaTime * 6, 360 * 10000000); 
    }
};