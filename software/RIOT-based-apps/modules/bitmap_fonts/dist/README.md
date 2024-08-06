# BDF to C Converter

This contains a small script that converts bitmap fonts in the BDF format into
C code using a custom binary encoding that is explained in the API
documentation of the matrix\_fonts module in the parent folder.

The script is so small because I use the [bdfparser][repo-bdfparser] library by
[tomchen](https://github.com/tomchen), which I just vendored in for ease of use
and reproducibility.

The font is token from the [Matrix-Fonts repository][repo-matrix-fonts] by
[trip5](https://github.com/trip5) and again vendored in for ease of use
and reproducibility.

## License

Both the font imported from the [Matrix-Fonts repository][repo-matrix-fonts] as
well as the [bdfparser][repo-bdfparser] are licensed under the MIT license.
For simplicity and consistency, I also license the script under MIT.

[repo-bdfparser]: https://github.com/tomchen/bdfparser
[repo-matrix-fonts]: https://github.com/trip5/Matrix-Fonts
