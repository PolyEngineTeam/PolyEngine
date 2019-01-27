#pragma once

#include <Defines.hpp>
#include <Math/BasicMath.hpp>
#include <Math/Vector.hpp>

namespace Poly
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