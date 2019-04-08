/*
 * Copyright (c) 2016-2019, NVIDIA CORPORATION.  All rights reserved.
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

#include <nvgpu/gk20a.h>
#include <nvgpu/pmu/therm.h>
#include <nvgpu/clk_arb.h>

#include "thrm.h"
#include "thrmpmu.h"

int nvgpu_pmu_handle_therm_event(struct nvgpu_pmu *pmu,
			struct nv_pmu_therm_msg *msg)
{
	struct gk20a *g = gk20a_from_pmu(pmu);

	nvgpu_log_fn(g, " ");

	switch (msg->msg_type) {
	case NV_PMU_THERM_MSG_ID_EVENT_HW_SLOWDOWN_NOTIFICATION:
		if (msg->hw_slct_msg.mask ==
			BIT(NV_PMU_THERM_EVENT_THERMAL_1)) {
				nvgpu_clk_arb_send_thermal_alarm(pmu->g);
		} else {
			nvgpu_pmu_dbg(g,
				"Unwanted/Unregistered thermal event received %d",
				msg->hw_slct_msg.mask);
		}
		break;
	default:
		nvgpu_pmu_dbg(g, "unkown therm event received %d",
			msg->msg_type);
		break;
	}

	return 0;
}

int nvgpu_therm_domain_sw_setup(struct gk20a *g)
{
	int status;

	status = therm_device_sw_setup(g);
	if (status != 0) {
		nvgpu_err(g,
			"error creating boardobjgrp for therm devices, status - 0x%x",
			status);
		goto exit;
	}

	status = therm_channel_sw_setup(g);
	if (status != 0) {
		nvgpu_err(g,
			"error creating boardobjgrp for therm channel, status - 0x%x",
			status);
		goto exit;
	}

	g->pmu.therm_rpc_handler = nvgpu_pmu_therm_rpc_handler;

exit:
	return status;
}

int nvgpu_therm_domain_pmu_setup(struct gk20a *g)
{
	return therm_send_pmgr_tables_to_pmu(g);
}

int nvgpu_therm_pmu_init_pmupstate(struct gk20a *g)
{
	/* If already allocated, do not re-allocate */
	if (g->therm_pmu != NULL) {
		return 0;
	}

	g->therm_pmu = nvgpu_kzalloc(g, sizeof(*g->therm_pmu));
	if (g->therm_pmu == NULL) {
		return -ENOMEM;
	}

	return 0;
}

void nvgpu_therm_pmu_free_pmupstate(struct gk20a *g)
{
	nvgpu_kfree(g, g->therm_pmu);
	g->therm_pmu = NULL;
}
