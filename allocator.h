
#include <queue>

using std::queue;

template <typename T>
class Allocator {
    T* pool;
    queue<int> free_list;

public:
    Allocator(long pool_size) {
        pool = (T*)malloc(pool_size * sizeof(T));
        for (int i = 0; i < pool_size; i++) {
            free_list.push(i);
        }
    }
    void* allocate() {
        if (free_list.empty()) {
            throw std::bad_alloc();
        }
        int index = free_list.front();
        free_list.pop();
        return &pool[index];
    }
    void deallocate(T* ptr) {
        int index = ptr - pool;
        free_list.push(index);
    }
    ~Allocator() {
        free(pool);
    }
};
