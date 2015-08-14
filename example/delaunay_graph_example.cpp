// Loba2d library

// Copyright (c) 2015 Andrii Sydorchuk

// Use, modification and distribution is subject to the MIT License (MIT).

#include <ctime>
#include <iostream>
#include <random>
#include <vector>

#include <boost/polygon/voronoi_builder.hpp>

#include "loba2d/delaunay_graph.hpp"

const int RANDOM_SEED = 27;
const int NUM_POINTS = 1000000;


int main() {
  std::mt19937 gen(RANDOM_SEED);
  boost::polygon::voronoi_builder<int> vb;
  loba2d::delaunay_graph dg;
  std::vector< std::pair<int, int> > input;

  for (int i = 0; i < NUM_POINTS; ++i) {
    int x = gen() & ((1 << 30) - 1);
    int y = gen() & ((1 << 30) - 1);
    input.push_back(std::make_pair(x, y));
    vb.insert_point(x, y);
  }

  double t = clock();
  vb.construct(&dg);
  double elapsed = (clock() - t) / 1E6;

  std::cout << "Delaunay index constructed in: " << elapsed << "(secs)" << std::endl;
  std::cout << "Num. vertices: " << dg.vertices().size() << std::endl;
  std::cout << "Num. triangles: " << dg.triangles().size() << std::endl;
  std::cout << "Num. half-edges: " << dg.edges().size() << std::endl;

  if (dg.num_triangles() <= 10) {
    std::cout << "Triangles coordinates:" << std::endl;
    for (loba2d::delaunay_graph::const_triangle_iterator it = dg.triangles().begin();
         it != dg.triangles().end(); ++it) {
      const loba2d::delaunay_graph::edge_type* e = it->incident_edge();
      std::cout << "[";
      for (int i = 0; i < 3; ++i) {
        std::size_t idx = e->vertex0()->source_index();
        std::cout << " (" << input[idx].first << ", " << input[idx].second << ")";
        e = e->next();
      }
      std::cout << " ]" << std::endl;
    }
  }
}
