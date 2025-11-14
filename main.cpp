#define _USE_MATH_DEFINES
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <string>

using namespace std;

// Basic structures

/**
 * @brief 2D point structure
 */
struct Point {
    float x, y;
    
    bool operator<(const Point& other) const {
        if (y != other.y) return y < other.y;  // Сначала по Y
        return x < other.x;                    // При равном Y - по X
    }
};

/**
 * @brief Direction vector
 */
struct Vector {
    float x, y;
};

/**
 * @brief Infinite line defined by origin point and direction vector
 */
struct Line {
    Point origin;
    Vector direction;
};

/**
 * @brief Line segment between two points
 */
struct Segment {
    Point start, end;
    
    bool operator<(const Segment& other) const {
        // Сортируем сначала по start, потом по end
        if (start.x != other.start.x) return start.x < other.start.x;
        if (start.y != other.start.y) return start.y < other.start.y;
        if (end.x != other.end.x) return end.x < other.end.x;
        return end.y < other.end.y;
    }
};

// Function prototypes
/**
 * @brief Parse 4 points into rectangle edges
 * 
 * @param points Vector of 4 points (will be sorted)
 * @return Vector of 4 segments representing rectangle edges
 */
vector<Segment> parseContour(vector<Point>& points);

/**
 * @brief Calculate center point of polygon
 * 
 * @param points Vector of points
 * @return Arithmetic mean of all points
 */
Point findCenter(const vector<Point>& points);

/**
 * @brief Calculate direction vector from angle
 * 
 * @param angle_deg Angle in degrees (0-360)
 * @return Normalized direction vector
 */
Vector calculateDirection(float angle_deg);

/**
 * @brief Find intersection point between line and segment
 * 
 * @param line Infinite line
 * @param segment Finite line segment  
 * @return Intersection point or (NAN, NAN) if no intersection
 */
Point findIntersection(const Line& line, const Segment& segment);

/**
 * @brief Hatch generator for SLM slicer
 * 
 * Usage:
 * ./hatch_generator --angle 45 --step 1
 * ./hatch_generator --angle 30 --step 0.5 --input points.txt --output result.txt
 * 
 * @param argc Argument count
 * @param argv Argument values
 * @return int Exit code (0 = success)
 */
int main(int argc, char* argv[]) {
    // Значения по умолчанию
    float angle = 45.0f;
    float step = 1.0f;
    string input_file = "input.txt";
    string output_file = "console";
    
    // Парсим аргументы командной строки
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--angle" && i + 1 < argc) {
            angle = stof(argv[++i]);
        } else if (arg == "--step" && i + 1 < argc) {
            step = stof(argv[++i]);
        } else if (arg == "--input" && i + 1 < argc) {
            input_file = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            output_file = argv[++i];
        }
    }
    
    // Чтение точек из файла
    vector<Point> points;
    ifstream infile(input_file);
    if (!infile) {
        cerr << "Error: Cannot open input file " << input_file << endl;
        return 1;
    }
    
    // Формат файла: каждая строка "x y"
    float x, y;
    while (infile >> x >> y) {
        points.push_back({x, y});
    }
    infile.close();
    
    if (points.size() != 4) {
        cerr << "Error: Expected 4 points, got " << points.size() << endl;
        return 1;
    }
    
    // Function calls
    vector<Segment> edges = parseContour(points);
    Point center = findCenter(points);
    Vector hatch_direction = calculateDirection(angle);
    
    float diagonal = sqrt(
    pow(points[3].x - points[0].x, 2) + 
    pow(points[3].y - points[0].y, 2)
    );
    int lines_per_side = (diagonal / step) + 3;

    // Расчет перпендикуляра и шага
    Vector perpendicular = {-hatch_direction.y, hatch_direction.x};
    float perp_length = sqrt(perpendicular.x * perpendicular.x + perpendicular.y * perpendicular.y);
    Vector step_vector = {
        perpendicular.x * (step / perp_length),
        perpendicular.y * (step / perp_length) 
    };

    // Цикл генерации линий
    vector<Line> hatch_lines;
    for (int i = -lines_per_side; i <= lines_per_side; i++) {
        Point line_origin = {
            center.x + step_vector.x * i,
            center.y + step_vector.y * i
        };
        hatch_lines.push_back({line_origin, hatch_direction});
    }

    vector<Segment> hatch_segments;

    // Для каждой линии штриховки
    for (const Line& hatch_line : hatch_lines) {
        vector<Point> intersections;
        
        // Ищем пересечения со всеми 4 гранями
        for (const Segment& edge : edges) {
            Point intersection = findIntersection(hatch_line, edge);
            
            // Если пересечение есть и не NaN
            if (!isnan(intersection.x)) {
                // Проверяем дубликаты "на лету"
                bool is_duplicate = false;
                for (const Point& existing : intersections) {
                    if (fabs(intersection.x - existing.x) < 1e-10 && 
                        fabs(intersection.y - existing.y) < 1e-10) {
                        is_duplicate = true;
                        break;
                    }
                }
                // Добавляем только уникальные точки
                if (!is_duplicate) {
                    intersections.push_back(intersection);
                }
            }
        }
        
        // Если нашлось ровно 2 уникальные точки - создаём отрезок
        if (intersections.size() == 2) {
            hatch_segments.push_back({intersections[0], intersections[1]});
        }
    }

    // Сортируем отрезки
    sort(hatch_segments.begin(), hatch_segments.end());

    // Вывод результатов
    if (output_file == "console") {
        // Вывод в консоль
        for (int i = 0; i < hatch_segments.size(); i++) {
            cout << "Line " << i+1 << ": (" 
                 << hatch_segments[i].start.x << "," << hatch_segments[i].start.y << ") -> ("
                 << hatch_segments[i].end.x << "," << hatch_segments[i].end.y << ")" << endl;
        }
    } 
    else {
        // Вывод в файл
        ofstream outfile(output_file);
        for (int i = 0; i < hatch_segments.size(); i++) {
            outfile << "Line " << i+1 << ": (" 
                    << hatch_segments[i].start.x << "," << hatch_segments[i].start.y << ") -> ("
                    << hatch_segments[i].end.x << "," << hatch_segments[i].end.y << ")" << endl;
        }
        outfile.close();
        cout << "Results written to " << output_file << endl;
    }
    
    return 0;
}

// Function stubs
vector<Segment> parseContour(vector<Point>& points) {
    if (points.size() != 4) {
        cerr << "Error: Expected 4 points, got " << points.size() << endl;
        return {};
    }

    sort(points.begin(), points.end());

    vector<Segment> edges = {
        {points[0], points[1]},  // bottom
        {points[2], points[3]},  // up
        {points[0], points[2]},  // left
        {points[1], points[3]}   // right
    };

    return edges;
}

Point findCenter(const vector<Point>& points) {
    Point center = {0, 0};
    // Calculate arithmetic mean of all points
    for (const auto& p : points) {
        center.x += p.x;
        center.y += p.y;
    }
    center.x /= points.size();
    center.y /= points.size();
    return center;
}

Vector calculateDirection(float angle_deg) {
    float angle_rad = angle_deg * M_PI / 180.0;
    return Vector{cos(angle_rad), sin(angle_rad)};
}

Point findIntersection(const Line& line, const Segment& segment) {
    // Разложение данных
    Point p1 = segment.start;
    Point p2 = segment.end;
    Point o = line.origin;
    Vector d = line.direction;
    Vector seg_vec = {p2.x - p1.x, p2.y - p1.y};
    
    // Определитель системы
    float det = d.x * (-seg_vec.y) - d.y * (-seg_vec.x);
    
    // Проверка параллельности
    if (fabs(det) < 1e-10) {
        return {NAN, NAN};
    }
    
    // Вектор правой части
    Vector right = {p1.x - o.x, p1.y - o.y};
    
    // Решение системы
    float t = (right.x * (-seg_vec.y) - right.y * (-seg_vec.x)) / det;
    float u = (d.x * right.y - d.y * right.x) / det;
    
    // Проверка попадания в отрезок
    if (u >= 0 && u <= 1) {
        return {o.x + d.x * t, o.y + d.y * t};
    }
    
    return {NAN, NAN};
}