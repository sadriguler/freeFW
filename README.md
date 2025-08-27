# freeFW
 electromagnetic solver

This repository aims to develop a full-wave electromagnetic analysis tool.

For now, it is capable to read Universal mesh files, i.e. unv, and extract mesh properties, i.e. edge sizes, triangle areas etc. The tool aims to develop a MoM solver.

# External Sources
- [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page), a C++ linear algebra library, is used for the matrix definitions in C++ for now. A possible expansion to the BLAS and LAPACk will be possibly required in the future.
## Modeling
A sphere of a radius of 300 mm was generated in FreeCAD. The model was meshed by using mesh framework in FreeCAD after cleaning 1D reference lines, which may make the geometry open. unv file was exported as an example model.

# Running the current functionality
Due to the dependency on Eigen, a user should download the related header files to the same folder. For compiling the function, 
```
gcc -I ./Eigen main.cpp -o main.exe
```
After compilation, simply run `main.exe` in a bash terminal.



