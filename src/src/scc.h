#pragma once

#include "config.h"
#include "auxillary.h"


// Screen & canvas calculations
namespace scc
{
    class Canvas
    {
    public:
        static const float minWidth, maxWidth, wheelStrength;
        const auxillary::vec2 topLeftInitial;
        const float widthInitial;
        auxillary::vec2 topLeft;
        float width;
        auxillary::BoundingBox view;
    
        Canvas(const auxillary::vec2& topLeftInit, float widthInit);

        void restoreDefaultView();
        void calculateView();

        // Conversions
        auxillary::vec2 pixelPosToCanvas(float x, float y) const;
        auxillary::vec2 pixelPosToCanvas(const sf::Vector2f& v) const;
        auxillary::vec2 pixelVecToCanvas(const auxillary::vec2& v) const;
        auxillary::vec2 canvasPosToPixel(const auxillary::vec2& v) const;
        float canvasDistToPixel(float x) const;

        sf::RectangleShape getLine(const auxillary::vec2& a, const auxillary::vec2& b, 
                                   float thickness = 1.) const;
    };
}
