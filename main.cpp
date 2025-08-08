#include "Vec.h"
#include "tgaimage.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include "Debug.hpp"
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);

void line(Vec2i a, Vec2i b, TGAImage& image, TGAColor color)
{

    bool steep = false;
    int x0 = a.x;
    int y0 = a.y;
    int x1 = b.x;
    int y1 = b.y;
    if (std::abs(y1 - y0) > std::abs(x1 - x0)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }

    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;

    int derror = std::abs(dy) * 2;
    int error = 0;

    float y = y0;
    for (int x = x0; x <= x1; x++) {
        // std::cout << "x: "<< x << "y: "<< y << std::endl;
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
        error += derror;
        if (error > dx) {
            y += (y1 > y0 ? 1 : -1);
            error -= dx * 2;
        }
    }
}

void triangle(Vec2i p0, Vec2i p1, Vec2i p2, TGAImage& image, TGAColor color)
{
    std::vector<Vec2i> points = { p0, p1, p2 };
    std::sort(points.begin(), points.end(), [](Vec2i a, Vec2i b) {
        if (a.y != b.y) {
            return a.y < b.y;
        } else {
            return a.x < b.x;
        }
    });

    p0 = points[0];
    p1 = points[1];
    p2 = points[2];
    // line(p0, p1, image, color);
    // line(p1, p2, image, color);
    // line(p2, p0, image, color);

    int total_height = p2.y - p0.y;
    int bottom_height = p1.y - p0.y;

    DEBUG(p0);DEBUG(p1);DEBUG(p2);
    std::cout << std::endl;
    // 防止除0
    if (bottom_height == 0) {
        image.set(p0.x, p0.y, red);
        image.set(p1.x, p1.y, green);
    } else {
        for (int y = p0.y; y <= p1.y; y++) {
            int now_height = y - p0.y;
            float alpha = (float)now_height / total_height;
            float beta = (float)now_height / bottom_height;
            Vec2i part_p = (p2-p0)*alpha +p0;
            Vec2i com_p = (p1-p0)*beta +p0;
            image.set(part_p.x, y, red);
            image.set(com_p.x, y, green);
        }
    }
}

int main(int argc, char** argv)
{
    TGAImage image(1000, 1000, TGAImage::RGB);

    Vec2i t0[3] = { Vec2i(10, 70), Vec2i(50, 160), Vec2i(70, 80) };
    Vec2i t1[3] = { Vec2i(180, 50), Vec2i(150, 1), Vec2i(70, 180) };
    Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };
    triangle(t0[0], t0[1], t0[2], image, red);
    triangle(t1[0], t1[1], t1[2], image, white);
    triangle(t2[0], t2[1], t2[2], image, green);

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    // system("pause");
    return 0;
}