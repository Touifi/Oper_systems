#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
const float precision = 0.000000000000001;

struct Line {
  float x;
  float y;
  float c;
};

float GetRandom(){
return (((float)rand())/RAND_MAX*200 - 100); //[-100; 100]
}

bool Eq(float a, float b) {
  if (fabs(a - b) > precision)
    return false;
  else
    return true;
}

void FindCommonDot(struct Line line1, struct Line line2) {
  if (!Eq(line2.x, 0) && !Eq(line2.y, 0)) {
    if ((line1.x / line2.x) == (line1.y / line2.y) &&
        (line1.y / line2.y) == (line1.c / line2.c) &&
        (line1.x / line2.x) == (line1.c / line2.c)) {
      printf("Любая точка будет точкой пересечения!\n");
      return;
    }
  }
  float X, Y;
  if (!Eq(line1.x * line2.y, line2.x * line1.y)) {
    X = (line1.y * line2.c - line2.y * line1.c) /
        (line1.x * line2.y - line1.y * line2.x);
    Y = (line1.c * line2.x - line1.x * line2.c) /
        (line1.x * line2.y - line1.y * line2.x);
    printf("(%f;%f)\n", X, Y);
    return;
  } else {
    printf("Нет общих точек!\n");
    return;
  }
}

int main(int argc, char *const argv[]) {
srand(time(NULL));
    struct Line line1, line2;
    line1.x = GetRandom();
    line1.y = GetRandom();
    line1.c = GetRandom();
    line2.x = GetRandom();
    line2.y = GetRandom();
    line2.c = GetRandom();
    printf("Уравнения:\n1)%fx%c%fy%c%f\n2)%fx%c%fy%c%f\n", line1.x, (line1.y>0) ? '+' : '\0',line1.y, (line1.c>0) ? '+' : '\0', line1.c, 
                                                           line2.x, (line2.y>0) ? '+' : '\0',line2.y, (line2.c>0) ? '+' : '\0',line2.c);
    FindCommonDot(line1, line2);
    return 0;
}