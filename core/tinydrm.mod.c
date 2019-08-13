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
	{ 0xb4938d5e, "kmalloc_caches" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0x254ab55c, "drm_dev_register" },
	{ 0xa0c4b438, "drm_dev_alloc" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0xbf2711bc, "drm_mode_config_cleanup" },
	{ 0xc1747f44, "drm_atomic_helper_connector_reset" },
	{ 0x9b91279e, "drm_helper_probe_single_connector_modes" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
	{ 0x67ea780, "mutex_unlock" },
	{ 0x71d3c3c7, "drm_dev_unref" },
	{ 0xd697e69a, "trace_hardirqs_on" },
	{ 0xf45cd9a4, "drm_connector_cleanup" },
	{ 0x8c3839e8, "drm_simple_display_pipe_init" },
	{ 0xd78a5352, "drm_atomic_helper_connector_duplicate_state" },
	{ 0xd7374ddf, "dma_buf_vunmap" },
	{ 0x8565b5b1, "drm_fbdev_generic_setup" },
	{ 0x6545a559, "drm_gem_fb_create_with_funcs" },
	{ 0x6cef1b8, "_dev_warn" },
	{ 0x5f754e5a, "memset" },
	{ 0xa924ed6e, "drm_mode_copy" },
	{ 0xb27a2af6, "drm_dev_unregister" },
	{ 0xc7a52aa4, "drm_mode_set_name" },
	{ 0xe8a034df, "drm_dev_exit" },
	{ 0xe346f67a, "__mutex_init" },
	{ 0x7c32d0f0, "printk" },
	{ 0xbf096a8b, "drm_mode_duplicate" },
	{ 0x16305289, "warn_slowpath_null" },
	{ 0xc271c3be, "mutex_lock" },
	{ 0xeb124b4e, "_dev_err" },
	{ 0x1dd71f7a, "drm_err" },
	{ 0xd3948f52, "drm_connector_init" },
	{ 0xae9e859a, "drm_modeset_unlock" },
	{ 0x5a789ff0, "drm_modeset_lock" },
	{ 0x2423ff9e, "spi_sync" },
	{ 0xf2ac7ab5, "devm_add_action" },
	{ 0x909a06fc, "drm_atomic_helper_connector_destroy_state" },
	{ 0xc2acc033, "hex_dump_to_buffer" },
	{ 0x425ad74b, "drm_atomic_helper_check" },
	{ 0xc713a5e3, "drm_atomic_helper_commit" },
	{ 0xdb7305a1, "__stack_chk_fail" },
	{ 0x2da81bff, "_raw_spin_lock_irq" },
	{ 0x733aaf68, "drm_crtc_send_vblank_event" },
	{ 0xd14f4ea7, "drm_format_plane_cpp" },
	{ 0xcd8d248c, "kmem_cache_alloc_trace" },
	{ 0xc94b1e4f, "drm_gem_cma_prime_import_sg_table" },
	{ 0x1d4e1877, "drm_mode_config_init" },
	{ 0x37a0cba, "kfree" },
	{ 0x9d669763, "memcpy" },
	{ 0x732fc684, "drm_atomic_helper_shutdown" },
	{ 0x2109a78c, "drm_dbg" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0x4b5bf123, "drm_mode_probed_add" },
	{ 0x2aab69cc, "drm_gem_cma_free_object" },
	{ 0x4f4a8222, "dma_buf_vmap" },
	{ 0x1e30a605, "param_ops_uint" },
	{ 0x798bd36a, "drm_dev_enter" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=drm,drm_kms_helper";


MODULE_INFO(srcversion, "D877C0DA052E0E829DF873B");
