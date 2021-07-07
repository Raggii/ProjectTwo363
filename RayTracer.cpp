/*==================================================================================
* COSC 363  Computer Graphics (2021)
* Department of Computer Science and Software Engineering, University of Canterbury.
*
* A basic ray tracer
* See Lab07.pdf  for details.
*===================================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "SceneObject.h"
#include "Ray.h"
#include <GL/freeglut.h>
#include "Plane.h"
#include "Cone.h"
#include "Cylinder.h"
#include "TextureBMP.h"
#include <cstdlib> // for random
using namespace std;

TextureBMP texture;

const float WIDTH = 20.0;
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 500;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;

float angle;
float normaliseRayHit;
float normaliseDirectionRay;

vector<SceneObject*> sceneObjects;



//---The most important function in a ray tracer! ----------------------------------
//   Computes the colour value obtained by tracing a ray and finding its
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
    glm::vec3 backgroundCol(0);                     //Background colour = (0,0,0)
    glm::vec3 lightPosMainOne(-20, 100, -3);                 //Light's position
    glm::vec3 lightPosMainTwo(20, 100, -3);                 //Light's position
    glm::vec3 color(0);
    SceneObject* obj;

    ray.closestPt(sceneObjects);                    //Compare the ray with all objects in the scene
    if(ray.index == -1) return backgroundCol;       //no intersection
    obj = sceneObjects[ray.index];                  //object on which the closest point of intersection is found

    color = obj->lighting(lightPosMainOne, -ray.dir, ray.hit);

    // ****************************************************
    //
    // Stars - will change back planes black to white with a random funciton
    // Procedural pattern
    //
    //*****************************************************

    if(ray.index == 13){
        int randomNumber = rand() % 51;
        if(randomNumber == 50) {
            color = glm::vec3(1,1,1);
            obj->setColor(color);
        } else {

            color = glm::vec3(0,0,0);
            obj->setColor(color);
        }
    }



    // ****************************************************
    //
    // Textures
    // on non planar object
    //
    //*****************************************************
    if(ray.index == 11) { // The texture for the moon
        glm::vec3 normalVec = obj->normal(ray.hit);

        float texcoordt = atan2(normalVec.x, normalVec.z)/(2*M_PI) + 0.5;
        float texcoords =  -asin(normalVec.y)/ M_PI + 0.5;
        if(texcoords > 0 && texcoords < 1 && texcoordt > 0 && texcoordt < 1) {

            color=texture.getColorAt(texcoords, texcoordt);
            obj->setColor(color);
        }

    }

    // ****************************************************
    //
    // Checkered pattern
    //
    //*****************************************************


        if (ray.index == 9 || ray.index == 10) { // for the left hand side

            int stripeWidthRed = 5;
            int stripeWidthWhite = 2;
            int iz = (ray.hit.z) / stripeWidthRed;
            int k = iz % 2;
            int ix = (ray.hit.x) / stripeWidthWhite;
            int j = ix % 2;
            if(ray.hit.x > 0) {
                if (k == 0)
                {
                    color = glm::vec3(.4,0,0);
                    if(j == 0) {
                        color = glm::vec3(.8,.8,.8);
                        }
                    //}
                } else {
                    color = glm::vec3(.8,.8,.8);
                    if(j == 0) {
                        color = glm::vec3(.4,0,0);
                    }
                }
            } else {
                if (k == 0)
                {
                    color = glm::vec3(.8,.8,.8);
                    if(j == 0) {
                        color =  glm::vec3(.4,0,0);
                        }
                } else {
                    color =  glm::vec3(.4,0,0);
                    if(j == 0) {
                        color = glm::vec3(.8,.8,.8);
                    }
                }
            }
        obj->setColor(color);
    }


    // ****************************************************
    //
    // Lights and Shadows
    //
    //*****************************************************


    glm::vec3 lightVecMainOne = lightPosMainOne -ray.hit;
    Ray shadowRayOne(ray.hit, lightVecMainOne);

    glm::vec3 lightVecMainTwo = lightPosMainTwo -ray.hit;
    Ray shadowRayTwo(ray.hit, lightVecMainTwo);

    shadowRayOne.closestPt(sceneObjects);
    shadowRayTwo.closestPt(sceneObjects);

    float shadowValue;
    if(shadowRayOne.index > -1 && sceneObjects[shadowRayOne.index]->isRefractive()) {
        if(shadowRayTwo.index > -1 && sceneObjects[shadowRayTwo.index]->isRefractive()) {
            shadowValue = 0.1f;
        } else {
            shadowValue = 0;
        }
    }else {
        shadowValue = 0;
    }

    if(shadowRayOne.index > -1 && shadowRayTwo.index > -1 ) {
        color = (0.2f - shadowValue) *obj->getColor();
    } else if(shadowRayOne.index > -1) {
        color = (0.4f - shadowValue) *obj->getColor();
    } else if(shadowRayTwo.index > -1 ) {
        color = (0.4f - shadowValue) *obj->getColor();
    }


    // ****************************************************
    //
    // Reflectivity
    //
    //*****************************************************

    if (obj->isReflective() && step <MAX_STEPS)
    {
        float rho = obj->getReflectionCoeff();
        glm::vec3 normalVec = obj->normal(ray.hit);
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVec);
        Ray reflectedRay(ray.hit, reflectedDir);
        glm::vec3 reflectedColor = trace(reflectedRay, step + 1);
        color = color + (rho*reflectedColor);

    }

    // ****************************************************
    //
    // Transparent
    //
    //*****************************************************

     if (obj->isTransparent() && step <MAX_STEPS)
    {
        float eta = 1/1.00;
        float rho = obj->getRefractionCoeff();
        glm::vec3 normalVec = obj->normal(ray.hit);
        glm::vec3 g = glm::refract(ray.dir,normalVec,eta);

        Ray refrRay(ray.hit, g);
        refrRay.closestPt(sceneObjects);
        //cout << refrRay.hit.x << "\n";

        glm::vec3 m = obj->normal(refrRay.hit);
        glm::vec3 h = glm::refract(g,-m,1.0f/eta);
        Ray refrRay2(refrRay.hit, h);
        glm::vec3 refractedColor = trace(refrRay2, step + 1);
        color = refractedColor + color;
    }


    // ****************************************************
    //
    // Refractive
    //
    //*****************************************************

     if (obj->isRefractive() && step <MAX_STEPS)
    {
        float eta = 1/1.5;
        float rho = obj->getRefractionCoeff();
        glm::vec3 normalVec = obj->normal(ray.hit);
        glm::vec3 g = glm::refract(ray.dir,normalVec,eta);
        Ray refrRay(ray.hit, g);
        refrRay.closestPt(sceneObjects);
        glm::vec3 m = obj->normal(refrRay.hit);
        glm::vec3 h = glm::refract(g,-m,1.0f/eta);
        Ray refrRay2(refrRay.hit, h);
        glm::vec3 refractedColor = trace(refrRay2, step + 1);
        color = refractedColor + color;
    }



    // ****************************************************
    //
    // Spotlight
    //
    //*****************************************************

    glm::vec3 sorce = glm::vec3(-20,0,-70); //the right hand side lamp post
    glm::vec3 direction = glm::vec3(0,-1,-1);
    //glm::vec3 normalisedRay


    glm::vec3 normRay = glm::normalize(ray.hit-lightPosMainOne);

    float lengthRay = glm::length(normRay);
    float lengthDirec = glm::length(direction);


    float spotRayDotDirection =  glm::dot(glm::normalize(ray.hit-lightPosMainOne), direction);


    angle = acos(spotRayDotDirection/(lengthRay * lengthDirec)) * (180/M_PI);
    if(angle <=5) {

         color = obj->getColor() * 1.1f;
    }


    return color;
}




//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
    float xp, yp;  //grid point
    float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
    float cellY = (YMAX-YMIN)/NUMDIV;  //cell height
    glm::vec3 eye(0., 0., 0.);

    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    SceneObject *s = new Sphere(glm::vec3(0, 5, -100), 20);
    s->setColor(glm::vec3(0, 1, 0));


    glBegin(GL_QUADS);  //Each cell is a tiny quad.
    for(int i = 0; i < NUMDIV; i++) //Scan every cell of the image plane
    {
        xp = XMIN + i*cellX;
        for(int j = 0; j < NUMDIV; j++)
        {
            yp = YMIN + j*cellY;

    // ****************************************************
    //
    // Anti Aliasing
    //
    //****************************************************
            glm::vec3 dir(xp+0.75*cellX, yp+0.75*cellY, -EDIST);
            glm::vec3 dirTwo(xp+0.75*cellX, yp+0.25*cellY, -EDIST);
            glm::vec3 dirThree(xp+0.25*cellX, yp+0.25*cellY, -EDIST);
            glm::vec3 dirFour(xp+0.25*cellX, yp+0.75*cellY, -EDIST);

            Ray ray = Ray(eye, dir);
            Ray rayTwo = Ray(eye, dirTwo);
            Ray rayThree = Ray(eye, dirThree);
            Ray rayFour = Ray(eye, dirFour);

            glm::vec3 col = trace (ray, 1);
            glm::vec3 colTwo = trace (rayTwo, 1);
            glm::vec3 colThree = trace (rayThree, 1);
            glm::vec3 colFour = trace (rayFour, 1);

            // then average all the colours to get less pixelated

            glm::vec3 colourAverage = (col + colTwo + colThree + colFour) / glm::vec3(4,4,4);

            glColor3f(colourAverage.r, colourAverage.g, colourAverage.b);
            glVertex2f(xp, yp);             //Draw each cell with its color value
            glVertex2f(xp+cellX, yp);
            glVertex2f(xp+cellX, yp+cellY);
            glVertex2f(xp, yp+cellY);
        }
    }

    glEnd();
    glFlush();
}


// ***************************************************************************************************************************
//
// Features added
//
//****************************************************************************************************************************



void lightPost(int xDirection, int zDirection, bool trans)
{

    Cone *cone = new Cone(glm::vec3(xDirection,2,zDirection), 5, 8); // need to normalise theese values
    cone->setColor(glm::vec3(0.5,.5,.5));
    sceneObjects.push_back(cone);

    Cylinder *cylinder = new Cylinder(glm::vec3(xDirection,-15,zDirection), 1, 12); // on the ground up to given height
    cylinder->setColor(glm::vec3(0.5,.5,.5));
    sceneObjects.push_back(cylinder);
    cylinder->setShininess(5);

    Sphere *sphere = new Sphere(glm::vec3(xDirection,0,zDirection), 3);
    sphere->setColor(glm::vec3(0,0,0));
    sceneObjects.push_back(sphere);
    sphere->setShininess(5);
    if(trans) {
        sphere->setTransparency(true);
    } else {
        sphere->setRefractivity(true,.9,1.5);
    }
}


void building()
{


    Plane *rightSide =
        new Plane(glm::vec3(0,10,-100),
        glm::vec3(0,-15,-100),
        glm::vec3(15,-15,-140),
        glm::vec3(15,10,-140));
        rightSide->setColor(glm::vec3(.6,.6,.6));

    sceneObjects.push_back(rightSide);

    Plane *leftSide =
        new Plane(glm::vec3(-15,10,-140),
        glm::vec3(-15,-15,-140),
        glm::vec3(0,-15,-100),
        glm::vec3(0,10,-100));
        leftSide->setColor(glm::vec3(.6,.6,.6));

    sceneObjects.push_back(leftSide);

    Plane *backSide =
        new Plane(glm::vec3(15,10,-140),
        glm::vec3(15,-15,-140),
        glm::vec3(-15,-15,-140),
        glm::vec3(-15,10,-140));
        backSide->setColor(glm::vec3(.6,.6,.6));

    sceneObjects.push_back(backSide);

    Plane *roofRight =
        new Plane(glm::vec3(-15,10,-140),
        glm::vec3(0,10,-100),
        glm::vec3(0,20,-120));
        roofRight->setColor(glm::vec3(.6,.6,.6));

    sceneObjects.push_back(roofRight);

    Plane *roofLeft =
        new Plane(glm::vec3(0,10,-100),
        glm::vec3(15,10,-140),
        glm::vec3(0,20,-120));
        roofLeft->setColor(glm::vec3(.6,.6,.6));

    sceneObjects.push_back(roofLeft);



}


void theMoon()
{

    Sphere *moon = new Sphere(glm::vec3(-30.0, 30, -150.0), 6.0);
    //sphere1->setColor(glm::vec3(1,0,1));   //Set colour to blue
    sceneObjects.push_back(moon);         //Add sphere to scene objects
    moon->setShininess(5);

}

void skyBox()
{

    Plane *bottomPlane =
        new Plane(glm::vec3(-200., -15, -40),
        glm::vec3(200., -15, -40),
        glm::vec3(200., -15, -200),
        glm::vec3(-200., -15, -200));
        bottomPlane->setColor(glm::vec3(0,.5,0));

    sceneObjects.push_back(bottomPlane);
    bottomPlane->setSpecularity(false); // Sets the floor plane non spec


    Plane *backPlane =
        new Plane(glm::vec3(200., -15, -200),
        glm::vec3(200., 100, -200),
        glm::vec3(-200., 100, -200),
        glm::vec3(-200., -15, -200));
        backPlane->setColor(glm::vec3(0,0,0));

    sceneObjects.push_back(backPlane);
    backPlane->setSpecularity(false); // Sets the floor plane non spec



}


// The two mirror on the side of the building
void refleciveMirror()
{

    Plane *rightSideMirror =
        new Plane(glm::vec3(0,5,-99.9),
        glm::vec3(0,-5,-99.9),
        glm::vec3(15,-5,-139.9),
        glm::vec3(15,5,-139.9));
        rightSideMirror->setColor(glm::vec3(0.25,0.25,0.25));
    rightSideMirror->setReflectivity(true, .9);
    sceneObjects.push_back(rightSideMirror);


}


//---This function initializes the scene -------------------------------------------
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);

    glClearColor(0, 0, 0, 1);


    lightPost(-13, -75, false);
    lightPost(20,-130, true);

    building();

    texture = TextureBMP("moonText.bmp");
    theMoon();

    skyBox();

    refleciveMirror();

}


int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracing");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
