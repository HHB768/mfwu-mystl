template <typename T>
class SomeAlloc; // Forward declaration of SomeAlloc

// Forward declaration of ABase (a base class for A)
template <typename T>
class ABase {
public:
    virtual ~ABase() = default;
};

// B now uses `ABase<T>` instead of `A<T, Hasher, Alloc>` directly
template <typename T>
class B {
    T t_;
    ABase<T>* a_;  // Pointer to a base class instead of full A
};

// Define A, which inherits from ABase
template <typename T, typename Hasher, typename Alloc = SomeAlloc<B<T>>>
class A : public ABase<T> {
public:
    using BType = B<T>;  // Now B does not need to reference A directly
private:
    BType* elements_;
};

int main() {
    using MyHasher = int; // Replace with an actual hasher type
    A<int, MyHasher> aInstance; // Instantiate A
    return 0;
}
