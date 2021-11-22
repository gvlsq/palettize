# palettize

Palette generator based on k-means clustering with CIELAB colors.

![Palette example](https://i.imgur.com/Ahk7vyE.jpg)

Original photo by Francesco Ungaro from Pexels.

## Motivation

This application uses a standard implementation of k-means clustering to generate palettes. The twist is in how colors from the input image are compared to each cluster.

A straightforward implementation might compute these color differences using RGB values directly from the image, but this can lead to issues: the math might say that two colors are perceptually similar when in reality they are not.

A better approach might transform each color so that its value accounts for human perception - then the comparisons should be more meaningful. To achieve this in this app, all colors are transformed to [CIELAB](https://en.wikipedia.org/wiki/CIELAB_color_space#Perceptual_differences) before any comparisons take place.

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
