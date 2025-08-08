#include "Vec.h"
Vec2i::Vec2i(int x,int y){
    this->x = x;
    this->y = y;
}

Vec2i Vec2i::operator+ (const Vec2i& other) const{
    return Vec2i(x+other.x,y+other.y);
}

Vec2i Vec2i::operator- (const Vec2i& other) const{
    return Vec2i(x-other.x,y-other.y);
}

Vec2i Vec2i::operator* (const float p) const{
    return Vec2i(x*p,y*p);
}