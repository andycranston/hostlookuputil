static char *version = "@(!--#) @(#) hostlookuputil.c, sversion 0.1.0, fversion 001, 30-september-2025";

/*
#define DEBUG
*/

/*
 *  hostlookuputil.c
 *
 *  a utility to do forward and reverse lookups of hostnames and IP addresses
 *  using the getaddrinfo system call
 *
 */

/**********************************************************************/

/*
 *  Links:
 *    https://www.delftstack.com/howto/c/getaddrinfo-in-c/
 *
 *  Notes:
 *
 */

/**********************************************************************/

/*
 *  includes
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>

/**********************************************************************/

/*
 *  defines
 */

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define FORWARD_MODE 0
#define REVERSE_MODE 1

/**********************************************************************/

/*
 *  global variables
 */

char			*progname;

/**********************************************************************/

/* function */
void usage()
{
	fprintf(stderr, "%s: usage %s [ -f ] [ -r ] hostname/ip_address ...\n", progname, progname);

	exit(2);
}

/**********************************************************************/

/* function */
char *basename(s)
	char    *s;
{
	char    *bn;

	bn = s;

	while (*s != '\0') {
		if (*s == '/') {
			if (*(s+1) != '\0') {
				bn = s+1;
			}
		}

		s++;
	}

	return bn;
}

/**********************************************************************/

/*
 *  Main
 */

/* function */
int main(argc, argv)
	int     argc;
	char    *argv[];
{
	int	mode;
	int	arg;

	progname = basename(argv[0]);

	mode = FORWARD_MODE;

	if (argc == 1) {
		if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "-h") == 0)) {
			usage();
		}
	}

	arg = 1;
	while (arg < argc) {
		if (strcmp(argv[arg], "-f") == 0) {
			mode = FORWARD_MODE;
		} else if (strcmp(argv[arg], "-r") == 0) {
			mode = REVERSE_MODE;
		} else {
			switch (mode) {
				case FORWARD_MODE:
					forward_lookup(argv[arg]);
					break;
				case REVERSE_MODE:
					reverse_lookup(argv[arg]);
					break;
				default:
					fprintf(stderr, "%s: internal error - mode = %d which is out of range\n", progname, mode);
					exit(2);
			}
		}

		arg++;
	}

	return 0;
}

/**********************************************************************/

/* end of file: hostlookuputil.c */
