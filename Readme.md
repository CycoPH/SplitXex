# SplitXex #
A tool to split out/extract code segments from an Atari 8-bit executable file.

## Command line options ##
```
Split an Atari 8-bit executable file into components and export them.
Usage:
SplitXex in.xex [#block #block ... out.obx]
        in.xex - the input file. If this is the only parameter list the parts
        #1-#255 - The segment number to dump to the output
        out.obj - File where data is written to
```

## How does it work ##
The Atari executable file format breaks the file into segments.  Each segment has a header which determines where the data of the segment needs to be loaded. The program parses these headers and gives each one a sequential number.

As an example we can look at a very simple program, found in `Example/rainbow.xex`
```
FF FF E0 02 E1 02 00 20 00 20 13 20 A9 00 18 69 A0 6D 0B D4 8D 0A D4 8D 1A D0 8D 18 D0 4C 00 20
```

Running the tool with just the executable specified we get
```
Loading rainbow.xex...
Segment #1 : 02E0 with 0x0002:2 bytes
Segment #2 : 2000 with 0x0014:20 bytes
```
There are two segments:
 - #1 to be loaded at 0x02E0 has two bytes (this is the run address)
- #2 to be loaded at 0x2000 has 20 bytes (this is the actual code to be executed)

To extract the code segment you do the following:
```
SplitXex rainbow.xex #2 next.obx
```

This will copy just the code segment into the next.obx file. Do note that the first segment in the target file will have the FF FF bytes prepended.  This indicates that it is the first header in an executable file.

## For some fun ##
You can rearrange the executable and move the start segment of the `rainbow.xex` program to the end.

```
splitxex rainbow.xex #2 #1 a.xex
```
This will copy the code segment into position 1 and the execute segment into position 2.
If you run `a.xex` it will still work as expected.

## Release Notes
### 1.0.0
- Initial release

