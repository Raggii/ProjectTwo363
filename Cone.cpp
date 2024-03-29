#include "Cone.h"
#include <math.h>

float Cone::intersect(glm::vec3 pos, glm::vec3 dir)
{
    float A = pos.x - center.x;
    float B = pos.z - center.z;
    float D = height - pos.y + center.y;

    float tan = (radius / height) * (radius / height);

    float a = (dir.x * dir.x) + (dir.z * dir.z) - (tan*(dir.y * dir.y));
    float b = (2*A*dir.x) + (2*B*dir.z) + (2*tan*D*dir.y);
    float c = (A*A) + (B*B) - (tan*(D*D));

    float delta = b*b - 4*(a*c);
    if(fabs(delta) < 0.001) return -1.0;
    if(delta < 0.0) return -1.0;

    float t1 = (-b - sqrt(delta))/(2*a);
    float t2 = (-b + sqrt(delta))/(2*a);
    float t;

    if (t1>t2) t = t2;
    else t = t1;

    float r = pos.y + t*dir.y;

    if ((r > center.y) and (r < center.y + height)) return t;
    else return -1;
}
/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the sphere.
*/
glm::vec3 Cone::normal(glm::vec3 p)
{
    float r = sqrt((p.x-center.x)*(p.x-center.x) + (p.z-center.z)*(p.z-center.z));
    glm::vec3 n = glm::vec3 (p.x-center.x, r*(radius/height), p.z-center.z);

    float normValue = sqrt(n.x*n.x + n.y*n.y + n.z*n.z);
    glm::vec3 normN = glm::vec3(n.x/normValue, n.y/normValue, n.z/normValue);
    return normN;
}
