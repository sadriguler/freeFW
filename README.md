# freeFW
 electromagnetic solver

This repository aims to develop a full-wave electromagnetic analysis tool.

For now, it is capable to read Universal mesh files, i.e. unv, and extract mesh properties, i.e. edge sizes, triangle areas etc. The tool aims to develop a MoM solver.

# External Sources
- Eigen linear algebra is used for the matrix definitions in C++ for now. A possible expansion to the BLAS and LAPACk will be possibly required in the future.

# Running the current functionality
Due to the dependency on Eigen, a user should download the related header files to the same folder. For compiling the function, 
```
gcc -I ./Eigen main.cpp -o main.exe
```
After compilation, simply run `main.exe` in a bash terminal.
