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
	{ 0x2893b509, "tinydrm_swab16" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0xf9a482f9, "msleep" },
	{ 0xe1ff49ef, "single_open" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x1c68f7c9, "tinydrm_xrgb8888_to_rgb565" },
	{ 0x7e4e42a7, "drm_mode_config_reset" },
	{ 0x7d226f5d, "regulator_disable" },
	{ 0xe91d322b, "drm_gem_fb_create_handle" },
	{ 0xed370ccc, "arm_dma_ops" },
	{ 0x3dc2d2c2, "single_release" },
	{ 0xcaa71834, "tinydrm_spi_max_transfer_size" },
	{ 0x9b72bb05, "seq_puts" },
	{ 0xcb3ccbb4, "devm_tinydrm_init" },
	{ 0x9d562f41, "seq_printf" },
	{ 0xe2fae716, "kmemdup" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
	{ 0x97106714, "memdup_user_nul" },
	{ 0x67ea780, "mutex_unlock" },
	{ 0x85df9b6c, "strsep" },
	{ 0x18a85eba, "tinydrm_spi_transfer" },
	{ 0x222e31a8, "debugfs_create_file" },
	{ 0x4e80c45c, "seq_read" },
	{ 0x6cef1b8, "_dev_warn" },
	{ 0x5f754e5a, "memset" },
	{ 0x11089ac7, "_ctype" },
	{ 0xf9019aa0, "kstrtou8" },
	{ 0xe346f67a, "__mutex_init" },
	{ 0x25f36d88, "tinydrm_merge_clips" },
	{ 0xab28a428, "drm_fb_cma_get_gem_obj" },
	{ 0x92d5472d, "tinydrm_spi_bpw_supported" },
	{ 0x16305289, "warn_slowpath_null" },
	{ 0xc271c3be, "mutex_lock" },
	{ 0xeb124b4e, "_dev_err" },
	{ 0x1dd71f7a, "drm_err" },
	{ 0x2423ff9e, "spi_sync" },
	{ 0x61084c95, "dma_buf_begin_cpu_access" },
	{ 0x50d575f1, "drm_dev_printk" },
	{ 0x12a38747, "usleep_range" },
	{ 0xdb7305a1, "__stack_chk_fail" },
	{ 0x23217b0c, "tinydrm_memcpy" },
	{ 0x422bd68a, "tinydrm_display_pipe_init" },
	{ 0x4e095f11, "drm_get_format_name" },
	{ 0x38dc34cc, "seq_lseek" },
	{ 0x37a0cba, "kfree" },
	{ 0x9d669763, "memcpy" },
	{ 0x3185b03d, "tinydrm_xrgb8888_to_rgb666" },
	{ 0xc55e5bb0, "gpiod_set_value_cansleep" },
	{ 0x2109a78c, "drm_dbg" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0x91bbe098, "dma_buf_end_cpu_access" },
	{ 0xa8bf34d3, "tinydrm_fb_dirty" },
	{ 0x7ed07774, "devm_kmalloc" },
	{ 0x58358d36, "regulator_enable" },
	{ 0xa6b8037b, "drm_gem_fb_destroy" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=tinydrm,drm,drm_kms_helper";


MODULE_INFO(srcversion, "8D18BBB83597DB5B170FC48");
