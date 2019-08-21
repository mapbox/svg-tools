#include "path.hpp"

#include <mapbox/svg/path_parser.hpp>

#include "expect.hpp"

namespace mapbox {
namespace svg {
namespace test {

struct PathVertexReceiver {
public:
    Path path;

    void moveTo(double x, double y, bool relative) {
        path.emplace_back(PathCommand::MoveTo(x, y, relative));
    }

    void closePath() {
        path.emplace_back(PathCommand::ClosePath());
    }

    void lineTo(double x, double y, bool relative) {
        path.emplace_back(PathCommand::LineTo(x, y, relative));
    }

    void horizontalLineTo(double x, bool relative) {
        path.emplace_back(PathCommand::HorizontalLineTo(x, relative));
    }

    void verticalLineTo(double y, bool relative) {
        path.emplace_back(PathCommand::VerticalLineTo(y, relative));
    }

    void curveTo(double x1, double y1, double x2, double y2, double x, double y, bool relative) {
        path.emplace_back(PathCommand::CurveTo(x1, y1, x2, y2, x, y, relative));
    }

    void smoothCurveTo(double x2, double y2, double x, double y, bool relative) {
        path.emplace_back(PathCommand::SmoothCurveTo(x2, y2, x, y, relative));
    }

    void quadraticCurveTo(double x1, double y1, double x, double y, bool relative) {
        path.emplace_back(PathCommand::QuadraticCurveTo(x1, y1, x, y, relative));
    }

    void smoothQuadraticCurveTo(double x, double y, bool relative) {
        path.emplace_back(PathCommand::SmoothQuadraticCurveTo(x, y, relative));
    }

    void arc(double rx, double ry, double xAxisRotation, bool largeArcFlag, bool sweepFlag, double x, double y, bool relative) {
        path.emplace_back(PathCommand::Arc(rx, ry, xAxisRotation, largeArcFlag, sweepFlag, x, y, relative));
    }
};

} // namespace test
} // namespace svg
} // namespace mapbox

int main() {
    using namespace mapbox::svg;
    using namespace mapbox::svg::test;

    PathVertexReceiver receiver;
    PathParser<PathVertexReceiver> parser(receiver);

    receiver.path.clear();
    EXPECT_TRUE(parser("M6,12,4,4a2 2 0 1 1-2 2A2 2 0 0 1 6 12Z"));
    EXPECT_FALSE(parser.hasError());
    EXPECT_EQUALS((test::Path{
                      test::PathCommand::MoveTo(6, 12, false),
                      test::PathCommand::MoveTo(4, 4, false),
                      test::PathCommand::Arc(2, 2, 0, true, true, -2, 2, true),
                      test::PathCommand::Arc(2, 2, 0, false, true, 6, 12, false),
                      test::PathCommand::ClosePath(),
                  }),
                  receiver.path);

    receiver.path.clear();
    EXPECT_FALSE(parser("M"));
    EXPECT_EQUALS(PathParseErrorType::NumberParsing, parser.errorType());
    EXPECT_EQUALS(1, parser.errorOffset());

    receiver.path.clear();
    EXPECT_TRUE(parser("M0,0"));
    EXPECT_FALSE(parser.hasError());
    EXPECT_EQUALS((test::Path{
                      test::PathCommand::MoveTo(0, 0, false),
                  }),
                  receiver.path);
}
