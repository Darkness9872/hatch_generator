# Hatch Generator for SLM Slicer

C++ module for generating hatch patterns inside rectangular contours.

## Features

- Generates parallel hatch lines at any angle (0-180°)
- Configurable step distance between lines  
- Handles arbitrarily oriented rectangles
- File-based input/output support
- CMake build system
- Doxygen documentation ready

## Build

### Prerequisites
- C++20 compatible compiler (GCC, Clang, MSVC)
- CMake 3.10+

### Compilation
```bash
# Clone and build
git clone <repository-url>
cd hatch_generator
mkdir build && cd build
cmake ..
make
```

### Or compile directly
g++ -std=c++20 -o hatch_generator main.cpp
Usage
Basic Example
```bash
# Use default square (0,0)-(10,10)
./hatch_generator --angle 45 --step 1
```

### Custom input/output files
./hatch_generator --angle 30 --step 0.5 --input points.txt --output result.txt

### Command Line Options

| Option   | Description                               | Default      |
|----------|-------------------------------------------|--------------|
| `--angle` | Hatch angle in degrees (0-180)           | 45           |
| `--step`  | Distance between hatch lines             | 1.0          |
| `--input` | Input file with 4 boundary points        | `input.txt`  |
| `--output`| Output file for hatch lines              | `console`    |
### Input File Format
Create input.txt with 4 points (one per line):

```text
0 0
10 0
10 10
0 10
```
### Output Example
```text
Line 1: (0,0) -> (10,10)
Line 2: (0.1,10) -> (0,9.9)
```
...
## Documentation
Code contains Doxygen-style comments. Generate full documentation:

```bash
doxygen Doxyfile
# Open docs/html/index.html
```
## Algorithm Overview
Parse 4 input points into rectangle edges

Calculate center point and bounding box

Generate infinite parallel lines with given angle/step

Clip lines to rectangle boundaries

Output resulting hatch segments

Testing
Tested with:

Various rectangle orientations

Angles from 0° to 180°

Different step sizes

Edge cases (lines through corners)

## License
MIT License
