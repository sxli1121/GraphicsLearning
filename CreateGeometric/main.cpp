#include <iostream>
#include "MeshData.h"

int main()
{
    MeshData test;
    //test.CreateStar(1.0f, 0.5f, 0.4f,9);
    //test.CreateSphere_s4(1,4);
    //test.CreateSphere_s6(1, 8);
    //test.CreateSphere_s8(1, 4);
    //test.CreateSphere_s12(1, 3);
    //test.CreateSphere_s20(1, 3);
    //test.CreateSphere_uv(1, 10, 20);
    //test.CreateCapsule(0.5f, 1,6);
    //test.CreateRing(0.5f,1,12,20);
    //test.CreatePlane(3,3,5);
    test.Save2File("../GraphicsLearning/res/mesh/CreateSphere_s12.txt");
    return 0;
}
