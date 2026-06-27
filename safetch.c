#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <limits.h>

#define BLUE  "\033[34m"
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define UNDERLINE "\033[4m"


char os[256];
char kernel[256];
char uptime[64];
char shell[128];
char de_wm[128];
char terminal[256];
char cpu[256];
char gpu[256];
char memory[64];
char username[LOGIN_NAME_MAX];
char hostname[HOST_NAME_MAX + 1];

void get_os() {
	char line[256];
	FILE *f = fopen("/etc/os-release", "r");
	while (fgets(line, sizeof(line), f)) {
		if (strncmp(line, "PRETTY_NAME=", 12) == 0) {
			char *smt = strchr(line, '=') + 1;
			if (*smt == '"') smt++;
			smt[strcspn(smt, "\"\n")] = '\0';
			strncpy(os, smt, sizeof(os) - 1);
		}
	}
	fclose(f);
}

void get_kernel() {
	struct utsname u;
	uname(&u);
	strncpy(kernel, u.release, sizeof(kernel) - 1);
}

void get_uptime() {
	struct sysinfo si;
	sysinfo(&si);
	long hours = si.uptime / 3600;
	long minutes = (si.uptime % 3600) / 60;
	snprintf(uptime, sizeof(uptime), "%ldh %ldm", hours, minutes);
	
}

void get_shell() {
	char *thingy = getenv("SHELL");
	char *thingz = strrchr(thingy, '/');
	if (thingz != NULL) thingz++;
	strncpy(shell, thingz, sizeof(shell) - 1);
}

void get_deorwm() {
	char *ok = getenv("XDG_CURRENT_DESKTOP");
	char *tty = ttyname(STDIN_FILENO);
	if (ok == NULL) ok = getenv("DESKTOP_SESSION");
	if (ok == NULL && getenv("WAYLAND_DISPLAY")) ok = "Wayland session";
	if (ok == NULL && getenv("DISPLAY")) ok = "X11 session";
	if (ok == NULL) ok = (tty != NULL) ? tty : "man we have checked everything what is this display";
	strncpy(de_wm, ok, sizeof(de_wm) - 1);
}

void get_term() {
	char *trmnl = getenv("TERM_PROGRAM");
	if (trmnl == NULL) trmnl = getenv("TERM");
	strncpy(terminal, trmnl, sizeof(terminal) - 1);
}

void get_cpu() {
	char ln[256];
	FILE *f = fopen("/proc/cpuinfo", "r");
	while(fgets(ln, sizeof(ln), f)) {
		if (strncmp(ln, "model name", 10) == 0) {
		char *vall = strchr(ln, ':');
		if (vall != NULL) {
   			vall++;                            
	        	if (*vall == ' ') vall++;           
    			vall[strcspn(vall, "\n")] = '\0';
    			strncpy(cpu, vall, sizeof(cpu) - 1);
    			break;
		}
	}


	}
	fclose(f);
	
}

void get_gpu() {
	FILE *p = popen("lspci | grep -i 'VGA\\|3D\\|Display'", "r");
	char lne[256];
	fgets(lne, sizeof(lne), p);
	pclose(p);

	char *value = strrchr(lne, ':');
	if (value != NULL) {
		value++;
		if (*value == ' ') value++;
		value[strcspn(value, "\n")] = '\0';
		strncpy(gpu, value, sizeof(gpu) - 1);
	}	
}

void get_memory() {
    char line[256];
    unsigned long total = 0, available = 0;
    FILE *f = fopen("/proc/meminfo", "r");
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "MemTotal:", 9) == 0)
            sscanf(line, "MemTotal: %lu kB", &total);
        if (strncmp(line, "MemAvailable:", 13) == 0)
            sscanf(line, "MemAvailable: %lu kB", &available);
    }
    fclose(f);
    unsigned long used = (total - available) / 1024;
    total = total / 1024;
    snprintf(memory, sizeof(memory), "%luMiB / %luMiB", used, total);
}

void get_usrhst() {
	char *user = getenv("USER");
	if (user != NULL) strncpy(username, user, sizeof(username) - 1);
	gethostname(hostname, sizeof(hostname));
}

int main(int argc, char *argv[]) {

int visible_len(const char *s) {
    int len = 0;
    while (*s) {
        if (*s == '\033') {
            while (*s && *s != 'm') s++;
        } else {
            len++;
        }
        s++;
    }
    return len;
}

  int no_art = 0;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-n") == 0) no_art = 1;
    else if (strcmp(argv[i], "-v") == 0) {
      printf("safetch 1.0.0\n");
      return 0;
    } else if (strcmp(argv[i], "-h") == 0) {
      printf("Safetch is a simple, fast, and rock-solid fetch program.\n\n");
      printf("Safetch commands:\n");
      printf("`-h`: Shows this help message\n");
      printf("`-n`: Shows safetch without the ASCII art\n");
      printf("`-v`: Shows safetch version\n\n");
      printf("To have an ASCII art next to your fetch information, create a file called `ascii-art` in `~/.config/safetch` (full path: ~/.config/safetch/ascii-art). And then, put your desired ASCII art in that file. Save it, and you now have a custom ASCII art next to your fetch information.\n");
      printf("Safetch does not come with a default ASCII art or a distro ASCII art, so you put the ASCII art to whatever you want yourself.\n");
      return 0;
    } else {
      printf("Unrecognized command: %s\nTry " BOLD "`-h`" RESET " for more information.\n", argv[1]);
      return 0;
    }
  }

	get_usrhst();
	get_os();
	get_kernel();
	get_uptime();
	get_shell();
	get_deorwm();
	get_term();
	get_cpu();
	get_gpu();
	get_memory();

	char art[64][256];
	int art_lines = 0;
	char path[PATH_MAX];
	char *home = getenv("HOME");
	snprintf(path, sizeof(path), "%s/.config/safetch/ascii-art", home);
	FILE *f = fopen(path, "r");
	if (f != NULL) {
		while (fgets(art[art_lines], sizeof(art[0]), f) && art_lines < 64) {
			art[art_lines][strcspn(art[art_lines], "\n")] = '\0';
			art_lines++;
		}
		fclose(f);
	}

   char info[11][256];
    snprintf(info[0], sizeof(info[0]), BLUE "%s" RESET " | " BLUE "%s" RESET, username, hostname);
    snprintf(info[1], sizeof(info[1]), "---------------");
    snprintf(info[2], sizeof(info[2]), BLUE "OS: " RESET "%s", os);
    snprintf(info[3], sizeof(info[3]), BLUE "Kernel: " RESET "Linux %s", kernel);
    snprintf(info[4], sizeof(info[4]), BLUE "Uptime: " RESET "%s", uptime);
    snprintf(info[5], sizeof(info[5]), BLUE "Shell: " RESET "%s", shell);
    snprintf(info[6], sizeof(info[6]), BLUE "Session: " RESET "%s", de_wm);
    snprintf(info[7], sizeof(info[7]), BLUE "Terminal: " RESET "%s", terminal);
    snprintf(info[8], sizeof(info[8]), BLUE "CPU: " RESET "%s", cpu);
    snprintf(info[9], sizeof(info[9]), BLUE "GPU: " RESET "%s", gpu);
    snprintf(info[10], sizeof(info[10]), BLUE "Memory: " RESET "%s", memory);

    int info_lines = 11;

    int ttl = art_lines > info_lines ? art_lines : info_lines;
for (int i = 0; i < ttl; i++) {
    if (no_art) {
    if (i < info_lines) printf("%-0s%s\n", "", info[i]);
   } else {
    if (art_lines > 0) {
        if (i < art_lines)
            printf("%-30s  ", art[i]);
        else
            printf("%-30s  ", "");
    } else {
        printf("%-0s", "");  // indent when no art
    }

    if (i < info_lines)
        printf("%s", info[i]);
    printf("\n");
}
}
    
	return 0;
}
