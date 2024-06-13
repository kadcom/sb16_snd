# SB16 Simple Waveform Player for 80x86

This is a simple program that plays a waveform for sound blaster.
It can be 8-bit or 16-bit, mono or stereo, 8kHz, 11kHz, 22kHz, 44kHz.

It's written purely in C and you need OpenWatcom to compile it.

## Compiling

This program is written in C and you need OpenWatcom to compile it. Download 
OpenWatcom from [here](https://open-watcom.github.io/) for version 2.0 and
download "Latest CI Build" from the "Release" section.

You can also use older version of OpenWatcom. It should work with version 1.9.
Download from [here](https://www.openwatcom.org/).

Just run `wmake` in the root directory. It will create an `.exe` program
that you can run on [DOSBox](https://www.dosbox.com/), [86Box](https://86box.net/)
or a real 80x86 machine.

Before running `wmake`, you need to set the environment variables for OpenWatcom.

For example, in Windows, you can run the following commands:

```
> owsetenv.bat 
```

In Linux, you can run the following commands:

```
$ source owsetenv.sh
```

Then you can run `wmake` to compile the program.

```
$ wmake
```

The options for compiling using `wmake` are:

- `PROC_TYPE` - The processor type. It can be `8086`, `80286`, `80386`, `80486` or `Pentium`.
- `DEBUG` - If defined, it will compile with debugging information. Otherwise, it will compile with optimizations.
  The default is to compile with debugging information.

## Running

Just run the program with the name of the file you want to play. The command 
line is:

```
sb16.exe -8 | -11 | -22 <filename.raw>
```

For example:

```
sb16.exe -8 bmu8_8k.raw
```

### Converting Wave to Raw for playing 

You can use ffmpeg to convert a WAV file to a raw file. For example:

```
ffmpeg -i input.wav -f u8 -ar 8000 -ac 1 output.raw
```

or use sox 

```
sox input.wav -r 8000 -c 1 -b 8 output.raw
```

You can then use teh raw file to play with the program.

## Limitation 

The program only plays raw PCM data. It doesn't support WAV files with headers for now

## License

This code is licensed under the GPLV3 License. See the `LICENSE` file for more information.
