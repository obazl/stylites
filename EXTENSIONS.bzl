load(
    "@rules_rust//crate_universe:defs.bzl",
    "crate",
    "crates_repository",
)
load(
    "@rules_rust//crate_universe:repositories.bzl",
    "crate_universe_dependencies",
)

load(
    "@rules_rust//crate_universe:deps_bootstrap.bzl",
    "cargo_bazel_bootstrap",
)

print("EXEXEXE")
# cargo_bazel_bootstrap()

# crate_universe_dependencies()  # bootstrap = True)



def _crate_repo_impl(repository_ctx):
    print("CRATE REPO RULE")
    repository_ctx.file(
        "WORKSPACE.bazel",
        content = "# do not remove"
    )
    repository_ctx.file(
        "BUILD.bazel",
        content = "# do not remove"
    )
    repository_ctx.file(
        "defs.bzl",
        content = "# do not remove"
    )

###########
_crate_repo = repository_rule(
    implementation = _crate_repo_impl,
    attrs = {}
)

################

## TAG CLASSES
_init = tag_class(
    attrs = { }
    #     "packages": attr.string_list(),
    #     "_tool": attr.label(
    #         allow_single_file = True,
    #         default = "@obazl//new:coswitch",
    #         executable = True,
    #         cfg = "exec"
    #     )
    # }
)

## EXTENSION IMPL
def _crates_impl(module_ctx):
    print("CRATES ext");

    print("XXXXXXXXXXXXXXXX")
    crate_universe_dependencies() #bootstrap = True)
    cargo_bazel_bootstrap()
    print("YYYYYYYYYYYYYYYY")


    # _crate_repo(name = "crate_index")

    crates_repository(
        name = "crate_index",
        cargo_lockfile = "@stylites//:Cargo.lock",
        # lockfile must exist, may be empty
        lockfile = "@stylites//:cargo-bazel-lock.json",
        # generator = "@cargo_bazel_bootstrap//:cargo-bazel",
        packages = {
            "ansi_term": crate.spec(version = "0.12.1"),
            "anyhow": crate.spec(version  = "1.0.72"),
            "atty": crate.spec(version  = "0.2.14"),
            "clap": crate.spec(version  = "2.32"),
            "ctrlc": crate.spec(version = "3.4.0", features = ["termination"]),
            "difference": crate.spec(version  = "2.0.0"),
            "dirs": crate.spec(version  = "5.0.1"),
            "glob": crate.spec(version = "0.3.1"),
            "html-escape": crate.spec(version = "0.2.13"),
            "indexmap": crate.spec(version = "2.0.0"),
            "lazy_static": crate.spec(version = "1.4.0"),
            "log": crate.spec(version = "0.4.14",
                              features = ["std"]),
            "path-slash": crate.spec(version = "0.2.1"),
            "regex": crate.spec(version = "1.9.1"),
            "regex-syntax": crate.spec(version = "0.7.4"),
            "rustc-hash": crate.spec(version = "1.1.0"),
            "semver": crate.spec(version = "1.0.18"),
            # Due to https://github.com/serde-rs/serde/issues/2538
        "serde": crate.spec(version = "1.0, < 1.0.172",
                            features = ["derive"]),
            "serde_json": crate.spec(version = "1.0",
                                     features = ["preserve_order"]),
            "smallbitvec": crate.spec(version = "2.5.1"),
            "tiny_http": crate.spec(version = "0.12.0"),
            "walkdir": crate.spec(version = "2.3.3"),
            "webbrowser": crate.spec(version = "0.8.10"),
            "which": crate.spec(version = "4.4.0")
        },

    # manifests = [
        #     "@stylites~1.0.0//src/runtime/bindings/rust:Cargo.toml",
        #     "@stylites//tools/cli:Cargo.toml",
        #     "@stylites//tools/cli/config:Cargo.toml",
        #     "@stylites//tools/cli/src/tests/proc_macro:Cargo.toml",
        #     # "@stylites//:Cargo.toml",

    #     # "//cli/loader:Cargo.toml",
        #     # "//highlight:Cargo.toml",
        #     # "//tags:Cargo.toml",
        # ],
        supported_platform_triples = ["aarch64-apple-darwin"],
    )


########################
crates = module_extension(
  implementation = _crates_impl,
  tag_classes = {"init": _init},
)
