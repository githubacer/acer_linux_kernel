#ifndef __ACER_AUDIO_CONTROL_H__
#define __ACER_AUDIO_CONTROL_H__

#include <mach/tegra_wm8903_pdata.h>
#include <linux/delay.h>
#include <sound/wm8903.h>

/* Define for Fops function */
#define ACER_AUDIO_CONTROL_IOCTL_MAGIC	'f'
#ifdef CONFIG_ACER_ES305
#define IOC_MAXNR						4
#define WM8903_SET_REG					_IO(ACER_AUDIO_CONTROL_IOCTL_MAGIC, 1)
#define WM8903_GET_REG					_IO(ACER_AUDIO_CONTROL_IOCTL_MAGIC, 2)
#define ES305_SET_CMD					_IO(ACER_AUDIO_CONTROL_IOCTL_MAGIC, 3)
#define ES305_GET_CMD					_IO(ACER_AUDIO_CONTROL_IOCTL_MAGIC, 4)
#else
#define IOC_MAXNR						2
#define WM8903_SET_REG					_IO(ACER_AUDIO_CONTROL_IOCTL_MAGIC, 1)
#define WM8903_GET_REG					_IO(ACER_AUDIO_CONTROL_IOCTL_MAGIC, 2)
#endif

extern int switch_audio_table(int control_mode);
extern bool handset_mic_detect(struct snd_soc_codec *codec);

typedef enum {
	DEFAULT             = 0,
	MIC                 = 1,
	VOICE_UPLINK        = 2,
	VOICE_DOWNLINK      = 3,
	VOICE_CALL          = 4,
	CAMCORDER           = 5,
	VOICE_RECOGNITION   = 6,
	VOICE_COMMUNICATION = 7,

	AUDIO_SOURCE_CNT,
	AUDIO_SOURCE_MAX    = AUDIO_SOURCE_CNT - 1,
} audio_source_t;

struct acer_gpio_data {
	/* gpio */
	int spkr_en;
	int hp_det;
	int bypass_en;
};

struct acer_state_data {
	bool int_mic;
	bool ext_mic;
};

struct acer_audio_data {
	int mode;
	int input_table;
	struct snd_soc_codec* codec;
	const char *pin;
	struct acer_gpio_data gpio;
	struct acer_state_data state;
};

struct wm8903_codec_info {
	uint32_t reg;
	uint32_t val;
};

#ifdef CONFIG_ACER_ES305
struct es305_dsp_info {
	uint32_t cmd;
	uint32_t val;
};
#endif

#define SOC_DAPM_SET_PARAM(xname) \
{	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = xname, \
	.info = snd_soc_dapm_info_iconia_param, \
	.get = snd_soc_dapm_get_iconia_param, \
	.put = snd_soc_dapm_put_iconia_param, \
	.private_value = (unsigned long)xname }

int snd_soc_dapm_info_iconia_param(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_info *uinfo);
int snd_soc_dapm_get_iconia_param(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *uncontrol);
int snd_soc_dapm_put_iconia_param(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *uncontrol);

#endif
