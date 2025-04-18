#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

#include "../src/impl.h"
#include "Timer.h"
#include "manifold/cross_section.h"
#include "manifold/manifold.h"
#include "manifold/meshIO.h"

namespace manifold {

std::ostream& operator<<(std::ostream& s, const manifold::vec3& v) {
  s << "(" << v.x << ", " << v.y << ", " << v.z << ")";
  return s;
}

manifold::Manifold Fillet(const MeshGL& meshGL, double radius,
                          const std::vector<size_t>& halfedge = {}) {
  std::shared_ptr<Manifold::Impl> impl =
      std::make_shared<Manifold::Impl>(meshGL);

  manifold::Manifold m;

  auto vec = impl->halfedge_;

  // Map to sorted idx
  std::unordered_map<int, int> oldHalfedge2New;
  for (size_t tri = 0; tri < impl->NumTri(); ++tri) {
    int oldTri = impl->meshRelation_.triRef[tri].tri;
    for (int i : {0, 1, 2}) oldHalfedge2New[3 * oldTri + i] = 3 * tri + i;
  }
  std::vector<size_t> newEdge = halfedge;
  for (size_t& edge : newEdge) {
    edge = oldHalfedge2New[edge];
  }

  // One range neighbour vertex except endVert
  auto getNeighbour = [](const size_t& halfedge,
                         const Vec<Halfedge>& vec) -> std::vector<int> {
    std::vector<int> r;
    int tri = halfedge / 3;
    int idx = halfedge % 3;

    while (true) {
      const int next = vec[tri * 3 + (idx + 1) % 3].pairedHalfedge;
      if (next == halfedge) break;

      r.push_back(vec[next].startVert);

      tri = next / 3;
      idx = next % 3;
    };

    return r;
  };

  for (const size_t& halfedge : newEdge) {
    auto r1 = getNeighbour(halfedge, impl->halfedge_);
    auto r2 =
        getNeighbour(impl->halfedge_[halfedge].pairedHalfedge, impl->halfedge_);

    std::vector<ivec3> f;
    for (int i = 0; i != r1.size() - 1; i++) {
      f.emplace_back(r1[0], r1[1], impl->halfedge_[halfedge].endVert);
    }

    for (int i = 0; i != r2.size() - 1; i++) {
      f.emplace_back(r2[0], r2[1], impl->halfedge_[halfedge].endVert);
    }

    const auto& edge = vec[halfedge];
    const vec3 target =
        impl->vertPos_[edge.startVert] - impl->vertPos_[edge.endVert];

    // FIXME: The cylinder length should be determine by r1 neighbour, current
    // set manually
    float len = la::length(target) * 2;

    /*
            v4
           / \
          /   \    <- f2
         /  n2 \
        v1 --- v2  <- Half edge
         \  n1 /
          \   /    <- f1
           \ /
            v3
    */

    vec3 v1v2 = impl->vertPos_[edge.endVert] - impl->vertPos_[edge.startVert],
         v1v3 = impl->vertPos_[*r1.begin()] - impl->vertPos_[edge.startVert],
         v1v4 = impl->vertPos_[*r1.end()] - impl->vertPos_[edge.startVert];

    vec3 v1, v2, v3 = impl->vertPos_[*r1.begin()],
                 v4 = impl->vertPos_[*r1.end()];

    // Inv normal
    vec3 n1 = la::normalize(la::cross(v1v2, v1v3)),
         n2 = la::normalize(la::cross(v1v4, v1v2));

    // Cross line
    vec3 n3 = la::normalize(v1v2);
    float A1 = n1.x, B1 = n1.y, C1 = n1.z,
          D1 = A1 * -v3.x + B1 * -v3.y + C1 * -v3.z -
               radius * std::sqrt(A1 * A1 + B1 * B1 * +C1 * C1),
          D3 = A1 * -v3.x + B1 * -v3.y + C1 * -v3.z;

    float A2 = n2.x, B2 = n2.y, C2 = n2.z,
          D2 = A2 * -v4.x + B2 * -v4.y + C2 * -v4.z -
               radius * std::sqrt(A2 * A2 + B2 * B2 * +C2 * C2),
          D4 = A2 * -v4.x + B2 * -v4.y + C2 * -v4.z;

    vec3 p;
    if ((A1 * B2 - A2 * B1) > impl->epsilon_) {
      p = vec3((B2 * D1 - B1 * D2) / (A1 * B2 - A2 * B1),
               (A1 * D2 - A2 * D1) / (A1 * B2 - A2 * B1), 0);
    }

    vec3 p1 = p + n3;

    double det1 = 0, det2 = 0;

    // Determine whether intersect with original tri
    {
      // Project p, p1 to f1, f2

      vec3 PF1p = p - (n1 * p + D3) / (la::length2(n1)) * n1,
           PF1p1 = p1 - (n1 * p1 + D3) / (la::length2(n1)) * n1,
           PF2p = p - (n2 * p + D4) / (la::length2(n2)) * n2,
           PF2p1 = p1 - (n2 * p1 + D4) / (la::length2(n2)) * n2;

      vec3 e1 = la::cross(PF1p - PF1p1, PF1p - v1);
      det1 = la::dot(e1, la::cross(PF1p - PF1p1, PF1p - v2)) *
             la::dot(e1, la::cross(PF1p - PF1p1, PF1p - v3));

      vec3 e2 = la::cross(PF2p - PF2p1, PF2p - v1);
      det2 = la::dot(e2, la::cross(PF2p - PF2p1, PF1p - v2)) *
             la::dot(e2, la::cross(PF2p - PF2p1, PF1p - v4));
    }

    if (det1 <= impl->epsilon_ && det2 <= impl->epsilon_) {
      // Inside tri

      // Create cylinder
      Manifold cylinder = Manifold::Cylinder(len, radius);

      cylinder.Rotate(la::angle(vec3(1, 0, 0), target));

      // v1 project to cross line

      vec3 pv1 = (v1 - p);
      double t = -la::dot(pv1, n3) / la::dot(n3, n3);
      vec3 origin = p + t * n3;

      // Offset origin by len

      origin -= len / 4 * n3;

      cylinder.Translate(origin);

      // Cut with r1 neighbour

      for (ivec3 v : f) {
        vec3 ab = impl->vertPos_[v.x] - impl->vertPos_[v.y];
        vec3 ac = impl->vertPos_[v.z] - impl->vertPos_[v.y];
        vec3 normal = la::cross(ab, ac);
        double off = la::dot(normal, impl->vertPos_[v.x]);

        // Transform to origin and normal
        cylinder = cylinder.SplitByPlane(normal, off).second;
      }

      // TODO: determine cross edge and union boundary
      m.Split(cylinder);

    } else {
      // Radius out range
      throw std::exception();
    }
  }

  return manifold::Manifold(impl);
}

}  // namespace manifold

int main() {
  manifold::Manifold import = manifold::ImportMesh("../data/Cube.obj", true);
  import.GetTolerance();

  assert(import.Status() == manifold::Manifold::Error::NoError);

  manifold::Fillet(import.GetMeshGL(), std::vector<size_t>{0});
}