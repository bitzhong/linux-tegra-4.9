/*
 * Copyright (c) 2019, NVIDIA CORPORATION.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef ACR_H
#define ACR_H

#include "acr_bootstrap.h"
#ifdef CONFIG_NVGPU_ACR_LEGACY
#include "acr_blob_construct_v0.h"
#endif
#include "acr_blob_construct_v1.h"

struct gk20a;
struct nvgpu_acr;
struct wpr_carveout_info;

#define nvgpu_acr_dbg(g, fmt, args...) \
	nvgpu_log(g, gpu_dbg_pmu, fmt, ##args)

/*
 * Falcon UCODE header index.
 */
#define FLCN_NL_UCODE_HDR_OS_CODE_OFF_IND              (0U)
#define FLCN_NL_UCODE_HDR_OS_CODE_SIZE_IND             (1U)
#define FLCN_NL_UCODE_HDR_OS_DATA_OFF_IND              (2U)
#define FLCN_NL_UCODE_HDR_OS_DATA_SIZE_IND             (3U)
#define FLCN_NL_UCODE_HDR_NUM_APPS_IND                 (4U)

/*
 * There are total N number of Apps with code and offset defined in UCODE header
 * This macro provides the CODE and DATA offset and size of Ath application.
 */
#define FLCN_NL_UCODE_HDR_APP_CODE_START_IND           (5U)
#define FLCN_NL_UCODE_HDR_APP_CODE_OFF_IND(N, A) \
	(FLCN_NL_UCODE_HDR_APP_CODE_START_IND + ((A)*2U))
#define FLCN_NL_UCODE_HDR_APP_CODE_SIZE_IND(N, A) \
	(FLCN_NL_UCODE_HDR_APP_CODE_START_IND + ((A)*2U) + 1U)
#define FLCN_NL_UCODE_HDR_APP_CODE_END_IND(N) \
	(FLCN_NL_UCODE_HDR_APP_CODE_START_IND + ((N)*2U) - 1U)

#define FLCN_NL_UCODE_HDR_APP_DATA_START_IND(N) \
	(FLCN_NL_UCODE_HDR_APP_CODE_END_IND(N) + 1U)
#define FLCN_NL_UCODE_HDR_APP_DATA_OFF_IND(N, A) \
	(FLCN_NL_UCODE_HDR_APP_DATA_START_IND(N) + ((A)*2U))
#define FLCN_NL_UCODE_HDR_APP_DATA_SIZE_IND(N, A) \
	(FLCN_NL_UCODE_HDR_APP_DATA_START_IND(N) + ((A)*2U) + 1U)
#define FLCN_NL_UCODE_HDR_APP_DATA_END_IND(N) \
	(FLCN_NL_UCODE_HDR_APP_DATA_START_IND(N) + ((N)*2U) - 1U)

#define FLCN_NL_UCODE_HDR_OS_OVL_OFF_IND(N) \
	(FLCN_NL_UCODE_HDR_APP_DATA_END_IND(N) + 1U)
#define FLCN_NL_UCODE_HDR_OS_OVL_SIZE_IND(N) \
	(FLCN_NL_UCODE_HDR_APP_DATA_END_IND(N) + 2U)

#define HSBIN_ACR_BL_UCODE_IMAGE "pmu_bl.bin"
#define HSBIN_ACR_UCODE_IMAGE "acr_ucode.bin"
#define HSBIN_ACR_AHESASC_NON_FUSA_PROD_UCODE "acr_ahesasc_prod_ucode.bin"
#define HSBIN_ACR_ASB_NON_FUSA_PROD_UCODE "acr_asb_prod_ucode.bin"
#define HSBIN_ACR_AHESASC_NON_FUSA_DBG_UCODE "acr_ahesasc_dbg_ucode.bin"
#define HSBIN_ACR_ASB_NON_FUSA_DBG_UCODE "acr_asb_dbg_ucode.bin"

#define HSBIN_ACR_AHESASC_FUSA_PROD_UCODE "acr_ahesasc_fusa_prod_ucode.bin"
#define HSBIN_ACR_ASB_FUSA_PROD_UCODE "acr_asb_fusa_prod_ucode.bin"
#define HSBIN_ACR_AHESASC_FUSA_DBG_UCODE "acr_ahesasc_fusa_dbg_ucode.bin"
#define HSBIN_ACR_ASB_FUSA_DBG_UCODE "acr_asb_fusa_dbg_ucode.bin"

#define GM20B_FECS_UCODE_SIG "fecs_sig.bin"
#define T18x_GPCCS_UCODE_SIG "gpccs_sig.bin"

#define GV100_FECS_UCODE_SIG "gv100/fecs_sig.bin"
#define GV100_GPCCS_UCODE_SIG "gv100/gpccs_sig.bin"

#define TU104_FECS_UCODE_SIG "tu104/fecs_sig.bin"
#define TU104_GPCCS_UCODE_SIG "tu104/gpccs_sig.bin"

#define LSF_SEC2_UCODE_IMAGE_BIN "sec2_ucode_image.bin"
#define LSF_SEC2_UCODE_DESC_BIN "sec2_ucode_desc.bin"
#define LSF_SEC2_UCODE_SIG_BIN "sec2_sig.bin"

#define ACR_COMPLETION_TIMEOUT_MS 10000U /*in msec */

struct acr_lsf_config {
	u32 falcon_id;
	u32 falcon_dma_idx;
	bool is_lazy_bootstrap;
	bool is_priv_load;

	int (*get_lsf_ucode_details)(struct gk20a *g, void *lsf_ucode_img);
	void (*get_cmd_line_args_offset)(struct gk20a *g, u32 *args_offset);
};

struct nvgpu_acr {
	struct gk20a *g;

	u32 bootstrap_owner;

	/* LSF properties */
	u32 lsf_enable_mask;
	struct acr_lsf_config lsf[FALCON_ID_END];

	/*
	 * non-wpr space to hold LSF ucodes,
	 * ACR does copy ucode from non-wpr to wpr
	 */
	struct nvgpu_mem ucode_blob;
	/*
	 * Even though this mem_desc wouldn't be used,
	 * the wpr region needs to be reserved in the
	 * allocator in dGPU case.
	 */
	struct nvgpu_mem wpr_dummy;

	/* ACR member for different types of ucode */
	/* For older dgpu/tegra ACR cuode */
	struct hs_acr acr;
	/* ACR load split feature support */
	struct hs_acr acr_ahesasc;
	struct hs_acr acr_asb;

	int (*prepare_ucode_blob)(struct gk20a *g);
	int (*alloc_blob_space)(struct gk20a *g, size_t size,
		struct nvgpu_mem *mem);
	void (*patch_wpr_info_to_ucode)(struct gk20a *g, struct nvgpu_acr *acr,
		struct hs_acr *acr_desc, bool is_recovery);
	void (*acr_fill_bl_dmem_desc)(struct gk20a *g,
		struct nvgpu_acr *acr, struct hs_acr *acr_desc,
		u32 *acr_ucode_header);
	int (*bootstrap_hs_acr)(struct gk20a *g, struct nvgpu_acr *acr,
		struct hs_acr *acr_desc);

	void (*get_wpr_info)(struct gk20a *g, struct wpr_carveout_info *inf);
};

#endif /* ACR_H */
