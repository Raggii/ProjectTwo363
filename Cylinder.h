#ifndef H_Cylinder
#define H_Cylinder

#include "SceneObject.h"

/**
This file Holds
 */
class Cylinder : public SceneObject
{

private:

    glm::vec3 center;
    float radius = 1;
    float height = 1;

public:
    Cylinder() {};

    Cylinder(glm::vec3 c, float r, float h): center(c), radius(r), height(h) {};

    float intersect(glm::vec3 pos, glm::vec3 dir);

    glm::vec3 normal(glm::vec3 p);


};

#endif //!H_BOX
