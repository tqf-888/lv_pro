/* Copyright (c) 2019-2035 Allwinner Technology Co., Ltd. ALL rights reserved.

 * Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
 * the the People's Republic of China and other countries.
 * All Allwinner Technology Co.,Ltd. trademarks are used with permission.

 * DISCLAIMER
 * THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
 * IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
 * IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
 * ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
 * ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
 * COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
 * YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.


 * THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
 * PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
 * THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
 * OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SOUND_SETTING_H
#define SOUND_SETTING_H

/************************ 配置主键与字段名 ************************/
#define SOUND_MAINKEY              ("sound")   // INI 配置文件中的主段名（[sound]）
#define SOUND_VOLUME               ("volume")  // 音量字段名
#define SOUND_MODE_NAME            ("sound_mode")  // 音效模式字段名
#define SOUND_OUTPUT_NAME          ("sound_output") // 音频输出设备字段名

/************************ 音效模式字符串值 ************************/
#define SOUND_MODE_STANDARD_NAME   ("s_standard") // 标准模式对应的字符串
#define SOUND_MODE_MUSIC_NAME      ("s_music")    // 音乐模式字符串
#define SOUND_MODE_MOVIE_NAME      ("s_movie")    // 电影模式字符串
#define SOUND_MODE_SPORT_NAME      ("s_sport")    // 运动模式字符串
#define SOUND_MODE_USER_NAME       ("s_user")     // 用户自定义模式字符串

/************************ 低音/高音字段名 ************************/
#define SOUND_BASS                  ("bass")   // 低音增益字段名
#define SOUND_TREBLE                ("treble") // 高音增益字段名

/************************ 音效模式枚举值 ************************/
#define SOUND_MODE_STANDARD        (0)   // 标准模式 ID
#define SOUND_MODE_MUSIC           (1)   // 音乐模式 ID
#define SOUND_MODE_MOVIC           (2)   // 电影模式 ID（注意拼写为 MOVIC）
#define SOUND_MODE_SPORTS          (3)   // 运动模式 ID
#define SOUND_MODE_USER            (4)   // 用户模式 ID

/************************ 音频输出设备枚举值(lvgl) ************************///
#define SOUND_OUTPUT_SPEAKER       (0)   // 扬声器
#define SOUND_OUTPUT_ARC           (1)   // ARC（音频回传通道）
#define SOUND_OUTPUT_BT            (2)   // 蓝牙设备
#define SOUND_OUTPUT_HEADPHONE     (3)   // 耳机
#define SOUND_OUTPUT_OWA           (4)   // OWA（可能指光纤/同轴等数字音频）

/************************ 函数声明 ************************/

/**
 * @brief 设置音效模式（标准/音乐/电影/运动/用户）
 * @param value 模式 ID，取 SOUND_MODE_* 宏定义的值
 * @return 成功返回 0，失败返回 -1
 */
int factory_set_sound_mode(int value);

/**
 * @brief 获取当前音效模式
 * @return 模式 ID（SOUND_MODE_* 宏），失败返回 -1
 */
int factory_get_sounde_mode(void);   // 函数名中多了一个 'e'，实际应为 factory_get_sound_mode

/**
 * @brief 设置音频输出设备（扬声器/ARC/蓝牙/耳机/OWA）
 * @param value 设备 ID，取 SOUND_OUTPUT_* 宏定义的值
 * @return 成功返回 0，失败返回 -1
 */
int factory_set_sound_output(int value);

/**
 * @brief 获取当前音频输出设备
 * @return 设备 ID（SOUND_OUTPUT_* 宏），失败返回 -1
 */
int factory_get_sounde_output(void); // 函数名中多了一个 'e'

/**
 * @brief 设置系统音量
 * @param value 音量值（范围通常为 0-100）
 * @return 成功返回 0，失败返回 -1
 */
int factory_set_sound_volume(int value);

/**
 * @brief 获取当前系统音量
 * @return 音量值（0-100），失败返回 -1
 */
int factory_get_sounde_volume(void); // 函数名中多了一个 'e'

/**
 * @brief 设置音频处理参数（低音/高音等，与音效模式关联）
 * @param mode   音效模式 ID（SOUND_MODE_*），决定参数属于哪个模式
 * @param subkey 参数名，如 SOUND_BASS 或 SOUND_TREBLE
 * @param value  要设置的数值（通常范围 -10 ~ 10）
 * @return 成功返回 0，失败返回 -1
 */
int factory_set_aq_param(int mode, char *subkey, int value);

/**
 * @brief 获取指定音效模式下的音频处理参数（低音/高音等）
 * @param mode   音效模式 ID（SOUND_MODE_*）
 * @param subkey 参数名，如 SOUND_BASS 或 SOUND_TREBLE
 * @return 参数数值，失败返回 -1
 */
int factory_get_aq_param(int mode, char *subkey);

#endif /* SOUND_SETTING_H */
