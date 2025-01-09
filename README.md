# QuixFlux

## A Flux Architecture Implementation for Qt6 QML

QuixFlux is a modern implementation of the Flux Architecture tailored specifically for Qt6 QML. It builds upon the foundational work of [QuickFlux](https://github.com/benlau/quickflux.git) but introduces significant updates and optimizations to take full advantage of Qt6 capabilities.

---

### Key Features

1. **Qt6 Compatibility**: Designed exclusively for Qt6, with no backward compatibility for Qt5.
2. **Improved File Structure**: Adheres to the recommended organizational patterns for QML modules in Qt6.
3. **Modern Type Declarations**: Utilizes `QML_ELEMENT` and `QML_NAMED_ELEMENT()` macros for clearer and more maintainable QML type registration.
4. **Optimized Codebase**: Includes streamlined source file inclusions to enhance performance and maintainability.
5. **Consistent Coding Style**: Aligns with the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) for better readability and collaboration.

---

### Getting Started

#### Prerequisites
- Qt6 (6.2 or newer recommended)

#### Installation
To include QuixFlux in your project, clone the repository and integrate the module into your Qt project:

```bash
git clone https://github.com/fajar-adianto/quixflux.git
```

Add the module path to your `CMakeLists.txt` file.
```cmake
add_subdirectory(quixflux)
```

Or, use `FetchContent` in your `CMakeLists.txt` file.

```cmake
FetchContent_Declare(
    quixflux
    GIT_REPOSITORY "https://github.com/fajar-adianto/quixflux.git"
    GIT_TAG main
)
FetchContent_MakeAvailable(quixflux)
```

Don't forget to add `target_link_libraries` in your `CMakeLists.txt` file.

```cmake
...
target_link_libraries(<TARGET>
    PRIVATE
        ...
        QuixFlux QuixFluxplugin
        ...
)
```

#### Usage
Import the QuixFlux module into your QML files:

```qml
import QuixFlux
```

Follow the Flux Architecture principles to create actions, stores, and dispatchers for your application logic, as described in [QuickFlux](https://github.com/benlau/quickflux.git). All components function and named similarly to QuickFlux but are prefixed with "Qx" for differentiation.

---

### Contributing
We welcome contributions to enhance QuixFlux. To contribute:
1. Fork this repository.
2. Create a feature branch for your changes.
3. Submit a pull request with a detailed description of your improvements.

Please adhere to the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) when contributing.

---

### License
QuixFlux is released under the [GNU General Public License v3](LICENSE). Feel free to use, modify, and distribute it as per the license terms.

---

### Acknowledgments
This project is inspired by [QuickFlux](https://github.com/benlau/quickflux.git). We thank its creators for their foundational work in bringing Flux Architecture to QML.

