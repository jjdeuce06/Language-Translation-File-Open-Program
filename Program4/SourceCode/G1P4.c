/***********HEADER*******************/
/*  GROUP 1:
        - Margo Bonal: bon8330@pennwest.edu
        - John Gerega: ger9822@pennwest.edu
        - Luke Ruffing: RUF96565@pennwest.edu
    COURSE: Language Translation - CMSC-4180-100
    PROGRAM NAME: Parser
*/
/***********END HEADER**************/

#include <stdio.h>
#include <stdlib.h>
#include "file_util.h"
#include "scanner.h"
#include "parser.h"

/*
 * main
 * - handles command-line/file setup
 * - opens files
 * - starts parser
 * - parses from <system goal>
 * - prints totals / closes files in wrapup
 */
int main(int argc, char *argv[])
{
    FILE_STATUS status;
    char buffer[128];

    /* Print header/banner */
    header();

    /* Handle command-line arguments / prompts */
    status = handleArgs(argc, argv);

    /* If user chose to quit, stop here */
    if (status == FILE_QUIT)
    {
        printf("Program Terminated\n");
        return 0;
    }

    /* Open input, output, listing, temp files */
    openFiles();

    /* Initialize parser + scanner state */
    parser_startup();

    /* Begin parsing from the grammar start symbol */
    system_goal(buffer);

    /* Print totals to listing file */
    parser_finish();

    /* Cleanup / append temp file / close files */
    wrapup();

    return 0;
}