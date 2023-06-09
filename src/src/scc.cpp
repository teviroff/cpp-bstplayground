#include "scc.h"


namespace scc
{
    #pragma region Canvas
    const float Canvas::minWidth = 10.f, Canvas::maxWidth = 60.f, Canvas::wheelStrength = 1.f;

    Canvas::Canvas(const auxillary::vec2& topLeftInit, float widthInit)
        : topLeftInitial(topLeftInit), widthInitial(widthInit), topLeft(topLeftInit), width(widthInit),
        view{}
    {
        calculateView();
    }

    void Canvas::restoreDefaultView()
    {
        topLeft = topLeftInitial, width = widthInitial;
        calculateView();
    }

    void Canvas::calculateView()
    {
        view = auxillary::BoundingBox::CreateFromPosition(
            topLeft, auxillary::vec2(width, -width / config::ASPECT)
        );
    }

    auxillary::vec2 Canvas::pixelPosToCanvas(float x, float y) const
    {
        return auxillary::vec2(
            topLeft.x + x / (config::WINDOW_SIZE_X - 1) * width,
            topLeft.y - y / (config::WINDOW_SIZE_Y - 1) * width / config::ASPECT
        );
    }

    auxillary::vec2 Canvas::pixelPosToCanvas(const sf::Vector2f& v) const
    {
        return auxillary::vec2(
            topLeft.x + v.x / (config::WINDOW_SIZE_X - 1) * width,
            topLeft.y - v.y / (config::WINDOW_SIZE_Y - 1) * width / config::ASPECT
        );
    }
    
    auxillary::vec2 Canvas::pixelVecToCanvas(const auxillary::vec2& v) const
    {
        return auxillary::vec2(
            v.x / (config::WINDOW_SIZE_X - 1) * width,
            v.y / (config::WINDOW_SIZE_Y - 1) * width / config::ASPECT
        );
    }

    auxillary::vec2 Canvas::canvasPosToPixel(const auxillary::vec2& v) const
    {
        return sf::Vector2f(
            (v.x - topLeft.x) / width * (config::WINDOW_SIZE_X - 1),
            -(v.y - topLeft.y) / width * (config::WINDOW_SIZE_Y - 1) * config::ASPECT
        );
    }

    float Canvas::canvasDistToPixel(float x) const
    {
        return x / width * (config::WINDOW_SIZE_X - 1);
    }

    sf::RectangleShape Canvas::getLine(const auxillary::vec2& a, const auxillary::vec2& b,
                                       float thickness) const
    {
        sf::RectangleShape line(sf::Vector2f(
            canvasDistToPixel(std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y))),
            thickness
        ));
        line.setOrigin(0.f, thickness / 2.f);
        line.setRotation(
            -std::atan2(canvasDistToPixel(b.y - a.y), canvasDistToPixel(b.x - a.x)) / PI * 180
        );
        line.setPosition(canvasPosToPixel(a));
        return line;
    }
    #pragma endregion
}	
