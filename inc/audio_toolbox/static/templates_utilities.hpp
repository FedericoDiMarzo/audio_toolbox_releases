#pragma once

#include <algorithm>
#include <cstdint>
#include <functional>
#include <numeric>
#include <variant>
#include <vector>

#include "audio_toolbox/core/types.hpp"
#include "audio_toolbox/math/math.hpp"
#include "audio_toolbox/static/macros.hpp"

namespace audio_toolbox
{

/**
 * @brief Checks if a container contains a value.
 *
 * @tparam C Container type.
 * @tparam T Value type.
 * @param container Container reference.
 * @param value Value to check.
 * @return True if the value is in the container, false otherwise.
 */
template <typename C, typename T>
inline bool contains(C& container, T value);

/**
 * @brief Returns the index of a value in a container.
 *
 * @note The element must be present in the container.
 *
 * @tparam C Container type.
 * @tparam T Value type.
 * @param container Container reference.
 * @param value Value to check.
 * @return Index of the value in the container.
 */
template <typename C, typename T>
inline uint32_t index_of(C& container, T value);

/**
 * @brief Similar to numpy's linspace function.
 *
 * @param start Beginning of the range.
 * @param end End of the range.
 * @param n Length of the range.
 * @return Vector with n evenly spaced values between start and end.
 */
std::vector<real_type> linear_space(real_type start, real_type end, uint32_t n);

/**
 * @brief Remove an item from a container.
 *
 * @tparam C Container type.
 * @tparam T Item type.
 * @param container Container reference.
 * @param item Item to remove.
 * @return True if the item was removed, false otherwise.
 */
template <typename C, typename T>
inline bool remove_item(C& container, T item);

/**
 * @brief Visit a variant and call the process method of the selected type.
 *
 * @tparam V Variant type.
 * @tparam Args Arguments type.
 * @param variant Variant to visit.
 * @param args Arguments to pass to the function.
 */
template <typename V, typename... Args>
inline void visit_process(V& variant, Args&&... args);

/**
 * @brief In place variant constructor.
 *
 * @tparam V Variant type.
 * @tparam T Type to construct.
 * @tparam Args Arguments type.
 * @param args Arguments to pass to the constructor.
 * @return Variant with the constructed type.
 */
template <typename V, typename T, typename... Args>
V init_variant(Args&&... args);

/**
 * @brief This class is used to dispatch updates to a list of observers.
 * An observer item mantains its own ownership.
 *
 * @tparam T1 Observer type.
 */
template <typename T1>
class UpdateDispatcher
{
  public:
    UpdateDispatcher() = default;

    AT_DISABLE_COPY_AND_MOVE_M(UpdateDispatcher);

    /**
     * @brief Add a new observer to the dispatcher.
     *
     * @param observer Parameter to be added.
     */
    void add_observer(T1& observer);

    /**
     * @brief Remove an observer from the dispatcher.
     *
     * @param observer Parameter to be removed.
     * @return True if the observer was removed, false otherwise.
     */
    bool remove_observer(T1& observer);

    /**
     * @brief Update the observers.
     *
     * @param arg Argument to be passed to the observers by reference.
     */
    template <typename... Args>
    void update(Args&&... args);

  private:
    std::vector<T1*> observers;
};

} // namespace audio_toolbox

#include "audio_toolbox/static/templates_utilities_imp.hpp"
