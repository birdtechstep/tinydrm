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
	{ 0x9fbe38ff, "mipi_dbi_debugfs_init" },
	{ 0xe80175ae, "drm_release" },
	{ 0x25b2e4a4, "drm_open" },
	{ 0xa9e8985c, "drm_gem_cma_mmap" },
	{ 0x513e7e26, "drm_ioctl" },
	{ 0x8f1975e2, "drm_poll" },
	{ 0xf65a150a, "drm_read" },
	{ 0xa3c9b39, "noop_llseek" },
	{ 0xc1de622b, "drm_gem_fb_simple_display_pipe_prepare_fb" },
	{ 0xcc347b22, "tinydrm_display_pipe_update" },
	{ 0xe504d4da, "mipi_dbi_pipe_disable" },
	{ 0x589244a3, "driver_unregister" },
	{ 0x9e91f158, "__spi_register_driver" },
	{ 0x50d575f1, "drm_dev_printk" },
	{ 0x644f8b6c, "devm_tinydrm_register" },
	{ 0x68ba0f15, "mipi_dbi_init" },
	{ 0x5ddf986, "mipi_dbi_spi_init" },
	{ 0x9813695f, "device_property_read_u32_array" },
	{ 0x66725cad, "devm_of_find_backlight" },
	{ 0xf9cca6be, "devm_regulator_get" },
	{ 0x7e9ce80e, "devm_gpiod_get_optional" },
	{ 0x7ed07774, "devm_kmalloc" },
	{ 0xdb7305a1, "__stack_chk_fail" },
	{ 0xb69faf83, "mipi_dbi_enable_flush" },
	{ 0xf9a482f9, "msleep" },
	{ 0xd6abd5db, "mipi_dbi_command_stackbuf" },
	{ 0x8eeeada, "mipi_dbi_poweron_conditional_reset" },
	{ 0x2109a78c, "drm_dbg" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x27083a23, "tinydrm_shutdown" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=drm,tinydrm,mipi-dbi,drm_kms_helper";

MODULE_ALIAS("spi:mi0283qt");
MODULE_ALIAS("of:N*T*Cmulti-inno,mi0283qt");
MODULE_ALIAS("of:N*T*Cmulti-inno,mi0283qtC*");

MODULE_INFO(srcversion, "6DD2FA06D11528F7C82F9ED");
