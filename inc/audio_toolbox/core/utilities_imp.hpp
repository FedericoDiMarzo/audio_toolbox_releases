

namespace audio_toolbox {

template <typename C, typename T>
bool contains(C& container, T value) {
  return std::find(std::begin(container), std::end(container), value) !=
         std::end(container);
};

template <typename C, typename T>
uint32_t index_of(C& container, T value) {
  return std::distance(
      container.begin(),
      std::find(std::begin(container), std::end(container), value));
};

template <typename C, typename T>
bool remove_item(C& container, T item) {
  auto original_size = container.size();
  container.erase(std::remove(std::begin(container), std::end(container), item),
                  container.end());
  return container.size() < original_size;
}

template <typename V, typename... Args>
void visit_process(V& variant, Args&&... args) {
  std::visit([&](auto& filter) { filter.process(std::forward<Args>(args)...); },
             variant);
}

template <typename V, typename T, typename... Args>
V init_variant(Args&&... args) {
  return V(std::in_place_type<T>, std::forward<Args>(args)...);
}

template <typename T1>
void UpdateDispatcher<T1>::add_observer(T1& observer) {
  observers.push_back(&observer);
}

template <typename T1>

bool UpdateDispatcher<T1>::remove_observer(T1& observer) {
  return remove_item(observers, &observer);
}

template <typename T1>
template <typename... Args>
void UpdateDispatcher<T1>::update(Args&&... args) {
  // Perfect forwarding of the references
  std::for_each(std::begin(observers), std::end(observers), [&](T1* observer) {
    observer->update(std::forward<Args>(args)...);
  });
}

}  // namespace audio_toolbox
