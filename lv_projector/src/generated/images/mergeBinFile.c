/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl/lvgl.h"

#if LV_USE_FS_RAWFS

const rawfs_size_t rawfs_file_count = 8;
rawfs_file_t rawfs_files[8] = {
	0x0, 0, 2422372, "/bdfxbhdf.bin",
	0x24f664, 0, 1966150, "/foiwdshboa1.bin",
	0x42f6aa, 0, 3072004, "/bhdrehrtfdh.bin",
	0x71d6ae, 0, 3072004, "/sdpnvbpsd.bin",
	0xa0b6b2, 0, 3072004, "/feswgvwes.bin",
	0xcf96b6, 0, 3072004, "/dgvsdgbsdgb.bin",
	0xfe76ba, 0, 3072004, "/dsgwsegbsr.bin",
	0x12d56be, 0, 2803204, "/sedgdvsdgbdrh.bin",

};

#endif  /*LV_USE_FS_RAWFS*/ 