static char *version = "@(!--#) @(#) hostlookuputil.c, sversion 0.1.0, fversion 004, 30-september-2025";

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

#define MAX_LINE_LENGTH 1023

#define MAX_HOSTNAME_LENGTH 1023

#define NSSWITCH_FILENAME "/etc/nsswitch.conf"
/*
#define NSSWITCH_FILENAME "/tmp/nsswitch.conf"
*/

/**********************************************************************/

/*
 *  global variables
 */

char			*progname;

/**********************************************************************/

/* function */
void usage()
{
	fprintf(stderr, "%s: usage %s [ -f ] [ -r ] [ -4 ] [ -6 ] hostname/ip_address ...\n", progname, progname);

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

void show_hosts_nsswitch(nsswitchfilename)
	char	*nsswitchfilename;
{
	FILE	*nsswitchfile;
	char	line[MAX_LINE_LENGTH + sizeof(char)];
	char	hostsline[MAX_LINE_LENGTH + sizeof(char)];
	int	match_count;

	if ((nsswitchfile = fopen(nsswitchfilename, "r")) == NULL) {
		printf("Cannot open name service switch file \"%s\"\n", nsswitchfilename);
	} else {
		hostsline[0] = '\0';

		match_count = FALSE;

		while (fgets(line, MAX_LINE_LENGTH, nsswitchfile) != NULL) {
			if (strncmp(line, "hosts:", 6) == 0) {
				match_count++;

				if (hostsline[0] == '\0') {
					strncpy(hostsline, line, MAX_LINE_LENGTH);
					hostsline[MAX_LINE_LENGTH] = '\0';
				}
			}
		}

		if (match_count == 0) {
			printf("No hosts: entry in name service switch file \"%s\"\n", nsswitchfilename);
		} else if (match_count == 1) {
			printf("%s", hostsline);
		} else {
			printf("Multiple hosts: entries (%d to be precise) in name service switch file \"%s\"\n", match_count, nsswitchfilename);

		}

		fclose(nsswitchfile);
	}

	return;
}

/**********************************************************************/

/* function */
void forward_lookup(hostname, address_family)
	char	*hostname;
	int	address_family;
{
	struct addrinfo		hints;
	struct addrinfo		*res, *tmp;
	char			host[MAX_HOSTNAME_LENGTH + sizeof(char)];
	int			retcode;

	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_family = address_family;

	retcode = getaddrinfo(hostname, NULL, &hints, &res);

	if (retcode != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(retcode));
		return;
	}

	for (tmp = res; tmp != NULL; tmp = tmp->ai_next) {
		getnameinfo(tmp->ai_addr, tmp->ai_addrlen, host, sizeof(host), NULL, 0, NI_NUMERICHOST);
		puts(host);
  	}

	freeaddrinfo(res);

	return;
}

/**********************************************************************/

/* function */
void reverse_lookup(ipaddress, address_family)
	char	*ipaddress;
	int	address_family;
{
	struct addrinfo		hints;
	struct addrinfo		*res, *tmp;
	char			host[MAX_HOSTNAME_LENGTH + sizeof(char)];
	int			retcode;

	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_family = address_family;

	retcode = getaddrinfo(ipaddress, NULL, &hints, &res);

	if (retcode != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(retcode));
		return;
	}

	for (tmp = res; tmp != NULL; tmp = tmp->ai_next) {
		getnameinfo(tmp->ai_addr, tmp->ai_addrlen, host, sizeof(host), NULL, 0, 0);
		puts(host);
  	}

	freeaddrinfo(res);

	return;
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
	int	address_family;
	int	arg;

	progname = basename(argv[0]);

	if (argc == 2) {
		if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0)) {
			usage();
		}
	}

	show_hosts_nsswitch(NSSWITCH_FILENAME);

	mode = FORWARD_MODE;
	address_family = AF_INET;

	arg = 1;
	while (arg < argc) {
		if (strcmp(argv[arg], "-f") == 0) {
			mode = FORWARD_MODE;
		} else if (strcmp(argv[arg], "-r") == 0) {
			mode = REVERSE_MODE;
		} else if (strcmp(argv[arg], "-4") == 0) {
			 address_family = AF_INET;
		} else if (strcmp(argv[arg], "-6") == 0) {
			 address_family = AF_INET6;
		} else {
			switch (mode) {
				case FORWARD_MODE:
					forward_lookup(argv[arg], address_family);
					break;
				case REVERSE_MODE:
					reverse_lookup(argv[arg], address_family);
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
