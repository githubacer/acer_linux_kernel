/*
 * arch/arm/mach-tegra/include/mach/irqs.h
 *
 * Copyright (C) 2010 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __MACH_TEGRA_IRQS_H
#define __MACH_TEGRA_IRQS_H

/* Primary Interrupt Controller */
#define INT_TMR1			0
#define INT_TMR2			1
#define INT_RTC				2
#define INT_I2S2			3
#define INT_SHR_SEM_INBOX_IBF		4
#define INT_SHR_SEM_INBOX_IBE		5
#define INT_SHR_SEM_OUTBOX_IBF		6
#define INT_SHR_SEM_OUTBOX_IBE		7
#define INT_VDE_UCQ_ERROR		8
#define INT_VDE_SYNC_TOKEN		9
#define INT_VDE_BSE_V			10
#define INT_VDE_BSE_A			11
#define INT_VDE_SXE			12
#define INT_I2S1			13
#define INT_SDIO1			14
#define INT_SDIO2			15
#define INT_VDE				17
#define INT_USB				20
#define INT_HSMMC			22
#define INT_EIDE			23
#define INT_NANDFLASH			24
#define INT_VCP				25
#define INT_APB_DMA			26
#define INT_AHB_DMA			27
#define INT_GNT_0			28
#define INT_GNT_1			29
#define INT_SECONDARY_NIRQ		30
#define INT_SECONDARY_NFIQ		31

/* Secondary Interrupt Controller */
#define INT_GPIO1			32
#define INT_GPIO2			33
#define INT_GPIO3			34
#define INT_GPIO4			35
#define INT_UARTA			36
#define INT_UARTB			37
#define INT_I2C				38
#define INT_SPI				39
#define INT_TWC				40
#define INT_TMR3			41
#define INT_TMR4			42
#define INT_FLOW_RSM0			43
#define INT_FLOW_RSM1			44
#define INT_SPDIF			45
#define INT_UARTC			46
#define INT_MIPI			47
#define INT_EVENTA			48
#define INT_EVENTB			49
#define INT_EVENTC			50
#define INT_EVENTD			51
#define INT_VFIR			52
#define INT_DVC				53
#define INT_SYS_STATS_MON		54
#define INT_GPIO5			55
#define INT_CPU_INTR			56
#define INT_SPI1			59
#define INT_SPB_DMA_COP			60
#define INT_AHB_DMA_COP			61
#define INT_DMA_TX			62
#define INT_DMA_RX			63

/* Tertiary Interrupt Controller */
#define INT_HOST1X_COP_SYNCPT		65
#define INT_HOST1X_MPCORE_SYNCPT	66
#define INT_HOST1X_COP_GENERAL		67
#define INT_HOST1X_MPCORE_GENERAL	68
#define INT_MPE_GENERAL			69
#define INT_VI_GENERAL			70
#define INT_EPP_GENERAL			71
#define INT_ISP_GENERAL			72
#define INT_2D_GENERAL			73
#define INT_DISPLAY_GENERAL		74
#define INT_DISPLAY_B_GENERAL		75
#define INT_HDMI			76
#define INT_TVO_GENERAL			77
#define INT_MC_GENERAL			78
#define INT_EMC_GENERAL			79
#define INT_CMC_GENERAL			80
#define INT_NOR_FLASH			81
#define INT_AC97			82
#define INT_SPI_2			83
#define INT_SPI_3			84
#define INT_I2C2			85
#define INT_KBC				86
#define INT_EXTERNAL_PMU		87
#define INT_GPIO6			88
#define INT_TVDAC			89
#define INT_SW_RESERVED			95

#define NR_IRQS 96
#endif
