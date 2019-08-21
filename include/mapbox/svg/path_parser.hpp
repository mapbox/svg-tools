#pragma once

#include <cstdlib>
#include <cctype>
#include <cmath>

namespace mapbox {
namespace svg {

enum class PathParseErrorType {
    None,
    NumberParsing,
    FlagParsing,
    CommandParsing,
};

// Interface:
// 
// struct VertexReceiver {
//     void moveTo(double x, double y, bool relative);
//     void closePath();
//     void lineTo(double x, double y, bool relative);
//     void horizontalLineTo(double x, bool relative);
//     void verticalLineTo(double y, bool relative);
//     void curveTo(double x1, double y1, double x2, double y2, double x, double y, bool relative);
//     void smoothCurveTo(double x2, double y2, double x, double y, bool relative);
//     void quadraticCurveTo(double x1, double y1, double x, double y, bool relative);
//     void smoothQuadraticCurveTo(double x, double y, bool relative);
//     void arc(double rx, double ry, double xAxisRotation, bool largeArcFlag, bool sweepFlag, double x, double y, bool relative);
// };
//
//
// Usage:
// 
// VertexReceiver receiver;
// mapbox::svg::PathParser<VertexReceiver> parser(receiver);
// parser("M6,12,4,4a2 2 0 1 1-2 2A2 2 0 0 1 6 12Z");

template <typename VertexReceiver>
class PathParser {
public:
    PathParser(VertexReceiver& t_) : t(t_) {
    }
    PathParser(const PathParser&) = delete;
    PathParser(PathParser&&) = delete;

    bool operator()(const char* str) {
        begin = str;
        cursor = str;
        error = PathParseErrorType::None;
        next = nullptr;

        char command;
        double x, y, x1, y1, x2, y2, xAxisRotation;
        bool relative, largeArcFlag, sweepFlag;

        skipWhitespace();
        while (*cursor) {
            command = *cursor++;
            relative = command >= 'a';
            skipWhitespace();

            if (command == 'Z' || command == 'z') { // closepath
                skipWhitespace();
                t.closePath();
                continue;
            }

            do {
                switch (command) {
                    case '\0':
                        break;
                    case 'M': case 'm': // moveto
                        if (!parseNumber(x)) return false;
                        if (!parseNumber(y)) return false;
                        t.moveTo(x, y, relative);
                        break;
                    case 'L': case 'l': // lineto
                        if (!parseNumber(x)) return false;
                        if (!parseNumber(y)) return false;
                        t.lineTo(x, y, relative);
                        break;
                    case 'H': case 'h': // horizontal lineto
                        if (!parseNumber(x)) return false;
                        t.horizontalLineTo(x, relative);
                        break;
                    case 'V': case 'v': // vertical lineto
                        if (!parseNumber(y)) return false;
                        t.verticalLineTo(y, relative);
                        break;
                    case 'C': case 'c': // curveto
                        if (!parseNumber(x1)) return false;
                        if (!parseNumber(y1)) return false;
                        if (!parseNumber(x2)) return false;
                        if (!parseNumber(y2)) return false;
                        if (!parseNumber(x)) return false;
                        if (!parseNumber(y)) return false;
                        t.curveTo(x1, y1, x2, y2, x, y, relative);
                        break;
                    case 'S': case 's': // smooth curveto
                        if (!parseNumber(x2)) return false;
                        if (!parseNumber(y2)) return false;
                        if (!parseNumber(x)) return false;
                        if (!parseNumber(y)) return false;
                        t.smoothCurveTo(x2, y2, x, y, relative);
                        break;
                    case 'Q': case 'q': // quadratic bezier curveto
                        if (!parseNumber(x1)) return false;
                        if (!parseNumber(y1)) return false;
                        if (!parseNumber(x)) return false;
                        if (!parseNumber(y)) return false;
                        t.quadraticCurveTo(x1, y1, x, y, relative);
                        break;
                    case 'T': case 't': // smooth quadratic bezier curveto
                        if (!parseNumber(x)) return false;
                        if (!parseNumber(y)) return false;
                        t.smoothQuadraticCurveTo(x, y, relative);
                        break;
                    case 'A': case 'a': // elliptical arc
                        if (!parseNumber(x1)) return false;
                        if (!parseNumber(y1)) return false;
                        if (!parseNumber(xAxisRotation)) return false;
                        if (!parseFlag(largeArcFlag)) return false;
                        if (!parseFlag(sweepFlag)) return false;
                        if (!parseNumber(x)) return false;
                        if (!parseNumber(y)) return false;
                        t.arc(x1, y1, xAxisRotation, largeArcFlag, sweepFlag, x, y, relative);
                        break;
                    default:
                        error = PathParseErrorType::CommandParsing;
                        return false;

                }
                // parse another instance of this command if there are more numbers
            } while ((*cursor >= '0' && *cursor <= '9') || *cursor == '.' || *cursor == '-');
        }
        return true;
    }

    bool hasError() const {
        return error != PathParseErrorType::None;
    }

    PathParseErrorType errorType() const {
        return error;
    }

    std::ptrdiff_t errorOffset() const {
        return cursor - begin;
    }

private:
    bool parseNumber(double& value) {
        if (*cursor) {
            value = strtod(cursor, &next);
            if (cursor != next && value != HUGE_VAL) {
                cursor = next;
                skipSeparator();
                return true;
            }
        }
        error = PathParseErrorType::NumberParsing;
        return false;
    }

    bool parseFlag(bool& value) {
        if (*cursor == '0') {
            value = false;
        } else if (*cursor == '1') {
            value = true;
        } else {
            error = PathParseErrorType::FlagParsing;
            return false;
        }
        ++cursor;
        skipSeparator();
        return true;
    }

    void skipWhitespace() {
        while (*cursor == ' ' || *cursor == '\t' || *cursor == '\n' || *cursor == '\r') ++cursor;
    }

    void skipSeparator() {
        skipWhitespace();
        if (*cursor == ',') ++cursor;
        skipWhitespace();
    }

private:
    VertexReceiver& t;
    const char* begin;
    const char* cursor;
    char* next;
    PathParseErrorType error;
};

} // namespace svg
} // namespace mapbox
