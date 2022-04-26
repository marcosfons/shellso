# shellso

A shell written in C for the Operational Systems subject at UFSJ taught by
[Rafael Sachetto](https://github.com/rsachetto/).

Online documentation available in [here](https://marcosfons.github.io/shellso).


## Table of contents

1. [Setup](#Setup)
2. [Usage](#Usage)
3. What it does not support
4. [Authors](#Authors)



### Setup

Clone this project in [online repo](#https://github.com/marcosfons/shellso).
Be sure that you have installed some `C` compiler and `make`. As optional
dependencies you can install:
- [`Doxygen`](https://www.doxygen.nl/manual/install.html) to generate docs
- [`Criterion`](https://criterion.readthedocs.io/en/master/setup.html) to run
	the tests


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

- [Marcos Fonseca](https://github.com/marcosfons)

