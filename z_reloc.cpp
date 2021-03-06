#include <cassert>
#include <z_reloc/z_reloc.hpp>
#include <reloc/reloc_pool.hpp>

using namespace reloc;

void test1() {
    unsigned char* pd = new unsigned char[10 * 1024];
    reloc_pool<64> pool(pd, 10 * 1024);
    const char data[] = { 0, 1, 2, 3, 4, 5 };
    z_reloc::sized_ptr ptr = z_reloc::deflate(pool, data, sizeof(data));
    z_reloc::sized_ptr ptr2 = z_reloc::inflate(pool, ptr.ptr.pin().get(), ptr.size);
    assert(ptr2.size == sizeof(data));
    assert(std::equal(data, data + sizeof(data),
                      static_cast<unsigned char*>(ptr2.ptr.pin().get())));

    pool.deallocate(ptr.ptr);
    pool.deallocate(ptr2.ptr);
}

void test2() {
    unsigned char* pd = new unsigned char[10 * 1024];
    reloc_pool<1> pool(pd, 10 * 1024);
    const char data[] = { 0, 1, 2, 3, 4, 5 };

    reloc::reloc_ptr p1 = pool.allocate(2);
    reloc::reloc_ptr p2 = pool.allocate(2);
    pool.deallocate(p1);
    // 初期サイズを 1 にして断片化させておくことで、reallocate 時に確実に allocate が動くようにする
    z_reloc::sized_ptr ptr = z_reloc::deflate(pool, data, sizeof(data), 1);
    z_reloc::sized_ptr ptr2 = z_reloc::inflate(pool, ptr.ptr.pin().get(), ptr.size, 1);
    assert(ptr2.size == sizeof(data));
    assert(std::equal(data, data + sizeof(data),
                      static_cast<unsigned char*>(ptr2.ptr.pin().get())));

    pool.deallocate(p2);
    pool.deallocate(ptr.ptr);
    pool.deallocate(ptr2.ptr);
}

int main() {
    test1();
    test2();
}

