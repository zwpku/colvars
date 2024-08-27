#ifndef COLVARBIAS_OPES_H
#define COLVARBIAS_OPES_H

#include "colvarbias.h"

#include <vector>

// OPES_METAD implementation: swiped from OPESmetad.cpp from PLUMED
// NOTE: The "explore" mode is not implemented
class colvarbias_opes: public colvarbias {
public:
  struct kernel {
    cvm::real m_height;
    std::vector<cvm::real> m_center;
    std::vector<cvm::real> m_sigma;
    kernel() {}
    kernel(cvm::real h, const std::vector<cvm::real>& c,
           const std::vector<cvm::real>& s):
      m_height(h), m_center(c), m_sigma(s) {}
  };
  /// Communication between different replicas
  enum Communication {
    /// One replica (default)
    single_replica,
    /// Hills added concurrently by several replicas
    multiple_replicas
  };
  colvarbias_opes(char const *key);
  virtual int init(std::string const &conf) override;
  virtual int update() override;
  std::ostream &write_state_data(std::ostream &os) override;
  std::istream &read_state_data(std::istream &is) override;
  template <typename OST> OST &write_state_data_template_(OST &os);
  template <typename IST> IST &read_state_data_template_(IST &os);
  std::string const traj_file_name(const std::string& suffix) const;
  virtual int write_output_files() override;
private:
  int update_opes();
  int calculate_opes();
  void save_state();
  cvm::real getProbAndDerivatives(const std::vector<cvm::real>& cv, std::vector<cvm::real>& der_prob, std::vector<cvm::real>& dist) const;
  cvm::real evaluateKernel(const kernel& G, const std::vector<cvm::real>& x) const;
  cvm::real evaluateKernel(const kernel& G, const std::vector<cvm::real>& x, std::vector<cvm::real>& accumulated_derivative, std::vector<cvm::real>& dist) const;
  void addKernel(const double height, const std::vector<cvm::real>& center, const std::vector<cvm::real>& sigma, const double logweight);
  void addKernel(const double height, const std::vector<cvm::real>& center, const std::vector<cvm::real>& sigma);
  size_t getMergeableKernel(const std::vector<cvm::real>& giver_center, const size_t giver_k) const;
  void mergeKernels(kernel& k1, const kernel& k2) const;
  void updateNlist(const std::vector<cvm::real>& center);
  struct traj_line {
    double rct;
    double zed;
    double neff;
    double work;
    size_t nker;
    size_t nlker;
    size_t nlsteps;
  };
  void writeTrajBuffer();
  void showInfo() const;
private:
  cvm::real m_barrier;
  cvm::real m_biasfactor;
  cvm::real m_bias_prefactor;
  cvm::real m_temperature;
  cvm::step_number m_pace;
  cvm::step_number m_adaptive_sigma_stride;
  cvm::step_number m_adaptive_counter;
  cvm::step_number m_counter;
  cvm::real m_compression_threshold;
  cvm::real m_compression_threshold2;
  bool m_adaptive_sigma;
  bool m_fixed_sigma;
  bool m_no_zed;
  // bool m_restart;
  bool m_nlist;
  bool m_recursive_merge;
  std::vector<cvm::real> m_nlist_param;
  std::vector<cvm::real> m_sigma0;
  std::vector<cvm::real> m_sigma_min;
  cvm::real m_epsilon;
  cvm::real m_sum_weights;
  cvm::real m_sum_weights2;
  cvm::real m_cutoff;
  cvm::real m_cutoff2;
  cvm::real m_zed;
  cvm::real m_old_kdenorm;
  cvm::real m_kdenorm;
  cvm::real m_val_at_cutoff;
  cvm::real m_rct;
  cvm::real m_neff;
  std::vector<kernel> m_kernels;
  std::vector<kernel> m_delta_kernels;
  std::vector<cvm::real> m_av_cv;
  std::vector<cvm::real> m_av_M2;
  std::ostringstream m_kernels_output;
  std::vector<cvm::real> m_nlist_center;
  std::vector<size_t> m_nlist_index;
  std::vector<cvm::real> m_nlist_dev2;
  size_t m_nlist_steps;
  bool m_nlist_update;
  bool m_nlist_pace_reset;
  size_t m_nker;
  bool m_calc_work;
  cvm::real m_work;
  /// Communication between different replicas
  Communication comm;
  /// \brief Identifier for this replica
  std::string            replica_id;
  size_t m_num_walkers;
  size_t shared_freq;
  size_t m_num_threads;
  size_t m_nlker;
  // size_t m_state_stride;
  // std::unordered_map<std::string, std::string> m_kernel_output_components;
  std::string m_kernels_output_headers;
  cvm::step_number m_traj_output_frequency;
  traj_line m_traj_line;
  std::ostringstream m_traj_oss;
  bool m_is_first_step;
  std::vector<cvm::real> m_cv;
  // For saving states
  decltype(m_zed) m_saved_zed;
  decltype(m_sum_weights) m_saved_sum_weights;
  decltype(m_sum_weights2) m_saved_sum_weights2;
  decltype(m_kernels) m_saved_kernels;
};

#endif // COLVARBIAS_OPES_H