#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "gopt.h"
/* #include "log.h" */
#include "unity.h"
/* #include "utarray.h" */
/* #include "utstring.h" */

/* #include "macros.h" */

#include "tree_sitter/api.h"

extern struct option options[];

bool verbose;
bool debug;

TSLanguage *tree_sitter_imp(void);
TSParser *parser;

char *cmd;

void setUp(void) {
    /* log_info("setup"); */
}

void tearDown(void) {
    /* log_info("teardown"); */
}

void cleanup(void)
{
    /* https://wiki.sei.cmu.edu/confluence/display/c/FIO23-C.+Do+not+exit+with+unflushed+data+in+stdout+or+stderr */
    /* Do cleanup */
    /* printf("All cleaned up!\n"); */
    if (fflush(stdout) == EOF) {
        /* Handle error */
    }
}

void testa(void) {
    /* log_debug("testa") */
    const char *source_code = "skip";
    TSTree *tree = ts_parser_parse_string(
                                          parser,
                                          NULL,
                                          source_code,
                                          strlen(source_code)
                                          );
    // Get the root node of the syntax tree.
    TSNode root_node = ts_tree_root_node(tree);

    char *string = ts_node_string(root_node);
    fprintf(stderr, "Syntax tree: %s\n", string);

    TEST_ASSERT_EQUAL_STRING(string,
                             "(program (stmt (skip)))");
}

enum OPTS {
    FLAG_HELP,
    FLAG_DEBUG,
    FLAG_DEBUG_CONFIG,
    FLAG_DEBUG_SCM,
    FLAG_DEBUG_SCM_LOADS,

    FLAG_SHOW_CONFIG,
    FLAG_TRACE,
    FLAG_VERBOSE,

    LAST
};

struct option options[] = {
    /* 0 */
    [FLAG_DEBUG] = {.long_name="debug",.short_name='d',
                    .flags=GOPT_ARGUMENT_FORBIDDEN | GOPT_REPEATABLE},
    [FLAG_DEBUG_CONFIG] = {.long_name="debug-config",
                           .flags=GOPT_ARGUMENT_FORBIDDEN},
    [FLAG_DEBUG_SCM] = {.long_name="debug-scm", .short_name = 'D',
                        .flags=GOPT_ARGUMENT_FORBIDDEN},
    [FLAG_DEBUG_SCM_LOADS] = {.long_name="debug-scm-loads",
                              .flags=GOPT_ARGUMENT_FORBIDDEN},
    [FLAG_SHOW_CONFIG] = {.long_name="show-config",
                          .flags=GOPT_ARGUMENT_FORBIDDEN},
    [FLAG_TRACE] = {.long_name="trace",.short_name='t',
                    .flags=GOPT_ARGUMENT_FORBIDDEN},
    [FLAG_VERBOSE] = {.long_name="verbose",.short_name='v',
                      .flags=GOPT_ARGUMENT_FORBIDDEN | GOPT_REPEATABLE},
    [FLAG_HELP] = {.long_name="help",.short_name='h',
                   .flags=GOPT_ARGUMENT_FORBIDDEN},
    [LAST] = {.flags = GOPT_LAST}
};

void print_usage(char *test) {
    printf("Usage:\t$ bazel test test:%s [-- flags]\n", test);
    /* printf("  Link strategies:\n"); */
    /* printf("\t--//config/clibs/link=static\tStatic link/load (static libs). Default.\n"); */
    /* printf("\t--//config/clibs/link=shared\tStatic link, dynamic load (shared libs)\n"); */
    /* printf("\t--//config/clibs/link=runtime\tLink/load at runtime (dload shared libs)\n"); */
    /* printf("  (prefix @libs7 if used as external repo), e.g.\n"); */
    /* printf("\t\t--@libs7//config/clibs/link=shared\n"); */

    printf("  Flags (repeatable)\n");
    printf("\t-d, --debug\t\tEnable all debugging flags.\n");
    /* printf("\t--debug-config\t\tEnable all config debugging flags.\n"); */
    /* printf("\t--debug-scm\t\tEnable all scheme debugging flags.\n"); */
    printf("\t-t, --trace\t\tEnable trace flags.\n");
    printf("\t-v, --verbose\t\tEnable verbosity. Repeatable.\n");

}

void set_options(char *test, struct option options[])
{
    /* log_trace("set_options"); */
    if (options[FLAG_HELP].count) {
        print_usage(test);
        exit(EXIT_SUCCESS);
    }
    if (options[FLAG_DEBUG].count) {
        debug = true;
        /* libs7_debug = true; */
    }
    if (options[FLAG_TRACE].count) {
        /* libs7_trace = true; */
    }
    if (options[FLAG_VERBOSE].count) {
        /* verbosity = options[FLAG_VERBOSE].count; */
        /* log_info("verbosity: %d", verbosity); */
        verbose = true;
    }
}

/* void print_debug_env(void) */
/* { */
/*     log_debug("getcwd: %s", getcwd(NULL, 0)); */
/*     log_debug("getenv(PWD): %s", getenv("PWD")); */

/*     // $HOME - not reliable, use getpwuid() instead */
/*     log_debug("getenv(HOME): %s", getenv("HOME")); */
/*     struct passwd* pwd = getpwuid(getuid()); */
/*     log_debug("pwd->pw_dir: %s", pwd->pw_dir); */

/*     // BAZEL_CURRENT_REPOSITORY: null when run from 'home' repo, 'libs7' when run as external repo */
/*     log_debug("BAZEL_CURRENT_REPOSITORY (macro): '%s'", BAZEL_CURRENT_REPOSITORY); */

/*     // TEST_WORKSPACE: always the root ws */
/*     log_debug("TEST_WORKSPACE: '%s'", getenv("TEST_WORKSPACE")); */

/*     // BAZEL_TEST: should always be true when this is compiled as cc_test */
/*     log_debug("BAZEL_TEST: '%s'", getenv("BAZEL_TEST")); */

/*     // BUILD_WORK* vars: null under 'bazel test' */
/*     log_debug("BUILD_WORKSPACE_DIRECTORY: %s", getenv("BUILD_WORKSPACE_DIRECTORY")); */
/*     log_debug("BUILD_WORKING_DIRECTORY: %s", getenv("BUILD_WORKING_DIRECTORY")); */

/*     // TEST_SRCDIR - required for cc_test */
/*     log_debug("TEST_SRCDIR: %s", getenv("TEST_SRCDIR")); */
/*     log_debug("BINDIR: %s", getenv("BINDIR")); */

/*     /\* RUNFILES_MANIFEST_FILE: null on macos. *\/ */
/*     log_debug("RUNFILES_MANIFEST_FILE: %s", getenv("RUNFILES_MANIFEST_FILE")); */

/*     /\* RUNFILES_MANIFEST_FILE: null on macos. *\/ */
/*     log_debug("RUNFILES_MANIFEST_ONLY: %s", getenv("RUNFILES_MANIFEST_ONLY")); */

/*     /\* RUNFILES_DIR: set on macos for both bazel test and bazel run. *\/ */
/*     log_debug("RUNFILES_DIR: %s", getenv("RUNFILES_DIR")); */
/* } */

int main(int argc, char **argv)
{
    atexit(cleanup);

    if ( !getenv("BAZEL_TEST") ) {
        fprintf(stderr, "This test must be run in a Bazel environment: bazel test //path/to/test (or bazel run)\n" );
        exit(EXIT_FAILURE);
    }

    /* log_trace("WS: %s", getenv("TEST_WORKSPACE")); */
    /* log_debug("ARGV[0]: %s", argv[0]); */
    /* log_debug("CWD: %s", getcwd(NULL, 0)); */

    argc = gopt (argv, options);
    (void)argc;
    gopt_errors (argv[0], options);

    set_options("imp", options);

    /* if (debug) print_debug_env(); */

    parser = ts_parser_new();
    ts_parser_set_language(parser, tree_sitter_imp());

    UNITY_BEGIN();

    RUN_TEST(testa);

    ts_parser_delete(parser);

    return UNITY_END();
}
