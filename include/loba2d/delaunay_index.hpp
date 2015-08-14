// Loba2d library

// Copyright (c) 2015 Andrii Sydorchuk

// Use, modification and distribution is subject to the MIT License (MIT).

#ifndef LOBA2D_DELAUNAY_INDEX
#define LOBA2D_DELAUNAY_INDEX

#include <utility>
#include <vector>

namespace loba2d {

class delaunay_index_triangle {
 public:
  typedef std::size_t source_index_type;

  delaunay_index_triangle(
    source_index_type idx1,
    source_index_type idx2,
    source_index_type idx3
  ) {
    source_index_[0] = idx1;
    source_index_[1] = idx2;
    source_index_[2] = idx3;
  }

  source_index_type source_index(std::size_t vertex_index) const {
    return source_index_[vertex_index];
  }

 private:
  source_index_type source_index_[3];
};

class delaunay_index {
 public:
  typedef delaunay_index_triangle triangle_type;
  typedef std::vector<triangle_type> triangle_container_type;
  typedef triangle_container_type::const_iterator const_triangle_iterator;

  delaunay_index() {}

  const triangle_container_type& triangles() const {
    return triangles_;
  }

  std::size_t num_triangles() const {
    return triangles_.size();
  }

  void _reserve(std::size_t num_sites) {
    triangles_.reserve(num_sites << 1);
  }

  template <typename SITE>
  void _process_single_site(const SITE& site) {}

  template <typename SITE>
  std::pair<void*, void*> _insert_new_edge(const SITE& site1, const SITE& site2) {
    return std::make_pair(
        reinterpret_cast<void*>(site1.initial_index()),
        reinterpret_cast<void*>(site2.initial_index())
    );
  }

  template <typename SITE, typename CIRCLE>
  std::pair<void*, void*> _insert_new_edge(
      const SITE& site1, const SITE& site3, const CIRCLE& circle,
      void* data12, void* data23) {

    triangles_.push_back(
        triangle_type(
            reinterpret_cast<std::size_t>(data23),
            site1.initial_index(),
            site3.initial_index()
        )
    );
    return std::make_pair(
        reinterpret_cast<void*>(site1.initial_index()),
        reinterpret_cast<void*>(site3.initial_index())
    );
  }

  void _build() {}

 private:
  triangle_container_type triangles_;

  // Disallow copy constructor and operator=
  delaunay_index(const delaunay_index&);
  void operator=(const delaunay_index&);
};

}  // loba2d

#endif  // LOBA2D_DELAUNAY_INDEX
