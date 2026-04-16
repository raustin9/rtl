# Retrofitted Template Library
A collection of useful modern (C++17 onward) utilities for use in C++ 14 libraries and applications.

The focus of this is to map as closely to the standard as possible rather than trying to "improve" anything.

## Examples
#### span
```c++
void do_something(rtl::span<int> data) noexcept
{
    for(int num : data) {
        process(num);
    }
}

int main(void)
{
    std::vector<int> large_data {...};
    do_something(large_data);
}
```
also convertible from array
```c++
void do_something(rtl::span<int> data) noexcept
{
    for(int num : data) {
        process(num);
    }
}

int main(void)
{
    std::array<int, LARGE_AMOUNT> large_data {...};
    do_something(large_data);
}
```

#### string_view
```c++
void print_string(rtl::string_view sv, std::ostream& out = std::cout) noexcept
{
    out << sv << std::endl;
}

int main(void)
{
    std::string name = "Placeholder McDoctorate";
    print_string(name);
}
```

#### expected<T, E>
```c++
rtl::expected<int, Error> get_age(std::string name) noexcept
{
    std::optional<Person> person = find_person(name);
    if (!person.has_value())
    {
        return rtl::unexpected<Error>(Error("No person found with name"));
    }

    return person->age;    
}
```

#### as_const
```c++
class MyClass {
public:
    const int& get() const { return m_value; }
    
    int& get() {
        return const_cast<int&>(
            rtl::as_const(*this).get()
        );
    }

private:
    int m_value;
};
```

#### construct_at
```c++
// Standard value construction
MyObject obj;

rtl::construct_at(std::addressof(obj));
```

```c++
// With constructor args:
class MyObject {
public:
    MyObject(int value) {}
};

MyObject obj;
rtl::construct_at(std::addressof(obj), 1);
```
```c++
// With array types
MyObject objects[NUM_OBJECTS];

rtl::construct_at(std::addressof(objects));
```

## Building the tests
Our project is set up with cmake for building the tests.

Once you have cloned the repo, run these commands from the root:
```bash
$ cmake -B build .
```
```bash
$ cmake --build build
```

Then you can run the tests from the executable that is built.

Our CMake configuration is not very opinionated, so we do not define the output directory for build artifacts, so this will vary system to system.
However, it is likely somewhere in your `build` directory.

We have tested support for Visual Studio and CLion, so either of those IDEs should allow you to build and run teh test suite as well.