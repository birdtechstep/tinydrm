// SPDX-License-Identifier: GPL-2.0+
/*
 * DRM driver for Sitronix ST7796 panels
 *
 * Copyright 2023 BIRD TECHSTEP <t.artsamart@gmail.com>
 *
 * Based on mi0283qt.c:
 * Copyright 2016 Noralf Trønnes
 */

#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/property.h>
#include <linux/spi/spi.h>

#include <drm/drm_atomic_helper.h>
#include <drm/drm_drv.h>
#include <drm/drm_fb_helper.h>
#include <drm/drm_gem_atomic_helper.h>
#include <drm/drm_gem_dma_helper.h>
#include <drm/drm_managed.h>
#include <drm/drm_mipi_dbi.h>
#include <drm/drm_modeset_helper.h>
#include <video/mipi_display.h>

#define ST7796_INVON 		0x21
#define ST7796_COLMOD 		0x3A

#define ST7796_FRMCTR1		0xB1
#define ST7796_FRMCTR2  	0xB2
#define ST7796_FRMCTR3  	0xB3
#define ST7796_INVCTR   	0xB4
#define ST7796_DFUNCTR  	0xB6
#define ST7796_ETMOD    	0xB7
#define ST7796_PWCTR1   	0xC0
#define ST7796_PWCTR2   	0xC1
#define ST7796_PWCTR3   	0xC2
#define ST7796_PWCTR4   	0xC3
#define ST7796_PWCTR5   	0xC4
#define ST7796_VMCTR    	0xC5
#define ST7796_GMCTRP1  	0xE0 // Positive Gamma Correction
#define ST7796_GMCTRN1  	0xE1 // Negative Gamma Correction
#define ST7796_DOCA     	0xE8 // Display Output Ctrl Adjust
#define ST7796_CSCON    	0xF0 // Command Set Control

#define ST7796_MADCTL_BGR	BIT(3)
#define ST7796_MADCTL_MV	BIT(5)
#define ST7796_MADCTL_MX	BIT(6)
#define ST7796_MADCTL_MY	BIT(7)

struct st7796_cfg {
	const struct drm_display_mode mode;
};

struct st7796_priv {
	struct mipi_dbi_dev dbidev;	/* Must be first for .release() */
	const struct st7796_cfg *cfg;
};

static void st7796_enable(struct drm_simple_display_pipe *pipe,
			     struct drm_crtc_state *crtc_state,
			     struct drm_plane_state *plane_state)
{
	struct mipi_dbi_dev *dbidev = drm_to_mipi_dbi_dev(pipe->crtc.dev);
//	struct st7796_priv *priv = container_of(dbidev, struct st7796_priv,
//						 dbidev);
	struct mipi_dbi *dbi = &dbidev->dbi;
	u8 addr_mode;
	int ret, idx;

	if (!drm_dev_enter(pipe->crtc.dev, &idx))
		return;

	DRM_DEBUG_KMS("\n");

	ret = mipi_dbi_poweron_conditional_reset(dbidev);
	if (ret < 0)
		goto out_exit;
	if (ret == 1)
		goto out_enable;

	mipi_dbi_command(dbi, MIPI_DCS_SET_DISPLAY_OFF);
	// Set color mode
	mipi_dbi_command(dbi, ST7796_COLMOD, 0x55);
	// Enable extension command 2 partI
	mipi_dbi_command(dbi, ST7796_CSCON, 0xC3);
	// Enable extension command 2 partII
	mipi_dbi_command(dbi, ST7796_CSCON, 0x96);
	//1-dot inversion
	mipi_dbi_command(dbi, ST7796_INVCTR, 0x01);
	//Display Function Control //Bypass
	mipi_dbi_command(dbi, ST7796_DFUNCTR, 0x80, 0x22, 0x3B);
	mipi_dbi_command(dbi, ST7796_DOCA, 0x40, 0x8A, 0x00, 0x00, 0x29, 0x19, 0xA5, 0x33);
	//Power control2   //VAP(GVDD)=3.85+( vcom+vcom offset), VAN(GVCL)=-3.85+( vcom+vcom offset)
	mipi_dbi_command(dbi, ST7796_PWCTR2, 0x06);
	//Power control 3  //Source driving current level=low, Gamma driving current level=High
	mipi_dbi_command(dbi, ST7796_PWCTR3, 0xA7);
	//VCOM Control    //VCOM=0.9
	mipi_dbi_command(dbi, ST7796_VMCTR, 0x18);
	msleep(120);

	mipi_dbi_command(dbi, ST7796_GMCTRP1,
			0xF0, 0x09, 0x0B, 0x06, 0x04, 0x15, 0x2F, 0x54,
			0x42, 0x3C, 0x17, 0x14, 0x18, 0x1B);
	mipi_dbi_command(dbi, ST7796_GMCTRN1,
			0xE0, 0x09, 0x0B, 0x06, 0x04, 0x03, 0x2B, 0x43,
			0x42, 0x3B, 0x16, 0x14, 0x17, 0x1B);
	msleep(120);

	//Command Set control // Disable extension command 2 partI
	mipi_dbi_command(dbi, ST7796_CSCON, 0x3C);
	//Command Set control // Disable extension command 2 partII
	mipi_dbi_command(dbi, ST7796_CSCON, 0x69);

	mipi_dbi_command(dbi, ST7796_INVON);
	mipi_dbi_command(dbi, MIPI_DCS_EXIT_SLEEP_MODE);
	msleep(120);

	mipi_dbi_command(dbi, MIPI_DCS_EXIT_IDLE_MODE);
	mipi_dbi_command(dbi, MIPI_DCS_SET_DISPLAY_ON);
	msleep(100);

out_enable:
	switch (dbidev->rotation) {
	default:
		//addr_mode = ST7796_MADCTL_MX;
		addr_mode = 0;
break;
	case 90:
		addr_mode = ST7796_MADCTL_MX | ST7796_MADCTL_MV;
		break;
	case 180:
		addr_mode = ST7796_MADCTL_MX | ST7796_MADCTL_MY;
		break;
	case 270:
		addr_mode = ST7796_MADCTL_MV | ST7796_MADCTL_MY;
		break;
	}
//	if (priv->cfg->rgb)
		addr_mode |= ST7796_MADCTL_BGR;
	
	mipi_dbi_command(dbi, MIPI_DCS_SET_ADDRESS_MODE, addr_mode);
	mipi_dbi_enable_flush(dbidev, crtc_state, plane_state);
out_exit:
	drm_dev_exit(idx);
}

static const struct drm_simple_display_pipe_funcs st7796_pipe_funcs = {
	.mode_valid = mipi_dbi_pipe_mode_valid,
	.enable = st7796_enable,
	.disable = mipi_dbi_pipe_disable,
	.update = mipi_dbi_pipe_update,
};

//static const struct drm_display_mode st7796_mode = {
//	DRM_SIMPLE_MODE(320, 480, 49, 74),
//};

static const struct st7796_cfg pitft350_cfg = {
	.mode		= { DRM_SIMPLE_MODE(320, 480, 49, 74) },
};

static const struct st7796_cfg pitft392_cfg = {
	.mode		= { DRM_SIMPLE_MODE(320, 320, 74, 74) },
};
DEFINE_DRM_GEM_DMA_FOPS(st7796_fops);

static const struct drm_driver st7796_driver = {
	.driver_features	= DRIVER_GEM | DRIVER_MODESET | DRIVER_ATOMIC,
	.fops			= &st7796_fops,
	DRM_GEM_DMA_DRIVER_OPS_VMAP,
	.debugfs_init		= mipi_dbi_debugfs_init,
	.name			= "st7796",
	.desc			= "Sitronix ST7796",
	.date			= "20231128",
	.major			= 1,
	.minor			= 0,
};

static const struct of_device_id st7796_of_match[] = {
	{ .compatible = "pitft350,st7796", .data = &pitft350_cfg },
	{ .compatible = "pitft392,st7796", .data = &pitft392_cfg },
	{ }
};
MODULE_DEVICE_TABLE(of, st7796_of_match);

static const struct spi_device_id st7796_id[] = {
	{ "st7796", 0 },
	{ }
};
MODULE_DEVICE_TABLE(spi, st7796_id);

static int st7796_probe(struct spi_device *spi)
{
	struct device *dev = &spi->dev;
	const struct st7796_cfg *cfg;
	struct mipi_dbi_dev *dbidev;
	struct drm_device *drm;
	struct mipi_dbi *dbi;
	struct gpio_desc *dc;
	u32 rotation = 0;
	int ret;

	cfg = device_get_match_data(&spi->dev);
	if (!cfg)
		cfg = (void *)spi_get_device_id(spi)->driver_data;
	
	dbidev = devm_drm_dev_alloc(dev, &st7796_driver,
				    struct mipi_dbi_dev, drm);
	if (IS_ERR(dbidev))
		return PTR_ERR(dbidev);

	dbi = &dbidev->dbi;
	drm = &dbidev->drm;

	dbi->reset = devm_gpiod_get_optional(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(dbi->reset))
		return dev_err_probe(dev, PTR_ERR(dbi->reset), "Failed to get GPIO 'reset'\n");

	dc = devm_gpiod_get_optional(dev, "dc", GPIOD_OUT_LOW);
	if (IS_ERR(dc))
		return dev_err_probe(dev, PTR_ERR(dc), "Failed to get GPIO 'dc'\n");

	dbidev->backlight = devm_of_find_backlight(dev);
	if (IS_ERR(dbidev->backlight))
		return PTR_ERR(dbidev->backlight);

	device_property_read_u32(dev, "rotation", &rotation);

	ret = mipi_dbi_spi_init(spi, dbi, dc);
	if (ret)
		return ret;

//	ret = mipi_dbi_dev_init(dbidev, &st7796_pipe_funcs, &st7796_mode, rotation);
	ret = mipi_dbi_dev_init(dbidev, &st7796_pipe_funcs, &cfg->mode, rotation);
	if (ret)
		return ret;

	drm_mode_config_reset(drm);

	ret = drm_dev_register(drm, 0);
	if (ret)
		return ret;

	spi_set_drvdata(spi, drm);

	drm_fbdev_generic_setup(drm, 0);

	return 0;
}

static void st7796_remove(struct spi_device *spi)
{
	struct drm_device *drm = spi_get_drvdata(spi);

	drm_dev_unplug(drm);
	drm_atomic_helper_shutdown(drm);
}

static void st7796_shutdown(struct spi_device *spi)
{
	drm_atomic_helper_shutdown(spi_get_drvdata(spi));
}

static struct spi_driver st7796_spi_driver = {
	.driver = {
		.name = "st7796",
		.of_match_table = st7796_of_match,
	},
	.id_table = st7796_id,
	.probe = st7796_probe,
	.remove = st7796_remove,
	.shutdown = st7796_shutdown,
};
module_spi_driver(st7796_spi_driver);

MODULE_DESCRIPTION("Sitronix ST7796 DRM driver");
MODULE_AUTHOR("BIRD TECHSTEP <t.artsamart@gmail.com>");
MODULE_LICENSE("GPL");
