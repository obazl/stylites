# tree-sitter - bazel-enabled

[![CICD badge]][CICD]
[![DOI](https://zenodo.org/badge/14164618.svg)](https://zenodo.org/badge/latestdoi/14164618)

[CICD badge]: https://github.com/tree-sitter/tree-sitter/actions/workflows/CICD.yml/badge.svg
[CICD]: https://github.com/tree-sitter/tree-sitter/actions/workflows/CICD.yml

BAZEL STATUS: Very lightly tested, on MacOS only. This works: `bazel run cli:tree-sitter`. The `cli` branch contains a custom
`tree_sitter_genparser` rule, a version of `tree-sitter-cli` modified to improve the UI (and play nicer with Bazel), and simple test case (`bazel build example/hello:klingon.c`).

Branch `test` contains `//examples/imp`, an implementation of the [imp](https://softwarefoundations.cis.upenn.edu/lf-current/Imp.html) language (grammar from [How to write a tree-sitter grammar in an afternoon](https://siraben.dev/2022/03/01/tree-sitter.html). Builds the parser as a `cc_library` and includes a `cc_test` target that uses [Unity](https://github.com/ThrowTheSwitch/Unity).

-----
Original readme:

Tree-sitter is a parser generator tool and an incremental parsing library. It can build a concrete syntax tree for a source file and efficiently update the syntax tree as the source file is edited. Tree-sitter aims to be:

- **General** enough to parse any programming language
- **Fast** enough to parse on every keystroke in a text editor
- **Robust** enough to provide useful results even in the presence of syntax errors
- **Dependency-free** so that the runtime library (which is written in pure C) can be embedded in any application

## Links

- [Documentation](https://tree-sitter.github.io)
- [Rust binding](lib/binding_rust/README.md)
- [WASM binding](lib/binding_web/README.md)
- [Command-line interface](cli/README.md)
