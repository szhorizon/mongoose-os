#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

#include "mgos.h"
#include "mgos_hal.h"
#include "mgos_system.h"

struct ubuntu_wdt {
  bool enabled;
  int timeout;
  struct timeval last_feed;
};

static struct ubuntu_wdt s_mgos_wdt;

static pthread_mutex_t s_mgos_mux = PTHREAD_MUTEX_INITIALIZER;

void mgos_lock(void) {
  pthread_mutex_lock(&s_mgos_mux);
  return;
}

void mgos_unlock(void) {
  pthread_mutex_unlock(&s_mgos_mux);
  return;
}

struct mgos_rlock_type *mgos_rlock_create(void) {
  pthread_mutex_t *l = calloc(1, sizeof(pthread_mutex_t));

  pthread_mutex_init(l, NULL);
  return (struct mgos_rlock_type *) l;
}

void mgos_rlock(struct mgos_rlock_type *l) {
  if (!l) {
    return;
  }
  pthread_mutex_lock((pthread_mutex_t *) l);
  return;
}

void mgos_runlock(struct mgos_rlock_type *l) {
  if (!l) {
    return;
  }

  pthread_mutex_unlock((pthread_mutex_t *) l);
  return;
}

void mgos_rlock_destroy(struct mgos_rlock_type *l) {
  if (!l) {
    return;
  }
  pthread_mutex_destroy((pthread_mutex_t *) l);
  free(l);
  return;
}

size_t mgos_get_heap_size(void) {
  long s, ps;

  s = sysconf(_SC_PHYS_PAGES);
  ps = sysconf(_SC_PAGESIZE);
  return s * ps;
}

size_t mgos_get_free_heap_size(void) {
  long s, ps;

  s = sysconf(_SC_AVPHYS_PAGES);
  ps = sysconf(_SC_PAGESIZE);
  return s * ps;
}

size_t mgos_get_min_free_heap_size(void) {
  // LOG(LL_INFO, ("Not implemented"));
  return 0;
}

void mgos_dev_system_restart(void) {
  LOG(LL_INFO, ("Not implemented yet"));
  return;
}

size_t mgos_get_fs_memory_usage(void) {
  LOG(LL_INFO, ("Not implemented yet"));
  return 0;
}

/* in vfs-common/src/mgos_vfs.c
 * size_t mgos_get_fs_size(void) {
 * return 0;
 * }
 */

/* in vfs-common/src/mgos_vfs.c
 * size_t mgos_get_free_fs_size(void) {
 * return 0;
 * }
 */

/* in vfs-common/src/mgos_vfs.c
 * void mgos_fs_gc(void) {
 * return;
 * }
 */

void mgos_wdt_feed(void) {
  gettimeofday(&s_mgos_wdt.last_feed, NULL);
  return;
}

void mgos_wdt_set_timeout(int secs) {
  s_mgos_wdt.timeout = secs;
  return;

  (void) secs;
}

void mgos_wdt_enable(void) {
  s_mgos_wdt.enabled = true;
  return;
}

void mgos_wdt_disable(void) {
  s_mgos_wdt.enabled = false;
  return;
}

/* in mongoose-os/fw/src/mgos_system.c
 * void mgos_system_restart(void) {
 * return;
 * }
 */

void mgos_msleep(uint32_t msecs) {
  usleep(msecs * 1000);
  return;
}

void mgos_usleep(uint32_t usecs) {
  usleep(usecs);
  return;
}

void mgos_ints_disable(void) {
  // LOG(LL_INFO, ("Not implemented"));
  return;
}

void mgos_ints_enable(void) {
  // LOG(LL_INFO, ("Not implemented"));
  return;
}

bool mgos_invoke_cb(mgos_cb_t cb, void *arg, bool from_isr) {
  // LOG(LL_INFO, ("Not implemented"));
  return true;

  (void) cb;
  (void) arg;
  (void) from_isr;
}

uint32_t mgos_get_cpu_freq(void) {
  int fd = open("/proc/cpuinfo", O_RDONLY);
  char *p;
  char buf[2048];
  ssize_t len;
  uint32_t freq = 0;

  if (!fd) {
    LOG(LL_ERROR, ("Cannot open /proc/cpuinfo"));
    goto exit;
  }
  len = read(fd, buf, 2048);
  p = NULL;
  while (len > 0 && !p) {
    long mhz;

    p = strcasestr(buf, "cpu MHz");
    p += 7;
    while (*p && (isspace(*p) || *p == ':')) {
      p++;
    }
    mhz = atol(p);
    if (mhz > 0) {
      freq = mhz * 1e6;
      goto exit;
    }
    len = read(fd, buf, 2048);
  }

exit:
  if (fd > 0) {
    close(fd);
  }
  return freq;
}
