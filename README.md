# Interactive Art Gallery Guarding

## Overview

This project is an interactive GUI program written in C++ designed to calculate the minimal number of camera locations required to guard the interior of a given polygon layout. The application allows users to create or import a simple polygon and then calculates and displays an optimal layout of cameras that can cover the entire polygon. The project leverages advanced computational geometry techniques, such as polygon triangulation and visibility algorithms, to ensure efficient and accurate camera placement. The program adheres to Chvátal's theorem, ensuring a solution with a camera count that does not exceed ⌊n/3⌋, where n is the number of polygon vertices.

## Languages and Utilities Used

- **Programming Language:** C++
- **Libraries:** 
  - CGAL (Computational Geometry Algorithms Library)
  - Qt (for GUI)
- **Build System:** CMake
- **Package Manager:** vcpkg

## Environment Setup

- **IDE:** Visual Studio 2022 Community Edition
- **Operating Systems:** Windows
- **Memory Usage:** up to 50MB

## Installation Instructions

- Full instructions locatied in the "Installation Instructions.txt"

1. **Install Visual Studio 2022 Community Edition**  
   [Download Visual Studio 2022 Community](https://visualstudio.microsoft.com/)

2. **Install CMake**  
   [Download CMake](https://cmake.org/download/)

3. **Install vcpkg and required packages**  
   Create a development folder (e.g., `C:\dev\`), then run the following commands:

   ```bash
   git clone https://github.com/microsoft/vcpkg
   cd vcpkg
   .\bootstrap-vcpkg.bat
   ./vcpkg.exe install yasm-tool:x86-windows
   ./vcpkg.exe install cgal
   ./vcpkg.exe install qt5
