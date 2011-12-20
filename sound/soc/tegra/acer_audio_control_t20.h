#ifndef __ACER_AUDIO_CONTROL_H__
#define __ACER_AUDIO_CONTROL_H__

#include <mach/tegra_wm8903_pdata.h>
#include <linux/delay.h>
#include <sound/wm8903.h>

/* settings */
#define R44_CTS_LRIN_VOL 0x05
#define R44_ASR_LRIN_VOL 0x16
#define R44_SPK_LRIN_VOL 0x10
#define R44_HP_LRIN_VOL 0x1C
#define R59_SPK_LINEOUT_VOL 0x3F
#define R24_DAC_BOOST_0dB 0x00
#define R24_DAC_BOOST_6dB 0x01
#define R41_DRC_MAXGAIN_38dB 0x03
#define R42_COMPRESSOR_SLOP_R0 0x05
#define R42_COMPRESSOR_SLOP_DEFAULT_R0 0x04
#define R43_COMPRESSOR_THRESSHOLD_T 0x14
#define R43_COMPRESSOR_THRESSHOLD_DEFAULT_T 0
#define R43_COMPRESSOR_THRESSHOLD_YT 0x01
#define R43_COMPRESSOR_THRESSHOLD_DEFAULT_YT 0
#if defined(CONFIG_MACH_PICASSO_E)
#define HPOUT_VOL 0xB0
#define LINEOUT_VOL 0xB7
#elif defined(CONFIG_MACH_PICASSO)
#define HPOUT_VOL 0xB8
#define LINEOUT_VOL 0xB7
#endif

/* Acoustic table */
#ifdef CONFIG_ACER_FM_SINGLE_MIC
#define ACOUSTIC_DEVICE_MIC_VOIP_TABLE                  0x02
#define ACOUSTIC_HEADSET_MIC_VOIP_TABLE                 0x04
#define ACOUSTIC_DEVICE_MIC_RECORDING_TABLE             0x02
#define ACOUSTIC_HEADSET_MIC_RECORDING_TABLE            0x04
#define ACOUSTIC_CAMCORDER_RECORDER_TABLE               0x06
#define ACOUSTIC_DEVICE_MIC_MUSIC_RECOGNITION_TABLE     0x03
#define ACOUSTIC_HEADSET_MIC_MUSIC_RECOGNITION_TABLE    0x05
#define ACOUSTIC_SPEECH_RECOGNITION_TABLE               0x07
#define ACOUSTIC_CTS_VERIFIER_TABLE                     0x08
#else
#define ACOUSTIC_DEVICE_MIC_VOIP_TABLE                  0x02
#define ACOUSTIC_DEVICE_MIC_RECORDING_TABLE             0x02
#define ACOUSTIC_CAMCORDER_TABLE                        0x04
#define ACOUSTIC_FRONT_CAMCORDER_TABLE                  0x02
#define ACOUSTIC_REAR_CAMCORDER_TABLE                   0x06
#define ACOUSTIC_DEVICE_MIC_MUSIC_RECOGNITION_TABLE     0x04
#define ACOUSTIC_SPEECH_RECOGNITION_TABLE               0x07
#define ACOUSTIC_CTS_VERIFIER_TABLE                     0x07
#endif

#ifdef CONFIG_ACER_FM_SINGLE_MIC
extern void mic_switch(struct tegra_wm8903_platform_data *pdata);
extern void set_int_mic_state(bool state);
extern void set_ext_mic_state(bool state);
#ifdef CONFIG_MACH_PICASSO_E
extern void setAudioCABC(int enable);
#endif
#endif
extern int switch_audio_table(int control_mode, bool fromAP);
extern void acer_volume_setting(struct snd_soc_codec *codec, struct snd_pcm_substream *substream);
extern bool handset_mic_detect(struct snd_soc_codec *codec);
int tune_codec_setting(int control_mode);
void set_voip_hp_gain(struct snd_soc_codec* codec);
void set_voip_spk_gain(struct snd_soc_codec* codec);
void set_cts_spk_gain(struct snd_soc_codec* codec);
void set_asr_spk_gain(struct snd_soc_codec* codec);

enum mic_mode{
	SINGLE_MIC = 1,
	DUAL_MIC,
};

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

struct acer_table_data {
	int input;
	int output;
};

struct acer_gpio_data {
	int spkr_en;
	int hp_det;
	int int_mic_en;
};

struct acer_state_data {
	bool int_mic;
	bool ext_mic;
	int old;
};

struct acer_mode_data {
	int control;
	int input_source;
	int ap_control;
};

struct acer_audio_data {
	struct snd_soc_codec* codec;
	const char *pin;
	bool AP_Lock;
	struct acer_table_data table;
	struct acer_gpio_data gpio;
	struct acer_state_data state;
	struct acer_mode_data mode;
};

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
