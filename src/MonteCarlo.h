#ifndef IMGUI_SFML_EXAMPLE_MONTECARLO_H
#define IMGUI_SFML_EXAMPLE_MONTECARLO_H

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "imgui-SFML.h"
#include "imgui.h"

struct MonteKarlo {
    sf::VertexArray m_dots;
    unsigned int m_dotsNum;
    float m_area;

    MonteKarlo(const sf::Vector2<sf::Vector2f> &boundaries,
               const std::function<bool(sf::Vector2f)> &isInFigure,
               const unsigned int dotsNum);

    float computeArea(const sf::Vector2<sf::Vector2f> &boundaries,
                      const std::function<bool(sf::Vector2f)> &isInFigure);
};

struct Plot {
    sf::VertexArray m_grid;
    sf::VertexArray m_quad;
    MonteKarlo m_figure;
    sf::Vector2<sf::Vector2f> m_boundaries;

    Plot(const sf::Vector2<sf::Vector2f> &boundaries,
         const std::function<bool(sf::Vector2f)> &isInFigure,
         const unsigned int dotsNum);

    sf::Transform toWindowCoordsTrans(const sf::Vector2u &windowSize);
};

sf::VertexArray makeQuad(sf::Vector2<sf::Vector2f> boundaries);

sf::VertexArray makeGrid(const float maxDist);

sf::VertexArray makeDots(sf::Vector2<sf::Vector2f> boundaries, const int dotsAmount);

bool makeExamplesGUI(std::function<bool(sf::Vector2f)> &figureFunc, sf::Vector2<sf::Vector2f> &boundaries);

bool makeCircleGUI(std::function<bool(sf::Vector2f)> &figureFunc, sf::Vector2<sf::Vector2f> &boundaries);
bool makeQuadGUI(std::function<bool(sf::Vector2f)> &figureFunc, sf::Vector2<sf::Vector2f> &boundaries);
bool makeTriangleGUI(std::function<bool(sf::Vector2f)> &figureFunc, sf::Vector2<sf::Vector2f> &boundaries);
bool makePolynomialGUI(std::function<bool(sf::Vector2f)> &figureFunc);
bool makePolygonGUI(std::function<bool(sf::Vector2f)> &figureFunc);

bool makeMonteKarloGUI(float area, sf::Vector2<sf::Vector2f> &boundaries, int &dotsNum);

#endif //IMGUI_SFML_EXAMPLE_MONTECARLO_H
