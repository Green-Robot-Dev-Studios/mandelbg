# MandelBG
## Set your X11 root window to fractals!

- A program that allows you to set a fractal as your root window in X11 or wayland through the Xwayland compatibility layer.
- Implemented so far: 
    * Mandelbrot and Julia
    * Basic color animation
    * Multi-monitor support
- TODO:
    * Add more animation options
    * Add command line animation functionality
    * Overwrite file manager based root windows
    * Don't overwrite unused monitor wallpapers
    * Publish to package manager

## Instructions
### WARNING: Still under heavy development. It will not work if you have a file manager on your root window.
- `make all`
- `./mandelbg -m <monitor>`
    * Where monitor is the name of your monitor found in the `xrandr` utility. Example: `HDMI-0`

Once this program is finished, it will be added to a package manager.