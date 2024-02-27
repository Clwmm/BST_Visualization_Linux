// Mateusz Ka³wa

// SFML
#include "SFML/Graphics.hpp"

// ImGUI
#include "include/imgui.h"
#include "include/imgui-SFML.h"

// VBT
#include "VisualBinaryTree.h"

// STD
#include <sstream>

// VALIDATING INSERT TEXT
bool validateText(const char* input)
{
    if (*input == '\0')
        return false;
    while (*input) {
        if (!std::isdigit(*input)) {
            return false;
        }
        ++input;
    }
    return true;
}

int main()
{
    // SFML WINDOW
    sf::RenderWindow window(sf::VideoMode(1100, 1100), "Window Title");
    
    // IMGUI WINDOW
    ImGui::SFML::Init(window);
    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = 2.f;

    // SETTINGS
    float viewSize = 150.f;
    float nextViewSize = 0.f;
    sf::Vector2f viewPosition(0.f, 0.f);
    sf::Vector2f nextViewPosition(0.f, 0.f);

    // VIEW SETTING
    sf::View view(viewPosition, sf::Vector2f(viewSize, viewSize));
    window.setView(view);

    // FONT INIT
    sf::Font font;
    if (!font.loadFromFile("res/font.ttf")) {
        std::cerr << "Cannot load font.ttf" << std::endl;
        exit(EXIT_FAILURE);
    }

    // TIME
    sf::Clock clock;
    sf::Clock imguiClock;
    float deltatime = 0.f;

    // VISUAL BINARY TREE INIT
    vbt::VisualBinaryTree<int> vbt(font);
    /*vbt.insert(50);
    vbt.insert(25);
    vbt.insert(75);
    vbt.insert(10);
    vbt.insert(40);
    vbt.insert(60);
    vbt.insert(90);
    vbt.insert(5);
    vbt.insert(15);
    vbt.insert(30);
    vbt.insert(45);
    vbt.insert(55);
    vbt.insert(65);
    vbt.insert(80);
    vbt.insert(95);*/

    // USED TEXT
    std::string alert = "";
    std::string getLineStr;
    std::string word;
    static char inputCharText[3] = "";

    // DELTATIME INIT
    deltatime = clock.restart().asSeconds();

    while (window.isOpen())
    {
        // CALCULATING DELTATIME
        deltatime = clock.restart().asSeconds();

        // HANDLING EVENTS
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::KeyPressed:
                switch (event.key.code)
                {
                case sf::Keyboard::Escape:
                    window.close();
                    break;

                default:
                    break;
                }
                break;
            default:
                break;
            }
        }

        // BUILDING AND UPDATING IMGUI
        ImGui::SFML::Update(window, imguiClock.restart());

        ImGui::Begin("Visual Binary Tree");
        ImGui::InputText("##", inputCharText, IM_ARRAYSIZE(inputCharText));
        ImGui::Columns(4);

        // INSERT COLUMN 1
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.f, 1.f, 0.f, 1.0f));
        if (ImGui::Button("Insert"))
            if (validateText(inputCharText))
                vbt.insert(std::stoi(inputCharText), viewPosition);
        ImGui::PopStyleColor();

        // DELETE COLUMN 2
        ImGui::NextColumn();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        if (ImGui::Button("Delete"))
            if (validateText(inputCharText))
                vbt.remove(std::stoi(inputCharText));
        ImGui::PopStyleColor();

        // SEARCH COLUMN 3
        ImGui::NextColumn();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
        if (ImGui::Button("Search"))
            if (validateText(inputCharText))
                vbt.search(std::stoi(inputCharText));
        ImGui::PopStyleColor();

        // CLEAR COLUMN 4
        ImGui::NextColumn();
        if (ImGui::Button("Clear"))
        {
            vbt.clear();
            alert = "Tree cleared";
        }
        ImGui::Columns(1);
            
        ImGui::Text(alert.c_str());

        std::string temp;
        temp = "Size: " + std::to_string(vbt.getSize());
        ImGui::Text(temp.c_str());
        temp = "Depth: " + std::to_string(vbt.depth());
        ImGui::Text(temp.c_str());
        temp = "Minimum: " + std::to_string(vbt.minimum());
        ImGui::Text(temp.c_str());
        temp = "Maximum: " + std::to_string(vbt.maximum());
        ImGui::Text(temp.c_str());
        ImGui::Text("Inorder:");
        ImGui::Text(vbt.inorder().c_str());
        ImGui::End();


        // UPDATING VISUAL BINARY TREE
        vbt.update(deltatime, alert);

        // VIEW POSITION AND SIZE UPDATING
        if (vbt.getRoot())
        {
            sf::Vector2f dir = vbt.getRoot()->position - viewPosition;
            float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

            if (dist > 1.0f)
            {
                dir /= dist;
                viewPosition.x += dir.x * vbt::MOVE_SPEED * deltatime;
                viewPosition.y += dir.y * vbt::MOVE_SPEED * deltatime;
            }

            view.setCenter(viewPosition);
        }
        nextViewSize = std::max(vbt.getWidth(), vbt.getHeight()) + 100.f;
        viewSize += (nextViewSize - viewSize) * deltatime;
        view.setSize(sf::Vector2f(viewSize, viewSize));
        window.setView(view);

        // DRAWING
        window.clear(sf::Color(18, 33, 43));
        vbt.draw(window);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
