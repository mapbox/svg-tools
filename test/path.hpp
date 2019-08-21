#pragma once

#include <mapbox/svg/path_parser.hpp>

#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <vector>

namespace mapbox {
namespace svg {

::std::ostream& operator<<(::std::ostream& os, const PathParseErrorType error) {
    switch (error) {
        case PathParseErrorType::NumberParsing: return os << "PathParseErrorType::NumberParsing";
        case PathParseErrorType::FlagParsing: return os << "PathParseErrorType::FlagParsing";
        case PathParseErrorType::CommandParsing: return os << "PathParseErrorType::CommandParsing";
        default: return os << "PathParseErrorType::None";
    }
}

namespace test {

namespace {

bool equals(const double a, const double b) {
    return std::abs(a - b) < std::numeric_limits<double>::epsilon();
}

} // namespace

class PathCommand {
public:
    enum class Type : uint8_t {
        MoveTo,
        ClosePath,
        LineTo,
        HorizontalLineTo,
        VerticalLineTo,
        CurveTo,
        SmoothCurveTo,
        QuadraticCurveTo,
        SmoothQuadraticCurveTo,
        Arc,
    } type;

    const bool relative;
    const bool largeArcFlag;
    const bool sweepFlag;
    const double x, y;
    const double x1, y1;
    const double x2, y2;
    const double xAxisRotation;

private:
    PathCommand(const Type type_,
                const bool relative_,
                const bool largeArcFlag_,
                const bool sweepFlag_,
                const double x_,
                const double y_,
                const double x1_,
                const double y1_,
                const double x2_,
                const double y2_,
                const double xAxisRotation_)
        : type(type_),
          relative(relative_),
          largeArcFlag(largeArcFlag_),
          sweepFlag(sweepFlag_),
          x(x_),
          y(y_),
          x1(x1_),
          y1(y1_),
          x2(x2_),
          y2(y2_),
          xAxisRotation(xAxisRotation_) {
    }

public:
    bool operator==(const PathCommand& other) const {
        return type == other.type && relative == other.relative &&
               largeArcFlag == other.largeArcFlag && sweepFlag == other.sweepFlag &&
               equals(x, other.x) && equals(y, other.y) && equals(x1, other.x1) &&
               equals(y1, other.y1) && equals(x2, other.x2) && equals(y2, other.y2) &&
               equals(xAxisRotation, other.xAxisRotation);
    }

public:
    static PathCommand MoveTo(double x, double y, bool relative) {
        return { Type::MoveTo, relative, false, false, x, y, 0, 0, 0, 0, 0 };
    }

    static PathCommand ClosePath() {
        return { Type::ClosePath, false, false, false, 0, 0, 0, 0, 0, 0, 0 };
    }

    static PathCommand LineTo(double x, double y, bool relative) {
        return { Type::LineTo, relative, false, false, x, y, 0, 0, 0, 0, 0 };
    }

    static PathCommand HorizontalLineTo(double x, bool relative) {
        return { Type::HorizontalLineTo, relative, false, false, x, 0, 0, 0, 0, 0, 0 };
    }

    static PathCommand VerticalLineTo(double y, bool relative) {
        return { Type::VerticalLineTo, relative, false, false, 0, y, 0, 0, 0, 0, 0 };
    }

    static PathCommand
    CurveTo(double x1, double y1, double x2, double y2, double x, double y, bool relative) {
        return { Type::CurveTo, relative, false, false, x, y, x1, y1, x2, y2, 0 };
    }

    static PathCommand SmoothCurveTo(double x2, double y2, double x, double y, bool relative) {
        return { Type::SmoothCurveTo, relative, false, false, x, y, 0, 0, x2, y2, 0 };
    }

    static PathCommand QuadraticCurveTo(double x1, double y1, double x, double y, bool relative) {
        return { Type::QuadraticCurveTo, relative, false, false, x, y, x1, y1, 0, 0, 0 };
    }

    static PathCommand SmoothQuadraticCurveTo(double x, double y, bool relative) {
        return { Type::SmoothQuadraticCurveTo, relative, false, false, x, y, 0, 0, 0, 0, 0 };
    }

    static PathCommand Arc(double rx,
                           double ry,
                           double xAxisRotation,
                           bool largeArcFlag,
                           bool sweepFlag,
                           double x,
                           double y,
                           bool relative) {
        return { Type::Arc, relative, largeArcFlag, sweepFlag, x, y, rx, ry, 0, 0, xAxisRotation };
    }
};

::std::ostream& operator<<(::std::ostream& os, const PathCommand& command) {
    switch (command.type) {
    case PathCommand::Type::MoveTo:
        os << "MoveTo { x: " << command.x << ", y: " << command.y << " }";
        break;

    case PathCommand::Type::ClosePath:
        os << "ClosePath";
        break;

    case PathCommand::Type::LineTo:
        os << "LineTo { x: " << command.x << ", y: " << command.y << " }";
        break;

    case PathCommand::Type::HorizontalLineTo:
        os << "HorizontalLineTo { x: " << command.x << " }";
        break;

    case PathCommand::Type::VerticalLineTo:
        os << "VerticalLineTo { y: " << command.y << " }";
        break;

    case PathCommand::Type::CurveTo:
        os << "CurveTo { x1: " << command.x1 << ", y1: " << command.y1 << ", x2: " << command.x2
           << ", y2: " << command.y2 << ", x: " << command.x << ", y: " << command.y << " }";
        break;

    case PathCommand::Type::SmoothCurveTo:
        os << "SmoothCurveTo { x2: " << command.x2 << ", y2: " << command.y2 << ", x: " << command.x
           << ", y: " << command.y << " }";
        break;

    case PathCommand::Type::QuadraticCurveTo:
        os << "QuadraticCurveTo { x1: " << command.x1 << ", y1: " << command.y1
           << ", x: " << command.x << ", y: " << command.y << " }";
        break;

    case PathCommand::Type::SmoothQuadraticCurveTo:
        os << "SmoothQuadraticCurveTo { x: " << command.x << ", y: " << command.y << " }";
        break;

    case PathCommand::Type::Arc:
        os << "Arc { rx: " << command.x1 << ", ry: " << command.y1
           << ", xAxisRotation: " << command.xAxisRotation
           << ", largeArc: " << (command.largeArcFlag ? "true" : "false")
           << ", sweep: " << (command.sweepFlag ? "true" : "false") << ", x: " << command.x
           << ", y: " << command.y << " }";
        break;
    default:
        os << "Unknown";
        break;
    }
    if (command.relative) {
        os << " (relative)";
    }
    return os;
}

using Path = std::vector<PathCommand>;

::std::ostream& operator<<(::std::ostream& os, const Path& path) {
    os << "Path {" << std::endl;
    for (const auto& command : path) {
        os << "    " << command << "," << std::endl;
    }
    os << "}";
    return os;
}

} // namespace test
} // namespace svg
} // namespace mapbox
