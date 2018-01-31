# Attractors #

> Attractors visualisation program: Buddhabrot and Clifford attractors.

## Installation ##

First, install [Visual Studio 2017](https://www.visualstudio.com/downloads/). Open in Visual Studio "attractors.sln", and compile solution.
No system dependencies used, STL only based C++11 code. So, others compilators also can be used to build programs.

## Buddhabrot ##

<a href="http://images.sevstar.net/images/22946534505656593756.jpg" target="_blank">
<img src="http://images.sevstar.net/images/22946534505656593756_thumb.png" border="0" alt="Buddhabrot"/></a>

1) Compile this tool.
2) Set current directory to destination, where must be placed result TGA-image. For example, execute at console followed commands:
```batch
mkdir "c:/attractors"
cd /d "c:/attractors"
```
3) Run tool with command line parameters, that can be followed:
```
command line parameters usage:
  -fn string
      output tga-image file name, default is "buddhabrot.tga".
  -nx number
      image width, in pixels, default is 1000.
  -ny number
      image height, in pixels, default is 1000.
  -sq number
      image square width and height (alternative for nx/ny), in pixels.
  -nmax number
      length of sequence to test escape status (also known as bailout), default is 500.
  -tmax number
      number of iterations, multiples of 1 million, default is 50.
  -pool number
      working threads pool size, default is 4 for multicore processors.
```
To produce image above execute `Buddhabrot.x64.exe -sq=2500 -fn buddhasingle.tga -nmax=2500 -tmax=2500`

## Clifford attractors ##

<a href="http://images.sevstar.net/17209669908976374767.jpg" target="_blank">
<img src="http://images.sevstar.net/images/17209669908976374767_thumb.png" border="0" alt="17209669908976374767.jpg" /></a>
<a href="http://images.sevstar.net/15444013609743366105.jpg" target="_blank">
<img src="http://images.sevstar.net/images/15444013609743366105_thumb.png" border="0" alt="15444013609743366105.jpg" /></a>
<a href="http://images.sevstar.net/96240766713180008594.jpg" target="_blank">
<img src="http://images.sevstar.net/images/96240766713180008594_thumb.png" border="0" alt="96240766713180008594.jpg" /></a>

There 2 tools for images rendering: batch-mode tool and param-mode tool. Compile them at first, and set current directory as for Buddhabrot.
Param-mode tool lets place individual parameters at command prompt for each rendering.
Batch-mode tool takes the file `ca_param.txt` with configuration, that includes image parameters and geometry description.
Preset provided with sources makes images as above. Play with those parameters to get different rendering results.
You can make as much images as you like by one tool call.

## License

Author: &copy; schwarzlichtbezirk (schwarzlichtbezirk@gmail.com)  
The project is released under the [MIT license](http://www.opensource.org/licenses/MIT).
