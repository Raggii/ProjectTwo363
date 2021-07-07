#ifndef H_Cone
#define H_Cone

#include "SceneObject.h"

/**
 * Defines a simple Sphere located at 'center'
 * with the specified radius
 */
class Cone : public SceneObject
{

private:
    glm::vec3 center;
    float radius = 1;
    float height = 1;

public:
    Cone() {};

    Cone(glm::vec3 c, float r, float h) : center(c), radius(r), height(h) {};

    float intersect(glm::vec3 pos, glm::vec3 dir);

    glm::vec3 normal(glm::vec3 p);

};

#endif //!H_Cone
