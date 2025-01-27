/*
   Copyright (C) 2019-2022 by Synge Todo <wistaria@phys.s.u-tokyo.ac.jp>

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef LATTICE_UNITCELL_HPP
#define LATTICE_UNITCELL_HPP

#include <exception>
#include <vector>
#include <set>
#include "types.hpp"
#include "coordinate.hpp"
#include "offset.hpp"

namespace lattice {

  using VS = std::vector<std::size_t>;
  
  // using VOFF = std::vector<offset_t>;
class unitcell {
public:
  struct site_t {
    site_t() {}
    site_t(const coordinate_t& pos, int tp) : coordinate(pos), type(tp) {}
    coordinate_t coordinate;
    int type;
  };
  
  struct bond_t {
    bond_t() {}
    bond_t(std::size_t s, std::size_t t, offset_t os, int tp) :
      source(s), target(t), target_offset(os), type(tp) {}
    std::size_t source, target;
    offset_t target_offset;
    int type;
  };

  struct multi_t {
    multi_t() {}
    multi_t(std::size_t s, VS t_v, std::vector<offset_t> os_v, int tp) :
      source(s), target_v(t_v), target_offset_v(os_v), type(tp) {}
    size_t num_act() const {return target_v.size() + 1;}
    std::size_t source;
    VS target_v;
    std::vector<offset_t> target_offset_v;
    int type;
  };

  unitcell() {}
  unitcell(std::size_t dim) : dim_(dim) {}
  // unitcell(const unitcell& cell, const extent_t& extent);
  // unitcell(const unitcell& cell, const span_t& span);

  std::size_t dimension() const { return dim_; }
  std::size_t num_sites() const { return sites_.size(); }
  std::size_t num_bonds() const { return bonds_.size(); }
  std::size_t num_multis() const { return multis_.size(); }
  const site_t& site(std::size_t s) const { return sites_[s]; }
  const bond_t& bond(std::size_t b) const { return bonds_[b]; }
  const multi_t& multi(std::size_t b) const { return multis_[b]; }
  std::size_t max_neighbors() const {
    std::vector<std::size_t> num_neighbors(num_sites(), 0);
    for (std::size_t b = 0; b < num_bonds(); ++b) {
      num_neighbors[bonds_[b].source] += 1;
      num_neighbors[bonds_[b].target] += 1;
    }
    return *std::max_element(num_neighbors.begin(), num_neighbors.end());
  }
  
  std::size_t add_site(const coordinate_t& pos, int tp) {
    if (std::size_t(pos.size()) != dimension())
      throw std::invalid_argument("site coordinate dimension mismatch");
    for (std::size_t i = 0; i < dimension(); ++i) {
      if (pos[i] < 0 || pos[i] >= 1.0)
        throw std::invalid_argument("site coordinate out of range");
    }
    std::size_t s = sites_.size();
    sites_.push_back(site_t(pos, tp));
    return s;
  }
    
  std::size_t add_bond(std::size_t s, std::size_t t, const offset_t& os, int tp) {
    if (m_types_.find(tp) != m_types_.end() && m_types_.size()!=0) {throw std::invalid_argument("There already exists the same type in multi type (tried to add new bond)");}

    if (s >= num_sites() || t >= num_sites())
      throw std::invalid_argument("site index out of range");
    if (std::size_t(os.size()) != dimension())
      throw std::invalid_argument("unitcell offset dimension mismatch");
    std::size_t b = bonds_.size() + multis_.size();
    bonds_.push_back(bond_t(s, t, os, tp));
    b_types_.insert(tp);
    return b;
  }

  std::size_t add_multi(std::size_t s, VS t_v, const std::vector<offset_t>& os_v, int tp) {
    if (t_v.size() != os_v.size()) throw std::invalid_argument("size of t_v and os_v must be same");
    if (t_v.size() == 1) throw std::invalid_argument("size of t_v must be larger than 1 for add_multi");
    if (b_types_.find(tp) != b_types_.end() && b_types_.size()!=0) {throw std::invalid_argument("There already exists the same type in bond type (tried to add new multi)");}

    for (int i=0; i<t_v.size(); i++){
      auto t = t_v[i];
      auto os = os_v[i];
      if (s >= num_sites() || t >= num_sites())
        throw std::invalid_argument("site index out of range");
      if (std::size_t(os.size()) != dimension())
        throw std::invalid_argument("unitcell offset dimension mismatch");
    }
    std::size_t b = bonds_.size() + multis_.size();
    multis_.push_back(multi_t(s, t_v, os_v, tp));
    m_types_.insert(tp);
    return b;
  }

  static unitcell simple(std::size_t dim) {
    unitcell cell(dim);
    cell.add_site(coordinate_t::Zero(dim), 0);
    for (std::size_t m = 0; m < dim; ++m) {
      offset_t os = offset_t::Zero(dim);
      os(m) = 1;
      cell.add_bond(0, 0, os, 0);
    }
    return cell;
  }

  std::size_t dim_;
  std::vector<site_t> sites_;
  std::vector<bond_t> bonds_;
  std::vector<multi_t> multis_;
  std::set<int> b_types_;
  std::set<int> m_types_;


private:
  
};

inline std::size_t dimension(const unitcell& cell) {
  return cell.dimension();
}

} // end namespace lattice

#endif
