// SPDX-License-Identifier: GPL-2.0+
/*
 * DRM driver for Ilitek ILI9488 panels
 *
 * Copyright 2019 BIRD TECHSTEP <t.artsamart@gmail.com>
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

#include <drm/drm_fb_helper.h>
#include <drm/drm_gem_framebuffer_helper.h>
#include <drm/drm_modeset_helper.h>
#include <drm/tinydrm/mipi-dbi.h>
#include <drm/tinydrm/tinydrm-helpers.h>
#include <video/mipi_display.h>

/* Level 1 Commands (from the display Datasheet) */
#define ILI9488_CMD_NOP					0x00
#define ILI9488_CMD_SOFTWARE_RESET			0x01
#define ILI9488_CMD_READ_DISP_ID			0x04
#define ILI9488_CMD_READ_ERROR_DSI			0x05
#define ILI9488_CMD_READ_DISP_STATUS			0x09
#define ILI9488_CMD_READ_DISP_POWER_MODE		0x0A
#define ILI9488_CMD_READ_DISP_MADCTRL			0x0B
#define ILI9488_CMD_READ_DISP_PIXEL_FORMAT		0x0C
#define ILI9488_CMD_READ_DISP_IMAGE_MODE		0x0D
#define ILI9488_CMD_READ_DISP_SIGNAL_MODE		0x0E
#define ILI9488_CMD_READ_DISP_SELF_DIAGNOSTIC		0x0F
#define ILI9488_CMD_ENTER_SLEEP_MODE			0x10
#define ILI9488_CMD_SLEEP_OUT				0x11
#define ILI9488_CMD_PARTIAL_MODE_ON			0x12
#define ILI9488_CMD_NORMAL_DISP_MODE_ON			0x13
#define ILI9488_CMD_DISP_INVERSION_OFF			0x20
#define ILI9488_CMD_DISP_INVERSION_ON			0x21
#define ILI9488_CMD_PIXEL_OFF				0x22
#define ILI9488_CMD_PIXEL_ON				0x23
#define ILI9488_CMD_DISPLAY_OFF				0x28
#define ILI9488_CMD_DISPLAY_ON				0x29
#define ILI9488_CMD_COLUMN_ADDRESS_SET			0x2A
#define ILI9488_CMD_PAGE_ADDRESS_SET			0x2B
#define ILI9488_CMD_MEMORY_WRITE			0x2C
#define ILI9488_CMD_MEMORY_READ				0x2E
#define ILI9488_CMD_PARTIAL_AREA			0x30
#define ILI9488_CMD_VERT_SCROLL_DEFINITION		0x33
#define ILI9488_CMD_TEARING_EFFECT_LINE_OFF		0x34
#define ILI9488_CMD_TEARING_EFFECT_LINE_ON		0x35
#define ILI9488_CMD_MEMORY_ACCESS_CONTROL		0x36
#define ILI9488_CMD_VERT_SCROLL_START_ADDRESS		0x37
#define ILI9488_CMD_IDLE_MODE_OFF			0x38
#define ILI9488_CMD_IDLE_MODE_ON			0x39
#define ILI9488_CMD_COLMOD_PIXEL_FORMAT_SET		0x3A
#define ILI9488_CMD_WRITE_MEMORY_CONTINUE		0x3C
#define ILI9488_CMD_READ_MEMORY_CONTINUE		0x3E
#define ILI9488_CMD_SET_TEAR_SCANLINE			0x44
#define ILI9488_CMD_GET_SCANLINE			0x45
#define ILI9488_CMD_WRITE_DISPLAY_BRIGHTNESS		0x51
#define ILI9488_CMD_READ_DISPLAY_BRIGHTNESS		0x52
#define ILI9488_CMD_WRITE_CTRL_DISPLAY			0x53
#define ILI9488_CMD_READ_CTRL_DISPLAY			0x54
#define ILI9488_CMD_WRITE_CONTENT_ADAPT_BRIGHTNESS	0x55
#define ILI9488_CMD_READ_CONTENT_ADAPT_BRIGHTNESS	0x56
#define ILI9488_CMD_WRITE_MIN_CAB_LEVEL			0x5E
#define ILI9488_CMD_READ_MIN_CAB_LEVEL			0x5F
#define ILI9488_CMD_READ_ABC_SELF_DIAG_RES		0x68
#define ILI9488_CMD_READ_ID1				0xDA
#define ILI9488_CMD_READ_ID2				0xDB
#define ILI9488_CMD_READ_ID3				0xDC

/* Level 2 Commands (from the display Datasheet) */
#define ILI9488_CMD_INTERFACE_MODE_CONTROL		0xB0
#define ILI9488_CMD_FRAME_RATE_CONTROL_NORMAL		0xB1
#define ILI9488_CMD_FRAME_RATE_CONTROL_IDLE_8COLOR	0xB2
#define ILI9488_CMD_FRAME_RATE_CONTROL_PARTIAL		0xB3
#define ILI9488_CMD_DISPLAY_INVERSION_CONTROL		0xB4
#define ILI9488_CMD_BLANKING_PORCH_CONTROL		0xB5
#define ILI9488_CMD_DISPLAY_FUNCTION_CONTROL		0xB6
#define ILI9488_CMD_ENTRY_MODE_SET			0xB7
#define ILI9488_CMD_BACKLIGHT_CONTROL_1			0xB9
#define ILI9488_CMD_BACKLIGHT_CONTROL_2			0xBA
#define ILI9488_CMD_HS_LANES_CONTROL			0xBE
#define ILI9488_CMD_POWER_CONTROL_1			0xC0
#define ILI9488_CMD_POWER_CONTROL_2			0xC1
#define ILI9488_CMD_POWER_CONTROL_NORMAL_3		0xC2
#define ILI9488_CMD_POWER_CONTROL_IDEL_4		0xC3
#define ILI9488_CMD_POWER_CONTROL_PARTIAL_5		0xC4
#define ILI9488_CMD_VCOM_CONTROL_1			0xC5
#define ILI9488_CMD_CABC_CONTROL_1			0xC6
#define ILI9488_CMD_CABC_CONTROL_2			0xC8
#define ILI9488_CMD_CABC_CONTROL_3			0xC9
#define ILI9488_CMD_CABC_CONTROL_4			0xCA
#define ILI9488_CMD_CABC_CONTROL_5			0xCB
#define ILI9488_CMD_CABC_CONTROL_6			0xCC
#define ILI9488_CMD_CABC_CONTROL_7			0xCD
#define ILI9488_CMD_CABC_CONTROL_8			0xCE
#define ILI9488_CMD_CABC_CONTROL_9			0xCF
#define ILI9488_CMD_NVMEM_WRITE				0xD0
#define ILI9488_CMD_NVMEM_PROTECTION_KEY		0xD1
#define ILI9488_CMD_NVMEM_STATUS_READ			0xD2
#define ILI9488_CMD_READ_ID4				0xD3
#define ILI9488_CMD_ADJUST_CONTROL_1			0xD7
#define ILI9488_CMD_READ_ID_VERSION			0xD8
#define ILI9488_CMD_POSITIVE_GAMMA_CORRECTION		0xE0
#define ILI9488_CMD_NEGATIVE_GAMMA_CORRECTION		0xE1
#define ILI9488_CMD_DIGITAL_GAMMA_CONTROL_1		0xE2
#define ILI9488_CMD_DIGITAL_GAMMA_CONTROL_2		0xE3
#define ILI9488_CMD_SET_IMAGE_FUNCTION			0xE9
#define ILI9488_CMD_ADJUST_CONTROL_2			0xF2
#define ILI9488_CMD_ADJUST_CONTROL_3			0xF7
#define ILI9488_CMD_ADJUST_CONTROL_4			0xF8
#define ILI9488_CMD_ADJUST_CONTROL_5			0xF9
#define ILI9488_CMD_SPI_READ_SETTINGS			0xFB
#define ILI9488_CMD_ADJUST_CONTROL_6			0xFC
#define ILI9488_CMD_ADJUST_CONTROL_7			0xFF

#define ILI9488_MADCTL_BGR	BIT(3)
#define ILI9488_MADCTL_MV	BIT(5)
#define ILI9488_MADCTL_MX	BIT(6)
#define ILI9488_MADCTL_MY	BIT(7)

static void sx035hv006_enable(struct drm_simple_display_pipe *pipe,
			     struct drm_crtc_state *crtc_state,
			     struct drm_plane_state *plane_state)
{
	struct tinydrm_device *tdev = pipe_to_tinydrm(pipe);
	struct mipi_dbi *mipi = mipi_dbi_from_tinydrm(tdev);
	u8 addr_mode;
	int ret;

	DRM_DEBUG_KMS("\n");

	ret = mipi_dbi_poweron_conditional_reset(mipi);
	if (ret < 0)
		return;
	if (ret == 1)
		goto out_enable;

	mipi_dbi_command(mipi, MIPI_DCS_SET_DISPLAY_OFF);

	/* Positive Gamma Control */
	mipi_dbi_command(mipi, ILI9488_CMD_POSITIVE_GAMMA_CORRECTION,
			 0x00, 0x03, 0x09, 0x08, 0x16,
			 0x0a, 0x3f, 0x78, 0x4c, 0x09,
			 0x0a, 0x08, 0x16, 0x1a, 0x0f);

	/* Negative Gamma Control */
	mipi_dbi_command(mipi, ILI9488_CMD_NEGATIVE_GAMMA_CORRECTION,
			 0x00, 0x16, 0x19, 0x03, 0x0f,
			 0x05, 0x32, 0x45, 0x46, 0x04,
			 0x0e, 0x0d, 0x35, 0x37, 0x0f);


	/* Power Control 1,2 */
	mipi_dbi_command(mipi, ILI9488_CMD_POWER_CONTROL_1, 0x17, 0x15);
	mipi_dbi_command(mipi, ILI9488_CMD_POWER_CONTROL_2, 0x41);

	/* VCOM Control 1 */
	mipi_dbi_command(mipi, ILI9488_CMD_VCOM_CONTROL_1, 0x00, 0x12, 0x80);


    /* Pixel Format */
	mipi_dbi_command(mipi, MIPI_DCS_SET_PIXEL_FORMAT, MIPI_DCS_PIXEL_FMT_18BIT<<1 | MIPI_DCS_PIXEL_FMT_18BIT);


	mipi_dbi_command(mipi, ILI9488_CMD_INTERFACE_MODE_CONTROL, 0x80);


	/* Frame Rate Control */
	/*	Frame rate = 60.76Hz.*/
	mipi_dbi_command(mipi, ILI9488_CMD_FRAME_RATE_CONTROL_NORMAL, 0xa1);


	/* Display Inversion Control */
	/*	2 dot inversion */
	mipi_dbi_command(mipi, ILI9488_CMD_DISPLAY_INVERSION_CONTROL, 0x02);


	/* Set Image Function */
	mipi_dbi_command(mipi, ILI9488_CMD_SET_IMAGE_FUNCTION, 0x00);


	/* Adjust Control 3 */
	mipi_dbi_command(mipi, ILI9488_CMD_ADJUST_CONTROL_3,
			 0xa9, 0x51, 0x2c, 0x82);

	/* CABC control 2 */
	mipi_dbi_command(mipi, ILI9488_CMD_CABC_CONTROL_2, 0xb0);


	/* Sleep OUT */
	mipi_dbi_command(mipi, ILI9488_CMD_SLEEP_OUT);

	msleep(120);

	mipi_dbi_command(mipi, ILI9488_CMD_NORMAL_DISP_MODE_ON);

	/* Display ON */
	mipi_dbi_command(mipi, ILI9488_CMD_DISPLAY_ON);
	msleep(100);    
out_enable:
	switch (mipi->rotation) {
	default:
		addr_mode = ILI9488_MADCTL_MX;
		break;
	case 90:
		addr_mode = ILI9488_MADCTL_MV;
		break;
	case 180:
		addr_mode = ILI9488_MADCTL_MY;
		break;
	case 270:
		addr_mode = ILI9488_MADCTL_MV | ILI9488_MADCTL_MY |
			    ILI9488_MADCTL_MX;
		break;
	}
	addr_mode |= ILI9488_MADCTL_BGR;
	mipi_dbi_command(mipi, MIPI_DCS_SET_ADDRESS_MODE, addr_mode);
	mipi_dbi_enable_flush(mipi, crtc_state, plane_state);
}

static const struct drm_simple_display_pipe_funcs ili9488_pipe_funcs = {
	.enable = sx035hv006_enable,
	.disable = mipi_dbi_pipe_disable,
	.update = tinydrm_display_pipe_update,
	.prepare_fb = drm_gem_fb_simple_display_pipe_prepare_fb,
};

static const struct drm_display_mode sx035hv006_mode = {
	TINYDRM_MODE(320, 480, 49, 73),
};

DEFINE_DRM_GEM_CMA_FOPS(ili9488_fops);

static struct drm_driver ili9488_driver = {
	.driver_features	= DRIVER_GEM | DRIVER_MODESET | DRIVER_PRIME | DRIVER_ATOMIC,
	.fops			= &ili9488_fops,
	TINYDRM_GEM_DRIVER_OPS,
	.debugfs_init		= mipi_dbi_debugfs_init,
	.name			= "ili9488",
	.desc			= "Ilitek ILI9488",
	.date			= "20190812",
	.major			= 1,
	.minor			= 0,
};

static const struct of_device_id ili9488_of_match[] = {
	{ .compatible = "makerlab,sx035hv006" },
	{ }
};
MODULE_DEVICE_TABLE(of, ili9488_of_match);

static const struct spi_device_id ili9488_id[] = {
	{ "sx035hv006", 0 },
	{ }
};
MODULE_DEVICE_TABLE(spi, ili9488_id);

static int ili9488_probe(struct spi_device *spi)
{
	struct device *dev = &spi->dev;
	struct mipi_dbi *mipi;
	struct gpio_desc *dc;
	u32 rotation = 0;
	int ret;

	mipi = devm_kzalloc(dev, sizeof(*mipi), GFP_KERNEL);
	if (!mipi)
		return -ENOMEM;

	mipi->reset = devm_gpiod_get_optional(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(mipi->reset)) {
		DRM_DEV_ERROR(dev, "Failed to get gpio 'reset'\n");
		return PTR_ERR(mipi->reset);
	}

	dc = devm_gpiod_get_optional(dev, "dc", GPIOD_OUT_LOW);
	if (IS_ERR(dc)) {
		DRM_DEV_ERROR(dev, "Failed to get gpio 'dc'\n");
		return PTR_ERR(dc);
	}

	mipi->backlight = devm_of_find_backlight(dev);
	if (IS_ERR(mipi->backlight))
		return PTR_ERR(mipi->backlight);

	device_property_read_u32(dev, "rotation", &rotation);

	ret = mipi_dbi_spi_init(spi, mipi, dc);
	if (ret)
		return ret;

	ret = mipi_dbi18_init(&spi->dev, mipi, &ili9488_pipe_funcs,
			    &ili9488_driver, &sx035hv006_mode, rotation);
	if (ret)
		return ret;

	spi_set_drvdata(spi, mipi);

	return devm_tinydrm_register(&mipi->tinydrm);
}

static void ili9488_shutdown(struct spi_device *spi)
{
	struct mipi_dbi *mipi = spi_get_drvdata(spi);

	tinydrm_shutdown(&mipi->tinydrm);
}

static struct spi_driver ili9488_spi_driver = {
	.driver = {
		.name = "ili9488",
		.of_match_table = ili9488_of_match,
	},
	.id_table = ili9488_id,
	.probe = ili9488_probe,
	.shutdown = ili9488_shutdown,
};
module_spi_driver(ili9488_spi_driver);

MODULE_DESCRIPTION("Ilitek ILI9488 DRM driver");
MODULE_AUTHOR("BIRD TECHSTEP <t.artsamart@gmail.com>");
MODULE_LICENSE("GPL");
