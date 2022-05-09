//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Miguel Maso Sotomayor
//

// System includes


// External includes


// Project includes
#include "includes/variables.h"
#include "includes/model_part.h"
#include "geometries/line_2d_2.h"
#include "utilities/variable_utils.h"
#include "utilities/parallel_utilities.h"
#include "utilities/reduction_utilities.h"
#include "calculate_distance_to_boundary_process.h"


namespace Kratos
{

void CalculateDistanceToBoundaryProcess::FindApproximatingGeometry(
    GeometryType::Pointer& pEntity,
    const ModelPart& rBoundaryPart)
{
    double low, left = std::numeric_limits<double>::max();
    double top, right = std::numeric_limits<double>::lowest();

    typedef CombinedReduction< MinReduction<double>,
                               MinReduction<double>,
                               MaxReduction<double>,
                               MaxReduction<double> > BoundingBoxReduction;

    std::tie(low, left, top, right) = block_for_each<BoundingBoxReduction>(rBoundaryPart.Nodes(), [&](NodeType& rNode){
        double x = rNode.X();
        double y = rNode.Y();
        return std::make_tuple(y, x, y, x);
    });

    auto point_0 = Kratos::make_shared<Point>(left, low);
    auto point_1 = Kratos::make_shared<Point>(right, low);
    auto point_2 = Kratos::make_shared<Point>(right, top);
    auto point_3 = Kratos::make_shared<Point>(left, top);

    Line2D2<Point> line_a(point_0, point_2);
    Line2D2<Point> line_b(point_1, point_3);

    double r_squared_a = RSquared(line_a, rBoundaryPart);
    double r_squared_b = RSquared(line_b, rBoundaryPart);

    if (r_squared_a > r_squared_b) {
        pEntity = Kratos::make_shared<Line2D2<Point>>(line_a);
    } else {
        pEntity = Kratos::make_shared<Line2D2<Point>>(line_b);
    }
    if (r_squared_a < mRSquaredThreshold && r_squared_b < mRSquaredThreshold) {
        KRATOS_INFO(Info()) << "The fitted boundary has an R squared smaller than " << std::to_string(mRSquaredThreshold) << ". A brute force search will be executed." << std::endl;
        mBruteForceSearch = true;
    } else {
        mBruteForceSearch = false;
    }
}

double CalculateDistanceToBoundaryProcess::RSquared(const GeometryType& rLine, const ModelPart& rBoundaryPart)
{
    double areas, squared_deviations;
    Point center = rLine.Center();

    typedef CombinedReduction< SumReduction<double>, SumReduction<double> > SumSumReduction;

    std::tie(areas, squared_deviations) = block_for_each<SumSumReduction>(rBoundaryPart.Nodes(), [&](NodeType& rNode){
        Point projected;
        double deviation = GeometricalProjectionUtilities::FastProjectOnLine2D(rLine, rNode, projected);
        double area = SquaredDistance(center, rNode);
        return std::make_tuple(area, std::pow(deviation, 2));
    });

    return 1.0 - squared_deviations / areas;
}

double CalculateDistanceToBoundaryProcess::SquaredDistance(const Point& rPointA, const Point& rPointB)
{
    array_1d<double,3> vector = rPointA.Coordinates() - rPointB.Coordinates();
    return inner_prod(vector, vector);
}

double CalculateDistanceToBoundaryProcess::Distance(const Point& rPointA, const Point& rPointB)
{
    return std::sqrt(SquaredDistance(rPointA, rPointB));
}

int CalculateDistanceToBoundaryProcess::Check()
{
    VariableUtils().CheckVariableExists(DISTANCE, mrModelPart.Nodes());
    return 0;
}

void CalculateDistanceToBoundaryProcess::ExecuteBeforeSolutionLoop()
{
    if (mBruteForceSearch) {
        block_for_each(mrModelPart.Nodes(), [&](NodeType& rNode){
            double& r_distance = rNode.FastGetSolutionStepValue(DISTANCE);
            for (auto& r_boundary_node : mrBoundaryPart.Nodes()) {
                const double new_distance = Distance(rNode, r_boundary_node);
                r_distance = std::min(r_distance, new_distance);
            }
        });
    } else {
        block_for_each(mrModelPart.Nodes(), [&](NodeType& rNode){
            Point projected;
            double& r_distance = rNode.FastGetSolutionStepValue(DISTANCE);
            const auto projection = std::abs(GeometricalProjectionUtilities::FastProjectOnLine2D(*mpBoundary, rNode, projected));
            r_distance = std::min(r_distance, projection);
        });
    }
}

const Parameters CalculateDistanceToBoundaryProcess::GetDefaultParameters() const
{
    auto default_parameters = Parameters(R"(
    {
        "computing_model_part_name" : "",
        "absorbing_boundary_name"   : "",
        "r_squared_threshold" : 0.99
    })");
    return default_parameters;
}

}  // namespace Kratos.
