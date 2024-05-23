# Langulus::Core
`Langulus::Core` is a common header-only C++23 library, that is used in [all other Langulus](https://github.com/Langulus) libraries and plug-ins.
It takes care of the four main aspects of the framework:
1. [**Configuration**](https://github.com/Langulus/Core/wiki/CMake-Options) - `LangulusCore` is a CMake `INTERFACE` target that serves as a common configurator for all other targets - it includes global compiler flags and options.
2. [**Type Navigation**](https://github.com/Langulus/Core/wiki/Type-Navigation) - patterns defined in the `Langulus` namespace, that are used practically everywhere. By type-navigation we mean things analogical to `std::remove_reference_t`, but improved and designed specifically with Langulus use cases in mind.
3. [**Basic concepts**](https://github.com/Langulus/Core/wiki/Basic-Concepts) - the `Langulus::CT` namespace is the `CT` concept library, containing some rudimentary and widely used concepts.
4. [**Utilities**](https://github.com/Langulus/Core/wiki/Utilities) - defined in the `Langulus` namespace. Contain various widely used types and functions.

# Standalone use
You wish to use some of the utilities or concept library for yourself without downloading the entire Langulus framework?
You can use this library on its own. If you have CMake 3.24+, the recommended way to get it is to add this to your CMakeLists.txt:
```cmake
include(FetchContent)
FetchContent_Declare(LangulusCore GIT_REPOSITORY https://github.com/Langulus/Core.git GIT_TAG main)
FetchContent_MakeAvailable(LangulusCore)
```
Then just link to your target to get all the include directories:
```cmake
target_link_libraries(YourLibrary PUBLIC LangulusCore)
```
---------------
Alternatively, you can always just download and copy the headers.

# Considerations
Langulus is still heavily under development, and is by nature highly experimental.
There could be numerous breaking changes, renames, design overhauls, etc. coming in the future.
Our design is led by practice - as more features are added to Langulus, more things will be requalified as 'common' and worthy of being moved to Langulus::Core.

# Community feedback
I would love feedback and suggestions, please feel free to join the [forum](https://github.com/Langulus/Core/discussions).
However it should be stated clearly, that Langulus is primarily a personal project that I've been developing for over 10 years, and is aiming primarily at satisfying my requirements.
As such, ideas and suggestions may not be necessarily implemented by me, unless I find them exceedingly useful.
If you really need me to add or improve something that isn't on my daily agenda, you should consider sponsoring me.
