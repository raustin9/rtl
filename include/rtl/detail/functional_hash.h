#ifndef __RTL_DETAIL_FUNCTIONAL_HASH
#define __RTL_DETAIL_FUNCTIONAL_HASH

#include <type_traits>

namespace rtl
{
    namespace __detail
    {
        // Thanks GCC :)
        // https://github.com/gcc-mirror/gcc/blob/master/libstdc++-v3/libsupc++/hash_bytes.cc
        inline std::size_t unaligned_load(const char* p)
        {
            std::size_t result;
            __builtin_memcpy(&result, p, sizeof(result));
            return result;
        }

        // Thanks GCC :)
        // https://github.com/gcc-mirror/gcc/blob/master/libstdc++-v3/libsupc++/hash_bytes.cc
        inline std::size_t load_bytes(const char* p, int n)
        {
            std::size_t result = 0;
            --n;
            do
                result = (result << 8) + static_cast<unsigned char>(p[n]);
            while (--n >= 0);
            return result;
        }

        // Thanks GCC :)
        // https://github.com/gcc-mirror/gcc/blob/master/libstdc++-v3/libsupc++/hash_bytes.cc
        inline std::size_t shift_mix(std::size_t v) { return v ^ (v >> 47); }

        // Thanks GCC :)
        // https://github.com/gcc-mirror/gcc/blob/master/libstdc++-v3/libsupc++/hash_bytes.cc
        template<typename T = std::size_t>
        typename std::enable_if<
            sizeof(T) == 4,
            std::size_t
        >::type
        __hash_bytes(const void* ptr, std::size_t len, std::size_t seed)
        {
            const std::size_t m = 0x5bd1e995;
            std::size_t hash = seed ^ len;
            const char* buf = static_cast<const char*>(ptr);

            // Mix 4 bytes at a time into the hash.
            while(len >= 4)
            {
                std::size_t k = unaligned_load(buf);
                k *= m;
                k ^= k >> 24;
                k *= m;
                hash *= m;
                hash ^= k;
                buf += 4;
                len -= 4;
            }

            std::size_t k;
            // Handle the last few bytes of the input array.
            // NOTE: we expect fallthrough on case statements here
            switch(len)
            {
                case 3:
                    k = static_cast<unsigned char>(buf[2]);
                    hash ^= k << 16;
                case 2:
                    k = static_cast<unsigned char>(buf[1]);
                    hash ^= k << 8;
                case 1:
                    k = static_cast<unsigned char>(buf[0]);
                    hash ^= k;
                    hash *= m;
            };

            // Do a few final mixes of the hash.
            hash ^= hash >> 13;
            hash *= m;
            hash ^= hash >> 15;
            return hash;
        }

        // Thanks GCC :)
        // http://github.com/gcc-mirror/gcc/blob/master/libstdc++-v3/libsupc++/hash_bytes.cc
        template<typename T = std::size_t>
        typename std::enable_if<
            sizeof(T) == 4,
            std::size_t
        >::type
        __fnv_hash_bytes(const void* ptr, std::size_t len, std::size_t hash)
        {
            const char* cptr = static_cast<const char*>(ptr);
            for (; len; --len)
            {
                hash ^= static_cast<std::size_t>(*cptr++);
                hash *= static_cast<std::size_t>(16777619UL);
            }
            return hash;
        }

        // Thanks GCC :)
        // https://github.com/gcc-mirror/gcc/blob/master/libstdc++-v3/libsupc++/hash_bytes.cc
        template<typename T = std::size_t>
        typename std::enable_if<
            sizeof(T) == 8,
            std::size_t
        >::type
        __hash_bytes(const void *ptr, std::size_t len, std::size_t seed)
        {
            static const std::size_t mul = (((std::size_t) 0xc6a4a793UL) << 32UL)
                                      + (std::size_t) 0x5bd1e995UL;
            const char *const buf = static_cast<const char *>(ptr);

            // Remove the bytes not divisible by the sizeof(std::size_t).  This
            // allows the main loop to process the data as 64-bit integers.
            const std::size_t len_aligned = len & ~(std::size_t) 0x7;
            const char *const end = buf + len_aligned;
            std::size_t hash = seed ^ (len * mul);
            for (const char *p = buf; p != end; p += 8)
            {
                const std::size_t data = shift_mix(unaligned_load(p) * mul) * mul;
                hash ^= data;
                hash *= mul;
            }
            if ((len & 0x7) != 0)
            {
                const std::size_t data = load_bytes(end, len & 0x7);
                hash ^= data;
                hash *= mul;
            }
            hash = shift_mix(hash) * mul;
            hash = shift_mix(hash);
            return hash;
        }

        template<typename T = std::size_t>
        typename std::enable_if<
            sizeof(T) == 8,
            std::size_t
        >::type
        __fnv_hash_bytes(const void* ptr, std::size_t len, std::size_t hash)
        {
            const char* cptr = static_cast<const char*>(ptr);
            for (; len; --len)
            {
                hash ^= static_cast<std::size_t>(*cptr++);
                hash *= static_cast<std::size_t>(1099511628211ULL);
            }
            return hash;
        }
        // Thanks GCC :)
        // https://github.com/gcc-mirror/gcc/blob/master/libstdc++-v3/libsupc++/hash_bytes.cc
        template<typename T = std::size_t>
        typename std::enable_if<
            sizeof(T) != 8 && sizeof(T) != 4,
            std::size_t
        >::type
        __hash_bytes(const void *ptr, std::size_t len, std::size_t seed)
        {
            std::size_t hash = seed;
            const char* cptr = static_cast<const char*>(ptr);
            for (; len; --len)
            {
                hash = (hash * 132) + *cptr++;
            }
            return hash;
        }

        template<typename T = std::size_t>
        typename std::enable_if<
            sizeof(T) != 8 && sizeof(T) != 4,
            std::size_t
        >::type
        __fnv_hash_bytes(const void* ptr, std::size_t len, std::size_t seed)
        {
            __hash_bytes(ptr, len, seed);
        }

        struct __hash_impl
        {
            static std::size_t hash(const void* ptr, std::size_t length, std::size_t seed = static_cast<std::size_t>(0xc70f6907UL))
            {
                return __hash_bytes(ptr, length, seed);
            }
        };
    }
}

#endif // __RTL_DETAIL_FUNCTIONAL_HASH
