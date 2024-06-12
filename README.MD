# SB16 Simple Waveform Player for 80x86

This is a simple program that plays a waveform for sound blaster.
It can be 8-bit or 16-bit, mono or stereo, 8kHz, 11kHz, 22kHz, 44kHz.

It's written purely in C and you need OpenWatcom to compile it.

## Compiling

Just run `wmake` in the root directory. It will create an `.exe` program
that you can run on [DOSBox](https://www.dosbox.com/), [86Box](https://86box.net/)
or a real 80x86 machine.

The options for compiling using `wmake` are:

- `PROC_TYPE` - The processor type. It can be `8086`, `80286`, `80386`, `80486` or `Pentium`.
- `DEBUG` - If defined, it will compile with debugging information. Otherwise, it will compile with optimizations.
  The default is to compile with debugging information.

## Running

Just run the program with the name of the file you want to play. For example:

```
sb16.exe test_wav.raw
```

## Limitation 

The program only plays raw PCM data. It doesn't support WAV files with headers for now

## License

This code is licensed under the GPLV3 License. See the `LICENSE` file for more information.