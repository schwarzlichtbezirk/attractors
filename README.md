# Attractors #

> Attractors visualisation program: Buddhabrot and Clifford attractors.

## Installation ##

First, install [Visual Studio 2017](https://www.visualstudio.com/downloads/). Open in Visual Studio "attractors.sln", and compile solution.
No Windows-specified system dependencies used, STL-only based C++11 code. So, others compilators also can be used to build programs.

## Usage steps ##

1) Compile selected tool.
2) Set current directory to destination, where must be placed auxiliary files and will be written result TGA-images.
For example, execute at console followed commands:
```batch
mkdir "c:/attractors"
cd /d "c:/attractors"
```
3) Run tool with command line descripted bellow parameters.

## Buddhabrot ##

[![buddhabrot](http://images.sevstar.net/images/22946534505656593756_thumb.png)](http://images.sevstar.net/images/22946534505656593756.jpg)

There 2 tools: managed by command line parameters, and managed by script. Command line parameters for first case can be followed:
```
command line parameters usage:
  -pool number
      working threads pool size, default is 4 for multicore processors.
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
```
To produce image above execute `buddhabrot.param.x64.exe -sq=2500 -fn buddhasingle.tga -nmax=2500 -tmax=2500`
To run second tool be sure that `buddhabrot.lua` script is placed to current directory, or put script to command line.

## Clifford attractors ##

[![clifford #1](http://images.sevstar.net/images/17209669908976374767_thumb.png)](http://images.sevstar.net/images/17209669908976374767.jpg)
[![clifford #2](http://images.sevstar.net/images/15444013609743366105_thumb.png)](http://images.sevstar.net/images/15444013609743366105.jpg)
[![clifford #3](http://images.sevstar.net/images/96240766713180008594_thumb.png)](http://images.sevstar.net/images/96240766713180008594.jpg)
[![clifford #4](http://images.sevstar.net/images/67101706209135196956_thumb.png)](http://images.sevstar.net/images/67101706209135196956.jpg)

There 3 tools for images rendering: dialog-mode tool, batch-mode tool and tool managed by script.
Compile them at first, and set current directory as for Buddhabrot.
Dialog-mode tool lets place individual parameters at command prompt for each rendering.
Batch-mode tool takes the file `cliford.txt` with configuration, that includes image parameters and geometry description.
Preset provided with sources makes images as above. Play with those parameters to get different rendering results.
You can make as much images as you like by one tool call.
Tool managed by script takes `cliford.lua` or script placed on command line parameter and execute it.
This tool provides best possibility to automate rendering.

## License

Author: &copy; schwarzlichtbezirk (schwarzlichtbezirk@gmail.com)  
The project is released under the [MIT license](http://www.opensource.org/licenses/MIT).
