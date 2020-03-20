// SPDX-License-Identifier: GPL-2.0+
/*
 * DRM driver for Ilitek ST7789 panels
 *
 * Copyright 2018 Thongchai Artsamart <t.artsamart@gmail.com>
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

// ST7789 specific commands used in init
#define ST7789_NOP			0x00
#define ST7789_SWRESET		0x01
#define ST7789_RDDID		0x04
#define ST7789_RDDST		0x09

#define ST7789_RDDPM		0x0A      // Read display power mode
#define ST7789_RDD_MADCTL	0x0B      // Read display MADCTL
#define ST7789_RDD_COLMOD	0x0C      // Read display pixel format
#define ST7789_RDDIM		0x0D      // Read display image mode
#define ST7789_RDDSM		0x0E      // Read display signal mode
#define ST7789_RDDSR		0x0F      // Read display self-diagnostic result (ST7789V)

#define ST7789_SLPIN		0x10
#define ST7789_SLPOUT		0x11
#define ST7789_PTLON		0x12
#define ST7789_NORON		0x13

#define ST7789_INVOFF		0x20
#define ST7789_INVON		0x21
#define ST7789_GAMSET		0x26      // Gamma set
#define ST7789_DISPOFF		0x28
#define ST7789_DISPON		0x29
#define ST7789_CASET		0x2A
#define ST7789_RASET		0x2B
#define ST7789_RAMWR		0x2C
#define ST7789_RGBSET		0x2D      // Color setting for 4096, 64K and 262K colors
#define ST7789_RAMRD		0x2E

#define ST7789_PTLAR		0x30
#define ST7789_VSCRDEF		0x33      // Vertical scrolling definition (ST7789V)
#define ST7789_TEOFF		0x34      // Tearing effect line off
#define ST7789_TEON			0x35      // Tearing effect line on
#define ST7789_MADCTL		0x36      // Memory data access control
#define ST7789_IDMOFF		0x38      // Idle mode off
#define ST7789_IDMON		0x39      // Idle mode on
#define ST7789_RAMWRC		0x3C      // Memory write continue (ST7789V)
#define ST7789_RAMRDC		0x3E      // Memory read continue (ST7789V)
#define ST7789_COLMOD		0x3A

#define ST7789_RAMCTRL		0xB0      // RAM control
#define ST7789_RGBCTRL		0xB1      // RGB control
#define ST7789_PORCTRL		0xB2      // Porch control
#define ST7789_FRCTRL1		0xB3      // Frame rate control
#define ST7789_PARCTRL		0xB5      // Partial mode control
#define ST7789_GCTRL		0xB7      // Gate control
#define ST7789_GTADJ		0xB8      // Gate on timing adjustment
#define ST7789_DGMEN		0xBA      // Digital gamma enable
#define ST7789_VCOMS		0xBB      // VCOMS setting
#define ST7789_LCMCTRL		0xC0      // LCM control
#define ST7789_IDSET		0xC1      // ID setting
#define ST7789_VDVVRHEN		0xC2      // VDV and VRH command enable
#define ST7789_VRHS			0xC3      // VRH set
#define ST7789_VDVSET		0xC4      // VDV setting
#define ST7789_VCMOFSET		0xC5      // VCOMS offset set
#define ST7789_FRCTR2		0xC6      // FR Control 2
#define ST7789_CABCCTRL		0xC7      // CABC control
#define ST7789_REGSEL1		0xC8      // Register value section 1
#define ST7789_REGSEL2		0xCA      // Register value section 2
#define ST7789_PWMFRSEL		0xCC      // PWM frequency selection
#define ST7789_PWCTRL1		0xD0      // Power control 1
#define ST7789_VAPVANEN		0xD2      // Enable VAP/VAN signal output
#define ST7789_CMD2EN		0xDF      // Command 2 enable
#define ST7789_PVGAMCTRL	0xE0      // Positive voltage gamma control
#define ST7789_NVGAMCTRL	0xE1      // Negative voltage gamma control
#define ST7789_DGMLUTR		0xE2      // Digital gamma look-up table for red
#define ST7789_DGMLUTB		0xE3      // Digital gamma look-up table for blue
#define ST7789_GATECTRL		0xE4      // Gate control
#define ST7789_SPI2EN		0xE7      // SPI2 enable
#define ST7789_PWCTRL2		0xE8      // Power control 2
#define ST7789_EQCTRL		0xE9      // Equalize time control
#define ST7789_PROMCTRL		0xEC      // Program control
#define ST7789_PROMEN		0xFA      // Program mode enable
#define ST7789_NVMSET		0xFC      // NVM setting
#define ST7789_PROMACT		0xFE      // Program action

#define ST7789_MADCTL_BGR	BIT(3)
#define ST7789_MADCTL_MV	BIT(5)
#define ST7789_MADCTL_MX	BIT(6)
#define ST7789_MADCTL_MY	BIT(7)

static void zjy130_ips_enable(struct drm_simple_display_pipe *pipe,
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

	mipi_dbi_command(mipi, ST7789_SLPOUT);   // Sleep out
	msleep(120);

	mipi_dbi_command(mipi, ST7789_NORON);    // Normal display mode on

  //------------------------------display and color format setting--------------------------------//
	mipi_dbi_command(mipi, ST7789_MADCTL, TFT_MAD_COLOR_ORDER);

  // JLX240 display datasheet
	mipi_dbi_command(mipi, 0xB6, 0x0A, 0x82);

	mipi_dbi_command(mipi, ST7789_COLMOD, 0x55);
	msleep(10);

  //--------------------------------ST7789V Frame rate setting----------------------------------//
	mipi_dbi_command(mipi, ST7789_PORCTRL, 0x0c, 0x0c, 0x00, 0x33, 0x33);

	mipi_dbi_command(mipi, ST7789_GCTRL, 0x35);      // Voltages: VGH / VGL

  //---------------------------------ST7789V Power setting--------------------------------------//
	mipi_dbi_command(mipi, ST7789_VCOMS, 0x28);		// JLX240 display datasheet
	mipi_dbi_command(mipi, ST7789_LCMCTRL, 0x0C);
	mipi_dbi_command(mipi, ST7789_VDVVRHEN, 0x01, 0xFF);
	mipi_dbi_command(mipi, ST7789_VRHS, 0x10);       // voltage VRHS
	mipi_dbi_command(mipi, ST7789_VDVSET, 0x20);
	mipi_dbi_command(mipi, ST7789_FRCTR2, 0x0f);
	mipi_dbi_command(mipi, ST7789_PWCTRL1, 0xa4, 0xa1);

  //--------------------------------ST7789V gamma setting---------------------------------------//
	mipi_dbi_command(mipi, ST7789_PVGAMCTRL, 0xd0, 0x00, 0x02, 0x07, 0x0a, 0x28, 0x32, 0x44, 0x42, 0x06, 0x0e, 0x12, 0x14, 0x17);
	mipi_dbi_command(mipi, ST7789_NVGAMCTRL, 0xd0, 0x00, 0x02, 0x07, 0x0a, 0x28, 0x31, 0x54, 0x47, 0x0e, 0x1c, 0x17, 0x1b, 0x1e);

	mipi_dbi_command(mipi, ST7789_INVON);

	mipi_dbi_command(mipi, ST7789_CASET, 0x00, 0x00, 0x00, 0xE5);    // Column address set
	mipi_dbi_command(mipi, ST7789_RASET, 0x00, 0x00, 0x00, 0xE5);    // Row address set
	msleep(120);

	mipi_dbi_command(mipi, ST7789_DISPON);    //Display on
	msleep(120);

out_enable:
	switch (mipi->rotation) {
	default:
		//addr_mode = ST7789_MADCTL_MX;
		addr_mode = 0x00;
        //mipi_dbi_command(mipi, ST7789_CASET, 0x00, 0x00, 0x00, 0xE5);    // Column address set
	    //mipi_dbi_command(mipi, ST7789_RASET, 0x00, 0x00, 0x00, 0xE5);    // Row address set
		break;
	case 90:
		//addr_mode = ST7789_MADCTL_MV;
		addr_mode = ST7789_MADCTL_MX | ST7789_MADCTL_MV;
        //mipi_dbi_command(mipi, ST7789_CASET, 0x00, 0x00, 0x00, 0xE5);    // Column address set
	    //mipi_dbi_command(mipi, ST7789_RASET, 0x00, 0x00, 0x00, 0xE5);    // Row address set
		break;
	case 180:
		//addr_mode = ST7789_MADCTL_MY;
		addr_mode = ST7789_MADCTL_MX | ST7789_MADCTL_MY;
        // rowstart 80
       	// mipi_dbi_command(mipi, ST7789_CASET, 0x00, 0x00, 0x00, 0xE5);    // Column address set
	  //  mipi_dbi_command(mipi, ST7789_RASET,   80, 0x00, 0x00, 80 + 0xE5);    // Row address set
		break;
	case 270:
		//addr_mode = ST7789_MADCTL_MV | ST7789_MADCTL_MY | ST7789_MADCTL_MX;
		addr_mode = ST7789_MADCTL_MV | ST7789_MADCTL_MY;
        // colstart 80;
        //mipi_dbi_command(mipi, ST7789_CASET,   80, 0x00, 0x00, 80 + 0xE5);    // Column address set
	    //mipi_dbi_command(mipi, ST7789_RASET, 0x00, 0x00, 0x00, 0xE5);    // Row address set
		break;
	}
	addr_mode |= ST7789_MADCTL_BGR;
	mipi_dbi_command(mipi, MIPI_DCS_SET_ADDRESS_MODE, addr_mode);
	mipi_dbi_enable_flush(mipi, crtc_state, plane_state);
}

static const struct drm_simple_display_pipe_funcs st7789_pipe_funcs = {
	.enable = zjy130_ips_enable,
	.disable = mipi_dbi_pipe_disable,
	.update = tinydrm_display_pipe_update,
	.prepare_fb = drm_gem_fb_simple_display_pipe_prepare_fb,
};

static const struct drm_display_mode zjy130_ips_mode = {
	TINYDRM_MODE(240, 240, 37, 37),
};

DEFINE_DRM_GEM_CMA_FOPS(st7789_fops);

static struct drm_driver st7789_driver = {
	.driver_features	= DRIVER_GEM | DRIVER_MODESET | DRIVER_PRIME | DRIVER_ATOMIC,
	.fops			= &st7789_fops,
	TINYDRM_GEM_DRIVER_OPS,
	.debugfs_init		= mipi_dbi_debugfs_init,
	.name			= "st7789",
	.desc			= "Sitronix ST7789",
	.date			= "20200320",
	.major			= 1,
	.minor			= 0,
};

static const struct of_device_id st7789_of_match[] = {
	{ .compatible = "makerlab,zjy130_ips" },
	{ }
};
MODULE_DEVICE_TABLE(of, st7789_of_match);

static const struct spi_device_id st7789_id[] = {
	{ "zjy130-ips", 0 },
	{ }
};
MODULE_DEVICE_TABLE(spi, st7789_id);

static int st7789_probe(struct spi_device *spi)
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

	ret = mipi_dbi_init(&spi->dev, mipi, &st7789_pipe_funcs,
			    &st7789_driver, &zjy130_ips_mode, rotation);
	if (ret)
		return ret;

	spi_set_drvdata(spi, mipi);

	return devm_tinydrm_register(&mipi->tinydrm);
}

static void st7789_shutdown(struct spi_device *spi)
{
	struct mipi_dbi *mipi = spi_get_drvdata(spi);

	tinydrm_shutdown(&mipi->tinydrm);
}

static struct spi_driver st7789_spi_driver = {
	.driver = {
		.name = "st7789",
		.of_match_table = st7789_of_match,
	},
	.id_table = st7789_id,
	.probe = st7789_probe,
	.shutdown = st7789_shutdown,
};
module_spi_driver(st7789_spi_driver);

MODULE_DESCRIPTION("Sitronix ST7789 DRM driver");
MODULE_AUTHOR("Thongchai Artsamart <t.artsamart@gmail.com>");
MODULE_LICENSE("GPL");
