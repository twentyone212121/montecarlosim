#include "MonteCarlo.h"
#include <random>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

MonteKarlo::MonteKarlo(const sf::Vector2<sf::Vector2f> &boundaries,
           const std::function<bool(sf::Vector2f)> &isInFigure, const unsigned int dotsNum) {
    m_dotsNum = dotsNum;
    m_dots = makeDots(boundaries, m_dotsNum);
    m_area = computeArea(boundaries, isInFigure);
}

float MonteKarlo::computeArea(const sf::Vector2<sf::Vector2f> &boundaries,
                  const std::function<bool(sf::Vector2f)> &isInFigure) {
    size_t in = 0;
    for (int i = 0; i < m_dots.getVertexCount(); ++i) {
        if (isInFigure(m_dots[i].position)) {
            m_dots[i].color = sf::Color::Magenta;
            in++;
        } else {
            m_dots[i].color = sf::Color::White;
        }
    }
    const float rectArea = (boundaries.x.y - boundaries.x.x) * (boundaries.y.y - boundaries.y.x);
    const float area = (float)in / (float)m_dotsNum * rectArea;
    return area;
}

Plot::Plot(const sf::Vector2<sf::Vector2f> &boundaries,
     const std::function<bool(sf::Vector2f)> &isInFigure,
     const unsigned int dotsNum)
        : m_quad(makeQuad(boundaries)),
          m_figure(MonteKarlo(boundaries, isInFigure, dotsNum)),
          m_boundaries(boundaries) {
    float maxDist = ceil(max(abs(boundaries.x.x), max(abs(boundaries.x.y),
                                                      max(abs(boundaries.y.x), abs(boundaries.y.y))))) + 1;
    m_grid = makeGrid(maxDist);
}

sf::Transform Plot::toWindowCoordsTrans(const sf::Vector2u &windowSize) {
    const float maxDist = ceil(max(abs(m_boundaries.x.x), max(abs(m_boundaries.x.y),
                                                              max(abs(m_boundaries.y.x), abs(m_boundaries.y.y))))) + 1;
    const float unitLen = (float) windowSize.x / (2 * maxDist);

    sf::Transform toWindowCoords = sf::Transform::Identity;
    toWindowCoords.translate((sf::Vector2f) windowSize / 2.0f);
    toWindowCoords.scale({unitLen, unitLen});

    return toWindowCoords;
}

sf::VertexArray makeQuad(sf::Vector2<sf::Vector2f> boundaries) {
    sf::VertexArray quad(sf::LineStrip, 5);

    quad[0].position = {boundaries.x.x, boundaries.y.x};
    quad[1].position = {boundaries.x.x, boundaries.y.y};
    quad[2].position = {boundaries.x.y, boundaries.y.y};
    quad[3].position = {boundaries.x.y, boundaries.y.x};
    quad[4].position = {boundaries.x.x, boundaries.y.x};

    for (int i = 0; i < quad.getVertexCount(); ++i) {
        quad[i].color = sf::Color::Red;
        quad[i].position.y *= -1.0f;
    }
    return quad;
}

sf::VertexArray makeGrid(const float maxDist) {
    sf::VertexArray grid(sf::Lines);

    for (float i = -maxDist; i <= maxDist; ++i) {
        grid.append(sf::Vertex({(float)i, -maxDist}));
        grid.append(sf::Vertex({(float)i,  maxDist}));
    }
    for (float i = -maxDist; i <= maxDist; ++i) {
        grid.append(sf::Vertex({-maxDist, (float)i}));
        grid.append(sf::Vertex({ maxDist, (float)i}));
    }

    for (int i = 0; i < grid.getVertexCount(); ++i) {
        if (grid[i].position.x == 0.0f || grid[i].position.y == 0.0f)
            grid[i].color = sf::Color::Yellow;
        else
            grid[i].color = sf::Color::Cyan;
        grid[i].position.y *= -1.0f;
    }
    return grid;
}

sf::VertexArray makeDots(sf::Vector2<sf::Vector2f> boundaries, const int dotsAmount) {
    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<float> xRand(boundaries.x.x, boundaries.x.y);
    uniform_real_distribution<float> yRand(boundaries.y.x, boundaries.y.y);

    sf::VertexArray dots(sf::Points, dotsAmount);
    for (int i = 0; i < dotsAmount; ++i) {
        dots[i].position = {xRand(mt), -yRand(mt)};
    }
    return dots;
}

bool makeExamplesGUI(std::function<bool(sf::Vector2f)> &figureFunc, sf::Vector2<sf::Vector2f> &boundaries) {
    bool changed = false;
    ImGui::Begin("Examples");
    ImGui::SetWindowFontScale(2.0f);

    static const char* items[] = {"Circle", "Quad", "Triangle", "Polynomial", "Polygon"};
    static int selectedItem = 0;

    if (ImGui::Combo("Shapes", &selectedItem, items, IM_ARRAYSIZE(items)))
        changed = true;

    switch (selectedItem) {
        case 0:
            if(makeCircleGUI(figureFunc, boundaries))
                changed = true;
            break;
        case 1:
            if (makeQuadGUI(figureFunc, boundaries))
                changed = true;
            break;
        case 2:
            if (makeTriangleGUI(figureFunc, boundaries))
                changed = true;
            break;
        case 3:
            if (makePolynomialGUI(figureFunc))
                changed = true;
            break;
        case 4:
            if (makePolygonGUI(figureFunc))
                changed = true;
            break;
    }

    ImGui::End();
    return changed;
}

bool makeCircleGUI(std::function<bool(sf::Vector2f)> &figureFunc, sf::Vector2<sf::Vector2f> &boundaries) {
    bool changed = false;
    static float radius = 1.0f;
    ImGui::InputFloat("Radius", &radius);
    if (ImGui::IsItemActive()) {
        changed = true;
        figureFunc = [](sf::Vector2f vec) { return vec.x * vec.x + vec.y * vec.y <= radius * radius; };
        boundaries = {{-radius, radius}, {-radius, radius}};
    }
    float s = M_PI * radius * radius;
    ImGui::Text(string("Area is " + to_string(s)).c_str());
    return changed;
}

bool makeQuadGUI(std::function<bool(sf::Vector2f)> &figureFunc, sf::Vector2<sf::Vector2f> &boundaries) {
    bool changed = false;
    static float a = 0.0f;
    static float b = 0.0f;
    ImGui::InputFloat("a", &a);
    if (ImGui::IsItemActive()) changed = true;
    ImGui::InputFloat("b", &b);
    if (ImGui::IsItemActive()) changed = true;

    if (changed) {
        figureFunc = [](sf::Vector2f vec) { return abs(vec.x) <= a / 2.0f && abs(vec.y) <= b / 2.0f; };
        boundaries = {{-a / 2.0f, a / 2.0f}, {-b / 2.0f, b / 2.0f}};
    }
    float s = a * b;
    ImGui::Text(string("Area is " + to_string(s)).c_str());
    return changed;
}

bool makeTriangleGUI(std::function<bool(sf::Vector2f)> &figureFunc, sf::Vector2<sf::Vector2f> &boundaries) {
    bool changed = false;
    static float a = 0.0f;
    static float b = 0.0f;
    static float c = 0.0f;

    ImGui::InputFloat("a", &a);
    if (ImGui::IsItemActive()) changed = true;
    ImGui::InputFloat("b", &b);
    if (ImGui::IsItemActive()) changed = true;
    ImGui::InputFloat("c", &c);
    if (ImGui::IsItemActive()) changed = true;

    if (changed && a != 0 && b != 0 && c != 0) {
        float cosa = (b * b + c * c - a * a) / (2.0f * b * c);
        sf::Vector2f B{b * cosa, b * sqrt(1 - cosa * cosa)};
        sf::Vector2f C{c, 0};

        figureFunc = [=](sf::Vector2f vec) {
            vec += {C.x / 2, -B.y / 2};
            float m = (vec.x * B.y - B.x * -vec.y) / (C.x * B.y - B.x * C.y);
            float l = (vec.x - m * C.x) / B.x;
            return m >= 0 && m <= 1 && l >= 0 && l <= 1 && m + l <= 1;
        };
        boundaries = {{-max(abs(B.x - C.x / 2.0f), C.x / 2.0f), max(abs(B.x - C.x / 2.0f), C.x / 2.0f)},
                      {-B.y / 2.0f, B.y / 2.0f}};
    }
    float p = (a + b + c) / 2.0f;
    float s = sqrt(p * (p - a) * (p - b) * (p - c));
    ImGui::Text(string("Area is " + to_string(s)).c_str());
    return changed;
}

bool makePolynomialGUI(std::function<bool(sf::Vector2f)> &figureFunc) {
    bool changed = false;
    static int n = 0;
    static vector<float> coefs(1, 0.0f);

    if (ImGui::InputInt("Degree", &n)) {
        if (n < 0) n = 0;
        changed = true;
        coefs = vector<float>(n + 1, 0.0f);
    }

    for (int i = 0; i <= n; ++i) {
        ImGui::InputFloat(string("n" + to_string(i)).c_str(), &coefs[i]);
        if (ImGui::IsItemActive()) changed = true;
    }

    if (changed) {
        figureFunc = [&](sf::Vector2f p) {
            float polynomial = 0.0f;
            for (int i = 0; i <= n; ++i) {
                polynomial += coefs[i] * pow(p.x, n - i);
            }
            return -p.y <= polynomial;
        };
    }
    return changed;
}

bool makePolygonGUI(std::function<bool(sf::Vector2f)> &figureFunc) {
    bool changed = false;
    static vector<sf::Vector2f> points;
    static int n = 0;

    if (ImGui::InputInt("# points", &n)) {
        if (n < 0) n = 0;
        changed = true;
        while (n > points.size()) {
            points.push_back({0.0f, 0.0f});
        }
        while (n < points.size() && n >= 0) {
            points.pop_back();
        }
    }

    for (int i = 0; i < n; ++i) {
        ImGui::InputFloat2(string("Vertex " + to_string(i + 1)).c_str(), (float*)&points[i]);
        if (ImGui::IsItemActive()) changed = true;
    }

    if (changed) {
        figureFunc = [&](sf::Vector2f p) {
            bool in = false;
            for (int i = 0; i < n - 1; ++i) {
                sf::Vector2f A = {points[i].x, -points[i].y};
                sf::Vector2f B = {points[i + 1].x, -points[i + 1].y};
                if(p.y < A.y != p.y < B.y && p.x < (B.x-A.x) * (p.y-A.y) / (B.y-A.y) + A.x){
                    in = !in;
                }
            }
            return in;
        };
    }

    return changed;
}

bool makeMonteKarloGUI(float area, sf::Vector2<sf::Vector2f> &boundaries, int &dotsNum) {
    bool changed = false;
    ImGui::Begin("Area");
    ImGui::SetWindowFontScale(2.0f);

    if (ImGui::InputFloat2("[x1; x2]", (float*)&boundaries.x))
        changed = true;
    if (ImGui::InputFloat2("[y1; y2]", (float*)&boundaries.y))
        changed = true;
    if (ImGui::SliderInt("Number of dots", &dotsNum, 0, 1000000))
        changed = true;

    ImGui::Text(std::string("Area = " + to_string(area)).c_str());
    ImGui::End();
    return changed;
}
