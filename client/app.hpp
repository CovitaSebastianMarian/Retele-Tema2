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

class App {
private:
    unsigned int width = 0;
    unsigned int height = 0;

    GLFWwindow* glfwWindow = nullptr;
    bool abonare_sport = false;
    bool abonare_vreme = false;
    bool abonare_combustibil = false;

    Client cl;

    char input_text[100];
    enum class InputType { NONE, VITEZA, ACCIDENT, CHAT } current_input = InputType::NONE;
public:
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


    }

    void ShowMessageBox() {
        if (current_input != InputType::NONE) {

            ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration
                | ImGuiWindowFlags_NoMove
                | ImGuiWindowFlags_NoResize
                | ImGuiWindowFlags_NoCollapse;

            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.5f));
            ImGui::Begin("##Ceva", nullptr, flags);
            ImGui::SetWindowPos(ImVec2(0, 0));
            ImGui::SetWindowSize(ImVec2(width, height));

            ImGui::End();
            ImGui::PopStyleColor();



            ImGui::SetNextWindowFocus();
            ImGui::Begin("Message!", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
            int w = 400;
            int h = 200;

            ImGui::SetWindowSize(ImVec2(w, h));
            ImGui::SetWindowPos(ImVec2(width / 2 - w / 2, height / 2 - h / 2));
            switch (current_input) {
            case InputType::ACCIDENT:
                ImGui::TextColored(ImVec4(0.8, 0.0, 0.1, 1.0), "Introdu locatia accidentului:");
                ImGui::InputText("##accident_input", input_text, 100);
                break;

            case InputType::VITEZA:
                ImGui::TextColored(ImVec4(0.8, 0.0, 0.1, 1.0), "Introdu viteza ta:");
                ImGui::InputText("##viteza_input", input_text, 100, ImGuiInputTextFlags_CharsDecimal);
                break;

            case InputType::CHAT:
                ImGui::TextColored(ImVec4(0.8, 0.0, 0.1, 1.0), "Scrie un mesaj:");
                ImGui::InputText("##chat_input", input_text, 100);
                break;
            };


            float padding = 10.0f;
            ImGui::SetCursorPosX(padding);
            ImGui::SetCursorPosY(h - 50 - padding);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.1f, 0.7f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.3f, 0.9f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.9f, 1.0f));
            if (ImGui::Button("Trimite", ImVec2(90, 50))) {
                std::string msg;
                switch (current_input) {
                case InputType::ACCIDENT:
                    msg = Message::Accident(input_text);
                    break;

                case InputType::VITEZA:
                    msg = Message::Viteza(input_text);
                    break;

                case InputType::CHAT:
                    msg = Message::Chat(input_text);
                    break;
                };
                current_input = InputType::NONE;
                auto r1 = cl.write_full(msg);
                if (r1.is_err()) {
                    log_error(r1.unwrap_err().c_str());
                    return;
                }

                auto r2 = cl.read_full();
                if (r2.is_err()) {
                    log_error(r2.unwrap_err().c_str());
                    return;
                }
                log_message("Mesaj primit: %s\n", r2.unwrap().c_str());
            }
            ImGui::PopStyleColor(3);

            ImGui::SameLine();

            float buttonWidth = 90.0f;
            ImGui::SetCursorPosX(w - buttonWidth - padding);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.1f, 0.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
            if (ImGui::Button("Cancel", ImVec2(buttonWidth, 50))) {
                current_input = InputType::NONE;
            }
            ImGui::PopStyleColor(3);

            ImGui::End();
        }
    }

    std::string last_cmd;

    void ShowCommandsUI() {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoCollapse;

        ImGui::Begin("##Main", nullptr, flags);
        ImGui::SetWindowPos(ImVec2(0, 0));
        ImGui::SetWindowSize(ImVec2(width, height));

        ImVec2 avail = ImGui::GetContentRegionAvail();

        float text_h = avail.y * 0.70f;
        ImGui::BeginChild("##TEXT_ZONE", ImVec2(avail.x, text_h), true);
        ImGui::TextWrapped("%s", log_buffer.c_str());

        float scrollY = ImGui::GetScrollY();
        float scrollMaxY = ImGui::GetScrollMaxY();
        if (scrollY >= scrollMaxY - 1.0f) {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::EndChild();

        ImGui::Spacing();


        ImVec2 avail2 = ImGui::GetContentRegionAvail();
        ImVec2 size = avail2;

        const int cols = 2;
        const int rows = 3;

        ImGuiStyle& style = ImGui::GetStyle();
        float w = (size.x - (cols - 1) * style.ItemSpacing.x) / cols;
        float h = (size.y - (rows - 1) * style.ItemSpacing.y) / rows;

        auto Btn = [&](const char* label, const ImVec4& c, auto func) {
            ImGui::PushStyleColor(ImGuiCol_Button, c);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(c.x + 0.1f, c.y + 0.1f, c.z + 0.1f, 1));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(c.x - 0.1f, c.y - 0.1f, c.z - 0.1f, 1));
            if (ImGui::Button(label, ImVec2(w, h))) func();
            ImGui::PopStyleColor(3);

            if (ImGui::IsItemHovered())
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            };

        ImGui::BeginChild("BUTTON_ZONE", size);

        // 1
        Btn("Viteza", { 0.9f,0.2f,0.2f,1.0f }, [&]() {
            current_input = InputType::VITEZA;
            input_text[0] = '\0';
            });
        ImGui::SameLine();
        Btn(abonare_vreme ? "Dezabonare Vreme" : "Abonare Vreme",
            abonare_vreme ? ImVec4(0.3f, 0.7f, 0.3f, 1) : ImVec4(0.2f, 0.4f, 0.9f, 1),
            [&]() {
                abonare_vreme = !abonare_vreme;
                auto r1 = cl.write_full(abonare_vreme ? Message::AbonareVreme() : Message::DezabonareVreme());
                if (r1.is_err()) {
                    log_error(r1.unwrap_err().c_str());
                    return;
                }

                auto r2 = cl.read_full();
                if (r2.is_err()) {
                    log_error(r2.unwrap_err().c_str());
                    return;
                }
                log_message("Mesaj primit: %s\n", r2.unwrap().c_str());
            });

        // 2
        Btn("Accident", { 0.9f,0.4f,0.1f,1.0f }, [&]() {
            current_input = InputType::ACCIDENT;
            input_text[0] = '\0';
            });
        ImGui::SameLine();
        Btn(abonare_sport ? "Dezabonare Sport" : "Abonare Sport",
            abonare_sport ? ImVec4(0.3f, 0.7f, 0.3f, 1) : ImVec4(0.2f, 0.4f, 0.9f, 1),
            [&]() {
                abonare_sport = !abonare_sport;
                last_cmd = abonare_sport ? "ABONARE_SPORT" : "DEZABONARE_SPORT";
                auto r1 = cl.write_full(abonare_sport ? Message::AbonareSport() : Message::DezabonareSport());
                if (r1.is_err()) {
                    log_error(r1.unwrap_err().c_str());
                    return;
                }

                auto r2 = cl.read_full();
                if (r2.is_err()) {
                    log_error(r2.unwrap_err().c_str());
                    return;
                }
                log_message("Mesaj primit: %s\n", r2.unwrap().c_str());
            });

        // 3
        Btn("Chat", { 0.5f,0.5f,0.5f,1.0f }, [&]() {
            current_input = InputType::CHAT;
            input_text[0] = '\0';
            });
        ImGui::SameLine();
        Btn(abonare_combustibil ? "Dezabonare Combustibil" : "Abonare Combustibil",
            abonare_combustibil ? ImVec4(0.3f, 0.7f, 0.3f, 1) : ImVec4(0.2f, 0.4f, 0.9f, 1),
            [&]() {
                abonare_combustibil = !abonare_combustibil;
                last_cmd = abonare_combustibil ? "ABONARE_COMBUSTIBIL" : "DEZABONARE_COMBUSTIBIL";
                auto r1 = cl.write_full(abonare_sport ? Message::AbonareCombustibil() : Message::DezabonareCombustibil());
                if (r1.is_err()) {
                    log_error(r1.unwrap_err().c_str());
                    return;
                }

                auto r2 = cl.read_full();
                if (r2.is_err()) {
                    log_error(r2.unwrap_err().c_str());
                    return;
                }
                log_message("Mesaj primit: %s\n", r2.unwrap().c_str());
            });

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


            ShowCommandsUI();
            ShowMessageBox();


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

