#include "./vec.h"

Vec vec_s(float a) {
  return (Vec) {a, a};
}

Vec vec(float a, float b) {
  return (Vec) {a, b};
}

bool vec_eq(Vec a, Vec b) {
  return a.x==b.x && a.y==b.y;
}

Vec vec_add(Vec a, Vec b) {
  return (Vec) {a.x+b.x, a.y+b.y};
}

Vec vec_sub(Vec a, Vec b) {
  return (Vec) {a.x-b.x, a.y-b.y};
}

Vec vec_mul(Vec a, Vec b) {
  return (Vec) {a.x*b.x, a.y*b.y};
}

Vec vec_div(Vec a, Vec b) {
  return (Vec) {a.x/b.x, a.y/b.y};
}

Vec vec_min(Vec a, Vec b) {
  float dy = a.y - b.y;
  float dx = a.x - b.x;

  if(dy<0.0) {
    return a;
  }
  else if (dy>0.0){
    return b;
  }

  if(dx<0.0) {
    return a;
  }
  return b;
}

Vec vec_max(Vec a, Vec b) {
  float dy = a.y - b.y;
  float dx = a.x - b.x;

  if(dy<0.0) {
    return b;
  }
  else if (dy>0.0){
    return a;
  }

  if(dx<0.0) {
    return b;
  }
  return a;  
}

Vec vec_scale(Vec a, float b) {
  return (Vec) {a.x*b, a.y*b};
}

Vec vec_off(Vec a, float b) {
  return (Vec) {a.x+b, a.y+b};
}
