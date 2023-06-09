#include "app.h"


int WinMain(int argc, char** argv)
{
    if (!app::font.loadFromFile("resources/CascadiaMonoPLItalic-BoldItalic.otf"))
        throw std::runtime_error("Failed to load font");
    trees::Node::font = app::font;

    sf::ContextSettings settings;
    settings.antialiasingLevel = 2;
    sf::RenderWindow window(
        sf::VideoMode(config::WINDOW_SIZE_X, config::WINDOW_SIZE_Y), 
        "BST Playground", sf::Style::Close, settings
    );

    if (!ImGui::SFML::Init(window))
        throw("Failed to initialize SFML!");

    if (app::logo.loadFromFile("resources/logo.png"))
        window.setIcon(app::logo.getSize().x, app::logo.getSize().y, app::logo.getPixelsPtr());

    ImGuiIO& io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    app::setupGridLines();

    sf::Clock deltaClock;
    while (window.isOpen()) 
    {
        ImGui::SFML::Update(window, deltaClock.restart());

        app::handleWindowEvents(&window);

        window.clear(sf::Color::White);
        if (app::showGrids)
            app::drawGrid(&window);
        app::drawTree(&window);

        if (app::displaySettings)
            app::showSettingsWindow();
        if (app::displayNodeActions)
            app::showNodeActionsWindow();
        if (app::displayNodeInfo)
            app::showNodeInfoWindow();
        if (app::displayInsertNode)
            app::showInsertNodePopup();
        if (app::displayInsertRandNodes)
            app::showInsertRandomNodesPopup();
        if (app::displayCanvasInfo)
            app::showCanvasInfoWindow(&window);

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}
