#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "imgui-SFML.h"
#include "imgui.h"

#include "MonteCarlo.h"

#include <cmath>
#include <iostream>
#include <random>

using namespace std;

template <typename T>
void printVector2(sf::Vector2<T> vec2, std::string &&text = "") {
    cout << text << vec2.x << ", " << vec2.y << endl;
}

bool inCircle(sf::Vector2f pos) {
    return pos.x * pos.x + pos.y * pos.y <= 1;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Monte Carlo Simulation");
    ImGui::SFML::Init(window);

    sf::Vector2<sf::Vector2f> boundaries = {{-1, 1}, {-1, 1}};
    std::function<bool(sf::Vector2f)> figureFunc = [](sf::Vector2f p) { return p.x * p.x + p.y * p.y <= 1; };
    int dotsNum = 100000;
    //створюємо графік для заданих границь, функціїї та точності методу
    Plot plot(boundaries, figureFunc, dotsNum);

    //матриця трансформації у координату екрану
    sf::Transform toWindowCoords = plot.toWindowCoordsTrans(window.getSize());

    sf::Clock deltaClock;
    //основний цикл відображення графіки
    while (window.isOpen()) {

        sf::Event event;
        //перевірка подій
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
                window.close();
        }
        ImGui::SFML::Update(window, deltaClock.restart());

        bool figureChanged = false;
        if (makeExamplesGUI(figureFunc, boundaries) || makeMonteKarloGUI(plot.m_figure.m_area, boundaries, dotsNum))
            figureChanged = true;

        if (figureChanged) {
            //якщо якісь параметри змінились, оновлюємо графік
            plot = Plot(boundaries, figureFunc, dotsNum);
            toWindowCoords = plot.toWindowCoordsTrans(window.getSize());
        }

        //малюємо по-черзі координатну сітку, прямокутник для визначення площі, випадкові точки та інтерфейс
        window.clear();

        window.draw(plot.m_grid, toWindowCoords);
        window.draw(plot.m_quad, toWindowCoords);
        window.draw(plot.m_figure.m_dots, toWindowCoords);
        ImGui::SFML::Render(window);

        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}