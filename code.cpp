#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;
typedef vector<int> permutation;
typedef long long result_type;

struct positions_of_values {
  int left, right;
  positions_of_values(int Left, int Right) {
    left = Left;
    right = Right;
  }
};

typedef vector<positions_of_values> positions_of_all_values;

int get_upper_value(int value) {
  return value * 2 + 1;
}

positions_of_all_values generate_segment_decomposition(int length_of_sigma, int length_of_pi, vector<int> assignment) {
  positions_of_all_values result;
  int last_used_value = 0;
  int max_value = length_of_sigma - 1;
  for(int i = 0;i < static_cast<int>(assignment.size());i++) {
    int current_left_value = get_upper_value(assignment[i]);
    int current_right_value = min(max_value, current_left_value + 1);
    result.push_back(positions_of_values(last_used_value, current_left_value));
    result.push_back(positions_of_values(current_left_value, current_right_value));
    last_used_value = current_right_value;
  }
  if(length_of_pi % 2 == 1) {
    result.push_back(positions_of_values(last_used_value, max_value));
  }
  return result;
}

vector<vector<int>> sort_values(int max_value, vector<vector<int>> possible_values_of_all) {
  vector<vector<int>> buckets;
  for(int i = 0;i <= max_value;i++) {
    buckets.push_back(vector<int>());
  }
  for(int i = 0;i < static_cast<int>(possible_values_of_all.size());i++) {
    for(int value : possible_values_of_all[i]) {
      buckets[value].push_back(i);
    }
  }
  for(int i = 0;i < static_cast<int>(possible_values_of_all.size());i++) {
    possible_values_of_all[i].clear();
  }
  for(int i = 0;i <= max_value;i++) {
    for(int position: buckets[i]) {
      possible_values_of_all[position].push_back(i);
    }
  }
  return possible_values_of_all;
}

permutation inverse(permutation perm) {
  int length = static_cast<int>(perm.size());
  permutation result;
  result.resize(length);
  for(int i = 0;i < length;i++) {
    result[perm[i] - 1] = i + 1;
  }
  return result;
}

struct dp_state {
  int value;
  result_type dp, prefix_sum;

  dp_state() {
    value = dp = prefix_sum = 0;
  }

  dp_state(int Value, result_type Dp, result_type Prefix_sum) {
    value = Value;
    dp = Dp;
    prefix_sum = Prefix_sum;
  }
};

vector<dp_state> update_dp(vector<dp_state> dp_states, vector<int> possible_values) {
  vector<dp_state> result;
  int position_in_dps = 0;
  for(int i = 0;i < static_cast<int>(possible_values.size());i++) {
    while(position_in_dps < static_cast<int>(dp_states.size()) && dp_states[position_in_dps].value < possible_values[i]) {
      position_in_dps++;
    }
    result_type value = 0;
    if(position_in_dps > 0) {
      value = dp_states[position_in_dps - 1].prefix_sum;
    }
    result_type prefix_sum = 0;
    if(i > 0) {
      prefix_sum = result.back().prefix_sum;
    }
    result.push_back(dp_state(possible_values[i], value, prefix_sum + value));
  }
  return result;
}

result_type greedy_algorithm(permutation sigma, permutation pi, positions_of_all_values positions) {

  vector<vector<int>> possible_values_of_all;
  int length_of_sigma = static_cast<int>(sigma.size());
  int length_of_pi = static_cast<int>(pi.size());

  for(int i = 0;i < length_of_pi;i++) {
    vector<int> possible_values;
    for(int j = positions[i].left;j <= positions[i].right;j++) {
      possible_values.push_back(sigma[j]);
    }
    possible_values_of_all.push_back(possible_values);
  }
  possible_values_of_all = sort_values(length_of_sigma, possible_values_of_all);
  permutation inverse_pi = inverse(pi);
  vector<dp_state> current_dp_statates;
  current_dp_statates.push_back(dp_state(0, static_cast<result_type>(1), static_cast<result_type>(1)));
  for(int i = 0;i < length_of_pi;i++) {
    current_dp_statates = update_dp(current_dp_statates, possible_values_of_all[inverse_pi[i] - 1]);
  }
  return current_dp_statates.back().prefix_sum;
}

result_type solve(permutation sigma, permutation pi) {
  int length_of_sigma = static_cast<int>(sigma.size());
  int length_of_pi = static_cast<int>(pi.size());

  if(length_of_pi == 1) {
    return length_of_sigma;
  }

  int number_of_values = length_of_sigma / 2;
  int number_of_elements = length_of_pi / 2;
  vector<int> assignment;
  for(int i = 0;i < number_of_elements;i++) {
    assignment.push_back(i);
  }
  result_type result = static_cast<result_type>(0);
  while(assignment.back() < number_of_values) {
    result += greedy_algorithm(sigma, pi, generate_segment_decomposition(length_of_sigma, length_of_pi, assignment));
    assignment[0]++;
    int repair = 0;
    while(repair + 1 < static_cast<int>(assignment.size()) && assignment[repair] == assignment[repair + 1]) {
        assignment[repair] = repair;
        assignment[repair + 1]++;
        repair++;
    }
  }
  return result;
}

int main() {
  // Input specification:
  // permutation of {1, 2, ..., n} and permutation of {1, 2, ..., k} both represented as a vector of ints
  solve({3, 2, 5, 4, 1}, {1, 3, 2}); // returns 2
  return 0;
}
