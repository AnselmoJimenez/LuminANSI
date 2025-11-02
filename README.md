# LuminANSI

<p align=center>
  <img src="./assets/LuminANSI.png" width="40%" height="40%">
</p>

> [!NOTE]
> This project is a work in progress and is not in it's final state!  

A 3D Graphics Engine written in pure C that renders 3D models as ASCII art in your terminal.  
Built entirely from mathematical foundations without relying on OpenGL, DirectX, or any graphics libraries, it implements the complete rendering pipeline:  

* Perspective Projection
* Surface Rasterization
* Depth Buffering
* Lighting

Load any OBJ file and watch it come to life as dynamic ANSI graphics with real-time rotation and dynamic shading.  

## Dependencies

This program renders meshes within the linux terminal, using the terminal cells as pixels to be filled in. Please ensure you have these tools installed in your linux distribution.  

* make
* gcc

WSL is also supported

### Supported Mesh Formats

* obj

Support for more mesh file formats to be added in future implementations.

## Usage

### Build

To build LuminANSI run:

```bash
make all
```

This command will assemble and link all the source files - creating the LuminANSI executable.

### Running LuminANSI  

> [!IMPORTANT]
> Zoom the terminal out to make your characters small enough, so the image rasterization will be clear. If not, you will see a mess of characters

To render an object to the screen, one needs to pass a file of the supported mesh formats listed above. You can use your own or one of the files that exist within the project's `tests` directory  

To run the program:

```bash
./LuminANSI -f path/to/file
```

To learn more about options, use the `-h` option

```bash
./LuminANSI -h
```

To quit the program, press `q` at any time

## ToDo

* Multithreading

## Contributions
**This project is not accepting contributions.**  
This is a personal project that I am maintaining on my own. Pull requests, merge requests, and issues will not be reviewed or addressed.  
Thank you for your interest.
