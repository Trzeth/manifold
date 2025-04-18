#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

#include "Timer.h"
#include "manifold/manifold.h"
#include "manifold/meshIO.h"

int main(int argc, char *argv[]) {
  if (true) {
    std::string fileName[]{"0.3k_Cube-Sphere",
                           "3k6_397d470613e97b2e96b8e7fff37b4ac3",
                           "12k_PMRV030_56_B5(1)",
                           "32k_example2-case",
                           "340k_circularArray_nxexport",
                           "1.5m_circularArray_nxexport2",
                           "10000x10000_1000x50bump",
                           "10000x10000_1000x-5000bump"};

    float offset[]{12.32233744059949,
                   4.140973699979539,
                   15.7378524583248,
                   0.06928203230275509,
                   0.05531572871337743,
                   5.531572871337743,
                   10,
                   10};

    int segment[]{100, 20, 20, 20, 20, 20, 20, 20};

    if (argc != 2) throw std::exception();

    int index = std::stoi(std::string(argv[1]));

    // std::ifstream f;
    // f.open("../data/" + fileName[index] + ".stl");
    // manifold::Manifold import = manifold::Manifold::ImportMeshGL64(f);
    // f.close();

    manifold::Manifold import =
        manifold::ImportMesh("../data/" + fileName[index] + ".obj", true);
    import.GetTolerance();

    assert(import.Status() == manifold::Manifold::Error::NoError);

    manifold::Manifold sphere =
        manifold::Manifold::Sphere(offset[index], segment[index]);
    std::cout << index << std::endl;

    TIC()

    manifold::Manifold r = import.MinkowskiSum(sphere);
    TOC()

    manifold::ExportMesh("../data/" + fileName[index] + "_output.obj",
                         r.GetMeshGL(), manifold::ExportOptions());

  } else {
    manifold::Manifold sphere = manifold::Manifold::Sphere(1.2, 20);
    manifold::Manifold cube = manifold::Manifold::Cube({2.0, 2.0, 2.0}, true);
    manifold::Manifold nonConvex = cube - sphere;

    manifold::ExportMesh("../data/test.glb", nonConvex.GetMeshGL(),
                         manifold::ExportOptions());

    manifold::Manifold import = manifold::ImportMesh("../data/test.glb");
    assert(import.Status() == manifold::Manifold::Error::NoError);
  }

  return 0;
}