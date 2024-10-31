# Pixelation-cli
A simple cli to pixelate a png image.
***
Pixelation is used to reduce an image's detail by enlarging individual pixels or groups of pixels so that each group represents a larger, uniform color block.
This results in a blocky, mosaic-like look, where small details are lost, and the image appears coarser. It gives a picture retro game like effect.
***
## Images

![pixelated](/Examples/pixelation.png)

<div style="display: flex; justify-content: space-around;">
  <img src="Examples/goku.png" alt="Image 1" width="500"/>
  <img src="Examples/goku_pixelated.png" alt="Image 2" width="500"/>
</div>

> PS: If you zoom in a little bit you'll be able to see the difference clearly
***
## Usage
#### For Windows-
**JUST USE WSL AND FOLLOW THE STEPS BELOW**
#### For Linux/MacOs -
Just make sure you have `libpng` and `gcc` on your machine.
```sh
gcc pixelate.c -o pixelate -lpng

```
This will compile the source code then just run the cli using
```sh
./pixelate <input_image_name.png>  <output_image_name.png> <pixelation_factor>(optional)
```
> Make sure the compiled binary and input image is in same folder or just add the compiled binary to path
