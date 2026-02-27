#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <stdlib.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <array>
#include "client.hpp"
#include "message.hpp"


struct VitezaContext {
    int* v;
    int cl;
};


int strada_index = 0;
const char* strazi[20] = {
    "Str. Palat",
    "Str. Cuza Vodă",
    "Str. Ștefan cel Mare",
    "Str. Anastasie Panu",
    "Str. Sf. Lazar",
    "Str. Vasile Lupu",
    "Str. Păcurari",
    "Str. Copou",
    "Str. Independenței",
    "Str. Tătărași",
    "Str. Moara de Vânt",
    "Str. Sf. Petru Movilă",
    "Str. Canta",
    "Str. Nicolina",
    "Str. Horea",
    "Str. Arcu",
    "Str. Palatul Culturii",
    "Str. Tudor Vladimirescu",
    "Str. Mitropoliei",
    "Str. Alexandru Lăpușneanu"
};

int viteza = 0;

class App {
private:
    unsigned int width = 0;
    unsigned int height = 0;

    GLFWwindow* glfwWindow = nullptr;
    bool abonare_sport = false;
    bool abonare_vreme = false;
    bool abonare_combustibil = false;

    Client cl;
    pthread_t id_read, id_viteza;

    char chat_buf[256] = "";


    enum class InputType { NONE, VITEZA, ACCIDENT, CHAT } current_input = InputType::NONE;
public:
    ~App() {
        pthread_cancel(id_read);
        pthread_join(id_read, NULL);
        pthread_cancel(id_viteza);
        pthread_join(id_viteza, NULL);
    }
    auto bind(const char* ip, const char* port) {
        return cl.bind_connection(ip, atoi(port));
    }
    void create(unsigned int w, unsigned int h, const std::string title) {
        width = w;
        height = h;

        if (!glfwInit()) {
            std::cerr << "Failed to init GLFW\n";
            return;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        glfwWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

        if (!glfwWindow) {
            std::cerr << "Failed to create window\n";
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(glfwWindow);
        glfwSwapInterval(1);

        glewExperimental = GL_TRUE;

        glfwMakeContextCurrent(glfwWindow);

        GLenum err = glewInit();
        if (err != GLEW_OK) {
            std::cerr << "Failed to init GLEW: " << glewGetErrorString(err) << "\n";
            return;
        }

        glViewport(0, 0, width, height);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Callback pentru resize
        glfwSetWindowUserPointer(glfwWindow, this);
        glfwSetFramebufferSizeCallback(glfwWindow, [](GLFWwindow* window, int w, int h) {
            App* win = static_cast<App*>(glfwGetWindowUserPointer(window));
            win->width = w;
            win->height = h;
            glViewport(0, 0, w, h);
            }
        );

        // Initializare ImGui

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF("/usr/share/fonts/noto/NotoSansMono-Bold.ttf", 20.0f);
        ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        {
            ImVec4* colors = ImGui::GetStyle().Colors;
            colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
            colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
            colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
            colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
            colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
            colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
            colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
            colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
            colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
            colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
            colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
            colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
            colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
            colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
            colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
            colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
            colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
            colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
            colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
            colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
            colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
            colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
            colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
            colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
            colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
            colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
            colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
            colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
            colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
            colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
            colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
            colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
            colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
            colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
            colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
            //colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
            //colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
            colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
            colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
            colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
            colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
            colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
            colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
            colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
            colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

            ImGuiStyle& style = ImGui::GetStyle();
            style.WindowPadding = ImVec2(8.00f, 8.00f);
            style.FramePadding = ImVec2(6.00f, 3.00f);
            style.CellPadding = ImVec2(6.00f, 6.00f);
            style.ItemSpacing = ImVec2(6.00f, 6.00f);
            style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
            style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
            style.IndentSpacing = 25;
            style.ScrollbarSize = 15;
            style.GrabMinSize = 10;
            style.WindowBorderSize = 1;
            style.ChildBorderSize = 1;
            style.PopupBorderSize = 1;
            style.FrameBorderSize = 1;
            style.TabBorderSize = 1;
            style.WindowRounding = 5.0f;
            style.ChildRounding = 0;
            style.FrameRounding = 10.0f;
            style.PopupRounding = 0;
            style.ScrollbarRounding = 9;
            style.GrabRounding = 0;
            style.LogSliderDeadzone = 4;
            style.TabRounding = 0;
        }
        int32_t* client_fd1 = new int32_t(cl.cl);
        pthread_create(&id_read, NULL, &App::thread_function_read, (void*)client_fd1);
        int32_t* client_fd2 = new int32_t(cl.cl);

        VitezaContext* vc = new VitezaContext;
        vc->v = &viteza;
        vc->cl = cl.cl;
        pthread_create(&id_viteza, NULL, &App::thread_function_viteza, (void*)vc);
    }

    static void* thread_function_read(void* arg) {
        int cl = *(int32_t*)arg;
        delete (int32_t*)arg;
        Client client;
        client.cl = cl;

        while (true) {
            auto r = client.read_full();
            if (r.is_err()) {
                log_error(r.unwrap_err().c_str());
                break;
            }
            log_message("%s", r.unwrap().c_str());
        }
        return nullptr;
    }

    static void* thread_function_viteza(void* arg) {
        VitezaContext vc = *(VitezaContext*)arg;
        delete (VitezaContext*)arg;
        Client client;
        client.cl = vc.cl;
        while (true) {
            auto r1 = client.write_full(Message::Strada(strazi[strada_index]));
            if (r1.is_err()) {
                log_error(r1.unwrap_err().c_str());
                break;
            }
            auto r2 = client.write_full(Message::Viteza(std::to_string(*vc.v)));
            if (r2.is_err()) {
                log_error(r2.unwrap_err().c_str());
                break;
            }
            sleep(10);
        }
        return nullptr;
    }


    void ShowCommandsUI3() {
        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse;

        ImGui::Begin("##Main", nullptr, flags);
        ImGui::SetWindowPos(ImVec2(0, 0));
        ImGui::SetWindowSize(ImVec2(width, height));

        ImVec2 avail = ImGui::GetContentRegionAvail();
        float left_w = avail.x * 0.5f;
        float right_w = avail.x * 0.5f;
        ImGuiStyle& style = ImGui::GetStyle();

        // STANGA – CHAT
        ImGui::BeginChild("##CHAT_LEFT", ImVec2(left_w, avail.y), true);

        float input_height = ImGui::GetFrameHeightWithSpacing() + style.ItemSpacing.y;
        ImGui::BeginChild("##CHAT_LOG", ImVec2(-1, -input_height), false);
        //ImGui::TextWrapped("%s", log_buffer.c_str());
        std::istringstream iss(log_buffer);
        std::string line;
        while (std::getline(iss, line)) {
            ImVec4 color = ImVec4(1, 1, 1, 1);

            if (line.find("[ATENTIE]") == 0)
                color = ImVec4(0.90f, 0.20f, 0.20f, 1.0f);
            else if (line.find("[ACCIDENT]") == 0)
                color = ImVec4(0.85f, 0.10f, 0.10f, 1.0f);
            else if (line.find("[TRAFIC]") == 0)
                color = ImVec4(1.00f, 0.85f, 0.20f, 1.0f);
            else if (line.find("[SYSTEM]") == 0)
                color = ImVec4(0.65f, 0.65f, 0.65f, 1.0f);
            else if (line.find("[TU]") == 0)
                color = ImVec4(0.20f, 0.80f, 0.20f, 1.0f);
            else if (line.find("[CHAT]") == 0)
                color = ImVec4(0.30f, 0.55f, 0.90f, 1.0f);
            else if (line.find("[VREME]") == 0)
                color = ImVec4(0.40f, 0.75f, 0.95f, 1.0f);
            else if (line.find("[SPORT]") == 0)
                color = ImVec4(0.95f, 0.55f, 0.15f, 1.0f);
            else if (line.find("[COMBUSTIBIL]") == 0)
                color = ImVec4(0.60f, 0.40f, 0.20f, 1.0f);


            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextWrapped("%s", line.c_str());
            ImGui::PopStyleColor();
        }
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 1.0f)
            ImGui::SetScrollHereY(1.0f);
        ImGui::EndChild();



        float button_width = 80.0f;
        ImGui::PushItemWidth(left_w - button_width - style.ItemSpacing.x - style.WindowPadding.x * 2);
        bool chat_input_enter = ImGui::InputText("##CHAT_INPUT", chat_buf, sizeof(chat_buf), ImGuiInputTextFlags_EnterReturnsTrue);
        if (chat_input_enter) {
            ImGui::SetKeyboardFocusHere(-1);
        }
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0, 0.5, 0.9, 1.0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0, 0.6, 1.0, 1.0));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0, 0.4, 0.8, 1.0));
        if (ImGui::Button("Trimite", ImVec2(button_width, 0)) || chat_input_enter) {
            if (strlen(chat_buf) > 0) {
                auto r1 = cl.write_full(Message::Chat(chat_buf));
                if (r1.is_err()) {
                    log_error(r1.unwrap_err().c_str());
                    return;
                }
                chat_buf[0] = '\0';
            }
        }
        ImGui::PopStyleColor(3);

        ImGui::EndChild();
        ImGui::SameLine();

        // DREAPTA – CONTROALE
        ImGui::BeginChild("##RIGHT_PANEL", ImVec2(right_w, avail.y), true);

        ImVec2 inner = ImGui::GetContentRegionAvail();


        float col_width = (inner.x - style.ItemSpacing.x) / 3.f;
        float viteza_col_width = (inner.x - style.ItemSpacing.x) * 0.3f;
        float strada_col_width = (inner.x - style.ItemSpacing.x) * 0.7f;

        ImGui::BeginGroup();
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Viteza km/h");
        ImGui::SetNextItemWidth(viteza_col_width);
        ImGui::InputInt("##INPUT_VITEZA", &viteza, 0, 0, ImGuiInputTextFlags_CharsDecimal);
        ImGui::EndGroup();

        ImGui::SameLine();

        // Coloana dreapta - Strada
        ImGui::BeginGroup();
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Strada");
        ImGui::SetNextItemWidth(strada_col_width);
        ImGui::Combo("##COMBO_STRAZI", &strada_index, strazi, IM_ARRAYSIZE(strazi));
        ImGui::EndGroup();

        ImGui::Spacing();


        float btn_height = 60.0f;

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0, 0.2, 0.7, 1.0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0, 0.3, 0.8, 1.0));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0, 0.1, 0.6, 1.0));
        if (ImGui::Button("Trimite\n viteza", ImVec2(col_width, btn_height))) {
            auto r1 = cl.write_full(Message::Strada(strazi[strada_index]));
            if (r1.is_err()) {
                log_error(r1.unwrap_err().c_str());
                return;
            }
            auto r2 = cl.write_full(Message::Viteza(std::to_string(viteza)));
            if (r2.is_err()) {
                log_error(r2.unwrap_err().c_str());
                return;
            }
        }
        ImGui::PopStyleColor(3);
        ImGui::SameLine();


        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9, 0.2, 0.0, 1.0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0, 0.3, 0.0, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8, 0.1, 0.0, 1.0f));
        if (ImGui::Button("Raporteaza\n accident", ImVec2(col_width, btn_height))) {
            auto r1 = cl.write_full(Message::Accident(strazi[strada_index]));
            if (r1.is_err()) {
                log_error(r1.unwrap_err().c_str());
                return;
            }
        }
        ImGui::PopStyleColor(3);
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1, 0.7, 0.1, 1.0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2, 0.8, 0.2, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0, 0.6, 0.0, 1.0f));
        if (ImGui::Button("Raporteaza\n  trafic", ImVec2(col_width, btn_height))) {
            auto r1 = cl.write_full(Message::Trafic(strazi[strada_index]));
            if (r1.is_err()) {
                log_error(r1.unwrap_err().c_str());
                return;
            }
        }
        ImGui::PopStyleColor(3);

        ImGui::Spacing();

        // Butoane mari
        float big_btn_height = (inner.y - ImGui::GetCursorPosY() - style.ItemSpacing.y * 2) / 3.0f;

        if (abonare_vreme) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1, 0.7, 0.1, 1.0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2, 0.8, 0.2, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0, 0.6, 0.0, 1.0f));
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9, 0.5, 0.0, 1.0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0, 0.6, 0.0, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8, 0.4, 0.0, 1.0f));
        }
        if (ImGui::Button(abonare_vreme ? "Dezabonare Vreme" : "Abonare Vreme", ImVec2(-1, big_btn_height))) {
            auto r1 = cl.write_full(!abonare_vreme ? Message::AbonareVreme() : Message::DezabonareVreme());
            if (r1.is_err()) {
                log_error(r1.unwrap_err().c_str());
                return;
            }
            else {
                abonare_vreme = !abonare_vreme;
            }
        }
        ImGui::PopStyleColor(3);


        if (abonare_sport) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1, 0.7, 0.1, 1.0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2, 0.8, 0.2, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0, 0.6, 0.0, 1.0f));
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9, 0.5, 0.0, 1.0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0, 0.6, 0.0, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8, 0.4, 0.0, 1.0f));
        }
        if (ImGui::Button(abonare_sport ? "Dezabonare Sport" : "Abonare Sport", ImVec2(-1, big_btn_height))) {
            auto r1 = cl.write_full(!abonare_sport ? Message::AbonareSport() : Message::DezabonareSport());
            if (r1.is_err()) {
                log_error(r1.unwrap_err().c_str());
                return;
            }
            else {
                abonare_sport = !abonare_sport;
            }
        }
        ImGui::PopStyleColor(3);


        if (abonare_combustibil) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1, 0.7, 0.1, 1.0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2, 0.8, 0.2, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0, 0.6, 0.0, 1.0f));
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9, 0.5, 0.0, 1.0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0, 0.6, 0.0, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8, 0.4, 0.0, 1.0f));
        }
        if (ImGui::Button(abonare_combustibil ? "Dezabonare Combustibil" : "Abonare Combustibil", ImVec2(-1, big_btn_height))) {
            auto r1 = cl.write_full(!abonare_combustibil ? Message::AbonareCombustibil() : Message::DezabonareCombustibil());
            if (r1.is_err()) {
                log_error(r1.unwrap_err().c_str());
                return;
            }
            else {
                abonare_combustibil = !abonare_combustibil;
            }
        }
        ImGui::PopStyleColor(3);

        ImGui::EndChild();

        ImGui::End();
    }

    void run() {
        while (glfwWindow && !glfwWindowShouldClose(glfwWindow)) {
            glClearColor(0.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();


            ShowCommandsUI3();


            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(glfwWindow);
            glfwPollEvents();
        }
        cl.close_connection();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(glfwWindow);
        glfwTerminate();
    }
};

