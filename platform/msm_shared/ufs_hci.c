/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <arch/ops.h>
#include <sys/types.h>
#include <reg.h>
#include <platform/interrupts.h>
#include <platform/iomap.h>
#include <platform/irqs.h>
#include <ufs_hw.h>
#include <utp.h>

uint64_t ufs_alloc_trans_req_list()
{
	void *ptr;

	ptr = memalign(lcm(CACHE_LINE, 1024), 32 * sizeof(struct utp_trans_req_desc));

	if (!ptr)
	{
		dprintf(CRITICAL, "Failed to allocate utrd list.\n");
	}

	return (addr_t) ptr;
}

uint64_t ufs_alloc_task_mgmt_req_list()
{
	addr_t ptr = (addr_t) memalign(1024, 1024);

	if (!ptr)
	{
		dprintf(CRITICAL, "Failed to allocate memory for Task mamagement request list.\n");
	}

	return ptr;
}

int ufs_enable_hci(struct ufs_dev *dev)
{
	int ret;

	/* Enable host controller */
	writel(UFS_HCE_ENABLE, UFS_HCE(dev->base));

	/* Wait until host controller is enabled. */
	ret = ufs_reg_target_val_timeout_loop(UFS_HCE(dev->base), 1, UFS_HCE_TIMEOUT);
	if (ret)
	{
		dprintf(CRITICAL, "Failed to enable UFS host controller.\n");
	}

	return ret;
}

void ufs_irq_enable(struct ufs_dev *dev, uint32_t irq)
{
	/* Clear all irqs. */
	writel(0xFFFFFFFF, UFS_IS(dev->base));

	writel(irq, UFS_IE(dev->base));
	register_int_handler(UFS_IRQ, ufs_irq_handler, dev);
	unmask_interrupt(UFS_IRQ);
}

enum handler_return ufs_irq_handler(void* data)
{
	uint32_t       val;
	struct ufs_dev *dev = (struct ufs_dev *) data;
	struct ufs_req_irq_type irq;

	val = readl(UFS_IS(dev->base));

	if (val & UFS_IS_SBFES || val & UFS_IS_HCFES || val & UFS_IS_UTPES || val & UFS_IS_DFES || val & UFS_IS_UE)
	{
		/* Controller might be in a bad state, unrecoverable error. */
		dprintf(CRITICAL, "UFS error\n");
		ASSERT(0);
	}

	while (val)
	{
		irq.irq_handled = 0;

		if (val & UFS_IS_UCCS)
		{
			/* UIC command */
			event_signal(&(dev->uic_data.uic_event), false);
			/* Clear irq. */
			writel(UFS_IS_UCCS, UFS_IS(dev->base));
			val        &= ~UFS_IS_UCCS;
			irq.irq_handled = UFS_IS_UCCS;
			continue;
		}
		else if (val & UFS_IS_UTRCS)
		{
			/* UTRD completion. */
			irq.list           = &(dev->utrd_data.list_head.list_node);
			irq.irq_handled    = UFS_IS_UTRCS;
			irq.door_bell_reg  = UFS_UTRLDBR(dev->base);

			/* Clear irq. */
			writel(irq.irq_handled, UFS_IS(dev->base));
			val	   &= ~irq.irq_handled;

			utp_process_req_completion(&irq);
		}
		else if (val & UFS_IS_UTMRCS)
		{
			/* UTMRD completion. */
			irq.list = &(dev->utmrd_data.list_head.list_node);
			irq.irq_handled = UFS_IS_UTMRCS;
			/* TODO: Fill in door bell reg for management requests. */

			/* Clear irq. */
			writel(irq.irq_handled, UFS_IS(dev->base));
			val	   &= ~irq.irq_handled;

			utp_process_req_completion(&irq);
		}
		else
		{
			dprintf(CRITICAL, "Unknown irq.\n");
			ASSERT(0);
		}
	}

	return INT_NO_RESCHEDULE;
}

int ufs_reg_target_val_timeout_loop(uint32_t reg_addr, uint32_t target_val, uint32_t timeout)
{
	uint32_t try_again;
	uint32_t val;

	try_again = timeout;

	do
	{
		try_again--;
		val = readl(reg_addr);
		} while (!(val & target_val) && try_again);

	if (!(val & target_val))
		return -UFS_FAILURE;
	else
		return UFS_SUCCESS;
}
