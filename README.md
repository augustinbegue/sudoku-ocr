# Sudoku OCR

## Usage

### GUI

The GUI can be used by executing the executable with the follwing name:

```
$ ./sudoku-ocr-gui
```

### CLI

The CLI automatically performs the image processing and grid splitting in verbose mode. Here is how to use it:

```
$ ./sudoku-ocr --help
Usage: ./main file [options]

Options:
        -o <file>     Save the output into <file>. (PNG format)
        -r <angle>    Rotate the image according to the specified <angle>.
        -m <file>     Save the mask image into <file>.  (PNG format)
        -v <path>     Verbose mode: save every step of the process in the folder <path>.
```

You can then use the network to process each digit individually

```
$ ./network test <path>
```

### Network

You can generate a training set by executing the script in the following path:

*Note: Please make sure you've got python3 installed along with Pillow (PIL)*

```
$ cd ./neural_network/training_set_generation

$ python3 generate.py
```

Then, you can train it using

```
$ ./network train
```

You can then use the network to process each digit individually

```
$ ./network test <path>
```

## Build

- Make sure you've installed ``make``, ``gcc``, ``sdl2``, ``sdl2_image`` and ``gtk3``

- Clone the repo

- Execute ``make`` at the root of the repo

## Contents of the Repository

### Project Reports
- ``./rapport_1.pdf`` : First Defense Report
- ``./rapport_2.pdf`` : Second Defense Report

### Pictures
-  ``./assets/grids/image_0[1-6].jpeg`` : Pictures Used for Testing 