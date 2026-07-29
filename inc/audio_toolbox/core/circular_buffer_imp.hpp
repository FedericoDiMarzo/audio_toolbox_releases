
namespace audio_toolbox
{

_CIRCULAR_BUFFER_TEMPLATE
_CIRCULAR_BUFFER_TYPE::CircularBuffer(uint32_t length)
    : buffer(length),
      tail_index(0),
      head_index(0),
      mask(length - 1)
{
    if (!is_power_of_two(length))
    {
        AT_THROW_M(std::invalid_argument, "Length must be a power of two.");
    }
    clear();
}

_CIRCULAR_BUFFER_TEMPLATE
T _CIRCULAR_BUFFER_TYPE::operator[](int32_t index)
{
    return buffer[(head_index - index) & mask];
}

_CIRCULAR_BUFFER_TEMPLATE
uint32_t _CIRCULAR_BUFFER_TYPE::size() const
{
    return (head_index - tail_index) & mask;
}

_CIRCULAR_BUFFER_TEMPLATE
void _CIRCULAR_BUFFER_TYPE::clear()
{
    tail_index = 0;
    head_index = 0;
    std::fill(buffer.begin(), buffer.end(), static_cast<T>(0));
}

_CIRCULAR_BUFFER_TEMPLATE
void _CIRCULAR_BUFFER_TYPE::fill(T value)
{
    clear();
    std::fill(buffer.begin(), buffer.end(), value);
    head_index = buffer.size() - 1;
}

_CIRCULAR_BUFFER_TEMPLATE
void _CIRCULAR_BUFFER_TYPE::push(T data_in)
{
    buffer[head_index] = data_in;
    increment_head();
    if (head_index == tail_index)
    {
        increment_tail();
    }
}

_CIRCULAR_BUFFER_TEMPLATE
T _CIRCULAR_BUFFER_TYPE::pop()
{
    if (head_index == tail_index)
    {
        return static_cast<T>(0);
    }

    T data_out = buffer[tail_index];
    increment_tail();
    return data_out;
}

_CIRCULAR_BUFFER_TEMPLATE
void _CIRCULAR_BUFFER_TYPE::increment_head() noexcept
{
    head_index = (head_index + 1) & mask;
}

_CIRCULAR_BUFFER_TEMPLATE
void _CIRCULAR_BUFFER_TYPE::increment_tail() noexcept
{
    tail_index = (tail_index + 1) & mask;
}

} // namespace audio_toolbox
