/**
 * This file contains the definition of the RegularExpression class.
 *
 * @file
 * @author Jort van Leenen
 * @copyright GNU General Public License v3.0
 */

#ifndef REGEXP_REGULAREXPRESSION_H
#define REGEXP_REGULAREXPRESSION_H

#include <cctype>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <string_view>
#include <vector>

class RegularExpression {
public:
  /**
   * Explicit default constructor.
   */
  RegularExpression() = default;

  /**
   * Construct an automaton representing a regular expression from a string.
   *
   * @param expression the string to construct the regular expression from
   */
  explicit RegularExpression(std::string_view expression);

  /**
   * Get the dot notation of the automaton representing the regular expression.
   *
   * @return the dot notation of the automaton
   */
  [[nodiscard]] std::string dot() const;

  /**
   * Check if the given string is accepted by the regular expression.
   *
   * @param string the string to check
   * @return true if the string matches the RegExp, false otherwise
   */
  [[nodiscard]] bool mat(std::string_view string) const;

private:
  /**
   * A state in the automaton representing the regular expression.
   *
   * The character '\0' is used to indicate that the state has only empty
   * transitions. Any other value for character (in practice [a-z]) indicates
   * that the state's first outgoing edge is a transition requiring that value.
   *
   * The value '-1' for an outgoing edge indicates that the state has no such
   * outgoing edge. Any other value for an outgoing edge indicates the index of
   * the state (in the automaton vector, see below) that the edge points to.
   *
   * As an example, by default, a state has no outgoing edges and, therefore, is
   * accompanied by the character '\0'.
   */
  struct State {
    char character = '\0';
    int first_outgoing = -1;
    int second_outgoing = -1;
  };

  /**
   * The automaton representing the regular expression.
   */
  std::vector<State> m_automaton{};

  /**
   * The initial state of the NFA.
   */
  int m_initial_state{};

  /**
   * ⟨expr⟩ := ⟨term⟩ [ | ⟨expr⟩ ]
   *
   * @param expression the (remaining) expression to parse
   * @param next_index the first free index to place the next state at
   * @param start_index the index representing the start of the (sub)automaton
   * @return the (sub)automaton representing the expression
   */
  static std::vector<State> expr(std::istringstream& expression,
                                 int& next_index, int& start_index);

  /**
   * ⟨term⟩ := ⟨fact⟩ [ ⟨term⟩ ]
   *
   * @param expression the (remaining) expression to parse
   * @param next_index the first free index to place the next state at
   * @param start_index the index representing the start of the (sub)automaton
   * @return the (sub)automaton representing the expression
   */
  static std::vector<State> term(std::istringstream& expression,
                                 int& next_index, int& start_index);

  /**
   * ⟨fact⟩ := ⟨lett⟩ [ * ] | ( ⟨expr⟩ ) [ * ]
   * ⟨lett⟩ := A | B | · · · | Z | a | b | · · · | z
   *
   * @param expression the (remaining) expression to parse
   * @param next_index the first free index to place the next state at
   * @param start_index the index representing the start of the (sub)automaton
   * @return the (sub)automaton representing the expression
   */
  static std::vector<State> fact(std::istringstream& expression,
                                 int& next_index, int& start_index);

  /**
   * Get a single line of dot notation for an edge.
   *
   * Note: two calls are required for a state with two edges due to this needing
   * two lines in dot notation.
   *
   * @param state_number the label to be used for the respective state
   * @param state the state to stringify to dot notation
   * @param first_edge true if the first edge should be used, false if second
   * @return the dot notation of the state
   */
  static std::string dotState(int state_number,
                              const RegularExpression::State& state,
                              bool first_edge);

  /**
   * Retrieve the set of states that do not have empty transitions and that can
   * be reached by empty transitions from the given set of states.
   *
   * Acts as a helper function for the recursive implementation of
   * traverseEmptyTransitionsRecursive().
   *
   * @param current_states the set of states to traverse from
   * @return the set of states that can be reached by empty or no transitions
   */
  [[nodiscard]] std::set<int> traverseEmptyTransitions(
      const std::set<int>& current_states) const;

  /**
   * Recursive implementation for traverseEmptyTransitions().
   *
   * @param current_states the set of states to traverse from
   * @param new_states the set of states that have been visited
   * @return the set of states that can be reached by empty or no transitions
   */
  [[nodiscard]] std::set<int> traverseEmptyTransitionsRecursive(
      const std::set<int>& current_states, std::set<int>& new_states) const;
};

#endif
