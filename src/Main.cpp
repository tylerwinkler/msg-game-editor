#include <fstream>
#include <map>
#include <sstream>

#include "SFML/Graphics/RenderWindow.hpp"

#include "SFML/Window/Event.hpp"

#include "imgui.h"
#include "imgui-sfml.h"

#include "WindowEnum.hpp"

struct WindowData
{
    int left;
    int top;
    int width;
    int height;
};

bool configExists()
{
    std::ifstream inFile;
    inFile.open("windows.ini");

    return inFile.good();
}

void saveConfig(sf::RenderWindow& main, std::map<int, sf::RenderWindow*>& windows)
{
    std::ofstream outFile;
    outFile.open("windows.ini");

    outFile << main.getPosition().x << ' ' << main.getPosition().y
        << ' ' << main.getSize().x << ' ' << main.getSize().y << "\n";

    for (auto it = windows.begin(); it != windows.end(); ++it)
    {
        sf::RenderWindow* window = it->second;
        outFile << window->getPosition().x << ' ' << window->getPosition().y
        << ' ' << window->getSize().x << ' ' << window->getSize().y << "\n";
    }

    outFile.close();
}

int strToInt(const std::string& str)
{
    std::stringstream ss(str);

    int output;
    ss >> output;

    return output;
}

std::vector<WindowData> loadConfig()
{
    std::vector<WindowData> data;

    std::ifstream inFile;
    inFile.open("windows.ini");

    while (inFile.good())
    {
        std::string line;
        std::getline(inFile, line);

        std::stringstream ss(line);
        WindowData windowData;

        std::string token;
        std::getline(ss, token, ' ');
        windowData.left = strToInt(token);

        std::getline(ss, token, ' ');
        windowData.top = strToInt(token);

        std::getline(ss, token, ' ');
        windowData.width = strToInt(token);

        std::getline(ss, token, ' ');
        windowData.height = strToInt(token);

        data.push_back(windowData);
    }

    inFile.close();

    return data;
}

int main(int argc, char* argv[])
{
    sf::RenderWindow mainWindow;
    sf::RenderWindow animationEditorWindow;
    sf::RenderWindow tilemapEditorWindow;

    if (configExists())
    {
        std::vector<WindowData> data = loadConfig();

        mainWindow.create(sf::VideoMode(data[0].width, data[0].height, 32), "msg-game-editor");
        mainWindow.setPosition(sf::Vector2i(data[0].left, data[0].top));

        animationEditorWindow.create(sf::VideoMode(data[1].width, data[1].height, 32), "Animation Editor");
        animationEditorWindow.setPosition(sf::Vector2i(data[1].left, data[1].top));

        tilemapEditorWindow.create(sf::VideoMode(data[2].width, data[2].height, 32), "Tilemap Editor");
        tilemapEditorWindow.setPosition(sf::Vector2i(data[2].left, data[2].top));
    }
    else
    {
        mainWindow.create(sf::VideoMode(800, 600, 32), "msg-game-editor");
        animationEditorWindow.create(sf::VideoMode(640, 480, 32), "Animation Editor");
        tilemapEditorWindow.create(sf::VideoMode(640, 480, 32), "Tilemap Editor");
    }

    animationEditorWindow.setVisible(false);
    tilemapEditorWindow.setVisible(false);

    std::map<int, sf::RenderWindow*> secondaryWindows;
    secondaryWindows.insert(std::make_pair(WINDOW_ANIM_EDITOR, &animationEditorWindow));
    secondaryWindows.insert(std::make_pair(WINDOW_TILEMAP_EDITOR, &tilemapEditorWindow));

    sf::Clock imguiClock;
    ImGui::SFML::Init(mainWindow);

    bool running = true;
    while (running)
    {
        sf::Event event;
        while (mainWindow.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            switch (event.type)
            {
            case sf::Event::Closed:
                running = false;
                break;
            default:
                break;
            }
        }

        mainWindow.clear();

        ImGui::SFML::Update(mainWindow, imguiClock.restart());

        ImGui::Begin("Toolbar");
        if (ImGui::Button("Animation Editor"))
        {
            secondaryWindows.at(WINDOW_ANIM_EDITOR)->setVisible(true);
        }
        else if (ImGui::Button("Tilemap Editor"))
        {
            secondaryWindows.at(WINDOW_TILEMAP_EDITOR)->setVisible(true);
        }
        else if (ImGui::Button("Export"))
        {

        }
        ImGui::End();

        ImGui::SFML::Render(mainWindow);

        mainWindow.display();

        for (auto it = secondaryWindows.begin(); it != secondaryWindows.end(); ++it)
        {
            sf::RenderWindow* window = it->second;
            sf::Event event;
            while (window->pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    window->setVisible(false);
                }
            }
            window->clear();
            window->display();
        }
    }

    saveConfig(mainWindow, secondaryWindows);

    return 0;
}
