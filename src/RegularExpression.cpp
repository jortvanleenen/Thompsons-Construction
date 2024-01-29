/**
 * This file contains the implementation of the RegularExpression class.
 *
 * @file
 * @author Jort van Leenen
 * @copyright GNU General Public License v3.0
 */

#include "RegularExpression.h"
#include <iterator>
#include <set>
#include <sstream>
#include <vector>

RegularExpression::RegularExpression(std::string_view expression) {
  std::istringstream inputStream{expression.data()};
  int next_index{0};
  int start_index{0};
  m_automaton = expr(inputStream, next_index, start_index);
  m_initial_state = start_index;
}

std::string RegularExpression::dot() const {
  std::ostringstream ss;
  ss << "digraph {\n"
     << "\trankdir = LR\n"
     << "\tnode [shape = circle, style = filled, fillcolor = gray93]\n"
     << "\t" << (m_automaton.empty() ? 1 : m_automaton.size())
     << " [shape = doublecircle]\n"
     << "\t0 [style = invisible]\n"
     << "\t0 -> " << m_initial_state + 1 << '\n';
  std::string output{ss.str()};

  int state_number{1};
  for (const auto& state: m_automaton) {
    if (state.first_outgoing != -1) {
      output += dotState(state_number, state, true);
    }
    if (state.second_outgoing != -1) {
      output += dotState(state_number, state, false);
    }
    ++state_number;
  }

  output.append("}");
  return output;
}

std::string RegularExpression::dotState(
    int state_number, const RegularExpression::State& state, bool first_edge) {
  return '\t' + std::to_string(state_number) + " -> "
         + (first_edge ? std::to_string(state.first_outgoing + 1)
                       : std::to_string(state.second_outgoing + 1))
         + " [label=\""
         + (std::islower(state.character) ? std::string{state.character}
                                          : "&epsilon;")
         + "\"]\n";
}

bool RegularExpression::mat(std::string_view string) const {
  std::string string_to_match{string == "$" ? "" : string}; // $ = empty string
  if (m_automaton.empty() && (!string_to_match.empty())) {
    return false;
  } else if (m_automaton.empty()) {
    return true;
  }

  std::set<int> current_states{traverseEmptyTransitions({m_initial_state})};
  for (auto character: string_to_match) {
    std::set<int> new_states{};
    for (auto state: current_states) {
      if (m_automaton[state].character == character) {
        new_states.insert(m_automaton[state].first_outgoing);
      }
    }
    current_states = traverseEmptyTransitions(new_states);
  }
  // Final state is always the last state due to the parser implementation
  return current_states.count(static_cast<int>(m_automaton.size() - 1)) == 1;
}

std::set<int> RegularExpression::traverseEmptyTransitions(
    const std::set<int>& current_states) const {
  std::set<int> new_states{};
  return traverseEmptyTransitionsRecursive(current_states, new_states);
}

std::set<int> RegularExpression::traverseEmptyTransitionsRecursive(
    const std::set<int>& current_states, std::set<int>& new_states) const {
  for (auto state: current_states) {
    new_states.insert(state);
    if (m_automaton[state].character == '\0') {
      if (m_automaton[state].first_outgoing != -1
          && new_states.count(m_automaton[state].first_outgoing) == 0) {
        new_states.merge(traverseEmptyTransitionsRecursive(
            std::set<int>{m_automaton[state].first_outgoing}, new_states));
      }
      if (m_automaton[state].second_outgoing != -1
          && new_states.count(m_automaton[state].second_outgoing) == 0) {
        new_states.merge(traverseEmptyTransitionsRecursive(
            std::set<int>{m_automaton[state].second_outgoing}, new_states));
      }
    }
  }

  return new_states;
}

// ⟨expr⟩ := ⟨term⟩ [ | ⟨expr⟩ ]
std::vector<RegularExpression::State> RegularExpression::expr(
    std::istringstream& expression, int& next_index, int& start_index) {
  std::vector<State> automaton{term(expression, next_index, start_index)};
  int front_index{start_index};

  if (expression.peek() == '|') {
    expression.get();

    std::vector<State> automaton_alternative{expr(expression, next_index,
                                                  start_index)};
    int alternative_front_index{start_index};

    automaton.back().first_outgoing = next_index + 1;
    automaton_alternative.back().first_outgoing = next_index + 1;
    automaton.insert(automaton.end(), automaton_alternative.begin(),
                     automaton_alternative.end());
    automaton.emplace_back(State{'\0', front_index, alternative_front_index});
    start_index = next_index;
    automaton.emplace_back(State{});
    next_index += 2;
  }

  return automaton;
}

// ⟨term⟩ := ⟨fact⟩ [ ⟨term⟩ ]
std::vector<RegularExpression::State> RegularExpression::term(
    std::istringstream& expression, int& next_index, int& start_index) {
  std::vector<State> automaton{fact(expression, next_index, start_index)};
  int front_index{start_index};

  if (expression.peek() == '(' || std::islower(expression.peek())) {
    std::vector<State> automaton_concatenation{term(expression, next_index,
                                                    start_index)};
    automaton.back().first_outgoing = start_index;
    automaton.insert(automaton.end(), automaton_concatenation.begin(),
                     automaton_concatenation.end());
    start_index = front_index;
  }

  return automaton;
}

// ⟨fact⟩ := ⟨lett⟩ [ * ] | ( ⟨expr⟩ ) [ * ]
// ⟨lett⟩ := A | B | · · · | Z | a | b | · · · | z
std::vector<RegularExpression::State> RegularExpression::fact(
    std::istringstream& expression, int& next_index, int& start_index) {
  std::vector<State> automaton{};
  if (expression.peek() == '(' || std::isalpha(expression.peek())) {
    if (expression.peek() == '(') {
      expression.get();
      automaton = expr(expression, next_index, start_index);
      expression.get(); // Right-parenthesis
    } else if (std::isalpha(expression.peek())) {
      automaton.emplace_back(State{static_cast<char>(expression.get()),
                                   next_index + 1});
      start_index = next_index;
      automaton.emplace_back(State{});
      next_index += 2;
    }

    if (expression.peek() == '*') {
      expression.get();

      automaton.back().first_outgoing = start_index;
      automaton.back().second_outgoing = next_index + 1;

      automaton.emplace_back(State{'\0', start_index, next_index + 1});
      start_index = next_index;
      automaton.emplace_back(State{});
      next_index += 2;
    }
  }

  return automaton;
}
