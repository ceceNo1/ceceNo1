/**
 * @file    libperf.h
 * @brief   libperf的插桩接口
 * @author  wang peng
 * @email   wangpeng@hygon.cn
 * @date    2023-01-24 13:43:49
 */

#ifndef _LIBPERF_H_
#define _LIBPERF_H_

#include <stdint.h>

#define LIBPERF_MAX_EVENTS 6

typedef struct {
  unsigned int events[LIBPERF_MAX_EVENTS];
  unsigned int events_count;

  int group_fd;
} libperf_t;

typedef uint64_t libperf_counter_t;
unsigned int exclude_kernel;
unsigned int eventIndx = 0;
unsigned int eventArry[LIBPERF_MAX_EVENTS];

int libperf_init(libperf_t **self);
int linperf_fini(libperf_t *self);
libperf_t * libperf_get();
int libperf_include_kernel(libperf_t *self);

int libperf_add_event(libperf_t *self, unsigned int event);

int libperf_start(libperf_t *self, int pid, int cpu);
int libperf_stop(libperf_t *self);

int libperf_read(libperf_t *self, libperf_counter_t *counters);

#endif /* _LIBPERF_H_ */
#ifndef _EXPECT_H_
#define _EXPECT_H_

#include <stdio.h>
#include <errno.h>

#define EXPECT(cond) do {                           \
  if (!(cond)) {                                    \
    fprintf(stderr, "Error:%s:%d: %s\n",            \
        __FILE__, __LINE__, #cond);                 \
  }                                                 \
} while (0)

#define EXPECT_ERRNO(cond) do {                     \
  if (!(cond)) {                                    \
    fprintf(stderr, "Error:%s:%d: %s\n",            \
        __FILE__, __LINE__, strerror(errno));       \
    exit(EXIT_FAILURE);                             \
  }                                                 \
} while (0)

#endif /* _EXPECT_H_ */
#ifndef COMPAT_H
#define COMPAT_H

#include <unistd.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>

typedef struct node {
  unsigned int event;
  unsigned int mask;
  const char title[128];
  char desc[128];
}PE_INFO_T;

enum eventEnum {
  ET_PIPE_0 = 0,
  ET_PIPE_1,
  ET_PIPE_2,
  ET_PIPE_3,
  ET_DATA_PIPE,
  ET_ST_PIPE,
  ET_TLB_PIPE_LATE,
  ET_HWPF,
  ET_TLB_PIPE_EARLY,
  ET_TLB_4KL2_HIT,
  ET_TLB_4KL2_MISS,
	ET_L2I_MISS, /// *
  ET_L2CACHEREQ_MISS = ET_L2I_MISS, /// *
  ET_L2CACHEREQ_HITS, /// *
  ET_L2CACHEREQ_HITX, /// *
  ET_ICACHE_FW32, /// *
	ET_L1I_MISS, /// *
  ET_ICACHE_FW32_MISS = ET_L1I_MISS, /// *
  ET_L1MISS_L2HIT,
  ET_L1MISS_L2MISS,
  ET_L1_HIT,
  ET_L1D_MISS, /// *
	ET_L1D_ACCESS, /// *
	ET_INSTS, /// *
  ET_CYCLE, /// *
	ET_L2D_MISS, /// *
	ET_L2D_ACCESS, /// *
	ET_BRANCH_MISSPRED, /// *
	ET_BRANCH, /// *
	ET_IC_INSTS, /// *
	ET_OC_INSTS, /// *
  ET_MAX
};

// hygon 1 event definition.
static PE_INFO_T eventInfo[ET_MAX] = {
  [ET_PIPE_0]             = {0x0, 0x01, "PIPE_TOTAL_0", "Fpu Pipe 0 Assignment"},
  [ET_PIPE_1]             = {0x0, 0x02, "PIPE_TOTAL_1", "Fpu Pipe 1 Assignment"},
  [ET_PIPE_2]             = {0x0, 0x04, "PIPE_TOTAL_2", "Fpu Pipe 2 Assignment"},
  [ET_PIPE_3]             = {0x0, 0x08, "PIPE_TOTAL_3", "Fpu Pipe 3 Assignment"},
  [ET_DATA_PIPE]          = {0x041, 0x01, "DataPipe", "LsMabAllocPipe DataPipe"},
  [ET_ST_PIPE]            = {0x041, 0x02, "StPipe", "LsMabAllocPipe StPipe"},
  [ET_TLB_PIPE_LATE]      = {0x041, 0x04, "TlbPipeLate",
                             "LsMabAllocPipe TlbPipeLate"},
  [ET_HWPF]               = {0x041, 0x08, "HWPF", "LsMabAllocPipe HWPF"},
  [ET_TLB_PIPE_EARLY]     = {0x041, 0x10, "TlbPipeEarly",
                             "LsMabAllocPipe TlbPipeEarly"},
	/// From cmetrics.py : L1D_missrate = ET_L1D_MISS / ET_L1D_ACCESS
  [ET_L1D_MISS]           = {0x041, 0x1f, "MAB allocation by pipe",
														 "L1 dcache misses"},
  [ET_L1D_ACCESS]         = {0x040, 0x00, "data cache accesses",
														 "L1 dcache accesses"},

  /// {0x045, 0x02, "TlbReload4KL2Hit", "32K L2 Hit"},
  /// {0x045, 0x04, "TlbReload2ML2Hit", "2M L2 Hit"},
  /// {0x045, 0x08, "TlbReload2ML2Hit", "1G L2 Hit"},
  /// {0x045, 0x20, "TlbReload32KL2Miss", "32k L2 Miss"},
  /// {0x045, 0x40, "TlbReload2ML2Miss", "2M L2 Miss"},
  /// {0x045, 0x80, "TlbReload1GL2Miss", "1G L2 Miss"},
  [ET_TLB_4KL2_HIT]       = {0x045, 0x01, "TlbReload4KL2Hit", "4K L2 Hit"},
  [ET_TLB_4KL2_MISS]      = {0x045, 0x10, "TlbReload4KL2Miss", "4k L2 Miss"},

	/// From cmetrics.py : L2I_missrate = ET_L2CACHEREQ_MISS / (ET_L2CACHEREQ_MISS + ET_L2CACHEREQ_HITS + ET_L2CACHEREQ_HITX)
  [ET_L2CACHEREQ_MISS]    = {0x064, 0x01, "IcFillMiss",
                             "L2 Instruction cache misses"},
  [ET_L2CACHEREQ_HITS]    = {0x064, 0x02, "IcFillHitS",
                             "L2CacheReqStat IcFillHitS"},
  [ET_L2CACHEREQ_HITX]    = {0x064, 0x04, "IcFillHitX",
                             "L2CacheReqStat IcFillHitX"},
	/// From cmetrics.py : L2D_missrate = ET_L2D_MISS / ET_L2D_ACCESS
  [ET_L2D_MISS]           = {0x064, 0x08, "LsRdBlkC", "L2 dcache misses"},
  [ET_L2D_ACCESS]         = {0x064, 0xf8,
														 "LsRdBlkCS+LsRdBlkLHitX+LsRdBlkLHitS+LsRdBlkX+LsRdBlkC",
														 "L2 dcache accesses"},

	/// From cmetrics.py : IPC = ET_INSTS / ET_CYCLE
	[ET_INSTS]							= {0x0c0, 0x0, "Instructions", "Instructions"},
  [ET_CYCLE]              = {0x076, 0x0, "Cycle", "Cycle"},

	/// From cmetrics.py : L1I_missrate = ET_ICACHE_FW32_MISS / ET_ICACHE_FW32
  [ET_ICACHE_FW32]        = {0x080, 0x0, "IcFw32", "Instruction cache fetch"},
  [ET_ICACHE_FW32_MISS]   = {0x081, 0x0, "IcFw32Miss",
                             "L1 Instruction cache misses"},

	/// From cmetrics.py : Branch_missrate = ET_BRANCH_MISSPRED / ET_BRANCH
	[ET_BRANCH_MISSPRED]		= {0x0c3, 0x0, "retired branch insts mispredicted", "branch predict miss"},
	[ET_BRANCH]							= {0x0c2, 0x0, "retired branch insts", "branch predict"},

	/// From arch
	[ET_IC_INSTS]						= {0x0aa, 0x0, "instructions from IC", "instructions from IC"},
	[ET_OC_INSTS]						= {0x0aa, 0x1, "instructions from OC", "instructions from OC"},


  [ET_L1MISS_L2HIT]       = {0x084, 0x0, "BpL1TlbMissL2Hit",
                             "L1 ITLB miss and L2 ITLB hit"},
  [ET_L1MISS_L2MISS]      = {0x085, 0x0, "BpL1TlbMissL2Miss",
                             "L1 ITLB miss and L2 ITLB miss"},
  [ET_L1_HIT]             = {0x094, 0x0, "L1ITLBHit", "L1 ITLB hit"}
};

#define GET_EVENT(ind) (eventInfo[ind].event | (eventInfo[ind].mask << 8))
#define GET_TITLE(ind) (eventInfo[ind].desc)

static inline int compat_perf_event_open(struct perf_event_attr *attr,
                                         pid_t pid,
                                         int cpu,
                                         int group_fd,
                                         unsigned long flags)
{
  return syscall(__NR_perf_event_open, attr, pid, cpu, group_fd, flags);
}

#endif /* COMPAT_H */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

static int libperf_attach(libperf_t *self,
                          int pid, int cpu,
                          int group_fd,
                          int config)
{
  int perf_fd;
  struct perf_event_attr attr;
  memset(&attr, 0, sizeof(struct perf_event_attr));

  attr.size = sizeof(struct perf_event_attr);
  attr.type = PERF_TYPE_RAW;

  attr.read_format = PERF_FORMAT_GROUP;
  attr.exclude_kernel = exclude_kernel;
  attr.exclude_user = 0;

  attr.config = config;

  perf_fd = compat_perf_event_open(&attr, pid, cpu, group_fd, 0);
  EXPECT_ERRNO(perf_fd != -1);
  return perf_fd;
}

static int libperf_attach_group(libperf_t *self, int pid, int cpu)
{
  int fd, group_fd = -1;
  for (unsigned int i = 0; i < self->events_count; i++) {
    fd = libperf_attach(self, pid, cpu, group_fd, self->events[i]);
    if (group_fd == -1)
      group_fd = fd;
  }
  return group_fd;
}

/**
 * @brief    对指定的perf对象进行初始化
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @param    self perf对象
 * @return   是否成功
 *   @retval   始终返回0，待完善
 */
int libperf_init(libperf_t **self)
{
  *self = (libperf_t*) malloc(sizeof(libperf_t));
  EXPECT_ERRNO(*self);
  memset(*self, 0, sizeof(libperf_t));
  exclude_kernel = 1;
  eventIndx = 0;
  return 0;
}

/**
 * @brief    释放指定的perf对象
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @param    self perf对象
 * @return   是否成功
 *   @retval   始终返回0，待完善
 */
int libperf_fini(libperf_t *self)
{
  free(self);
  return 0;
}
/**
 * @brief    拿到一个初始化了的perf结构体对象
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @return   返回一个结构体对象数据
 */
libperf_t * libperf_get() {
  libperf_t * perf;
  libperf_init(&perf);
  libperf_include_kernel(perf);

  return perf;
}

int libperf_printf() {
  int eventCount = eventIndx;

  for (unsigned int i = 0; i < eventCount; i++) {
    unsigned int eventID = eventArry[i];
    //printf("%s %lu\n", GET_TITLE(eventID), );
  }

  return 0;
}

/**
 * @brief    向perf对象中添加event事件
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @param    self perf对象
 * @param    eventID perf事件的ID
 * @return   是否成功
 *   @retval   0 始终返回0，待完善
 */
int libperf_add_event(libperf_t *self, unsigned int eventID)
{
  //eventArry[eventIndx ++] = eventID;
  //unsigned int event = GET_EVENT(eventID);
  unsigned int event = eventID;
  EXPECT(self->events_count < LIBPERF_MAX_EVENTS);
  self->events[self->events_count++] = event;
  return 0 ;
}

/**
 * @brief    <函数简介>
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @param    self perf对象
 * @param    pid 进程号
 * @param    cpu core核ID
 *           [pid, cpu]
 *           [0, -1] : 测试调用进程运行在任何CPU core上的数据
 *           [0, >0] : 只测试调用进程运行在指定的CPU core上的数据
 *           [>0, -1] : 测试指定进程运行在任何CPU core上的数据
 *           [>0, >0] : 只测试指定进程运行在指定CPU core上的数据
 *           [-1, >0] : 只测试所有进程运行在指定CPU core上的数据，需要特殊设置
 *                      参考https://www.man7.org/linux/man-pages/man2/perf_event_open.2.html
 *           [-1, -1] : 设置错误，返回错误
 * @return   是否成功
 *   @retval   0 始终返回0，待完善
 */
int libperf_start(libperf_t *self, int pid, int cpu)
{
  self->group_fd = libperf_attach_group(self, pid, cpu);
  EXPECT_ERRNO(self->group_fd != -1);
  return 0 ;
}

/**
 * @brief    停止指定perf对象的数据收集,待完善
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @param    self perf对象
 * @return   是否成功
 *   @retval   始终返回0，待完善
 */
int libperf_stop(libperf_t *self)
{
  return 0 ;
}

int libperf_include_kernel(libperf_t *self)
{
  exclude_kernel = 0;
  return 0 ;
}

/**
 * @brief    读取结果到缓存中
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @param    self perf对象
 * @param    counters libperf_counter_t类型数组，最大是6个元素
 * @return   是否成功
 *   @retval   始终返回0，待完善
 */
int libperf_read(libperf_t *self, libperf_counter_t *counters)
{
  unsigned int i;
  ssize_t size;
  struct read_format {
    uint64_t nr;
    struct {
      uint64_t value;
    } ctr[0];
  } *data;

  size = sizeof(uint64_t) * (self->events_count + 1);

  data = (struct read_format *)malloc(size);
  EXPECT_ERRNO(data != NULL);

  EXPECT_ERRNO(read(self->group_fd, data, size) == size);
  assert(data->nr == self->events_count);

  for (i = 0; i < data->nr; i++)
    counters[i] = data->ctr[i].value;

  return 0 ;
}

extern "C"
{
	int perf_cache_create();
	int perf_cache_add_event();
	int perf_cache_core_start(int core);
	int perf_cache_pid_start();
	int perf_cache_stop();
	int perf_cache_title();
	int perf_cache_data();
	int perf_cache_read_start();
	int perf_cache_read_end();

	int perf_brchipc_create();
	int perf_brchipc_add_event();
	int perf_brchipc_core_start(int core);
	int perf_brchipc_pid_start();
	int perf_brchipc_stop();
	int perf_brchipc_title();
	int perf_brchipc_data();
	int perf_brchipc_read_start();
	int perf_brchipc_read_end();

	int perf_ic_create();
	int perf_ic_add_event();
	int perf_ic_core_start(int core);
	int perf_ic_pid_start();
	int perf_ic_stop();
	int perf_ic_title();
	int perf_ic_data();
	int perf_ic_read_start();
	int perf_ic_read_end();
}

libperf_t * perf_cache = nullptr;
libperf_t * perf_brchipc = nullptr;
libperf_t * perf_ic = nullptr;
libperf_counter_t perf_cache_ctr1[6], perf_cache_ctr2[6];
libperf_counter_t perf_brchipc_ctr1[6], perf_brchipc_ctr2[6];
libperf_counter_t perf_ic_ctr1[6], perf_ic_ctr2[6];

/**
 * @brief    创建perf对象
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @return   int
 *   @retval   0 成功
 */
int perf_cache_create() {
	libperf_init(&perf_cache);
	libperf_include_kernel(perf_cache);

	return 0;
}

/**
 * @brief    创建branch/ipc perf对象
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @return   int
 *   @retval   0 成功
 */
int perf_brchipc_create() {
	libperf_init(&perf_brchipc);
	libperf_include_kernel(perf_brchipc);

	return 0;
}

/**
 * @brief    创建ic/oc perf对象
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @return   int
 *   @retval   0 成功
 */
int perf_ic_create() {
	libperf_init(&perf_ic);
	libperf_include_kernel(perf_ic);

	return 0;
}

/**
 * @brief    添加cache相关事件
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @return   int
 *   @retval   0 成功
 */
int perf_cache_add_event() {
	libperf_add_event(perf_cache, GET_EVENT(ET_L1D_MISS));
	libperf_add_event(perf_cache, GET_EVENT(ET_L1D_ACCESS));
	libperf_add_event(perf_cache, GET_EVENT(ET_L1I_MISS));
	libperf_add_event(perf_cache, GET_EVENT(ET_L2D_MISS));
	libperf_add_event(perf_cache, GET_EVENT(ET_L2D_ACCESS));
	libperf_add_event(perf_cache, GET_EVENT(ET_L2I_MISS));
	return 0;
}

/**
 * @brief    分支预测和IPC
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @return   int
 *   @retval   0 成功
 */
int perf_brchipc_add_event() {
	libperf_add_event(perf_brchipc, GET_EVENT(ET_BRANCH_MISSPRED));
	libperf_add_event(perf_brchipc, GET_EVENT(ET_BRANCH));
	libperf_add_event(perf_brchipc, GET_EVENT(ET_INSTS));
	libperf_add_event(perf_brchipc, GET_EVENT(ET_CYCLE));
	return 0;
}

/**
 * @brief    IC/OC指令数量
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @return   int
 *   @retval   0 成功
 */
int perf_ic_add_event() {
	libperf_add_event(perf_ic, GET_EVENT(ET_IC_INSTS));
	libperf_add_event(perf_ic, GET_EVENT(ET_OC_INSTS));
  return 0;
}

/**
 * @brief    开始收集当前程序在指定CPU core上的cache数据
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @return   int
 *   @retval   0 成功
 */
int perf_cache_core_start(int core) {
	libperf_start(perf_cache, 0, core);
	return 0;
}

/**
 * @brief    停止cache收集
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @return   int
 *   @retval   0 成功
 */
int perf_cache_stop() {
	libperf_stop(perf_cache);
	return 0;
}

/**
 * @brief    开始收集当前程序在指定CPU core上的branch / ipc数据
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @param    int core 指定core ID
 * @return   int
 *   @retval   0 成功
 */
int perf_brchipc_core_start(int core) {
	libperf_start(perf_brchipc, 0, core);
	return 0;
}

/**
 * @brief    停止branch/ipc收集
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @return   int
 *   @retval   0 成功
 */
int perf_brchipc_stop() {
	libperf_stop(perf_brchipc);
	return 0;
}

/**
 * @brief    开始收集当前程序在指定CPU core上的IC/OC指令数据
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @param    int core 指定core ID
 * @return   int
 *   @retval   0 成功
 */
int perf_ic_core_start(int core) {
	libperf_start(perf_ic, 0, core);
	return 0;
}

/**
 * @brief    停止ic/oc收集
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @return   int
 *   @retval   0 成功
 */
int perf_ic_stop() {
	libperf_stop(perf_ic);
	return 0;
}

/**
 * @brief    测试当前程序在任何CPU core上的cache数据
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @return   int
 *   @retval   0 成功
 */
int perf_cache_pid_start() {
	libperf_start(perf_cache, 0, -1);
	return 0;
}

/**
 * @brief    测试当前程序在任何CPU core上的branch / IPC数据
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @return   int
 *   @retval   0 成功
 */
int perf_brchipc_pid_start() {
	libperf_start(perf_brchipc, 0, -1);
	return 0;
}

/**
 * @brief    测试当前程序在任何CPU core上的IC/OC的指令数据
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @return   int
 *   @retval   0 成功
 */
int perf_ic_pid_start() {
	libperf_start(perf_ic, 0, -1);
	return 0;
}

/**
 * @brief    输出cache信息
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @return   int
 *   @retval   0 成功
 */
int perf_cache_title() {
  printf("\t%s,\t%s,\t%s,\t%s,\t%s,\t%s\n",
	        GET_TITLE(ET_L1D_MISS),
	        GET_TITLE(ET_L1D_ACCESS),
	        GET_TITLE(ET_L1I_MISS),
	        GET_TITLE(ET_L2D_MISS),
	        GET_TITLE(ET_L2D_ACCESS),
	        GET_TITLE(ET_L2I_MISS));
	return 0;
}

/**
 * @brief    输出cache收集的数据
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @return   int
 *   @retval   0 成功
 */
int perf_cache_data() {
  printf("\t%lu,\t%lu,\t%lu,\t%lu,\t%lu,\t%lu\n",
				 perf_cache_ctr2[0] - perf_cache_ctr1[0],
				 perf_cache_ctr2[1] - perf_cache_ctr1[1],
				 perf_cache_ctr2[2] - perf_cache_ctr1[2],
				 perf_cache_ctr2[3] - perf_cache_ctr1[3],
				 perf_cache_ctr2[4] - perf_cache_ctr1[4],
				 perf_cache_ctr2[5] - perf_cache_ctr1[5]);

	return 0;
}

/**
 * @brief    输出branch/ipc信息
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @return   int
 *   @retval   0 成功
 */
int perf_brchipc_title() {
  printf("\t%s,\t%s,\t%s,\t%s\n",
	        GET_TITLE(ET_BRANCH_MISSPRED),
	        GET_TITLE(ET_BRANCH),
	        GET_TITLE(ET_INSTS),
	        GET_TITLE(ET_CYCLE));
	return 0;
}

/**
 * @brief    输出branch / ipc数据
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @return   int
 *   @retval   0 成功
 */
int perf_brchipc_data() {
  printf("\t%lu,\t%lu,\t%lu,\t%lu\n",
				 perf_brchipc_ctr2[0] - perf_brchipc_ctr1[0],
				 perf_brchipc_ctr2[1] - perf_brchipc_ctr1[1],
				 perf_brchipc_ctr2[2] - perf_brchipc_ctr1[2],
				 perf_brchipc_ctr2[3] - perf_brchipc_ctr1[3]);
	return 0;
}

/**
 * @brief    输出IC/OC信息
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @return   int
 *   @retval   0 成功
 */
int perf_ic_title() {
  printf("\t%s,\t%s\n",
	        GET_TITLE(ET_IC_INSTS),
	        GET_TITLE(ET_OC_INSTS));
	return 0;
}

/**
 * @brief    输出IC/OC信息
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @return   int
 *   @retval   0 成功
 */
int perf_ic_data() {
  printf("\t%lu,\t%lu\n",
				 perf_ic_ctr2[0] - perf_ic_ctr1[0],
				 perf_ic_ctr2[1] - perf_ic_ctr1[1]);
	return 0;
}

/**
 * @brief    开始收集cache数据
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @return   int
 *   @retval   0 成功
 */
int perf_cache_read_start() {
	libperf_read(perf_cache, perf_cache_ctr1);
	return 0;
}

/**
 * @brief    结束收集cache数据
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @return   int
 *   @retval   0 成功
 */
int perf_cache_read_end() {
	libperf_read(perf_cache, perf_cache_ctr2);
	return 0;
}

/**
 * @brief    开始收集cache数据
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @return   int
 *   @retval   0 成功
 */
int perf_brchipc_read_start() {
	libperf_read(perf_brchipc, perf_brchipc_ctr1);
	return 0;
}

/**
 * @brief    结束收集branch/ipc数据
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @return   int
 *   @retval   0 成功
 */
int perf_brchipc_read_end() {
	libperf_read(perf_brchipc, perf_brchipc_ctr2);
	return 0;
}

/**
 * @brief    开始收集IC/OC数据
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @return   int
 *   @retval   0 成功
 */
int perf_ic_read_start() {
	libperf_read(perf_ic, perf_ic_ctr1);
	return 0;
}

/**
 * @brief    结束收集IC/OC数据
 * @author   wang peng
 * @email    wangpeng@hygon.cn
 * @return   int
 *   @retval   0 成功
 */
int perf_ic_read_end() {
	libperf_read(perf_ic, perf_ic_ctr2);
	return 0;
}

