#include "functions.h"

#include <math.h>
#include <stdio.h>

double CalculateCircleArea(double radius) { return (PI * pow(radius, 2.0)); }
double CalculateCirclePerimeter(double radius) { return (2.0 * PI * radius); }
double CalculateSphereVolume(double radius) {
  return ((4.0 / 3.0) * PI * pow(radius, 3.0));
}
