#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>

static pthread_t g_test_scroll_tid;
static int g_test_scroll_running = 0;

static void *test_scroll_bounce_thread(void *arg)
{
    (void)arg;

    const uint32_t window = 18;                 /* 每段18个 */
    const uint32_t max_slot = 1000;             /* 测试到1000 */
    const useconds_t step_delay_us = 200 * 1000;   /* 每步停200ms */
    const useconds_t reset_delay_us = 2 * 1000 * 1000; /* reset后停2秒 */

    while (g_test_scroll_running) {
        uint32_t start = 1;

        /* 正向：1-18, 19-36, ... */
        while (g_test_scroll_running && start <= max_slot) {
            uint32_t end = start + window - 1;
            if (end > max_slot) {
                end = max_slot;
            }

            printf("[TEST_SCROLL] forward: %u-%u\n", start, end);
            demo_ui_scroll_range(start, end);
            usleep(step_delay_us);

            if (end == max_slot) {
                break;
            }
            start += window;
        }

        if (!g_test_scroll_running) {
            break;
        }

        /* 反向：..., 19-36, 1-18 */
        if (max_slot > window) {
            start = ((max_slot - 1) / window) * window + 1;
        } else {
            start = 1;
        }

        while (g_test_scroll_running) {
            uint32_t end = start + window - 1;
            if (end > max_slot) {
                end = max_slot;
            }

            printf("[TEST_SCROLL] backward: %u-%u\n", start, end);
            demo_ui_scroll_range(start, end);
            usleep(step_delay_us);

            if (start <= 1) {
                break;
            }

            if (start > window) {
                start -= window;
            } else {
                start = 1;
            }
        }

        if (!g_test_scroll_running) {
            break;
        }

        printf("[TEST_SCROLL] reset all\n");
        demo_ui_reset_all();
        usleep(reset_delay_us);
    }

    return NULL;
}

void test_scroll_start(void)
{
    if (g_test_scroll_running) {
        return;
    }

    g_test_scroll_running = 1;
    pthread_create(&g_test_scroll_tid, NULL, test_scroll_bounce_thread, NULL);
}

void test_scroll_stop(void)
{
    if (!g_test_scroll_running) {
        return;
    }

    g_test_scroll_running = 0;
    pthread_join(g_test_scroll_tid, NULL);
}