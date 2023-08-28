// original: linenoise/example.c

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

#include "gopt.h"
#include "linenoise.h"
#include "log.h"
#include "utstring.h"

void completion(const char *buf, linenoiseCompletions *lc) {
    if (buf[0] == 'h') {
        linenoiseAddCompletion(lc,"hello");
        linenoiseAddCompletion(lc,"hello there");
    }
}

char *hints(const char *buf, int *color, int *bold) {
    if (!strcasecmp(buf,"hello")) {
        *color = 35;
        *bold = 0;
        return " World";
    }
    return NULL;
}

enum OPTS {
    FLAG_HELP,
    FLAG_DEBUG,
    FLAG_TRACE,
    FLAG_VERBOSE,
    FLAG_QUIET,
    FLAG_VERSION,

    FLAG_MULTILINE,
    FLAG_KEYCODES,
    FLAG_ASYNC,

    OPT_PARSER,
    LAST
};

static struct option options[] = {
    [FLAG_HELP] = {.long_name="help",.short_name='h',
                   .flags=GOPT_ARGUMENT_FORBIDDEN | GOPT_REPEATABLE},
    [FLAG_DEBUG] = {.long_name="debug",.short_name='d',
                    .flags=GOPT_ARGUMENT_FORBIDDEN | GOPT_REPEATABLE},
    [FLAG_TRACE] = {.long_name="trace",.short_name='t',
                    .flags=GOPT_ARGUMENT_FORBIDDEN},
    [FLAG_VERBOSE] = {.long_name="verbose",.short_name='v',
                      .flags=GOPT_ARGUMENT_FORBIDDEN | GOPT_REPEATABLE},
    [FLAG_QUIET] = {.long_name="quiet",.short_name='q',
                    .flags=GOPT_ARGUMENT_FORBIDDEN},
    [FLAG_VERSION] = {.long_name="version",
                    .flags=GOPT_ARGUMENT_FORBIDDEN},

    [FLAG_MULTILINE] = {.long_name="multiline",
                    .flags=GOPT_ARGUMENT_FORBIDDEN},
    [FLAG_KEYCODES] = {.long_name="keycodes",
                    .flags=GOPT_ARGUMENT_FORBIDDEN},
    [FLAG_ASYNC] = {.long_name="async",
                    .flags=GOPT_ARGUMENT_FORBIDDEN},

    [OPT_PARSER] = {.long_name="parser",
                    .flags=GOPT_ARGUMENT_REQUIRED},

    [LAST] = {.flags = GOPT_LAST}
};

void _print_usage(void) {
    printf("Usage:\t$ bazel run [@tree-sitter//]repl [flags] --parser <lang>\n");

    printf("Option:\n");
    printf("\t--parser <lang>\n");

    printf("Flags:\n");
    printf("\t-d, --debug\t\tEnable all debugging flags.\n");
    printf("\t-h, --help\t\tPrint help screen.\n");
    printf("\t-t, --trace\t\tEnable trace flags.\n");
    printf("\t-v, --verbose\t\tEnable verbosity. Repeatable.\n");
    printf("\t-q, --quiet\t\tSuppress msgs to stdout/stderr.\n");
    printf("\t--version\t\tShow version Id.\n");
    /* printf("\n"); */
    /* printf("INI file: $XDG_CONFIG_HOME/miblrc\n"); */

    printf("\n");
}

int main(int argc, char **argv) {
    char *line;
    char *prgname = argv[0];
    int async = 0;

    char *lang;

    /* Parse options, with --multiline we enable multi line editing. */
    int gopt_argc = gopt(argv, options);
    (void)gopt_argc;

    gopt_errors(argv[0], options);

    if (options[FLAG_HELP].count) {
        /* if (options[FLAG_HELP].count > 1) */
        /*     _print_debug_usage(); */
        /* else */
            _print_usage();
        exit(EXIT_SUCCESS);
    }

    if (options[FLAG_MULTILINE].count) {
            linenoiseSetMultiLine(1);
            printf("Multi-line mode enabled.\n");
    }
    if (options[FLAG_KEYCODES].count) {
            linenoisePrintKeyCodes();
            exit(EXIT_SUCCESS);
    }
    if (options[FLAG_ASYNC].count) {
            async = 1;
    }

    if (options[OPT_PARSER].count) {
        lang = options[OPT_PARSER].argument;
    } else {
        _print_usage();
        exit(EXIT_SUCCESS);
    }

    /* while(argc > 1) { */
    /*     argc--; */
    /*     argv++; */
    /*     if (!strcmp(*argv,"--multiline")) { */
    /*     } else if (!strcmp(*argv,"--keycodes")) { */
    /*     } else if (!strcmp(*argv,"--async")) { */
    /*     } else if (!strcmp(*argv,"--parser")) { */
    /*         log_debug("parser"); */
    /*         exit(0); */
    /*     } else { */
    /*         fprintf(stderr, "Usage: %s [--multiline] [--keycodes] [--async]\n", prgname); */
    /*         exit(1); */
    /*     } */
    /* } */

    log_debug("BAZEL_CURRENT_REPOSITORY (macro): '%s'", BAZEL_CURRENT_REPOSITORY);

    char *runfiles;
    if (strlen(BAZEL_CURRENT_REPOSITORY) == 0) {
        runfiles = realpath("_main", NULL);
    } else {
        runfiles = realpath("external/" BAZEL_CURRENT_REPOSITORY "/repl", NULL);
    }
    log_debug("RUNFILES: %s", runfiles);

    UT_string *dso;
    utstring_new(dso);
    utstring_printf(dso, "%s/lib%s" DSO_EXT,
                    runfiles, lang);

    log_debug("runfiles: %s", runfiles);
    log_debug("cwd: %s", getcwd(NULL,0));
    log_debug("dso: %s", utstring_body(dso));

    void *handle = dlopen(utstring_body(dso), RTLD_LAZY);
    if (!handle) {
        /* fail to load the library */
        fprintf(stderr, "Error: %s\n", dlerror());
        log_debug("dlopen failed for %s", utstring_body(dso));
        return EXIT_FAILURE;
    } else {
        log_debug("dlopen succeeded for %s", utstring_body(dso));
    }

    /* Set the completion callback. This will be called every time the
     * user uses the <tab> key. */
    linenoiseSetCompletionCallback(completion);
    linenoiseSetHintsCallback(hints);

    /* Load history from file. The history file is just a plain text file
     * where entries are separated by newlines. */
    linenoiseHistoryLoad("history.txt"); /* Load the history at startup */

    /* Now this is the main loop of the typical linenoise-based application.
     * The call to linenoise() will block as long as the user types something
     * and presses enter.
     *
     * The typed string is returned as a malloc() allocated string by
     * linenoise, so the user needs to free() it. */

    while(1) {
        if (!async) {
            line = linenoise("hello> ");
            if (line == NULL) break;
        } else {
            /* Asynchronous mode using the multiplexing API: wait for
             * data on stdin, and simulate async data coming from some source
             * using the select(2) timeout. */
            struct linenoiseState ls;
            char buf[1024];
            linenoiseEditStart(&ls,-1,-1,buf,sizeof(buf),"hello> ");
            while(1) {
		fd_set readfds;
		struct timeval tv;
		int retval;

		FD_ZERO(&readfds);
		FD_SET(ls.ifd, &readfds);
		tv.tv_sec = 1; // 1 sec timeout
		tv.tv_usec = 0;

		retval = select(ls.ifd+1, &readfds, NULL, NULL, &tv);
		if (retval == -1) {
		    perror("select()");
                    exit(1);
		} else if (retval) {
		    line = linenoiseEditFeed(&ls);
                    /* A NULL return means: line editing is continuing.
                     * Otherwise the user hit enter or stopped editing
                     * (CTRL+C/D). */
                    if (line != linenoiseEditMore) break;
		} else {
		    // Timeout occurred
                    static int counter = 0;
                    linenoiseHide(&ls);
		    printf("Async output %d.\n", counter++);
                    linenoiseShow(&ls);
		}
            }
            linenoiseEditStop(&ls);
            if (line == NULL) exit(0); /* Ctrl+D/C. */
        }

        /* Do something with the string. */
        if (line[0] != '\0' && line[0] != '/') {
            printf("echo: '%s'\n", line);
            linenoiseHistoryAdd(line); /* Add to the history. */
            linenoiseHistorySave("history.txt"); /* Save the history on disk. */
        } else if (!strncmp(line,"/historylen",11)) {
            /* The "/historylen" command will change the history len. */
            int len = atoi(line+11);
            linenoiseHistorySetMaxLen(len);
        } else if (!strncmp(line, "/mask", 5)) {
            linenoiseMaskModeEnable();
        } else if (!strncmp(line, "/unmask", 7)) {
            linenoiseMaskModeDisable();
        } else if (line[0] == '/') {
            printf("Unreconized command: %s\n", line);
        }
        free(line);
    }
    return 0;
}
