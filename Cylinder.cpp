#include "Cylinder.h"
#include <math.h>

float Cylinder::intersect(glm::vec3 pos, glm::vec3 dir)
{
    float a = (dir.x * dir.x) + (dir.z * dir.z);
    float b = 2*(dir.x*(pos.x-center.x) + dir.z*(pos.z-center.z));
    float c = (pos.x - center.x) * (pos.x - center.x) + (pos.z - center.z) * (pos.z - center.z) - (radius*radius);

    float delta = b*b - 4*(a*c);
    if(fabs(delta) < 0.001) return -1.0;
    if(delta < 0.0) return -1.0;

    float t1 = (-b - sqrt(delta))/(2*a);
    float t2 = (-b + sqrt(delta))/(2*a);
    float t;

    if (t1>t2) t = t2;
    else t = t1;

    float r = pos.y + t*dir.y;

    if ((r >= center.y) and (r <= center.y + height))return t;
    else return -1;
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the sphere.
*/
glm::vec3 Cylinder::normal(glm::vec3 p)
{
    glm::vec3 n = glm::vec3 (p.x-center.x,0,p.z-center.z);
    glm::vec3 nNormal = glm::vec3(n.x/radius,n.y/radius,n.z/radius);
    return nNormal;
}
