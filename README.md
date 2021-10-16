# Sudoku OCR

## Usage

```
$ ./main --help
Usage: ./main file [options]

Options:
        -o <file>     Save the output into <file>. (PNG format)
        -r <angle>    Rotate the image according to the specified <angle>.
        -m <file>     Save the mask image into <file>.  (PNG format)
        -v <path>     Verbose mode: save every step of the process in the folder <path>.

For more information, see: https://github.com/augustinbegue/sudoku-ocr
```

## Build

- Make sure you've installed ``make``, ``gcc``, ``sdl2`` and ``sdl2_image``

- Clone the repo

- Execute ``make`` at the root of the repo