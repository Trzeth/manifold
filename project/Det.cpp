#include <iomanip>
#include <iostream>

#include "manifold/manifold.h"

using namespace manifold;

void dump_meshgl(const MeshGL &mesh) {
  std::cout << "MeshGL\n";
  std::cout << "NumVert = " << mesh.NumVert() << "\n";
  std::cout << "NumTri = " << mesh.NumTri() << "\n";
  std::cout << "numProp = " << mesh.numProp << "\n";
  std::cout << "vertProperties =";
  std::cout << std::fixed << std::setprecision(4);
  const int verts_per_row = 3;
  int nprop = mesh.numProp;
  for (int i = 0; i < mesh.vertProperties.size(); i++) {
    if ((i % (verts_per_row * nprop)) == 0) {
      std::cout << "\n[" << i << "] ";
    }
    if ((i % mesh.numProp == 0)) {
      std::cout << " / ";
    }
    std::cout << mesh.vertProperties[i] << " ";
  }
  std::cout << "\n";
  if (mesh.mergeFromVert.size() > 0) {
    std::cout << "vert merges: ";
    for (int i = 0; i < mesh.mergeFromVert.size(); i++) {
      if ((i % 10) == 0) {
        std::cout << "\n";
      }
      std::cout << mesh.mergeFromVert[i] << " -> " << mesh.mergeToVert[i]
                << ", ";
    }
    std::cout << "\n";
  }
  std::cout << "tris";
  const int tris_per_row = 5;
  const int ntrivert = mesh.triVerts.size();
  for (int i = 0; i < ntrivert; i++) {
    if ((i % (tris_per_row * 3)) == 0) {
      std::cout << "\n[" << i << "] ";
    }
    if ((i % 3) == 0) {
      std::cout << " / ";
    }
    std::cout << mesh.triVerts[i] << " ";
  }
  std::cout << "\n";
}

void cubecube() {
  Manifold cube1 = Manifold::Cube(vec3(2.0, 2.0, 2.0), true);
  Manifold cube2 = Manifold::Cube(vec3(2.0, 2.0, 2.0), true)
                       .Translate(vec3(-1.1091, 0.88509, 1.3099));
  // MeshGL cube1_mgl = cube1.GetMeshGL();
  // MeshGL cube2_mgl = cube2.GetMeshGL();

  // std::cout << "cube1:\n";
  // dump_meshgl(cube1_mgl);
  // std::cout << "\ncube2:\n";
  // dump_meshgl(cube2_mgl);

  Manifold result = cube1 - cube2;
  MeshGL mgl = result.GetMeshGL();
  std::cout << "\ncube1 - cube2:\n";
  // dump_meshgl(mgl);
}

int main() {
  cubecube();
  return 0;
}
