/* ==================== [Includes] ========================================== */

#include "xfusion.h"

#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"
#include "xf_task.h"
#include "xf_init.h"
#include "osal_task.h"
#include "tcxo.h"
#include "pinctrl.h"
#include "gpio.h"
#include "hal_reboot.h"
#include "cpu_utils.h"
#include "tcxo.h"

/* ==================== [Defines] =========================================== */

#define TAG "tasks_xf_entry"
#define TASKS_XF_PREMAIN_DURATION_MS        1000
#define TASKS_XF_PREMAIN_PRIO               CONFIG_TASKS_XF_PREMAIN_PRIORITY
#define TASKS_XF_PREMAIN_STACK_SIZE         CONFIG_TASKS_XF_PREMAIN_STACK_SIZE
#define CLOCK_PER_SEC  1000*1000UL


#define SPI_TASK_STACK_SIZE             0x1000
#define SPI_TASK_DURATION_MS            500
#define SPI_TASK_PRIO                   (osPriority_t)(osPriorityNormal3)
#include "osal_debug.h"
#include "cmsis_os2.h"
#include <los_memory.h>

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

#if (CONFIG_ENABLE_SYS_RAM_LOG)
static void log_mem_task(const char *arg);
#endif // CONFIG_ENABLE_SYS_RAM_LOG

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

/* ==================== [Static Functions] ================================== */

static void *tasks_xf_premain(const char *arg)
{
    unused(arg);
    xfusion_init();
    while (1)
    {
        xfusion_run();
    }
    return NULL;
}

#if (CONFIG_ENABLE_RST_IO)
static void _rst_io_cb(pin_t pin, uintptr_t param)
{
    UNUSED(pin);
    UNUSED(param);
    osal_printk("It will reboot!\r\n");
    /* 重启前睡眠500ms */
    uapi_tcxo_delay_ms(500);
    cpu_utils_reset_chip_with_cause(REBOOT_CAUSE_UPG_COMPLETION);
}   

static void _rst_io_init(pin_t rst_io, pin_pull_t pull_type, uint32_t trigger)
{
    uapi_pin_set_mode(rst_io, HAL_PIO_FUNC_GPIO);
    uapi_pin_set_pull(rst_io, pull_type);
    gpio_select_core(rst_io, CORES_APPS_CORE);

    uapi_gpio_set_dir(rst_io, GPIO_DIRECTION_INPUT);
    uapi_gpio_register_isr_func(rst_io, trigger, _rst_io_cb);
}
#endif

static void tasks_xf_entry(void)
{
#if (CONFIG_ENABLE_RST_IO)
    _rst_io_init(CONFIG_RST_IO_NUM, CONFIG_RST_IO_PULL_TYPE, CONFIG_RST_IO_TRIGGER_TYPE);
#endif
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)tasks_xf_premain, 0, "TasksXFPremain",
                                      TASKS_XF_PREMAIN_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, TASKS_XF_PREMAIN_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();

#if (CONFIG_ENABLE_SYS_RAM_LOG)
    osThreadAttr_t attr;

    attr.name = "log_mem";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = SPI_TASK_STACK_SIZE;
    attr.priority = SPI_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)log_mem_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
#endif // CONFIG_ENABLE_SYS_RAM_LOG
}

#if (CONFIG_ENABLE_SYS_RAM_LOG)
static void log_mem_task(const char *arg)
{
    UNUSED(arg);

    LOS_MEM_POOL_STATUS status;
    while (1) {
        osal_msleep(2000);
        LOS_MemInfoGet(m_aucSysMem0, &status);
        printf("[SYS INFO] mem: used:%u, free:%u, max free node:%u; log: drop/all[%u/%u]\r\n", 
            status.uwTotalUsedSize, status.uwTotalFreeSize, status.uwMaxFreeNodeSize,
            log_get_missed_messages_count(), log_get_all_messages_count());
    }
}
#endif // CONFIG_ENABLE_SYS_RAM_LOG

app_run(tasks_xf_entry); /*!< Run the tasks_xf_entry. */
