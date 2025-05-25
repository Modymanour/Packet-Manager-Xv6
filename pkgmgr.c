#include "user.h"
#include "syscall.h"

void usage() {
  printf("Usage: pkgmgr <install|remove|update> <pkgname>\n");
  exit(1);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    usage();
  }

  char *cmd = argv[1];
  char *pkg = argv[2];
  int op;

  if (strcmp(cmd, "install") == 0) {
    op = 1;
  } else if (strcmp(cmd, "remove") == 0) {
    op = 2;
  } else if (strcmp(cmd, "update") == 0) {
    op = 3;
  } else {
    usage();
  }

  int res = pkgdb_op(op, pkg);
  if (res == 0) {
    printf("%s %s succeeded\n", cmd, pkg);
  } else {
    printf("%s %s failed\n", cmd, pkg);
  }

  exit(0);
}
