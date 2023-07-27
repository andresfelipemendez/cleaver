#include <stdio.h>
#include <stdlib.h>
#include "watcher.h"

void watcher(char* fileExtension) {
  printf("%s\n", fileExtension);
}

int main() {
  subscribeWatcher(&watcher); 
}
