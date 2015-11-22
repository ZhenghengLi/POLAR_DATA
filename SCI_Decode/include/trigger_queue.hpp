#include <queue>

template <class T, class Container = std::vector<T>, class Compare = std::greater<typename Container::value_type> >
class trigger_queue: public std::priority_queue<T, Container, Compare> {
private:
    T tail;
public:
    void push(const T& value);
    int distance() const;
};

template <class T, class Container, class Compare>
void trigger_queue<T, Container, Compare>::push(const T& value) {
    if (std::priority_queue<T, Container, Compare>::empty()) {
        tail = value;
		std::priority_queue<T, Container, Compare>::push(value);
    } else {
        if (std::priority_queue<T, Container, Compare>::comp(value, tail)) {
            tail = value;
        }
		std::priority_queue<T, Container, Compare>::push(value);
    }   
}

template <class T, class Container, class Compare>
int trigger_queue<T, Container, Compare>::distance() const {
    return tail - std::priority_queue<T, Container, Compare>::top();
}
