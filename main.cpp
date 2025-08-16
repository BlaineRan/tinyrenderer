#include "geometry.h"
#include "tgaimage.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include "Debug.hpp"
#include "model.h"
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

Model *model = NULL;
const int width = 800;
const int height = 800;
const float EPS = 1e-6f;
const float N_EPS = -1e-6f;
void line(Vec2i a, Vec2i b, TGAImage &image, TGAColor color)
{

    bool steep = false;
    int x0 = a.x;
    int y0 = a.y;
    int x1 = b.x;
    int y1 = b.y;
    if (std::abs(y1 - y0) > std::abs(x1 - x0))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }

    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;

    int derror = std::abs(dy) * 2;
    int error = 0;

    float y = y0;
    for (int x = x0; x <= x1; x++)
    {
        // std::cout << "x: "<< x << "y: "<< y << std::endl;
        if (steep)
        {
            image.set(y, x, color);
        }
        else
        {
            image.set(x, y, color);
        }
        error += derror;
        if (error > dx)
        {
            y += (y1 > y0 ? 1 : -1);
            error -= dx * 2;
        }
    }
}

Vec3f compute_barycentric(std::array<Vec2f, 3> &pts, Vec2f pt)
{
    Vec2f e1 = pts[1] - pts[0];
    Vec2f e2 = pts[2] - pts[0];
    float det = e1.x * e2.y - e1.y * e2.x;
    float invDet = 1.f / det;
    float invM[2][2] = {
        {e2.y * invDet, -e2.x * invDet}, {-e1.y * invDet, e1.x * invDet}};

    Vec2f p = pt - pts[0];
    float beta = invM[0][0] * p.x + invM[0][1] * p.y;
    float gamma = invM[1][0] * p.x + invM[1][1] * p.y;
    return Vec3f(1 - beta - gamma, beta, gamma);
}

Vec3f compute_barycentric_batch(std::array<Vec2f, 3> &pts, Vec2f pt, float (&invM)[2][2])
{
    Vec2f p = pt - pts[0];
    float beta = invM[0][0] * p.x + invM[0][1] * p.y;
    float gamma = invM[1][0] * p.x + invM[1][1] * p.y;
    return Vec3f(1 - beta - gamma, beta, gamma);
}

void triangle(Vec3f *pts, float *z_buffer, TGAImage &image, TGAColor color)
{
    int maxx = ceil(std::max({pts[0].x, pts[1].x, pts[2].x}));
    int minx = floor(std::min({pts[0].x, pts[1].x, pts[2].x}));
    int maxy = ceil(std::max({pts[0].y, pts[1].y, pts[2].y}));
    int miny = floor(std::min({pts[0].y, pts[1].y, pts[2].y}));

    // 防止越界
    maxx = std::min(maxx, image.get_width() - 1);
    maxy = std::min(maxy, image.get_height() - 1);
    minx = std::max(minx, 0);
    miny = std::max(miny, 0);

    // 批量计算重心坐标的预计算
    std::array<Vec2f, 3> pts2 = {Vec2f(pts[0].x, pts[0].y), Vec2f(pts[1].x, pts[1].y), Vec2f(pts[2].x, pts[2].y)};
    Vec2f e1 = pts2[1] - pts2[0];
    Vec2f e2 = pts2[2] - pts2[0];
    float det = e1.x * e2.y - e1.y * e2.x;
    // 一些错误的三角形会导致det为0，直接跳过避免除0
    if (std::abs(det) <= EPS)
        return;
    float invDet = 1.f / det;
    float invM[2][2] = {
        {e2.y * invDet, -e2.x * invDet}, {-e1.y * invDet, e1.x * invDet}};

    for (int x = minx; x <= maxx; x++)
    {
        for (int y = miny; y <= maxy; y++)
        {
            Vec3f bpt = compute_barycentric_batch(pts2, Vec2f(x+0.5f, y + 0.5f), invM);
            // 如果改点不在三角形内则跳过
            // why负向容差？因为如果是EPS，三角形边缘会存在"两边的三角形都不接受这个像素"的情况
            // 改成允许一定的负向容差，三角形们都会略微向外渲染一点点，从而解决边缘的黑点问题
            if (bpt.x < N_EPS || bpt.y < N_EPS || bpt.z < N_EPS)
                continue;
            // 在三角形内则进行深度测试
            // 此时是标准的-z朝向，越大越靠近camera
            float now_z = 0;
            for (int i = 0; i < 3; i++)
                now_z += pts[i].z * bpt[i];

            int idx = x + y * image.get_width();
            if (z_buffer[idx] > now_z)
                continue; // 保留更近(更大)的 z
            z_buffer[idx] = now_z;
            image.set(x, y, color);
        }
    }
}
void triangle_old(Vec2i p0, Vec2i p1, Vec2i p2, TGAImage &image, TGAColor color)
{
    std::vector<Vec2i> points = {p0, p1, p2};
    std::sort(points.begin(), points.end(), [](Vec2i a, Vec2i b)
              {
        if (a.y != b.y) {
            return a.y < b.y;
        } else {
            return a.x < b.x;
        } });

    p0 = points[0];
    p1 = points[1];
    p2 = points[2];
    // line(p0, p1, image, color);
    // line(p1, p2, image, color);
    // line(p2, p0, image, color);

    int total_height = p2.y - p0.y;
    int bottom_height = p1.y - p0.y;
    // 防止除0
    if (bottom_height == 0)
    {
        image.set(p0.x, p0.y, red);
        image.set(p1.x, p1.y, green);
    }
    else
    {
        // 此处y是三角形内部的遍历高度，加上p0.y才是真正的坐标
        // 在一个循环内完成对两部分的填充
        for (int y = 0; y <= total_height; y++)
        {
            bool isSecond = false;
            if (y >= bottom_height)
                isSecond = true;
            // 当前段的长度
            int segment_height = isSecond ? (p2.y - p1.y) : (p1.y - p0.y);
            float alpha = isSecond ? ((float)(y - bottom_height) / segment_height)
                                   : ((float)y / segment_height);
            float beta = (float)y / total_height;

            // 计算当前y对应的两边的x

            // part指的是需要分开计算的两条边，因此需要分情况确定两条边的x长度和相对更左边的点
            int part_base = isSecond ? (p2.x - p1.x) : (p1.x - p0.x);
            int part_bias = isSecond ? (p1.x) : (p0.x);

            int part_x = part_base * alpha + part_bias;
            int com_x = (p2.x - p0.x) * beta + p0.x;

            // 水平线填充
            if (part_x > com_x)
                std::swap(part_x, com_x);
            int now_y = y + p0.y;
            for (int i = part_x; i <= com_x; i++)
            {
                image.set(i, now_y, color);
            }
            // image.set(com_x,now_y,color);
        }
    }
}

Vec3f world2screen(Vec3f v)
{
    return Vec3f(int((v.x + 1.) * width / 2. + .5), int((v.y + 1.) * height / 2. + .5), v.z);
}

int main(int argc, char **argv)
{
    if (2 == argc)
    {
        model = new Model(argv[1]);
    }
    else
    {
        model = new Model("../obj/african_head/african_head.obj");
    }
    // 初始化z_buffer
    float *zbuffer = new float[width * height];
    for (int i = width * height; i--; zbuffer[i] = -std::numeric_limits<float>::max())
        ;

    TGAImage image(width, height, TGAImage::RGB);
    DEBUG(model->nfaces());
    // 光照方向
    Vec3f light_dir(0, 0, -1);

    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        Vec3f screen_pts[3];
        Vec3f world_pts[3];
        for (int i = 0; i < 3; i++)
        {
            screen_pts[i] = world2screen(model->vert(face[i]));
            world_pts[i] = model->vert(face[i]);
        }

        //计算当前面的法线
        Vec3f n = (world_pts[2]-world_pts[0])^(world_pts[1]-world_pts[0]);
        n.normalize();
        float intensity = n * light_dir;
        if (intensity <= 0) continue; // 如果强度小于等于0，跳过
        triangle(screen_pts, zbuffer, image, TGAColor(255 * intensity, 255 * intensity, 255 * intensity, 255));
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;

    return 0;
}