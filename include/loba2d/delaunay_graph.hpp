// Loba2d library

// Copyright (c) 2015 Andrii Sydorchuk

// Use, modification and distribution is subject to the MIT License (MIT).

#ifndef LOBA2D_DELAUNAY_GRAPH
#define LOBA2D_DELAUNAY_GRAPH

#include <utility>
#include <vector>

namespace loba2d {

// Forward declaration.
class delaunay_graph_edge;

class delaunay_graph_vertex {
 public:
  typedef std::size_t source_index_type;
  typedef delaunay_graph_edge edge_type;

  delaunay_graph_vertex(source_index_type source_index) :
      source_index_(source_index),
      incident_edge_(NULL) {}

  source_index_type source_index() const {
    return source_index_;
  }

  edge_type* incident_edge() { return incident_edge_; }
  const edge_type* incident_edge() const { return incident_edge_; }
  void incident_edge(edge_type* e) { incident_edge_ = e; }

 private:
  source_index_type source_index_;
  edge_type* incident_edge_;
};

class delaunay_graph_triangle {
 public:
  typedef delaunay_graph_edge edge_type;

  delaunay_graph_triangle() : incident_edge_(NULL) {}

  edge_type* incident_edge() { return incident_edge_; }
  const edge_type* incident_edge() const { return incident_edge_; }
  void incident_edge(edge_type* e) { incident_edge_ = e; }

 private:
  edge_type* incident_edge_;
};

class delaunay_graph_edge {
 public:
  typedef delaunay_graph_vertex vertex_type;
  typedef delaunay_graph_edge edge_type;

  delaunay_graph_edge() :
      vertex_(NULL),
      twin_(NULL),
      next_(NULL) {}

  vertex_type* vertex0() { return vertex_; }
  const vertex_type* vertex0() const { return vertex_; }
  void vertex0(vertex_type* v) { vertex_ = v; }

  vertex_type* vertex1() { return twin_->vertex_; }
  const vertex_type* vertex1() const { return twin_->vertex_; }

  edge_type* twin() { return twin_; }
  const edge_type* twin() const { return twin_; }
  void twin(edge_type* e) { twin_ = e; }

  edge_type* next() { return next_; }
  const edge_type* next() const { return next_; }
  void next(edge_type* e) { next_ = e; }

  edge_type* prev() { return twin_->next_; }
  const edge_type* prev() const { return twin_->next_; }

 private:
  vertex_type* vertex_;
  edge_type* twin_;
  edge_type* next_;
};

class delaunay_graph {
 public:
  typedef delaunay_graph_vertex vertex_type;
  typedef std::vector<vertex_type> vertex_container_type;
  typedef vertex_container_type::const_iterator const_vertex_iterator;

  typedef delaunay_graph_triangle triangle_type;
  typedef std::vector<triangle_type> triangle_container_type;
  typedef triangle_container_type::const_iterator const_triangle_iterator;

  typedef delaunay_graph_edge edge_type;
  typedef std::vector<edge_type> edge_container_type;
  typedef edge_container_type::const_iterator const_edge_iterator;

  delaunay_graph() {}

  const vertex_container_type& vertices() const {
    return vertices_;
  }

  const triangle_container_type& triangles() const {
    return triangles_;
  }

  const edge_container_type& edges() const {
    return edges_;
  }

  std::size_t num_vertices() const {
    return vertices_.size();
  }

  std::size_t num_triangles() const {
    return triangles_.size();
  }

  std::size_t num_edges() const {
    return edges_.size();
  }

  void _reserve(std::size_t num_sites) {
    vertices_.reserve(num_sites);
    triangles_.reserve(num_sites << 1);
    edges_.reserve((num_sites << 2) + (num_sites << 1));
  }

  template <typename SITE>
  void _process_single_site(const SITE& site) {
    vertices_.push_back(vertex_type(site.initial_index()));
  }

  template <typename SITE>
  std::pair<void*, void*> _insert_new_edge(const SITE& site1, const SITE& site2) {
    // Get sites' indexes.
    std::size_t site_index1 = site1.sorted_index();
    std::size_t site_index2 = site2.sorted_index();

    // Create a new half-edge that belongs to the first site.
    edges_.push_back(edge_type());
    edge_type& edge1 = edges_.back();

    // Create a new half-edge that belongs to the second site.
    edges_.push_back(edge_type());
    edge_type& edge2 = edges_.back();

    // Add the initial vertex during the first edge insertion.
    if (vertices_.empty()) {
      vertices_.push_back(vertex_type(site1.initial_index()));
      vertices_.back().incident_edge(&edge1);
    }

    // The second site represents a new site during site event
    // processing. Add a new vertex to the vertex records.
    vertices_.push_back(vertex_type(site2.initial_index()));
    vertices_.back().incident_edge(&edge2);

    // Set up pointers to vertices.
    edge1.vertex0(&vertices_[site_index1]);
    edge2.vertex0(&vertices_[site_index2]);

    // Set up twin pointers.
    edge1.twin(&edge2);
    edge2.twin(&edge1);

    // Return a pointer to the new half-edge.
    return std::make_pair(&edge2, &edge1);
  }

  template <typename SITE, typename CIRCLE>
  std::pair<void*, void*> _insert_new_edge(
      const SITE& site1, const SITE& site3, const CIRCLE& circle,
      void* data12, void* data23) {
    edge_type* edge12 = static_cast<edge_type*>(data12);
    edge_type* edge23 = static_cast<edge_type*>(data23);

    // Add a new triangle.
    triangles_.push_back(triangle_type());
    triangle_type& triangle = triangles_.back();
    triangle.incident_edge(edge12);

    // Add a new half-edge.
    edges_.push_back(edge_type());
    edge_type& new_edge1 = edges_.back();
    new_edge1.vertex0(&vertices_[site1.sorted_index()]);

    // Add a new half-edge.
    edges_.push_back(edge_type());
    edge_type& new_edge2 = edges_.back();
    new_edge2.vertex0(&vertices_[site3.sorted_index()]);

    // Update twin pointers.
    new_edge1.twin(&new_edge2);
    new_edge2.twin(&new_edge1);

    // Update next pointers.
    edge23->next(edge12);
    edge12->next(&new_edge1);
    new_edge1.next(edge23);

    // Return a pointer to the new half-edge.
    return std::make_pair(&new_edge2, &new_edge1);
  }

  void _build() {}

 private:
  vertex_container_type vertices_;
  triangle_container_type triangles_;
  edge_container_type edges_;

  // Disallow copy constructor and operator=
  delaunay_graph(const delaunay_graph&);
  void operator=(const delaunay_graph&);
};

}  // loba2d

#endif  // LOBA2D_DELAUNAY_GRAPH
