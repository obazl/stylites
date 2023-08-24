use anyhow::{Context, Error, Result};
use clap::{App, AppSettings, Arg};
// use glob::glob;
// use std::collections::HashSet;
// use std::path::{Path, PathBuf};
use std::{env}; //, fs, u64};
// use tree_sitter::{ffi, Point};
use generator::generator;
use ::generator::generator::logger;

const BUILD_VERSION: &'static str = env!("CARGO_PKG_VERSION");
const BUILD_SHA: Option<&'static str> = option_env!("BUILD_SHA");
const DEFAULT_GENERATE_ABI_VERSION: usize = 14;

fn main() {
    let result = run();
    if let Err(err) = &result {
        // Ignore BrokenPipe errors
        if let Some(error) = err.downcast_ref::<std::io::Error>() {
            if error.kind() == std::io::ErrorKind::BrokenPipe {
                return;
            }
        }
        if !err.to_string().is_empty() {
            eprintln!("{:?}", err);
        }
        std::process::exit(1);
    }
}

fn run() -> Result<()> {
    let version = if let Some(build_sha) = BUILD_SHA {
        format!("{} ({})", BUILD_VERSION, build_sha)
    } else {
        BUILD_VERSION.to_string()
    };

    let _debug_arg = Arg::with_name("debug")
        .help("Show parsing debug log")
        .long("debug")
        .short("d");

    // let debug_graph_arg = Arg::with_name("debug-graph")
    //     .help("Produce the log.html file with debug graphs")
    //     .long("debug-graph")
    //     .short("D");

    // let debug_build_arg = Arg::with_name("debug-build")
    //     .help("Compile a parser in debug mode")
    //     .long("debug-build")
    //     .short("0");

    // let paths_file_arg = Arg::with_name("paths-file")
    //     .help("The path to a file with paths to source file(s)")
    //     .long("paths")
    //     .takes_value(true);

    // let paths_arg = Arg::with_name("paths")
    //     .help("The source file(s) to use")
    //     .multiple(true);

    // let scope_arg = Arg::with_name("scope")
    //     .help("Select a language by the scope instead of a file extension")
    //     .long("scope")
    //     .takes_value(true);

    // let time_arg = Arg::with_name("time")
    //     .help("Measure execution time")
    //     .long("time")
    //     .short("t");

    let _quiet_arg = Arg::with_name("quiet")
        .help("Suppress main output")
        .long("quiet")
        .short("q");

    // let apply_all_captures_arg = Arg::with_name("apply-all-captures")
    //     .help("Apply all captures to highlights")
    //     .long("apply-all-captures");

    let matches = App::new("tree-sitter-generator")
        .author("Max Brunsfeld <maxbrunsfeld@gmail.com>")
        .about("Generates parsers")
        .version(version.as_str())
        .global_setting(AppSettings::ColoredHelp)
        .global_setting(AppSettings::DeriveDisplayOrder)
        .global_setting(AppSettings::DisableHelpSubcommand)
        .arg(Arg::with_name("log").long("log"))
        // .arg(Arg::with_name("grammar-path").index(1))
        .arg(Arg::with_name("grammar")
             .long("grammar")
             .short("g")
             .takes_value(true)
             .required(true)
             .value_name("grammar-path")
             // .help("path to grammar file"),
        )
        .arg(Arg::with_name("outpath")
             .long("out")
             .short("o")
             .takes_value(true)
             .required(true)
             .value_name("out-path")
             // .help("path to output file"),
        )
    // ABI version of the tree-sitter lib
        .arg(Arg::with_name("abi-version")
             .long("abi")
             .value_name("version")
             .help(&format!(
                 concat!(
                     "Select the language ABI version to generate (default {}).\n",
                     "Use --abi=latest to generate the newest supported version ({}).",
                 ),
                 DEFAULT_GENERATE_ABI_VERSION,
                 tree_sitter::LANGUAGE_VERSION,
             )),
        )
        .arg(Arg::with_name("rust-bindings")
             .long("rust-bindings")
             .takes_value(true)
             .value_name("rust-bindings-dir")
             .help("Generate rust bindings"),
        )
        .arg(Arg::with_name("node-bindings")
             .long("node-bindings")
             .takes_value(true)
             .value_name("node-bindings-dir")
             .help("Generate nodejs bindings"),
        )
        // .arg(Arg::with_name("libdir")
        //      .long("libdir")
        //      .takes_value(true)
        //      .value_name("path"),
        // )
        // .arg(Arg::with_name("report-states-for-rule")
        //      .long("report-states-for-rule")
        //      .value_name("rule-name")
        //      .takes_value(true),
        // )
        .arg(Arg::with_name("js-runtime")
             .long("js-runtime")
             .takes_value(true)
             .value_name("executable")
             .env("TREE_SITTER_JS_RUNTIME")
             .help("Use a JavaScript runtime other than node"),
        )
        .get_matches();
    println!("matches: {:?}", matches);

    let current_dir = env::current_dir().unwrap();
    println!("current_dir: {}", current_dir.display());

    // let config = Config::load()?;
    // let mut loader = loader::Loader::new()?;


    // match matches.subcommand() {
    //     ("generate", Some(matches)) => {
    let grammar_path = matches.value_of("grammar");
    let out_path = matches.value_of("outpath");
    let generate_rust_bindings = matches.value_of("rust-bindings");
    let node_bindings_outdir = matches.value_of("node-bindings");

    // let debug_build = matches.is_present("debug-build");
    // let build = matches.is_present("build");
    let _libdir = matches.value_of("libdir");
    let js_runtime = matches.value_of("js-runtime");
    let report_symbol_name = matches.value_of("report-states-for-rule").or_else(|| {
        if matches.is_present("report-states") {
            Some("")
        } else {
            None
        }
    });
    if matches.is_present("log") {
        logger::init();
    }
    let abi_version = matches.value_of("abi-version").map_or(
        Ok::<_, Error>(DEFAULT_GENERATE_ABI_VERSION),
        |version| {
            Ok(if version == "latest" {
                tree_sitter::LANGUAGE_VERSION
            } else {
                version
                    .parse()
                    .with_context(|| "invalid abi version flag")?
            })
        },
    )?;
    println!("generate_rust_bindings: {:?}",
             generate_rust_bindings);
    println!("node_bindings_outdir: {:?}",
             node_bindings_outdir);

    generator::generate_parser_in_directory(
        &current_dir,
        grammar_path,
        out_path,
        abi_version,
        // generate_bindings,
        generate_rust_bindings,
        node_bindings_outdir,
        report_symbol_name,
        js_runtime,
    )?;
    // _ => unreachable!(),
    Ok(())
}
