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
	{ 0x644f8b6c, "devm_tinydrm_register" },
	{ 0x7e4e42a7, "drm_mode_config_reset" },
	{ 0x422bd68a, "tinydrm_display_pipe_init" },
	{ 0xcb3ccbb4, "devm_tinydrm_init" },
	{ 0xe346f67a, "__mutex_init" },
	{ 0x5ddf986, "mipi_dbi_spi_init" },
	{ 0x9813695f, "device_property_read_u32_array" },
	{ 0x7f2fe4bf, "devm_gpiod_get" },
	{ 0x7ed07774, "devm_kmalloc" },
	{ 0x2223632, "mipi_dbi_buf_copy" },
	{ 0x25f36d88, "tinydrm_merge_clips" },
	{ 0xab28a428, "drm_fb_cma_get_gem_obj" },
	{ 0x18a85eba, "tinydrm_spi_transfer" },
	{ 0xe3f83fca, "mipi_dbi_spi_cmd_max_speed" },
	{ 0xc55e5bb0, "gpiod_set_value_cansleep" },
	{ 0x50d575f1, "drm_dev_printk" },
	{ 0xb69faf83, "mipi_dbi_enable_flush" },
	{ 0x63ae3169, "mipi_dbi_hw_reset" },
	{ 0xdb7305a1, "__stack_chk_fail" },
	{ 0xf9a482f9, "msleep" },
	{ 0xbce2399c, "mipi_dbi_command_buf" },
	{ 0x2109a78c, "drm_dbg" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x27083a23, "tinydrm_shutdown" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=drm,tinydrm,drm_kms_helper,mipi-dbi";

MODULE_ALIAS("spi:v220hf01a-t");
MODULE_ALIAS("of:N*T*Cvot,v220hf01a-t");
MODULE_ALIAS("of:N*T*Cvot,v220hf01a-tC*");

MODULE_INFO(srcversion, "B518840B69DBBE0D7855422");
