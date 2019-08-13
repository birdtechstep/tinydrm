#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

MODULE_INFO(intree, "Y");

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xef3d261e, "module_layout" },
	{ 0xa2103ef0, "drm_gem_cma_vm_ops" },
	{ 0xcb493660, "drm_gem_cma_dumb_create" },
	{ 0x479f7a26, "drm_gem_cma_prime_mmap" },
	{ 0x5957b8f, "drm_gem_cma_prime_vunmap" },
	{ 0x8eee1680, "drm_gem_cma_prime_vmap" },
	{ 0xbf2389b7, "tinydrm_gem_cma_prime_import_sg_table" },
	{ 0x3bad5e13, "drm_gem_cma_prime_get_sg_table" },
	{ 0xfbde6eb2, "drm_gem_prime_import" },
	{ 0x1fa8bd69, "drm_gem_prime_export" },
	{ 0x7efc88d5, "drm_gem_prime_fd_to_handle" },
	{ 0x79cdf0c4, "drm_gem_prime_handle_to_fd" },
	{ 0xbc5b733f, "drm_gem_cma_print_info" },
	{ 0xf2365a3, "tinydrm_gem_cma_free_object" },
	{ 0xe80175ae, "drm_release" },
	{ 0x25b2e4a4, "drm_open" },
	{ 0xa9e8985c, "drm_gem_cma_mmap" },
	{ 0x513e7e26, "drm_ioctl" },
	{ 0x8f1975e2, "drm_poll" },
	{ 0xf65a150a, "drm_read" },
	{ 0xa3c9b39, "noop_llseek" },
	{ 0xc1de622b, "drm_gem_fb_simple_display_pipe_prepare_fb" },
	{ 0xcc347b22, "tinydrm_display_pipe_update" },
	{ 0xa8bf34d3, "tinydrm_fb_dirty" },
	{ 0xe91d322b, "drm_gem_fb_create_handle" },
	{ 0xa6b8037b, "drm_gem_fb_destroy" },
	{ 0x589244a3, "driver_unregister" },
	{ 0x9e91f158, "__spi_register_driver" },
	{ 0xeb124b4e, "_dev_err" },
	{ 0x1fedd0e9, "gpiod_get_value_cansleep" },
	{ 0x12a38747, "usleep_range" },
	{ 0x67ea780, "mutex_unlock" },
	{ 0xc271c3be, "mutex_lock" },
	{ 0x91bbe098, "dma_buf_end_cpu_access" },
	{ 0x4a90ad49, "tinydrm_xrgb8888_to_gray8" },
	{ 0x61084c95, "dma_buf_begin_cpu_access" },
	{ 0xffec220d, "thermal_zone_get_temp" },
	{ 0xab28a428, "drm_fb_cma_get_gem_obj" },
	{ 0x3a26ed11, "sched_clock" },
	{ 0xaf8a2efc, "thermal_zone_get_zone_by_name" },
	{ 0x644f8b6c, "devm_tinydrm_register" },
	{ 0x2109a78c, "drm_dbg" },
	{ 0x7e4e42a7, "drm_mode_config_reset" },
	{ 0x422bd68a, "tinydrm_display_pipe_init" },
	{ 0xcb3ccbb4, "devm_tinydrm_init" },
	{ 0xf5b2f88c, "device_property_read_string" },
	{ 0x6cef1b8, "_dev_warn" },
	{ 0xa1e7c147, "spi_get_device_id" },
	{ 0x50d575f1, "drm_dev_printk" },
	{ 0x7f2fe4bf, "devm_gpiod_get" },
	{ 0x7ed07774, "devm_kmalloc" },
	{ 0xed370ccc, "arm_dma_ops" },
	{ 0xbe6a63ed, "of_match_device" },
	{ 0x9d669763, "memcpy" },
	{ 0x37a0cba, "kfree" },
	{ 0x8e865d3c, "arm_delay_ops" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0xcd8d248c, "kmem_cache_alloc_trace" },
	{ 0xb4938d5e, "kmalloc_caches" },
	{ 0xf9a482f9, "msleep" },
	{ 0xc55e5bb0, "gpiod_set_value_cansleep" },
	{ 0xdb7305a1, "__stack_chk_fail" },
	{ 0x2423ff9e, "spi_sync" },
	{ 0x5f754e5a, "memset" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x27083a23, "tinydrm_shutdown" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=drm,tinydrm,drm_kms_helper";

MODULE_ALIAS("spi:e1144cs021");
MODULE_ALIAS("spi:e1190cs021");
MODULE_ALIAS("spi:e2200cs021");
MODULE_ALIAS("spi:e2271cs021");
MODULE_ALIAS("of:N*T*Cpervasive,e1144cs021");
MODULE_ALIAS("of:N*T*Cpervasive,e1144cs021C*");
MODULE_ALIAS("of:N*T*Cpervasive,e1190cs021");
MODULE_ALIAS("of:N*T*Cpervasive,e1190cs021C*");
MODULE_ALIAS("of:N*T*Cpervasive,e2200cs021");
MODULE_ALIAS("of:N*T*Cpervasive,e2200cs021C*");
MODULE_ALIAS("of:N*T*Cpervasive,e2271cs021");
MODULE_ALIAS("of:N*T*Cpervasive,e2271cs021C*");

MODULE_INFO(srcversion, "8DB9B51A4313B2A4F95E4AB");
