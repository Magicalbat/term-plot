/*

This example shows live-updating graphs of computer resource usage.

Most of this is based on the following stackoverflow post:
https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process

*/

#define _DEFAULT_SOURCE
#define UNICODE
#define WIN32_LEAN_AND_MEAN

#define TERM_PLOT_IMPLEMENTATION
#include "../term_plot.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;

typedef double f64;

#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct {
    f64 cpu_usage;
    f64 mem_usage;
} resource_sample;

void plat_init(void);
resource_sample get_resource_sample(void);
f64 total_phys_mem(void);
f64 get_time_us(void);
void sleep_ms(u32 millis);

#define WINDOW_SIZE 128
#define SAMPLE_INTERVAL_S 3

u32 sample_pos = 0;
u32 total_samples = 0;
f64 cpu_usages[WINDOW_SIZE] = { 0 };
f64 mem_usages[WINDOW_SIZE] = { 0 };

int main(void) {
    plat_init();
    tp_figure* fig = tp_begin(NULL);

    char mem_label_cstr[256] = { 0 };
    snprintf(
        mem_label_cstr,
        sizeof(mem_label_cstr), 
        "Physical Memory Usage (percent of %5.2f GiB)",
        total_phys_mem() / (f64)(1 << 30)
    );
    tp_string8 mem_label = tp_str8_from_cstr((u8*)mem_label_cstr);

    sleep_ms(2000);

    while (1) {
        tp_reset(fig, &(tp_figure_desc){
            .title = TP_STR8("Resource Monitor")
        });

        resource_sample sample = get_resource_sample();

        cpu_usages[sample_pos] = sample.cpu_usage;
        mem_usages[sample_pos] = sample.mem_usage;

        sample_pos = (sample_pos + 1) % WINDOW_SIZE;
        total_samples++;

        tp_plot_lines(fig, 0, 0, &(tp_points_desc){
            .num_points = MIN(total_samples, WINDOW_SIZE),
            .ys = cpu_usages,
            .label = TP_STR8("CPU Usage (percent)"),
        });

        tp_plot_lines(fig, 0, 0, &(tp_points_desc){
            .num_points = MIN(total_samples, WINDOW_SIZE),
            .ys = mem_usages,
            .label = mem_label,
        });

        tp_setup_axes(fig, 0, 0, &(tp_axes_desc){
            .flags = TP_AXES_LEGEND | TP_AXES_NO_XNUMS,

            .xmin = 0, .xmax = WINDOW_SIZE,
            .ymin = 0, .ymax = 100,
        });

        printf("\x1b[?25l");
        tp_show(fig, false);
        printf("\x1b[H\x1b[?25h");

        f64 time_s = get_time_us() * 1e-6;
        f64 next_sec = ceil(time_s + (SAMPLE_INTERVAL_S - 1));
        sleep_ms((u32)((next_sec - time_s) * 1e3));
    }

    tp_end(fig);

    return 0;
}

#if defined(_WIN32)

#include <Windows.h>
#include <Pdh.h>

#pragma comment(lib, "pdh.lib")

static PDH_HQUERY cpu_query = { 0 };
static PDH_HCOUNTER cpu_total = { 0 };

static u64 perf_freq = 1;
static u64 init_ticks = 0;

void plat_init(void) {
    PdhOpenQuery(NULL, (DWORD_PTR)NULL, &cpu_query);
    // You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
    PdhAddEnglishCounter(cpu_query, L"\\Processor(_Total)\\% Processor Time", (DWORD_PTR)NULL, &cpu_total);
    PdhCollectQueryData(cpu_query);

    LARGE_INTEGER pf = { 0 };
    QueryPerformanceFrequency(&pf);
    perf_freq = (u64)pf.QuadPart;

    LARGE_INTEGER ticks = { 0 };
    QueryPerformanceCounter(&ticks);
    init_ticks = (u64)ticks.QuadPart;
}

resource_sample get_resource_sample(void) {
    resource_sample out = { 0 };

    PDH_FMT_COUNTERVALUE counter_val = { 0 };

    PdhCollectQueryData(cpu_query);
    PdhGetFormattedCounterValue(cpu_total, PDH_FMT_DOUBLE, NULL, &counter_val);

    out.cpu_usage = counter_val.doubleValue;

    MEMORYSTATUSEX mem_info = { .dwLength = sizeof(MEMORYSTATUSEX) };
    GlobalMemoryStatusEx(&mem_info);
    out.mem_usage = ((f64)(mem_info.ullTotalPhys - mem_info.ullAvailPhys) /
        (f64)mem_info.ullTotalPhys) * 100.0;

    return out;
}

f64 total_phys_mem(void) {
    MEMORYSTATUSEX mem_info = { .dwLength = sizeof(MEMORYSTATUSEX) };
    GlobalMemoryStatusEx(&mem_info);

    return (f64)mem_info.ullTotalPhys;
}

f64 get_time_us(void) {
    LARGE_INTEGER ticks = { 0 };
    QueryPerformanceCounter(&ticks);
    return (f64)((u64)ticks.QuadPart - init_ticks) * 1000000.0 / (f64)perf_freq;
}


void sleep_ms(u32 millis) {
    Sleep(millis);
}

#elif defined(__unix__)

#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/sysinfo.h>

static u64 start_time = 0;

static u64 last_total_user, last_total_user_low, last_total_sys, last_total_idle;

void plat_init(void) {
    struct timespec ts = { 0 };
    clock_gettime(CLOCK_MONOTONIC, &ts);
    start_time = (u64)ts.tv_sec * 1000000 + (u64)ts.tv_nsec / 1000;

    FILE* f = fopen("/proc/stat", "r");
    fscanf(
        f, "cpu %lu %lu %lu %lu",
        &last_total_user, &last_total_user_low, 
        &last_total_sys, &last_total_idle
    );
    fclose(f);
}

resource_sample get_resource_sample(void) {
    resource_sample out = { 0 };

    u64 total_user, total_user_low, total_sys, total_idle;

    FILE* f = fopen("/proc/stat", "r");
    fscanf(
        f, "cpu %lu %lu %lu %lu",
        &total_user, &total_user_low, 
        &total_sys, &total_idle
    );
    fclose(f);

    if (
        total_user < last_total_user || 
        total_user_low < last_total_user_low || 
        total_sys < last_total_sys ||
        total_idle < last_total_idle
    ) {
        out.cpu_usage = -1.0;
    } else {
        u64 num = (total_user - last_total_user) +
            (total_user_low - last_total_user_low) +
            (total_sys - last_total_sys);
        u64 den = num + (total_idle - last_total_idle);

        out.cpu_usage = ((f64)num / (f64)den) * 100.0;
    }

    last_total_user = total_user;
    last_total_user_low = total_user_low;
    last_total_sys = total_sys;
    last_total_idle = total_idle;

    struct sysinfo mem_info = { 0 };
    sysinfo(&mem_info);
    out.mem_usage = ((f64)(mem_info.totalram - mem_info.freeram) /
        (f64)mem_info.totalram) * 100.0;

    return out;
}

f64 total_phys_mem(void) {
    struct sysinfo mem_info = { 0 };
    sysinfo(&mem_info);

    return (f64)mem_info.totalram * mem_info.mem_unit;
}

f64 get_time_us(void) {
    struct timespec ts = { 0 };
    clock_gettime(CLOCK_MONOTONIC, &ts);
    u64 cur_time = (u64)ts.tv_sec * 1000000 + (u64)ts.tv_nsec / 1000;

    return (f64)(cur_time - start_time);
}

void sleep_ms(u32 millis) {
    usleep(millis * 1000);
}


#endif

