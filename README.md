# palettize

Palette generator based on k-means clustering with CIELAB colors.

![Palette example](https://i.imgur.com/Ahk7vyE.jpg)

Original photo by Francesco Ungaro from Pexels.

## About

This application uses a standard implementation of k-means clustering to generate palettes. The twist is in how colors from the source image are interpreted by the program.

A straightforward implementation might use RGB values directly from the image, but this can lead to issues: the distance function for k-means might indicate that two colors are similar when, perceptually, they are not.

This program transforms colors into [CIELAB](https://en.wikipedia.org/wiki/CIELAB_color_space#Perceptual_differences) space before performing any comparisons. The differences between colors should align better with how human eyes actually work, and the resulting palette should be more accurate.

For a description of the k-means algorithm itself, see [this YouTube video](https://youtu.be/4b5d3muPQmA?t=33).

## Usage

The following options are accepted on the command line, in order:

* ```<source path>``` - Path to the input image
* ```[cluster count]``` - Number of clusters used by the algorithm
* ```[seed]``` - Seed used by the algorithm
    * This may be useful if you wanted to re-run the application multiple times under the same conditions
* ```[sort type]``` - Metric by which colors in the palette should be sorted, from left to right
    * **weight** orders by the weight of each color in the input image
    * **red** orders by the distance of each color from red
    * **green** orders the colors by the distance of each color from green
    * **blue** orders the colors by the distance of each color from blue
* ```[dest path]``` - Path used for the output image
    * **palette.bmp** is used by default

## License

See the [LICENSE](https://github.com/gvlsq/palettize/blob/main/LICENSE) file.
