#include "audio_api.h"

#include "sound_setting.h"

AudioManager *adev = NULL;

int audio_get_volume(int *vol) {
    int ret = -1;
    if (adev == NULL) {
        ret = adev_open(&adev);
        if (ret < 0) {
            printf("adev_open failed\n");
            return ret;
        }
    }
    long min, max = 0;
    ret = adev_get_master_volume(adev, AUDIO_LEFT_CHANNEL, vol, &min, &max);
    if (ret < 0) {
        printf("%s failed to get vol\n", __func__);
    }
    return ret;

}
int audio_set_volume(int vol) {
    int ret = -1;
    if (adev == NULL) {
        ret = adev_open(&adev);
        if (ret < 0) {
            printf("adev_open failed\n");
            return ret;
        }
    }
    long cur_vol, min, max = 0;
    ret = adev_get_master_volume(adev, AUDIO_LEFT_CHANNEL, &cur_vol, &min, &max);
    if (ret < 0) {
        printf("%s failed to get left channel vol\n", __func__);
        return ret;
    }
    if (vol <= max && vol >= min) {
        adev_set_master_volume(adev, AUDIO_STEREO_CHANNEL, vol);
    }
    return ret;
}

int audio_get_mute(bool *status) {
    int ret = -1;
    if (adev == NULL) {
        ret = adev_open(&adev);
        if (ret < 0) {
            printf("adev_open failed\n");
            return ret;
        }
    }
    *status = false;
    ret = adev_get_mute(adev, status);
    if (ret < 0) {
        printf("adev_get_mute failed\n");
    }
    return ret;
}

int audio_set_mute(bool status) {
    int ret = -1;
    if (adev == NULL) {
        ret = adev_open(&adev);
        if (ret < 0) {
            printf("adev_open failed\n");
            return ret;
        }
    }
    ret = adev_set_mute(adev, status);
    if (ret < 0) {
        printf("adev_set_mute failed\n");
    }
    return ret;
}

int audio_set_eq_treble_gain(int8_t gain) {
    int ret = -1;
    if (adev == NULL) {
        ret = adev_open(&adev);
        if (ret < 0) {
            printf("adev_open failed\n");
            return ret;
        }
    }
    int channels = 0;
    int sample_rate = 0;
    // custom eq is fix to 5 bins.
    int bin_num = 5;
    eq_core_param *params = (eq_core_param *)malloc(sizeof(eq_core_param) * 5);
    ret = adev_get_eq_params(adev, &channels, &sample_rate, &params, bin_num);
    if (ret < 0) {
        printf("%s get eq failed\n", __func__);
        return ret;
    }
    params[3].gain = gain * 2;
    params[4].gain = gain * 2;
    ret = adev_set_eq_params(adev, channels, sample_rate, params, bin_num);
    if (ret < 0) {
        printf("%s set eq failed\n", __func__);
        return ret;
    }
    free(params);
    return ret;
}

int audio_set_eq_bass_gain(int8_t gain) {
    int ret = -1;
    if (adev == NULL) {
        ret = adev_open(&adev);
        if (ret < 0) {
            printf("adev_open failed\n");
            return ret;
        }
    }
    int channels = 0;
    int sample_rate = 0;
    // custom eq is fix to 5 bins.
    int bin_num = 5;
    eq_core_param *params = (eq_core_param *)malloc(sizeof(eq_core_param) * 5);
    ret = adev_get_eq_params(adev, &channels, &sample_rate, &params, bin_num);
    if (ret < 0) {
        printf("%s get eq failed\n", __func__);
        return ret;
    }
    params[0].gain = gain * 2;
    params[1].gain = gain * 2;
    ret = adev_set_eq_params(adev, channels, sample_rate, params, bin_num);
    if (ret < 0) {
        printf("%s set eq failed\n", __func__);
        return ret;
    }
    free(params);
    return ret;
}

int audio_set_eq_mode(int mode) {
    int ret = -1;
    if (adev == NULL) {
        ret = adev_open(&adev);
        if (ret < 0) {
            printf("adev_open failed\n");
            return ret;
        }
    }
    switch (mode) {
        case SOUND_MODE_STANDARD:
            ret = adev_set_sound_mode(adev, STANDARD);
            break;
        case SOUND_MODE_MUSIC:
            ret = adev_set_sound_mode(adev, MUSIC);
            break;
        case SOUND_MODE_MOVIC:
            ret = adev_set_sound_mode(adev, MOVIE);
            break;
        case SOUND_MODE_SPORTS:
            ret = adev_set_sound_mode(adev, SPORT);
            break;
        case SOUND_MODE_USER:
            ret = adev_set_sound_mode(adev, CUSTOM);
            break;
        default:
            ret = adev_set_sound_mode(adev, STANDARD);
            break;
    }
    if (ret < 0) {
        printf("%s mode %d failed\n", __func__, mode);
    }
    return ret;
}


int audio_set_output_device(int mode) {
    int ret = -1;
    if (adev == NULL) {
        ret = adev_open(&adev);
        if (ret < 0) {
            printf("adev_open failed\n");
            return ret;
        }
    }
    printf("%s mode = %d\n", __func__, mode);
    enum snd_device_type type;
    switch (mode) {
        case SOUND_OUTPUT_SPEAKER:
            type = OUT_SPK;
            break;
        case SOUND_OUTPUT_BT:
            type = OUT_A2DP;
            break;
        case SOUND_OUTPUT_ARC:
            type = OUT_HDMI_ARC;
            break;
        case SOUND_OUTPUT_HEADPHONE:
            type = OUT_HEADSET;
            break;
        case SOUND_OUTPUT_OWA:
            type = OUT_OWA;
            break;
        default:
            type = OUT_SPK;
            break;
    }
    if ((ret = adev_set_output_device(adev, type)) < 0) {
        printf("%s failed with error: %d\n", __func__, ret);
    }
    return  ret;
}


#include "system_api.h"
/**
 * @brief 设置音频参数（低音/高音/模式/输出），同时更新系统参数和底层驱动
 * @param type 参数类型
 * @param value 参数值（低音/高音：-10~10；模式/输出：对应的枚举值）
 * @return 成功返回0，失败返回-1
 */
int audio_set_param(sys_param_id type, int value)
{
    int ret = -1;

    switch (type) {
        case P_SOUND_BASS:
            lv_set_sys_param(P_SOUND_BASS, value);
            ret = audio_set_eq_bass_gain(value);
            break;

        case P_SOUND_TREBLE:
            lv_set_sys_param(P_SOUND_TREBLE, value);
            ret = audio_set_eq_treble_gain(value);
            break;

        case P_SOUND_MODE:
            lv_set_sys_param(P_SOUND_MODE, value);
            ret = audio_set_eq_mode(value);
            update_audio_param(); //需要更新
            break;

        case P_SOUND_OUT_MODE:
            lv_set_sys_param(P_SOUND_OUT_MODE, value);
            ret = audio_set_output_device(value);

#if ENABLE_ARC
        if (value == 1) 
        {      
            hdmirx_app_cec_set_arc_enable(1);
        } else if (value == 4) 
        { 
            hdmirx_app_cec_set_arc_enable(0);
        }
#endif
            break;

        default:
            printf("Invalid audio param type\n");
            return -1;
    }

    return ret;
}