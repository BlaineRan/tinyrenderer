#pragma once
class Vec2i {
    /* data */
public:
    int x = 0;
    int y = 0;
    Vec2i(int x, int y);
    Vec2i operator+ (const Vec2i& other) const;
    Vec2i operator- (const Vec2i& other) const;
    Vec2i operator* (const float p) const;
};
