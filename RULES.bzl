load("@bazel_skylib//lib:paths.bzl", "paths")

def _ts_impl(ctx):

    exe = ctx.file._tool.path

    tc = ctx.toolchains["@rules_nodejs//nodejs:toolchain_type"]
    node_file = tc.nodeinfo.tool_files
    node_path = tc.nodeinfo.target_tool_path

    print("node dir: %s" % node_file[0].dirname)
    # print("node target_tool_path: %s" % node.nodeinfo.target_tool_path)

    print("GRAMMAR: %s" % ctx.file.grammar)
    print("OUT: %s" % ctx.outputs.out)

    args = ctx.actions.args()
    args.add("generate")
    args.add_all(ctx.attr.args)
    args.add("--grammar", ctx.file.grammar.path)
    args.add("-o", ctx.outputs.out.path)

    # cmd = "echo PWD: `pwd`; {ts} generate {args} {grammar} --log; /bin/ls -l src".format(
    #     ts=exe,
    #     args = "", # ctx.attr.args,
    #     grammar = ctx.file.grammar.path
    # )

    ctx.actions.run(
        env = {"PATH": "$PATH:" + node_file[0].dirname},
        executable = exe,
        arguments = [args],
        inputs = [ctx.file.grammar],
        outputs = [ctx.outputs.out],
        tools = [ctx.file._tool] + node_file,
        mnemonic = "TreeSitter",
        progress_message = "generating parser"
    )

    return [DefaultInfo(files = depset([ctx.outputs.out]))]

#############
ts_genparser = rule(
    implementation = _ts_impl,
    attrs = {
        "grammar": attr.label(
            allow_single_file = True,
        ),
        "out": attr.output( ),
        "args": attr.string_list(
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

