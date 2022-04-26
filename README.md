# shellso

A shell written in C for the Operational Systems subject at UFSJ taught by
[Rafael Sachetto](https://github.com/rsachetto/).

Online documentation available in [here](https://marcosfons.github.io/shellso).


## Table of contents

1. [Setup](#Setup)
2. [Usage](#Usage)
3. What it does not support
5. [Authors](#Authors)



### Setup

Clone this project in [online repo](#https://github.com/marcosfons/shellso).
Be sure that you have installed some `C` compiler and `make`. As optional
dependencies you can install:
- [`Doxygen`](https://www.doxygen.nl/manual/install.html) to generate docs
- [`Criterion`](https://criterion.readthedocs.io/en/master/setup.html) to run
	the tests

If you choosed to run the tests with `Criterion`, install it and place the `.so`
files inside a `include` directory in the root folder. Or set the
`LD_LIBRARY_PATH` environment variable to its installation path.

### Usage

`Makefile` was used to make it easier the process of building the application.
Running make in the root folder of the project the message below will be
displayed.

```shell
$ make
Usage: make [target] ...
targets:
   clean                Clean all builded files
   debug                Build an executable to debug
   docs                 Generate and try to open docs
   help                 Show a help message
   release              Build a release executable
   test                 Build and run tests
```

To use the application the best way to compile it is by using the release
target.

```shell
make release
bin/shellso
```


### What it does not support

- Environment variable substitution
```shell
echo "$ENV_VAR"
```

- Environment variables before commands 
```shell
ENV_VAR=123 echo "$ENV_VAR"
```



### Authors

- [Filipe Mateus](https://github.com/fm4teus)
- [Marcos Fonseca](https://github.com/marcosfons)

