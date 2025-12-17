# Development Guidelines  

This document outlines best practices for developing and maintaining the atomic-brokerless-discovery project, which is a hybrid C++ and Python codebase. It covers coding standards, build configuration, testing permutations across compilers and linkers, and maintaining documentation in sync with the code.  

## General Principles  
- Prefer modern, idiomatic C++ (C++26) and Python (PEP 8/PEP 621) constructs.  
- Use continuous integration to validate builds and tests on multiple operating systems and compilers.  

## C++ Best Practices  
- Enable compiler warnings (`-Wall -Wextra -Wpedantic`) and treat warnings as errors in CI.  
- Use Clang‑Tidy, Clang‑Format, and CppCheck to ensure static analysis and formatting.  
- Use RAII and move semantics; avoid raw pointers.  
- Where concurrency is needed, prefer lock‑free data structures and avoid undefined behaviour.  
- Structure the code into small libraries (e.g. discovery layer, ring buffer, tests).  
- Use CMake presets to build against multiple compiler and linker permutations, such as:  
  - **Clang 22** and GCC latest for Linux (through toolchain files).  
  - Linkers: **lld**, **mold**, and GNU gold/ld. Provide presets for each linker.  
- Integrate vcpkg for managing third‑party dependencies.  

## Python Best Practices  
- Specify the Python version in `pyproject.toml` and `pixi.toml` (use the latest stable, currently ≥ 3.12).  
- Use type hints and type check with `mypy`.  
- Use `black` and `flake8` for code formatting and linting.  
- Keep Python modules in a `python/` directory to separate from C++ code.  
- Use `venv` or pixi to manage local environments.  

## Testing Strategy  
- Write unit tests for C++ using Catch2; tests should cover success and failure cases.  
- Write Python unit tests using `pytest`.  
- Parameterize the CI to test across:  
  - **Multiple compilers**: Clang 22, GCC (latest), and, if relevant, MSVC on Windows.  
  - **Multiple linkers**: lld, mold, gold/ld.  
  - **Different build types**: Debug, Release, and sanitized builds (ASan, UBSan, TSan, MSan, and the new realtime sanitizer).  
- Use CTest to orchestrate C++ test suites; integrate Python tests into the same workflow.  
- Include sanitizers through CMake options or toolchain files; ensure tests run with sanitizers enabled.  
- Use Docker or devcontainers to provide reproducible environments for Ubuntu 20.04 and 24.04.  

## Documentation  
- Use Doxygen to generate API documentation from C++ headers and Sphinx (with autodoc) for Python modules.  
- Store docs in a `docs/` directory and configure Sphinx to include the C++ Doxygen output via Breathe.  
- Maintain doc comments alongside code to ensure documentation stays synchronized.  
- Set up a CI job to build documentation on every push; fail the build if the docs do not compile.  
- Use GitHub Pages or a similar service to host generated documentation.  

## Contribution Workflow  
- New features and fixes should be developed on feature branches and submitted through pull requests.  
- Each pull request should include:  
  - Associated updates to documentation.  
  - Unit tests verifying the new behaviour.  
  - Updates to CMake presets or toolchain files if new compilers or linkers are supported.  
- Use GitHub Actions or another CI system to validate all build and test permutations for each pull request.  

By following these practices, the project can remain maintainable, testable, and well‑documented across both C++ and Python ecosystems.
