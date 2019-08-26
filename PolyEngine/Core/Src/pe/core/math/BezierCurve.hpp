#pragma once

#include <pe/Defines.hpp>
#include <pe/core/math/BasicMath.hpp>
#include <pe/core/math/Vector.hpp>

namespace pe::core::math
{
    /// <summary>Calculate Bezier Curve helper function</summary>
    /// <param name="anchors">Modified anchors of Bezier Curve</param>
    /// <param name="numOfPoints">Interpolation t</param>
    template<typename T>
    void BezierCurveImpl(std::vector<T>& anchors, float t)
    {
        for(size_t i = 1; i < anchors.size(); ++i)
            anchors[i-1] = ::Poly::Lerp(anchors[i-1], anchors[i], t);
        anchors.resize(anchors.size() - 1);
    }

    /// <summary>Calculate Bezier Curve point</summary>
    /// <param name="anchors">Input anchors of Bezier Curve</param>
    /// <param name="numOfPoints">Interpolation t</param>
    /// <returns>Point on the curve </returns>
    template<typename T>
    T BezierCurvePoint(std::vector<T> anchors, float t)
    {   
        ASSERTE(anchors.size(), "Size of anchors cannot be less than one!");
        while(anchors.size() > 1)
        {
            BezierCurveImpl(anchors, t);
        }
        return anchors[0];
    }   
} //end namespace Poly