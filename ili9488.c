// SPDX-License-Identifier: GPL-2.0+
/*
 * DRM driver for Ilitek ILI9488 panels
 *
 * Copyright 2019 BIRD TECHSTEP <t.artsamart@gmail.com>
 *
 * Based on mi0283qt.c:
 * Copyright 2016 Noralf Trønnes
 */

#include <linux/pm.h>
#include <linux/swab.h>

#include <drm/tinydrm/tinydrm.h>

#include <linux/debugfs.h>
#include <linux/dma-buf.h>
#include <linux/regulator/consumer.h>

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

int mipi_dbi18_init(struct device *dev, struct mipi_dbi *mipi,
		  const struct drm_simple_display_pipe_funcs *pipe_funcs,
		  struct drm_driver *driver,
		  const struct drm_display_mode *mode, unsigned int rotation);
static int mipi_dbi18_fb_dirty(struct drm_framebuffer *fb,
			     struct drm_file *file_priv,
			     unsigned int flags, unsigned int color,
			     struct drm_clip_rect *clips,
			     unsigned int num_clips);
int mipi_dbi18_buf_copy(void *dst, struct drm_framebuffer *fb,
		      struct drm_clip_rect *clip);

static const struct drm_framebuffer_funcs mipi_dbi_fb_funcs = {
	.destroy	= drm_gem_fb_destroy,
	.create_handle	= drm_gem_fb_create_handle,
	.dirty		= tinydrm_fb_dirty,
};

static const uint32_t mipi_dbi_formats[] = {
	DRM_FORMAT_RGB565,
	DRM_FORMAT_XRGB8888,
};

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

/**
 * tinydrm_18memcpy - Copy clip buffer
 * @dst: Destination buffer
 * @vaddr: Source buffer
 * @fb: DRM framebuffer
 * @clip: Clip rectangle area to copy
 */
void tinydrm_18memcpy(void *dst, void *vaddr, struct drm_framebuffer *fb,
		    struct drm_clip_rect *clip)
{
	unsigned int cpp = drm_format_plane_cpp(fb->format->format, 0);
	unsigned int pitch = fb->pitches[0];
	void *src = vaddr + (clip->y1 * pitch) + (clip->x1 * cpp);
	size_t len = (clip->x2 - clip->x1) * cpp;
	unsigned int y;

	for (y = clip->y1; y < clip->y2; y++) {
		memcpy(dst, src, len);
		src += pitch;
		dst += len;
	}
}
// EXPORT_SYMBOL(tinydrm_18memcpy);

/**
 * tinydrm_xrgb8888_to_rgb666 - Convert XRGB8888 to RGB666 clip buffer
 * @dst: RGB666 destination buffer
 * @vaddr: XRGB8888 source buffer
 * @fb: DRM framebuffer
 * @clip: Clip rectangle area to copy
 * @swap: Swap bytes
 *
 * Drivers can use this function for RGB666 devices that don't natively
 * support XRGB8888.
 */
void tinydrm_xrgb8888_to_rgb666(u8 *dst, void *vaddr,
				struct drm_framebuffer *fb,
				struct drm_clip_rect *clip)
{
	size_t len = (clip->x2 - clip->x1) * sizeof(u32);
	unsigned int x, y;
	u32 *src, *buf;

	buf = kmalloc(len, GFP_KERNEL);
	if (!buf)
		return;

	for (y = clip->y1; y < clip->y2; y++) {
		src = vaddr + (y * fb->pitches[0]);
		src += clip->x1;
		memcpy(buf, src, len);
		src = buf;
		for (x = clip->x1; x < clip->x2; x++) {
			*dst++ = ((*src & 0x000000FC));
			*dst++ = ((*src & 0x0000FC00) >> 8);
			*dst++ = ((*src & 0x00FC0000) >> 16);
			src++;
		}
	}

	kfree(buf);
}
EXPORT_SYMBOL(tinydrm_xrgb8888_to_rgb666);

static void tinydrm_rgb565_to_rgb666(u8 *dst, void *vaddr,
				     struct drm_framebuffer *fb,
				     struct drm_clip_rect *clip)
{
	// TODO: implement conversion
}
EXPORT_SYMBOL(tinydrm_rgb565_to_rgb666);


/**
 * mipi_dbi18_buf_copy - Copy a framebuffer, transforming it if necessary
 * @dst: The destination buffer
 * @fb: The source framebuffer
 * @clip: Clipping rectangle of the area to be copied
 *
 * Returns:
 * Zero on success, negative error code on failure.
 */
int mipi_dbi18_buf_copy(void *dst, struct drm_framebuffer *fb,
		      struct drm_clip_rect *clip)
{
	struct drm_gem_cma_object *cma_obj = drm_fb_cma_get_gem_obj(fb, 0);
	struct dma_buf_attachment *import_attach = cma_obj->base.import_attach;
	struct drm_format_name_buf format_name;
	void *src = cma_obj->vaddr;
	int ret = 0;

	if (import_attach) {
		ret = dma_buf_begin_cpu_access(import_attach->dmabuf,
					       DMA_FROM_DEVICE);
		if (ret)
			return ret;
	}

	switch (fb->format->format) {
	case DRM_FORMAT_RGB565:
		tinydrm_rgb565_to_rgb666(dst, src, fb, clip);
		break;
	case DRM_FORMAT_XRGB8888:
		tinydrm_xrgb8888_to_rgb666(dst, src, fb, clip);
		break;
	default:
		dev_err_once(fb->dev->dev, "Format is not supported: %s\n",
			     drm_get_format_name(fb->format->format,
						 &format_name));
		return -EINVAL;
	}

	if (import_attach)
		ret = dma_buf_end_cpu_access(import_attach->dmabuf,
					     DMA_FROM_DEVICE);
	return ret;
}
// EXPORT_SYMBOL(mipi_dbi18_buf_copy);

static int mipi_dbi18_fb_dirty(struct drm_framebuffer *fb,
			     struct drm_file *file_priv,
			     unsigned int flags, unsigned int color,
			     struct drm_clip_rect *clips,
			     unsigned int num_clips)
{
	struct drm_gem_cma_object *cma_obj = drm_fb_cma_get_gem_obj(fb, 0);
	struct tinydrm_device *tdev = fb->dev->dev_private;
	struct mipi_dbi *mipi = mipi_dbi_from_tinydrm(tdev);
	struct drm_clip_rect clip;
	int ret = 0;
	bool full;
	void *tr;

	if (!mipi->enabled)
		return 0;

	full = tinydrm_merge_clips(&clip, clips, num_clips, flags,
				   fb->width, fb->height);

	DRM_DEBUG("Flushing [FB:%d] x1=%u, x2=%u, y1=%u, y2=%u\n", fb->base.id,
		  clip.x1, clip.x2, clip.y1, clip.y2);

	if (!mipi->dc || !full ||
	    fb->format->format == DRM_FORMAT_XRGB8888) {
		tr = mipi->tx_buf;
		ret = mipi_dbi18_buf_copy(mipi->tx_buf, fb, &clip);
		if (ret)
			return ret;
	} else {
		tr = cma_obj->vaddr;
	}

	mipi_dbi_command(mipi, MIPI_DCS_SET_COLUMN_ADDRESS,
			 (clip.x1 >> 8) & 0xFF, clip.x1 & 0xFF,
			 (clip.x2 >> 8) & 0xFF, (clip.x2 - 1) & 0xFF);
	mipi_dbi_command(mipi, MIPI_DCS_SET_PAGE_ADDRESS,
			 (clip.y1 >> 8) & 0xFF, clip.y1 & 0xFF,
			 (clip.y2 >> 8) & 0xFF, (clip.y2 - 1) & 0xFF);

	ret = mipi_dbi_command_buf(mipi, MIPI_DCS_WRITE_MEMORY_START, tr,
				(clip.x2 - clip.x1) * (clip.y2 - clip.y1) * 2);

	return ret;
}

/**
 * mipi_dbi18_init - MIPI DBI 18bits initialization
 * @dev: Parent device
 * @mipi: &mipi_dbi structure to initialize
 * @pipe_funcs: Display pipe functions
 * @driver: DRM driver
 * @mode: Display mode
 * @rotation: Initial rotation in degrees Counter Clock Wise
 *
 * This function initializes a &mipi_dbi structure and it's underlying
 * @tinydrm_device. It also sets up the display pipeline.
 *
 * Supported formats: Native RGB666 and emulated XRGB8888.
 *
 * Objects created by this function will be automatically freed on driver
 * detach (devres).
 *
 * Returns:
 * Zero on success, negative error code on failure.
 */
int mipi_dbi18_init(struct device *dev, struct mipi_dbi *mipi,
		  const struct drm_simple_display_pipe_funcs *pipe_funcs,
		  struct drm_driver *driver,
		  const struct drm_display_mode *mode, unsigned int rotation)
{
	size_t bufsize = mode->vdisplay * mode->hdisplay * sizeof(u16);
	struct tinydrm_device *tdev = &mipi->tinydrm;
	int ret;

	if (!mipi->command)
		return -EINVAL;

	mutex_init(&mipi->cmdlock);

	mipi->tx_buf = devm_kmalloc(dev, bufsize, GFP_KERNEL);
	if (!mipi->tx_buf)
		return -ENOMEM;

	ret = devm_tinydrm_init(dev, tdev, &mipi_dbi_fb_funcs, driver);
	if (ret)
		return ret;

	tdev->fb_dirty = mipi_dbi18_fb_dirty;

	/* TODO: Maybe add DRM_MODE_CONNECTOR_SPI */
	ret = tinydrm_display_pipe_init(tdev, pipe_funcs,
					DRM_MODE_CONNECTOR_VIRTUAL,
					mipi_dbi_formats,
					ARRAY_SIZE(mipi_dbi_formats), mode,
					rotation);
	if (ret)
		return ret;

	tdev->drm->mode_config.preferred_depth = 18;
	mipi->rotation = rotation;

	drm_mode_config_reset(tdev->drm);

	DRM_DEBUG_KMS("preferred_depth=%u, rotation = %u\n",
		      tdev->drm->mode_config.preferred_depth, rotation);

	return 0;
}
// EXPORT_SYMBOL(mipi_dbi18_init);

MODULE_DESCRIPTION("Ilitek ILI9488 DRM driver");
MODULE_AUTHOR("BIRD TECHSTEP <t.artsamart@gmail.com>");
MODULE_LICENSE("GPL");
