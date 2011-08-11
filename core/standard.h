#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <math.h>

#include <ncurses.h>
#undef false
#undef true

#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#ifdef XT_PLATFORM_DARWIN
#include <sys/time.h>
#include <uuid/uuid.h>
#endif

#ifdef XT_PLATFORM_LINUX
#include <sys/time.h>
#include <uuid/uuid.h>
#endif

#ifdef XT_PLATFORM_NETBSD
#include <uuid.h>
#endif
