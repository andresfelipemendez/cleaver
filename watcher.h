#pragma once

typedef void (*watcher_ptr)(char*);
void subscribeWatcher(watcher_ptr watcher);

