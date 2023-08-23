load("@bazel_skylib//lib:paths.bzl", "paths")

def _ts_impl(ctx):

    exe = ctx.file._tool.path

    tc = ctx.toolchains["@rules_nodejs//nodejs:toolchain_type"]
    node_file = tc.nodeinfo.tool_files
    node_path = tc.nodeinfo.target_tool_path

    # print("node dir: %s" % node_file[0].dirname)
    # print("node target_tool_path: %s" % node.nodeinfo.target_tool_path)

    # print("GRAMMAR: %s" % ctx.file.grammar)
    # print("OUT: %s" % ctx.outputs.out)

    outputs = []

    args = ctx.actions.args()
    args.add("generate")
    args.add_all(ctx.attr.opts)
    args.add("--grammar", ctx.file.grammar.path)
    args.add("-o", ctx.outputs.out.path)
    # also emits src/node-types.json, src/tree_sitter/parser.h
    outputs.append(ctx.actions.declare_file("node-types.json"))
    outputs.append(ctx.actions.declare_file("parser.h"))

    if ctx.attr.rust_bindings:
        # generates in bindings/rust: lib.rs, build.rs
        # generates in cwd: Cargo.toml
        # TODO: generate BUILD.bazel
        outputs.append(ctx.actions.declare_file("bindings/rust/lib.rs"))
        outputs.append(ctx.actions.declare_file("bindings/rust/build.rs"))
        cargo = ctx.actions.declare_file("Cargo.toml")
        outputs.append(cargo)
        # buildbzl = ctx.actions.declare_file("BUILD.bazel")
        args.add("--rust-bindings", cargo.dirname)

    if ctx.attr.node_bindings:
        # generates in cwd:  binding.gyp
        # generates in bindings/js: index.js, binding.cc,
        # TODO: generate BUILD.bazel
        outputs.append(ctx.actions.declare_file("bindings/node/index.js"))
        outputs.append(ctx.actions.declare_file("bindings/node/binding.cc"))
        gyp = ctx.actions.declare_file("binding.gyp")
        outputs.append(gyp)
        # buildbzl = ctx.actions.declare_file("BUILD.bazel")
        args.add("--node-bindings", gyp.dirname)

    ctx.actions.run(
        env = {"PATH": "$PATH:" + node_file[0].dirname},
        executable = exe,
        arguments = [args],
        inputs = [ctx.file.grammar],
        outputs = [ctx.outputs.out] + outputs,
        tools = [ctx.file._tool] + node_file,
        mnemonic = "TreeSitter",
        progress_message = "generating parser"
    )

    return [DefaultInfo(files = depset([ctx.outputs.out]))]

#############
tree_sitter_genparser = rule(
    implementation = _ts_impl,
    attrs = {
        "grammar": attr.label(
            allow_single_file = True,
        ),
        "out": attr.output( ),
        "rust_bindings": attr.bool(),
        "node_bindings": attr.bool(),
        "opts": attr.string_list(
        ),
        "_tool": attr.label(
            allow_single_file = True,
            default = "//cli:tree-sitter",
            executable = True,
            cfg = "exec"
        )
    },
    toolchains = ["@rules_nodejs//nodejs:toolchain_type"]
)

