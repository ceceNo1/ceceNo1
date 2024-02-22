
# fgc-InstrumentationTool


## libPerf

基于perf event的性能分析接口

### 案例说明
```
#include "libperf.h"

... code ...


  libperf_t *perf;
  libperf_counter_t perf_ctr1[6], perf_ctr2[6];
  libperf_init(&perf);
  libperf_include_kernel(perf);
  libperf_add_event(perf, GET_EVENT(ET_DCACHE_ACCESSES));
  libperf_add_event(perf, GET_EVENT(ET_L1_DMISS));
  libperf_add_event(perf, GET_EVENT(ET_CYCLE));
  libperf_add_event(perf, GET_EVENT(ET_PIPE_1));
  libperf_start(perf, 0, 8);

  printf("\t%s,\t%s,\t%s,\t%s\n",
          GET_TITLE(ET_DCACHE_ACCESSES),
          GET_TITLE(ET_L1_DMISS),
          GET_TITLE(ET_CYCLE),
          GET_TITLE(ET_PIPE_1));
  libperf_read(perf, perf_ctr1);

	... code block ...

  libperf_read(perf, perf_ctr2);
  printf("\t%lu,\t%lu,\t%lu,\t%lu\n",
          perf_ctr2[0] - perf_ctr1[0],
          perf_ctr2[1] - perf_ctr1[1],
          perf_ctr2[2] - perf_ctr1[2],
          perf_ctr2[3] - perf_ctr1[3]);

  libperf_stop(perf);

... code ...

```

