# OGLCore

A small OpenGL 3.3 core-profile rendering library, extracted from a
[Cherno-style](https://www.youtube.com/playlist?list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2)
abstraction layer.

The library is **windowing-agnostic**: it never includes GLFW, SDL or any other
platform header. You create the window and the GL context, then hand OGLCore a
loader so it can resolve the entry points it needs.

```cpp
#include <OGLCore/Core.h>
#include <OGLCore/Renderer.h>
#include <OGLCore/Shader.h>

glfwMakeContextCurrent(window);
OGLCore::Init(glfwGetProcAddress);   // no cast: the signatures match exactly

OGLCore::Shader   shader("Ressources/Shaders/Basic.shader");
OGLCore::Renderer renderer;
renderer.Draw(vao, ibo, shader);
```

## What's in it

| Header | Purpose |
| --- | --- |
| `Core.h` | Entry-point loading, context info |
| `Renderer.h` | Draw calls, clear colour, viewport, blending |
| `Shader.h` | Single-file `#shader vertex` / `#shader fragment` programs |
| `Texture.h` | 2D textures via stb_image |
| `VertexArray.h` | VAOs, per-vertex and per-instance attribute buffers |
| `VertexBuffer.h` | Static and dynamic (streaming) vertex buffers |
| `IndexBuffer.h` | 32-bit element buffers |
| `VertexBufferLayout.h` | Typed attribute layouts with automatic stride |

glad and stb_image are vendored and **private** — they are not on the public
include path, so consumers are not tied to this library's loader choice.

All GL handle owners are move-only: copying is deleted, so a handle can never
be deleted twice.

## Building

Requires CMake 3.21+ and a C++17 compiler. Builds on Windows, Linux and macOS.

```sh
cmake -B build
cmake --build build
./build/Debug/OGLCoreSandbox     # the demo, also the library's smoke test
```

Options: `OGLCORE_BUILD_SANDBOX`, `OGLCORE_INSTALL` (both default on when
OGLCore is the top-level project, off when it is a subproject).

## Consuming it

```cmake
include(FetchContent)
FetchContent_Declare(OGLCore
    GIT_REPOSITORY https://github.com/elabideli/OPenGL.git
    GIT_TAG        master)
FetchContent_MakeAvailable(OGLCore)

target_link_libraries(my_app PRIVATE OGLCore::OGLCore)
```

`find_package(OGLCore)` works too, after `cmake --install`.

## Regenerating glad

The loader in `vendor/glad` is checked in so no Python is needed to build. To
regenerate it:

```sh
pip install glad2
python -m glad --api gl:core=3.3 --extensions="" --out-path vendor/glad --reproducible c
```
