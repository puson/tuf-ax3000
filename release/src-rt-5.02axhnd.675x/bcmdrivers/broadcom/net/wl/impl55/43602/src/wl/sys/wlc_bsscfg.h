/*
 * BSS Config related declarations and exported functions for
 * Broadcom 802.11abg Networking Device Driver
 *
 * Copyright 2019 Broadcom
 *
 * This program is the proprietary software of Broadcom and/or
 * its licensors, and may only be used, duplicated, modified or distributed
 * pursuant to the terms and conditions of a separate, written license
 * agreement executed between you and Broadcom (an "Authorized License").
 * Except as set forth in an Authorized License, Broadcom grants no license
 * (express or implied), right to use, or waiver of any kind with respect to
 * the Software, and Broadcom expressly reserves all rights in and to the
 * Software and all intellectual property rights therein.  IF YOU HAVE NO
 * AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
 * WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF
 * THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof, and to
 * use this information only in connection with your use of Broadcom
 * integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR
 * OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL,
 * SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR
 * IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 * IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii)
 * ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
 * OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY
 * NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 *
 * $Id: wlc_bsscfg.h 767468 2018-09-12 08:14:28Z $
 */
#ifndef _WLC_BSSCFG_H_
#define _WLC_BSSCFG_H_

#include "wlc_types.h"
#include "wlc_pub.h"
#include "wlc_phy_hal.h"
#include "wlc_key.h"
#include <proto/802.11r.h>

/* Check if a particular BSS config is AP or STA */
#if defined(AP) && !defined(STA)
#define BSSCFG_AP(cfg)		(1)
#define BSSCFG_STA(cfg)		(0)
#elif !defined(AP) && defined(STA)
#define BSSCFG_AP(cfg)		(0)
#define BSSCFG_STA(cfg)		(1)
#else
#define BSSCFG_AP(cfg)		((cfg)->_ap)
#define BSSCFG_STA(cfg)		(!(cfg)->_ap)
#endif /* defined(AP) && !defined(STA) */

#ifdef STA
#define BSSCFG_IBSS(cfg)	(/* BSSCFG_STA(cfg) && */!(cfg)->BSS)
#else
#define BSSCFG_IBSS(cfg)	(0)
#endif // endif

#ifdef PSTA
#define BSSCFG_PSTA(cfg)	((cfg)->_psta)
#else /* PSTA */
#define BSSCFG_PSTA(cfg)	(0)
#endif /* PSTA */
#ifdef WLAWDL
#define BSSCFG_AWDL(wlc, cfg)    (AWDL_SUPPORT((wlc)->pub) && (cfg)->is_awdl)
#else
#define BSSCFG_AWDL(wlc, cfg)    (0)
#endif // endif

#define BSSCFG_NAN(cfg)	FALSE
#ifdef ACKSUPR_MAC_FILTER
#define BSSCFG_ACKSUPR(cfg) (BSSCFG_AP(cfg) && cfg->acksupr_mac_filter)
#endif /* ACKSUPR_MAC_FILTER */
/*
 * XXX We need to include the interface definition before the bsscfg before they
 * hardly tied. Should we do more either dynamic allocation or parallel array
 * referencing ?
 */
/* forward declarations */
struct scb;

#include <wlc_rate.h>
#include <wlioctl.h>
#include <bcm_notif_pub.h>

#define NTXRATE		64	/* # tx MPDUs rate is reported for */
#define NVITXRATE	32	/* # Video tx MPDUs rate is reported for */

/* mac list */
#define MAXMACLIST	64     	/* max # source MAC matches */

#define WLC_BSSCFG_RATESET_DEFAULT	0
#define WLC_BSSCFG_RATESET_OFDM		1
#define WLC_BSSCFG_RATESET_MAX		WLC_BSSCFG_RATESET_OFDM

#define BCN_TEMPLATE_COUNT 2

/* Iterator for all bsscfgs:  (wlc_info_t *wlc, int idx, wlc_bsscfg_t *cfg) */
#define FOREACH_BSS(wlc, idx, cfg) \
	for (idx = 0; (int) idx < WLC_MAXBSSCFG; idx++) \
		if ((cfg = (wlc)->bsscfg[idx]) != NULL)

/* Iterator for all ibss bsscfgs:  (wlc_info_t *wlc, int idx, wlc_bsscfg_t *cfg) */
#define FOREACH_IBSS(wlc, idx, cfg) \
	for (idx = 0; (int) idx < WLC_MAXBSSCFG; idx++) \
		if (((cfg = (wlc)->bsscfg[idx]) != NULL) && (!cfg->BSS))

/* Iterator for "associated" bsscfgs:  (wlc_info_t *wlc, int idx, wlc_bsscfg_t *cfg) */
#define FOREACH_AS_BSS(wlc, idx, cfg) \
	for (idx = 0; cfg = NULL, (int) idx < WLC_MAXBSSCFG; idx++) \
		if ((wlc)->bsscfg[idx] != NULL && \
		    (wlc)->bsscfg[idx]->associated && \
		    (cfg = (wlc)->bsscfg[idx]) != NULL)

/* Iterator for "ap" bsscfgs:  (wlc_info_t *wlc, int idx, wlc_bsscfg_t *cfg) */
#define FOREACH_AP(wlc, idx, cfg) \
	for (idx = 0; cfg = NULL, (int) idx < WLC_MAXBSSCFG; idx++) \
		if ((wlc)->bsscfg[idx] != NULL && BSSCFG_AP((wlc)->bsscfg[idx]) && \
		    (cfg = (wlc)->bsscfg[idx]) != NULL)

/* Iterator for "up" AP bsscfgs:  (wlc_info_t *wlc, int idx, wlc_bsscfg_t *cfg) */
#define FOREACH_UP_AP(wlc, idx, cfg) \
	for (idx = 0; cfg = NULL, (int) idx < WLC_MAXBSSCFG; idx++) \
		if ((wlc)->bsscfg[idx] != NULL && BSSCFG_AP((wlc)->bsscfg[idx]) && \
		    (wlc)->bsscfg[idx]->up && \
		    (cfg = (wlc)->bsscfg[idx]) != NULL)

/* Iterator for "sta" bsscfgs:	(wlc_info_t *wlc, int idx, wlc_bsscfg_t *cfg) */
/* Iterator for STA bss configs:  (wlc_info_t *wlc, int idx, wlc_bsscfg_t *cfg) */
#define FOREACH_STA(wlc, idx, cfg) \
	for (idx = 0; (int) idx < WLC_MAXBSSCFG; idx++) \
		if ((cfg = (wlc)->bsscfg[idx]) && BSSCFG_STA(cfg))

/* Iterator for "associated" STA bss configs:  (wlc_info_t *wlc, int idx, wlc_bsscfg_t *cfg) */
#define FOREACH_AS_STA(wlc, idx, cfg) \
	for (idx = 0; cfg = NULL, (int) idx < WLC_MAXBSSCFG; idx++) \
		if ((wlc)->bsscfg[idx] != NULL && BSSCFG_STA((wlc)->bsscfg[idx]) && \
		    (wlc)->bsscfg[idx]->associated && \
		    (cfg = (wlc)->bsscfg[idx]) != NULL)

/* As above for all up PSTA BSS configs */
#define FOREACH_UP_PSTA(wlc, idx, cfg) \
	for (idx = 0; (int) idx < WLC_MAXBSSCFG; idx++) \
		if ((cfg = (wlc)->bsscfg[idx]) && BSSCFG_PSTA(cfg) && cfg->up)

/* As above for all PSTA BSS configs */
#define FOREACH_PSTA(wlc, idx, cfg) \
	for (idx = 0; (int) idx < WLC_MAXBSSCFG; idx++) \
		if ((cfg = (wlc)->bsscfg[idx]) && BSSCFG_PSTA(cfg))

#define WLC_IS_CURRENT_BSSID(cfg, bssid) \
	(!bcmp((char*)(bssid), (char*)&((cfg)->BSSID), ETHER_ADDR_LEN))
#define WLC_IS_CURRENT_SSID(cfg, ssid, len) \
	(((len) == (cfg)->SSID_len) && \
	 !bcmp((char*)(ssid), (char*)&((cfg)->SSID), (len)))

#ifdef DWDS
/* DWDS STA loopback list */
#define DWDS_SA_HASH_SZ		(32)
#define DWDS_SA_EXPIRE_TIME	(10)		/* 10 secs */
#define DWDS_SA_HASH(ea)	((ea[5] + ea[4] + ea[3] + ea[2] + ea[1]) & (DWDS_SA_HASH_SZ - 1))

typedef struct dwds_sa {
	struct ether_addr sa;
	uint32 last_used;
	struct dwds_sa *next;
} dwds_sa_t;
#endif /* DWDS */

#define AP_BSS_UP_COUNT(wlc) wlc_ap_bss_up_count(wlc)

#define PSTA_BSS_COUNT(wlc) wlc_psta_bss_count(wlc)
/*
 * Software packet template (spt) structure; for beacons and (maybe) probe responses.
 */
#define WLC_SPT_COUNT_MAX 2

/* Turn on define to get stats on SPT */
/* #define WLC_SPT_DEBUG */

typedef struct wlc_spt
{
	uint32		in_use_bitmap;	/* Bitmap of entries in use by DMA */
	wlc_pkt_t	pkts[WLC_SPT_COUNT_MAX];	/* Pointer to array of pkts */
	int		latest_idx;	/* Most recently updated entry */
	int		dtim_count_offset; /* Where in latest_idx is dtim_count_offset */
	uint8		*tim_ie;	/* Pointer to start of TIM IE in current packet */
	bool		bcn_modified;	/* Ucode versions, TRUE: push out to shmem */

#if defined(WLC_SPT_DEBUG)
	uint32		tx_count;	/* Debug: Number of times tx'd */
	uint32		suppressed;	/* Debug: Number of times supressed */
#endif /* WLC_SPT_DEBUG */
} wlc_spt_t;

/* In the case of 2 templates, return index of one not in use; -1 if both in use */
#define SPT_PAIR_AVAIL(spt) \
	(((spt)->in_use_bitmap == 0) || ((spt)->in_use_bitmap == 0x2) ? 0 : \
	((spt)->in_use_bitmap == 0x1) ? 1 : -1)

/* Is the given pkt index in use */
#define SPT_IN_USE(spt, idx) (((spt)->in_use_bitmap & (1 << (idx))) != 0)

#define SPT_LATEST_PKT(spt) ((spt)->pkts[(spt)->latest_idx])

/** MBSS debug counters */
typedef struct wlc_mbss_cnt {
	uint32		prq_directed_entries; /* Directed PRQ entries seen */
	uint32		prb_resp_alloc_fail;  /* Failed allocation on probe response */
	uint32		prb_resp_tx_fail;     /* Failed to TX probe response */
	uint32		prb_resp_retrx_fail;  /* Failed to TX probe response */
	uint32		prb_resp_retrx;       /* Retransmit suppressed probe resp */
	uint32		prb_resp_ttl_expy;    /* Probe responses suppressed due to TTL expry */
	uint32		bcn_tx_failed;	      /* Failed to TX beacon frame */

	uint32		mc_fifo_max;	/* Max number of BC/MC pkts in DMA queues */
	uint32		bcmc_count;	/* Total number of pkts sent thru BC/MC fifo */

	/* Timing and other measurements for PS transitions */
	uint32		ps_trans;	/* Total number of transitions started */
} wlc_mbss_cnt_t;

/* Set the DTIM count in the latest beacon */
#define BCN_TIM_DTIM_COUNT_OFFSET (TLV_BODY_OFF + DOT11_MNG_TIM_DTIM_COUNT) /* 2 */
#define BCN_TIM_BITMAPCTRL_OFFSET (TLV_BODY_OFF + DOT11_MNG_TIM_BITMAP_CTL) /* 4 */
#define BCN_TIM_DTIM_COUNT_SET(tim, value) (tim)[BCN_TIM_DTIM_COUNT_OFFSET] = (value)
#define BCN_TIM_BCMC_FLAG_SET(tim)         (tim)[BCN_TIM_BITMAPCTRL_OFFSET] |= 0x1
#define BCN_TIM_BCMC_FLAG_RESET(tim)       (tim)[BCN_TIM_BITMAPCTRL_OFFSET] &= 0xfe

/** association states */
typedef struct wlc_assoc {
	struct wl_timer *timer;		/* timer for auth/assoc request timeout */
	uint	type;			/* roam, association, or recreation */
	uint	state;			/* current state in assoc process */
	uint	flags;			/* control flags for assoc */

	bool	preserved;		/* Whether association was preserved */
	uint	recreate_bi_timeout;	/* bcn intervals to wait to detect our former AP
					 * when performing an assoc_recreate
					 */
	uint	verify_timeout;		/* ms to wait to allow an AP to respond to class 3
					 * data if it needs to disassoc/deauth
					 */
	uint8	retry_max;		/* max. retries */
	uint8	ess_retries;		/* number of ESS retries */
	uint8	bss_retries;		/* number of BSS retries */

	uint16	capability;		/* next (re)association request overrides */
	uint16	listen;
	struct ether_addr bssid;
	uint8	*ie;
	uint	ie_len;

	struct dot11_assoc_req *req;	/* last (re)association request */
	uint	req_len;
	bool	req_is_reassoc;		/* is a reassoc */
	struct dot11_assoc_resp *resp;	/* last (re)association response */
	uint	resp_len;

	bool	rt;			/* true if sta retry timer active */
#ifdef WLP2P
	uint8	auth_tx_retry;		/* authentication req tx retry number */
#endif /* WLP2P */
#ifdef ROBUST_DISASSOC_TX
	uint8	disassoc_tx_retry;		/* disassoc tx retry number */
#endif /* ROBUST_DISASSOC_TX */
} wlc_assoc_t;

/* roam states */
#define ROAM_CACHELIST_SIZE 		4
#define MAX_ROAM_CHANNEL		20
#define WLC_SRS_DEF_ROAM_CHAN_LIMIT	6	/* No. of cached channels for Split Roam Scan */

typedef struct wlc_roam {
	uint	bcn_timeout;		/* seconds w/o bcns until loss of connection */
	bool	assocroam;		/* roam to preferred assoc band in oid bcast scan */
	bool	off;			/* disable roaming */
	uint8	time_since_bcn;		/* second count since our last beacon from AP */
#ifdef WL_BCN_COALESCING
	bool	old_bcn;
#endif /* WL_BCN_COALESCING */
#ifdef BCMDBG
	uint8	tbtt_since_bcn;		/* tbtt count since our last beacon from AP */
#endif // endif
	uint8	minrate_txfail_cnt;	/* tx fail count at min rate */
	uint8	minrate_txpass_cnt;	/* number of consecutive frames at the min rate */
	uint	minrate_txfail_thresh;	/* min rate tx fail threshold for roam */
	uint	minrate_txpass_thresh;	/* roamscan threshold for being stuck at min rate */
	uint	txpass_cnt;		/* turn off roaming if we get a better tx rate */
	uint	txpass_thresh;		/* turn off roaming after x many packets */
	uint32	tsf_h;			/* TSF high bits (to detect retrograde TSF) */
	uint32	tsf_l;			/* TSF low bits (to detect retrograde TSF) */
	uint	scan_block;		/* roam scan frequency mitigator */
	uint	ratebased_roam_block;	/* limits mintxrate/txfail roaming frequency */
	uint	partialscan_period;	/* user-specified roam scan period */
	uint	fullscan_period; 	/* time between full roamscans */
	uint	reason;			/* cache so we can report w/WLC_E_ROAM event */
	uint	original_reason;	/* record the reason for precise reporting on link down */
	bool	active;			/* RSSI based roam in progress */
	bool	cache_valid;		/* RSSI roam cache is valid */
	uint	time_since_upd;		/* How long since our update? */
	uint	fullscan_count;		/* Count of full rssiroams */
	int	prev_rssi;		/* Prior RSSI, used to invalidate cache */
	uint	cache_numentries;	/* # of rssiroam APs in cache */
	bool	thrash_block_active;	/* Some/all of our APs are unavaiable to reassoc */
	bool	motion;			/* We are currently moving */
	int	RSSIref;		/* trigger for motion detection */
	uint16	motion_dur;		/* How long have we been moving? */
	uint16	motion_timeout;		/* Time left using modifed values */
	uint8	motion_rssi_delta;	/* Motion detect RSSI delta */
	bool	roam_scan_piggyback;	/* Use periodic broadcast scans as roam scans */
	bool	piggyback_enab;		/* Turn on/off roam scan piggybacking */
	struct {			/* Roam cache info */
		chanspec_t chanspec;
		struct ether_addr BSSID;
		uint16 time_left_to_next_assoc;
	} cached_ap[ROAM_CACHELIST_SIZE];
	uint8	ap_environment;		/* Auto-detect the AP density of the environment */
	bool	bcns_lost;		/* indicate if the STA can see the AP */
	uint8	consec_roam_bcns_lost;	/* counter to keep track of consecutive calls
					 * to wlc_roam_bcns_lost function
					 */
	uint8	roam_on_wowl; /* trigger roam scan (on bcn loss) for prim cfg from wowl watchdog */
	uint8   nfullscans;     /* Number of full scans before switching to partial scans */

	/* WLRCC: Roam Channel Cache */
	bool    rcc_mode;
	bool    rcc_valid;
	bool	roam_scan_started;
	bool	roam_fullscan;
	uint16	n_rcc_channels;
	chanspec_t *rcc_channels;
	uint16  max_roam_time_thresh;   /* maximum roam threshold time in ms */
	/* Roam hot channel cache */
	chanvec_t roam_chn_cache;	/* split roam scan channel cache */
	bool	roam_chn_cache_locked;	/* indicates cache was cfg'd manually */
	uint8	roam_chn_cache_limit;	/* Max chan to use from cache for split roam scan */

	/* Split roam scan */
	int16	roam_trigger_aggr;	/* aggressive trigger values set by host for roaming */
	uint16	roam_delta_aggr;	/* aggressive roam mode - roam delta value */
	bool	multi_ap;		/* TRUE when there are more than one AP for the SSID */
	uint8	split_roam_phase;	/* tracks the phases of a split roam scan */
	bool	split_roam_scan;	/* flag for enabling split roam scans */
	uint8	ci_delta;		/* roam cache invalidate delta */
	uint	roam_type;		/* Type of roam (standard, partial, split phase) */
	struct	wl_timer *timer;	/* timer to start roam after uatbtt */
	bool	timer_active;		/* Is roam timer active? */

	wl_reassoc_params_t *reassoc_param;
	uint8	reassoc_param_len;
	int8	roam_rssi_boost_thresh;
	int16	roam_prof_idx;		/* current roaming profile (based upon band/RSSI) */
	uint	prev_bcn_timeout;
#ifdef OPPORTUNISTIC_ROAM
	bool	oppr_roam_off; /* disable opportunistic roam */
#endif // endif
	uint16  prev_scan_home_time;
	uint32  link_monitor_last_update;	/* last time roam scan counters decreased */
} wlc_roam_t;

/* connection states */
/* XXX TODO: move states in wlc_assoc_t & wlc_roam_t that are used to detect/monitor
 * beacon lost and/or shared by both BSS and IBSS to here so that later on we can
 * optimize the memory usage by not allocating wlc_roam_t if possible...
 */
typedef struct {
	mbool	ign_bcn_lost_det;	/* skip bcn lost detection */
} wlc_cxn_t;

/* ign_bcn_lost_det requestor */
#define BCN_MON_REQ_NAN	(1<<0)

#define RSSI_PKT_WIN_SZ_MASK		0x00ff
#define RSSI_ADMIT_ALL_FRAME(x)		(((x) & 0x0100) != 0)
#define RSSI_ADMIT_MCAST_ONLY(x)	(((x) & 0x0200) != 0)
#define RSSI_PKT_WIN_DEBUG(x)		(((x) & 0x8000) != 0)

typedef struct wlc_link_qual {
	/* RSSI moving average */
	int	*rssi_pkt_window;	/* RSSI moving average window */
	int	rssi_pkt_index;		/* RSSI moving average window index */
	int	rssi_pkt_tot;		/* RSSI moving average total */
	uint16  rssi_pkt_count;		/* RSSI moving average count (maximum MA_WINDOW_SZ) */
	uint16  rssi_pkt_win_sz;	/* RSSI moving average window size (maximum MA_WINDOW_SZ) */
	int	rssi;			/* RSSI moving average */

	/* SNR moving average */
	int	snr;			/* SNR moving average */
	int32 	snr_ma;			/* SNR moving average total */
	uint8	snr_window[MA_WINDOW_SZ];	/* SNR moving average window */
	int	snr_index;		/* SNR moving average window index */
	int8 	snr_no_of_valid_values;	/* number of valid values in the window */

	/* RSSI event notification */
	wl_rssi_event_t *rssi_event;	/* RSSI event notification configuration. */
	uint8	rssi_level;		/* current rssi based on notification configuration */
	struct wl_timer *rssi_event_timer;	/* timer to limit event notifications */
	bool	is_rssi_event_timer_active;	/* flag to indicate timer active */

	bool	last_rssi_is_unicast;	/* last RSSI sample is from unicast frame */

	uint16  snr_pkt_win_sz;		/* SNR moving average window size (maximum MA_WINDOW_SZ) */
	bool	last_snr_is_unicast;	/* last SNR sample is from unicast frame */

	int	rssi_qdb;		/* RSSI qdb portion moving average */

	/* RSSI/SNR auto-deduction upon consecutive beacon loss */
	uint8	rssi_delta;		/* Reduce RSSI by this much upon consecutive beacon loss */
	uint8	snr_delta;		/* Reduce SNR by this much upon consecutive beacon loss */
	uint32	max_lq_bcn_loss;	/* beacon loss threshold in us */
	int32	lq_last_bcn_time;	/* local time for last beacon received */
	int8	lq_rssi_penalty_level;	/* RSSI/SNR penalty level */
} wlc_link_qual_t;

/** PM=2 receive duration duty cycle states */
typedef enum _pm2rd_state_t {
	PM2RD_IDLE,		/* In an idle DTIM period with no data to receive and
				 * no receive duty cycle active.  ie. the last
				 * received beacon had a cleared TIM bit.
				 */
	PM2RD_WAIT_BCN,		/* In the OFF part of the receive duty cycle.
				 * In PS mode, waiting for next beacon.
				 */
	PM2RD_WAIT_TMO,		/* In the ON part of the receive duty cycle.
				 * Out of PS mode, waiting for pm2_rcv timeout.
				 */
	PM2RD_WAIT_RTS_ACK	/* Transitioning from the ON part to the OFF part of
				 * the receive duty cycle.
				 * Started entering PS mode, waiting for a
				 * PM-indicated ACK from AP to complete entering
				 * PS mode.
				 */
} pm2rd_state_t;

/* defines for ps mode requestor module id */
#define WLC_BSSCFG_PS_REQ_MCHAN		(0x00000001)

/* Maximum number of immediate PM1 Null tx retries if no ack received */
#define PM_IMMED_RETRIES_MAX 2		/* 2 means 3 total tx attempts with 3 seq#'s */

/* Mode of operation for PM1 Null resends */
#define PS_RESEND_MODE_WDOG_ONLY	0	/* Legacy behavior: Resend at watchdog only */
#define PS_RESEND_MODE_BCN_NO_SLEEP	1	/* Resend immed 2x, then at bcn rx and at wdog.
						 * Do not sleep while waiting for the next beacon.
						 */
#define PS_RESEND_MODE_BCN_SLEEP	2	/* Resend immed 2x, then at bcn rx and at wdog.
						 * Sleep while waiting for the next beacon.
						 */
#define PS_RESEND_MODE_MAX		3

/** power management */
typedef struct wlc_pm_st {
	/* states */
	uint8	PM;			/* power-management mode (CAM, PS or FASTPS) */
	bool	PM_override;		/* no power-save flag, override PM(user input) */
	uint8	ps_resend_mode;		/* Mode of operation for PM2 PM Null resends */
	mbool	PMenabledModuleId;	/* module id that enabled pm mode */
	bool	PMenabled;		/* current power-management state (CAM or PS) */
	bool	PMawakebcn;		/* bcn recvd during current waking state */
	bool	PMpending;		/* waiting for tx status with PM indicated set */
	bool	priorPMstate;		/* Detecting PM state transitions */
	bool	PSpoll;			/* whether there is an outstanding PS-Poll frame */
	bool	check_for_unaligned_tbtt;	/* check unaligned tbtt */

	/* periodic polling */
	uint16	pspoll_prd;		/* pspoll interval in milliseconds, 0 == disable */
	struct wl_timer *pspoll_timer;	/* periodic pspoll timer */
	uint16	apsd_trigger_timeout;	/* timeout value for apsd_trigger_timer (in ms)
					 * 0 == disable
					 */
	struct wl_timer *apsd_trigger_timer;	/* timer for wme apsd trigger frames */
	bool	apsd_sta_usp;		/* Unscheduled Service Period in progress on STA */
	bool	WME_PM_blocked;		/* Can STA go to PM when in WME Auto mode */

	/* PM2 Receive Throttle Duty Cycle */
	uint16	pm2_rcv_percent;	/* Duty cycle ON % in each bcn interval */
	pm2rd_state_t pm2_rcv_state;	/* Duty cycle state */
	uint16	pm2_rcv_time;		/* Duty cycle ON time in ms */

	/* PM2 Return to Sleep */
	uint	pm2_sleep_ret_time;	/* configured time to return to sleep in ms */
	uint	pm2_sleep_ret_time_left; /* return to sleep countdown timer in ms */
	uint	pm2_last_wake_time;	/* last tx/rx activity tim in gptimer ticks(uSec) */
	bool	pm2_refresh_badiv;	/* PM2 timeout refresh with bad iv frames */

#ifdef ADV_PS_POLL
	/* send pspoll after TX */
	bool	adv_ps_poll;		/* enable/disable 'send_pspoll_after_tx' */
	bool	send_pspoll_after_tx;   /* send pspoll frame after last TX frame, to check
					 * any buffered frames in AP, during PM = 1,
					 * (or send ps poll in advance after last tx)
					 */
#endif // endif
	wlc_hrt_to_t *pm2_rcv_timer; /* recv duration timeout object used with
					 * multiplexed hw timers
					 */
	wlc_hrt_to_t *pm2_ret_timer; /* return to sleep timeout object used with
					 * multiplexed hw timers
					 */
	bool	pm2_ps0_allowed;	/* allow going to PS off state even when PMpending */
	mbool	PMblocked;		/* block any PSPolling in PS mode, used to buffer
					 * AP traffic, also used to indicate in progress
					 * of scan, rm, etc. off home channel activity.
					 */

	/* Dynamic Fast Return To Sleep settings */
	uint8 dfrts_logic;			/* Dynamic FRTS algorithm */
	uint16 dfrts_high_ms;			/* High FRTS timeout */
	uint16 dfrts_rx_pkts_threshold;		/* switching threshold # rx pkts */
	uint16 dfrts_tx_pkts_threshold;		/* switching threshold # tx pkts */
	uint16 dfrts_txrx_pkts_threshold;	/* switching threshold # tx+rx pkts */
	uint32 dfrts_rx_bytes_threshold;	/* switching threshold # rx bytes */
	uint32 dfrts_tx_bytes_threshold;	/* switching threshold # tx bytes */
	uint32 dfrts_txrx_bytes_threshold;	/* switching threshold # tx+rx bytes */

	/* Dynamic Fast Return To Sleep counters */
	uint16	dfrts_rx_pkts;			/* PM2 rx bytes count since wake */
	uint16	dfrts_tx_pkts;			/* PM2 tx pkts count since wake */
	uint32	dfrts_rx_bytes;			/* PM2 rx bytes count since wake */
	uint32	dfrts_tx_bytes;			/* PM2 tx bytes count since wake */

	/* Firmware level (not ucode) PM1 Null tx retries with new seq#s */
	uint8	pm_immed_retries;		/* # times PM1 Null immediately resent */
	uint8	sleep_during_bcn_ps_retry;	/* Unused.  Keep for ROM compatibility.
						 * Cannot rename this because it was the
						 * last struct member in some ROMs.
						 */

	int8	pm2_md_sleep_ext; /* In PM2&PM1 mode with more data indication on PSpoll Response
				   * wake up from PM2 sleep with a regular PM2 Beacon FRTS timer
				   */
	uint8	pspoll_md_cnt;
	uint8   pspoll_md_counter;

	/* Fast Return to sleep enhancements */
	uint16	pm2_bcn_sleep_ret_time;	/* PM2 return to sleep after bcn */
	uint8	pm2_rx_pkts_since_bcn;	/* PM2 rx pkts count since bcn */
	bool	dfrts_reached_threshold;	/* The current PM cycle has reached the
						 * DFRTS pkts/bytes threshold for using
						 * the high FRTS timeout.
						 */
	uint8	forcePMenabled;		/* indicate whether PMenabled was changed before going
					 * off-channel. It could be used to restore back to
					 * the previous PS state as coming back to on-channel
					 * after finishing off-channel operation.
					 * forcePMenabled is set when PMenabled is changed from
					 * FALSE to TRUE. Once this has happened for the bsscfg,
					 * no more this condition could happen for the same bsscfg.
					 * So, the multiple bits cannot be set.
					 * For this reason, uint8 is used instead of mbool or
					 * bitmap type variable to avoid misleading.
					 */
} wlc_pm_st_t;

/* join targets sorting preference */
#define MAXJOINPREFS		5	/* max # user-supplied join prefs */
#define MAXWPACFGS		16	/* max # wpa configs */
typedef struct {
	struct {
		uint8 type;		/* type */
		uint8 start;		/* offset */
		uint8 bits;		/* width */
		uint8 reserved;
	} field[MAXJOINPREFS];		/* preference field, least significant first */
	uint fields;			/* # preference fields */
	uint prfbmp;			/* preference types bitmap */
	struct {
		uint8 akm[WPA_SUITE_LEN];	/* akm oui */
		uint8 ucipher[WPA_SUITE_LEN];	/* unicast cipher oui */
		uint8 mcipher[WPA_SUITE_LEN];	/* multicast cipher oui */
	} wpa[MAXWPACFGS];		/* wpa configs, least favorable first */
	uint wpas;			/* # wpa configs */
	uint8 band;			/* 802.11 band pref */
} wlc_join_pref_t;

typedef struct wlc_smfs_elem {
	struct wlc_smfs_elem *next;
	wl_smfs_elem_t smfs_elem;
} wlc_smfs_elem_t;

typedef struct wlc_smf_stats {
	wl_smf_stats_t smfs_main;
	uint32 count_excl; /* counts for those sc/rc code excluded from the interested group */
	wlc_smfs_elem_t *stats;
} wlc_smf_stats_t;

typedef struct wlc_smfs_info {
	uint32 enable;
	wlc_smf_stats_t smf_stats[SMFS_TYPE_MAX];
} wlc_smfs_info_t;

#ifdef MBSS
/**
 * probe response element record
 * This is designed to capture certain IE's in the probe response that could be removed
 * based on the incoming probe request. For now, the need is just for ht ie's. However, more
 * elements can be added in the same fashion.
 */
typedef struct elmt_info {
	bool present;	/* is present */
	uint8 offset;	/* offset from the start of frame body */
	uint8 length;	/* the size of this element */
} elmt_info_t;

#define PRB_HTIE(cfg)	(cfg)->prb_ieinfo.ht_ie

/** please follow the order in which the ies are added in the prb resp */
typedef struct prb_ie_info {
	elmt_info_t ht_ie;
} prb_ie_info_t;
#endif /* MBSS */

#ifdef WLDPT
typedef struct wlc_dpt {
	/* variables in parent */
	uint8	ls_pending;		/* num of peers from which waiting for LS ack */
	uint8	po_pending;		/* num of peers waiting for port to be opened */
	/* variables configured in parent and applied to DPT */
	uint16	WPA_auth;		/* DPT link auth mode: WPA2-PSK or none */
	uint32	wsec;			/* DPT link encryption: AES or none */
	wsec_pmk_t pmk;			/* pmk material/passphrase */
	uint8	fnlen;			/* length of friendly name */
	uint8	fname[DPT_FNAME_LEN+1];	/* DPT friendly name */
} wlc_dpt_t;
#endif /* WLDPT */

#ifdef WLTDLS
typedef struct wlc_tdls {
	/* variables in parent */
	bool		ps_pending;		/* num of peers waiting for port to be opened */
	bool		ps_allowed;		/* can a U-APSD buffer STA be a sleep STA as well */
	bool		tdls_PMEnable;
	bool		tdls_PMAwake;
	/* variables configured in parent and applied to TDLS */
	uint16	WPA_auth;		/* TDLS link auth mode: WPA2-PSK or none */
	uint32	wsec;			/* TDKS link encryption: AES or none */
	//wsec_pmk_t pmk;			/* pmk material/passphrase */
	bool		initiator;
	bool		chsw_req_enabled;	/* enable to initiate Channel Switch Req */
	uint8		tdls_cap;
	uint32		up_time;
	uint32		tpk_lifetime;
	uint16		resp_timeout;	/* in sec the STA waits before timing out TDLS setup req */
	uint32		apsd_sta_settime;
#ifdef WL11N
	uint16		ht_capinfo;
	uint8		rclen;			/* regulatory class length */
	uint8		rclist[32];	/* regulatory class list */
#endif /* WL11N */
	uint8		*sup_ch_ie;
	int			sup_ch_ie_len;
} wlc_tdls_t;
#endif /* WLTDLS */

typedef struct wlc_wme {
	/* IE */
	wme_param_ie_t	wme_param_ie;		/* WME parameter info element,
						 * contains parameters in use locally.
						 */
	wme_param_ie_t	*wme_param_ie_ad;	/* WME parameter info element,
						 * contains parameters advertised to STA
						 * in beacons and assoc responses.
						 */
	/* EDCF */
	uint16		edcf_txop[AC_COUNT];	/* current txop for each ac */
	/* APSD */
	ac_bitmap_t	apsd_trigger_ac;	/* Permissible Acess Category in which APSD Null
						 * Trigger frames can be send
						 */
	bool		apsd_auto_trigger;	/* Enable/Disable APSD frame after indication in
						 * Beacon's PVB
						 */
	uint8		apsd_sta_qosinfo;	/* Application-requested APSD configuration */
	/* WME */
	bool		wme_apsd;		/* enable Advanced Power Save Delivery */
	bool		wme_noack;		/* enable WME no-acknowledge mode */
	ac_bitmap_t	wme_admctl;		/* bit i set if AC i under admission control */
} wlc_wme_t;

#define BSSCFG_BUF_KEY_NUM	2
#define BSSCFG_BUF_KEY_PAIR_ID	0
#define BSSCFG_BUF_KEY_GRP_ID	1

#ifdef STA
typedef struct wsec_key_buf_info {
	bool eapol_4way_m1_rxed;
	bool eapol_4way_m4_txed;
	bool key_buffered[BSSCFG_BUF_KEY_NUM];
	wsec_iv_t buf_key_iv[BSSCFG_BUF_KEY_NUM];
	wl_wsec_key_t buf_key[BSSCFG_BUF_KEY_NUM];
} wsec_key_buf_info_t;
#endif /* STA */

#ifdef STA
#define BSSCFG_WSEC_BUF_KEY_B4_M4(cfg)	((cfg) && (cfg)->wsec_buf_key_b4_m4)
#else
#define BSSCFG_WSEC_BUF_KEY_B4_M4(cfg)	0
#endif // endif

typedef struct wlc_obss_info {
	bool		coex_enab;		/* 20/40 MHz BSS Management enabled */
	bool		coex_permit;		/* 20/40 operation permitted */
	uint8		coex_det;		/* 40 MHz Intolerant device detected */
	uint8		coex_ovrd;		/* 40 MHz Intolerant device detected */
	bool		switch_bw_deferred;	/* defer the switch */
	uint8		coex_bits_buffered;	/* buffer coexistence bits due to scan */
	uint16		scan_countdown;		/* timer for schedule next OBSS scan */
	uint32		fid_time;		/* time when 40MHz intolerant device detected */
	uint32		coex_te_mask;		/* mask for trigger events */
	obss_params_t	params;			/* Overlapping BSS scan parameters */
} wlc_obss_info_t;

typedef struct wlc_bssload {
	wl_bssload_t	load;		/* Current BSS Load from associated AP's beacon */

	/* BSS Load event notification */
	wl_bssload_cfg_t event_cfg;	/* BSS Load event notification configuration. */
	uint32	event_tstamp;		/* last sent event's timestamp in ms */
	uint8	event_util_level;	/* last sent event's utilization level */
} wlc_bssload_t;

/**
 * Stores information about the relation between the local entity and a BSS. Information about the
 * BSS that is not specific for this local entity is stored in struct wlc_bss_info instead.
 */
struct wlc_bsscfg {
	wlc_info_t	*wlc;		/* wlc to which this bsscfg belongs to. */
	bool		up;		/* is this configuration up operational */
	bool		enable;		/* is this configuration enabled */
	bool		_ap;		/* is this configuration an AP */
	bool		_psta;		/* is this configuration a PSTA */
	bool		associated;	/* is BSS in ASSOCIATED state */
	struct wlc_if	*wlcif;		/* virtual interface, NULL for primary bsscfg */
	bool		BSS;		/* infrastructure or adhoc */
	bool		dtim_programmed;
	void		*authenticator;	/* pointer to authenticator state */
	uint8		SSID_len;	/* the length of SSID */
	uint8		SSID[DOT11_MAX_SSID_LEN];	/* SSID string */
	bool		closednet_nobcnssid;	/* hide ssid info in beacon */
	bool		closednet_nobcprbresp;	/* Don't respond to broadcast probe requests */
	uint8		ap_isolate;	/* ap isolate bitmap 0: disabled, 1: all 2: mulicast */
	struct scb *bcmc_scb[MAXBANDS]; /* one bcmc_scb per band */
	int8		_idx;		/* the index of this bsscfg,
					 * assigned at wlc_bsscfg_alloc()
					 */
	/* Multicast filter list */
	bool		allmulti;		/* enable all multicasts */
	uint		nmulticast;		/* # enabled multicast addresses */
	struct ether_addr	*multicast; 	/* ptr to list of multicast addresses */

	/* security */
	uint32		wsec;		/* wireless security bitvec */
	int16		auth;		/* 802.11 authentication: Open, Shared Key, WPA */
	int16		openshared;	/* try Open auth first, then Shared Key */
	bool		wsec_restrict;	/* drop unencrypted packets if wsec is enabled */
	bool		eap_restrict;	/* restrict data until 802.1X auth succeeds */
	uint16		WPA_auth;	/* WPA: authenticated key management */
	bool		wsec_portopen;	/* indicates keys are plumbed */
	wsec_iv_t	wpa_none_txiv;	/* global txiv for WPA_NONE, tkip and aes */
	int		wsec_index;	/* 0-3: default tx key, -1: not set */
	wsec_key_t	*bss_def_keys[WLC_DEFAULT_KEYS];	/* default key storage */
	bool		is_WPS_enrollee; /* To allow a join to an encrypted AP while wsec is 0
					  * for the purposes of geting WPS credentials
					  */
#if defined(MBSS) || defined(WLP2P)
	uint32		maxassoc;	/* Max associations for this bss */
#endif /* MBSS || WLP2P */
#ifdef MBSS
	wlc_pkt_t	probe_template;	/* Probe response master packet, including PLCP */
#ifdef WLLPRS
	wlc_pkt_t	lprs_template;	/* Legacy probe response master packet */
	prb_ie_info_t	prb_ieinfo;	/* information of certain ies of interest */
#endif /* WLLPRS */
	bool		prb_modified;	/* Ucode version: push to shm if true */
	wlc_spt_t	*bcn_template;	/* Beacon DMA template */
	int8		_ucidx;		/* the uCode index of this bsscfg,
					 * assigned at wlc_bsscfg_up()
					 */
	uint32		mc_fifo_pkts;	/* Current number of BC/MC pkts sent to DMA queues */
	uint32		prb_ttl_us;     /* Probe rsp time to live since req. If 0, disabled */
#ifdef WLCNT
	wlc_mbss_cnt_t *cnt;		/* MBSS debug counters */
#endif // endif
#if defined(BCMDBG_MBSS_PROFILE)
	uint32		ps_start_us;	/* When last PS (off) transition started */
	uint32		max_ps_off_us;	/* Max delay time for out-of-PS transitions */
	uint32		tot_ps_off_us;	/* Total time delay for out-of-PS transitions */
	uint32		ps_off_count;	/* Number of deferred out-of-PS transitions completed */
	bool		bcn_tx_done;	/* TX done on sw beacon */
#endif /* BCMDBG_MBSS_PROFILE */
#endif /* MBSS */

	/* TKIP countermeasures */
	bool		tkip_countermeasures;	/* flags TKIP no-assoc period */
	uint32		tk_cm_dt;	/* detect timer */
	uint32		tk_cm_bt;	/* blocking timer */
	uint32		tk_cm_bt_tmstmp;    /* Timestamp when TKIP BT is activated */
	bool		tk_cm_activate;	/* activate countermeasures after EAPOL-Key sent */
#ifdef AP
	uint8		aidmap[AIDMAPSZ];	/* aid map */
#endif // endif
	vndr_ie_listel_t	*vndr_ie_listp;	/* dynamic list of Vendor IEs */
	struct ether_addr	BSSID;		/* BSSID (associated) */
	struct ether_addr	cur_etheraddr;	/* h/w address */
	uint16                  bcmc_fid;	/* the last BCMC FID queued to TX_BCMC_FIFO */
	uint16                  bcmc_fid_shm;	/* the last BCMC FID written to shared mem */

	uint32		flags;		/* WLC_BSSCFG flags; see below */
#ifdef STA
	/* Association parameters. Used to limit the scan in join process. Saved before
	 * starting a join process and freed after finishing the join process regardless
	 * if the join is succeeded or failed.
	 */
	wl_join_assoc_params_t	*assoc_params;
	uint16			assoc_params_len;
	uint16			AID;		/* association id for each bss */
#endif // endif
	struct ether_addr	prev_BSSID;	/* MAC addr of last associated AP (BSS) */

#ifdef EXT_STA
	uint	packet_filter;		/* packet filter */
	exempt_list_t	*exempt_list;	/* exempt list */
	bool	exempt_unencrypt_m4;
#endif /* EXT_STA */
#ifdef NLO
	bool		nlo;		/* enable nlo */
#endif /* NLO */

#if defined(EXT_STA) || defined(MACOSX)
	bool	sendup_mgmt;		/* sendup mgmt per packet filter setting */
#endif // endif

	bool		ar_disassoc;	/* disassociated in associated recreation */

	int		auth_atmptd;	/* auth type (open/shared) attempted */

	wlc_bss_info_t	*target_bss;	/* BSS parms during tran. to ASSOCIATED state */
	wlc_bss_info_t	*current_bss;	/* BSS parms in ASSOCIATED state */

	wlc_assoc_t	*assoc;		/* association mangement */
	wlc_roam_t	*roam;		/* roam states */
	wlc_link_qual_t	*link;		/* link quality monitor */
	wlc_pm_st_t	*pm;		/* power management */

	/* join targets sorting preference */
	wlc_join_pref_t *join_pref;
	/* Give RSSI score of APs in preferred band a boost
	 * to make them fare better instead of always preferring
	 * the band. This is maintained separately from regular
	 * join pref as no order can be imposed on this preference
	 */
	struct {
		uint8 band;
		int8 rssi;
	} join_pref_rssi_delta;
	struct ether_addr join_bssid;

	/* BSSID entry in RCMTA, use the wsec key management infrastructure to
	 * manage the RCMTA entries.
	 */
	wsec_key_t	*rcmta;

	int8	PLCPHdr_override;	/* 802.11b Preamble Type override */

	/* 'unique' ID of this bsscfg, assigned at bsscfg allocation */
	uint16		ID;

	uint		txrspecidx;		/* index into tx rate circular buffer */
	ratespec_t     	txrspec[NTXRATE][2];	/* circular buffer of prev MPDUs tx rates */

#ifdef WL_BSSCFG_TX_SUPR
	/* tx suppression handling */
	struct pktq	*psq;			/* defer queue */
	bool		tx_start_pending;	/* defer on/off */
#endif // endif

#ifdef EXT_STA
	bool safeModeEnabled;
#endif /* EXT_STA */

#ifdef WLMCHAN
	wlc_mchan_context_t *chan_context;	/* chanspec context for bsscfg */
	uint16 sw_dtim_cnt;			/* dtim cnt kept upto date by sw */
	chanspec_t chanspec;			/* chanspec specified when starting AP */
	uint8 mchan_tbtt_since_bcn;		/* number of tbtt since last bcn */
	uint last_psc_intr_time;		/* last time we got a psc intr,
						 * same units as pub->now.
						 */
	bool in_psc;				/* currently in psc period */
	bool in_abs;				/* currently in abs period */
#endif /* WLMCHAN */

#ifdef STA
	/* Scan parameters. Used to modify the scan parameters in join process.
	 * Saved before starting a join process and freed after finishing the join
	 * regardless if the join is succeeded or failed.
	 */
	wl_join_scan_params_t	*scan_params;
#endif // endif

#ifdef WL11N
	/* SM PS */
	uint8	mimops_PM;
	uint8	mimops_ActionPM;
	uint8  	mimops_ActionRetry;
	bool    mimops_ActionPending;
#endif /* WL11N */

	/* WME */
	wlc_wme_t	*wme;			/* WME config and ACP used locally */

#ifdef WLDPT
	/* Direct Packet Transfer */
	wlc_dpt_t	*dpt;
#endif // endif

#ifdef WLTDLS
	/* Tunnel Direct Link Setup */
	wlc_tdls_t	*tdls;
	struct scb	*tdls_scb;
#endif /* WLTDLS */

#ifdef STA
	bool wsec_buf_key_b4_m4;
	wsec_key_buf_info_t *wsec_key_buf_info;
#endif // endif

	/* Broadcom proprietary information element */
	uint8		*brcm_ie;

	/* OBSS Coexistance info */
	wlc_obss_info_t	*obss;

	uint8		ext_cap_len; /* extend capability length */
	uint8		ext_cap[DOT11_EXTCAP_LEN_MAX]; /* extend capability */

	uint8		oper_mode; /* operating mode notification value */
	bool		oper_mode_enabled; /* operating mode is enabled */

	wlc_cxn_t *cxn;

	wl_join_scan_params_t *roam_scan_params; /* customize roam scans */

	bool _dwds;	/* Dynamic WDS */

	uint8		bcn_to_dly;		/* delay link down until roam complete */
	uint8		bcn_to_dly_flag;	/* flag to indicate pending link down */

	int8		saved_psta_mode;	/* Save STA mode if it is overridden by dwds */
	bool		wmf_ucast_igmp_query;	/* 1 to enable, 0 by default */
	bool		wmf_ucast_upnp;		/* 1 to enable, 0 by default */
	bool		mcast_regen_enable;	/* Multicast Regeneration is enabled or not */
	wlc_smfs_info_t *smfs_info;		/* SMF_STATS */
	wlc_trf_mgmt_info_t *trf_mgmt_info;	/* pointer to traffic management info block */
	bool		wmf_enable;		/* WMF is enabled or not */
	bool		wmf_ucast_igmp;		/* 1 to enable, 0 by default */
	bool		wmf_psta_disable;	/* shall unicats to psta (default) or not */
	struct wlc_wmf_instance	*wmf_instance;	/* WMF instance handle */

	bool block_disassoc_tx;
	bool		is_awdl;
	int8 rateset;				/* bss rateset */

	/* ps_pretend_retry_limit is for normal ps pretend, where ps pretend is activated
	 * on the very first failure so preventing all packet loss. When zero, this feature
	 * is completely inoperative. The ps_pretend_retry_limit sets the maximum number
	 * of successive attempts to enter ps pretend when a packet is having difficulty
	 * to be delivered.
	 */
	uint8   ps_pretend_retry_limit;

	wlc_bssload_t	*bssload;	/* BSS load reporting */

	/* LEAVE THESE AT THE END */
#ifdef BCMDBG
	/* Rapid PM transition */
	wlc_hrt_to_t *rpmt_timer;
	uint32	rpmt_1_prd;
	uint32	rpmt_0_prd;
	uint8	rpmt_n_st;
#endif // endif
	/* LEAVE THESE AT THE END */
	uint32 flags2;				/* bsscfg flags */

#if defined(BRCMAPIVTW) && !defined(BCM_APIVTW_ROM_COMPAT)
	bool	brcm_ap_iv_tw;
#endif /* BRCMAPIVTW */

	/* ps_pretend_threshold sets the number of successive failed TX status that
	 * must occur in order to enter the 'threshold' ps pretend. When zero, the 'threshold'
	 * ps pretend is completely inoperative.
	 * This threshold means packets are always lost, they begin to be saved
	 * once the threshold has been met.
	 */
	uint8	ps_pretend_threshold;

	bool osen;
	uint8 roamoffl;	/* roaming offload */
#if defined(BRCMAPIVTW) && defined(BCM_APIVTW_ROM_COMPAT)
	bool	brcm_ap_iv_tw;
#endif /* BRCMAPIVTW */
	roamoffl_bssid_list_t *roamoffl_bssid_list; /* AP list in the same IP group */
	struct ether_addr last_upd_bssid;			/* last BSSID updated to the host */
	wsec_pmk_t pmk;
	bool roamed;
#ifdef ACKSUPR_MAC_FILTER
	bool acksupr_mac_filter; /* enable or disable acksupr */
#endif /* ACKSUPR_MAC_FILTER */
	struct wl_timer *pilot_timer;     /* Measurement Piolt timer */
	uint8 mp_period;
#ifndef WLFBT_DISABLED
	uint16			fbt_mdid; /* Mobility Domain Identifier */
	char			fbt_r0kh_id[FBT_R0KH_ID_LEN]; /* Config R0 Key holder Identifier */
	uint			fbt_r0kh_id_len; /* R0 Key holder ID Length */
	uint8			fbt_r1kh_id[ETHER_ADDR_LEN];	/* Configured R1 Key holder ID */
	uint			fbt_reassoc_time;	/* reassoc deadline timer value in TU */
	uint8			fbt_over_ds;		/* FBT over DS support */
#endif /* WLFBT_DISABLED */
#ifdef WL_GLOBAL_RCLASS
	uint8			scb_without_gbl_rclass;	/* number of associated sta support global
							 * regulatory class
							 */
#endif /* WL_GLOBAL_RCLASS */
#ifdef MULTIAP
	uint8			*multiap_ie;	/* Multi-AP ie */
	bool			_map;		/* Multi-AP */
	uint8			map_attr;	/* Multi-AP attribute values */
#endif	/* MULTIAP */
#ifdef DWDS
	bool		dwds_loopback_filter;
	dwds_sa_t	*sa_list[DWDS_SA_HASH_SZ]; /* list of clients attached to the DWDS STA */
#endif /* DWDS */
};

#ifdef NO_ROAMOFFL_SUPPORT
#define ROAMOFFL_ENAB(cfg)			(0)
#define BSSCFG_ROAMOFFL(cfg, addr)	(0)
#else
#define ROAMOFFL_ENAB(cfg)			((cfg)->roamoffl)
#define BSSCFG_ROAMOFFL(cfg, addr)	(wlc_bsscfg_chk_roamoffl_bssid(cfg, addr))
#endif // endif

/* wlc_bsscfg_t flags */
#define WLC_BSSCFG_PSINFO	0x0		/* For trunk NDIS build. Not used in BISON05T */
#define WLC_BSSCFG_PRESERVE     0x1		/* preserve STA association on driver down/up */
#define WLC_BSSCFG_WME_DISABLE	0x2		/* Do not advertise/use WME for this BSS */
#define WLC_BSSCFG_PS_OFF_TRANS	0x4		/* BSS is in transition to PS-OFF */
#define WLC_BSSCFG_SW_BCN	0x8		/* The BSS is generating beacons in SW */
#define WLC_BSSCFG_SW_PRB	0x10		/* The BSS is generating probe responses in SW */
#define WLC_BSSCFG_HW_BCN	0x20		/* The BSS is generating beacons in HW */
#define WLC_BSSCFG_HW_PRB	0x40		/* The BSS is generating probe responses in HW */
#define WLC_BSSCFG_DPT		0x80		/* The BSS is for DPT link */
#define WLC_BSSCFG_WME_ASSOC	0x100		/* This Infra STA has WME association */
#define WLC_BSSCFG_BTA		0x200		/* The BSS is for BTA link */
#define WLC_BSSCFG_NOBCMC	0x400		/* The BSS has no broadcast/multicast traffic */
#define WLC_BSSCFG_NOIF		0x800		/* The BSS has no OS presentation */
#define WLC_BSSCFG_11N_DISABLE	0x1000		/* Do not advertise .11n IEs for this BSS */
#define WLC_BSSCFG_P2P		0x2000		/* The BSS is for p2p link */
#define WLC_BSSCFG_11H_DISABLE	0x4000		/* Do not follow .11h rules for this BSS */
#define WLC_BSSCFG_NATIVEIF	0x8000		/* The BSS uses native OS if */
#define WLC_BSSCFG_P2P_DISC	0x10000		/* The BSS is for p2p discovery */
#define WLC_BSSCFG_TX_SUPR	0x20000		/* The BSS is in absence mode */
#define WLC_BSSCFG_SRADAR_ENAB	0x40000		/* follow special radar rules for soft/ext ap */
#define WLC_BSSCFG_DYNBCN	0x80000		/* Do not beacon if no client is associated */
#define WLC_BSSCFG_P2P_RESET	0x100000	/* reset the existing P2P bsscfg to default */
#define WLC_BSSCFG_P2P_RECREATE_BSSIDX 0x200000	/* alloc new bssid_idx in wlc_bsscfg_p2p_init */
#define WLC_BSSCFG_AP_NORADAR_CHAN  0x00400000	/* disallow ap to start on radar channel */
#define WLC_BSSCFG_TDLS		    0x00800000	/* The BSS is for TDLS link */
#define WLC_BSSCFG_USR_RADAR_CHAN_SELECT	0x01000000	/* User will perform radar
								 * random chan select.
								 */
#define WLC_BSSCFG_BSSLOAD_DISABLE  0x02000000	/* Do not enable BSSLOAD for this BSS */
#define WLC_BSSCFG_VHT_DISABLE	    0x04000000	/* Do not advertise VHT IEs for this BSS */
#define WLC_BSSCFG_AWDL		0x08000000	/* This BSS is for AWDL link */
#define WLC_BSSCFG_EXCRX	0x10000000	/* Channel access is scheduled via excursion */
#define WLC_BSSCFG_TX_SUPR_ENAB	0x20000000	/* The BSS supports absence supprssion re-que */
#define WLC_BSSCFG_NO_AUTHENTICATOR 0x40000000 /* The BSS has no in-driver authenticator */
#define WLC_BSSCFG_ALLOW_FTOVERDS	0x80000000	/* Use of FBT Over-the-DS is allowed */

#define WLC_BSSCFG_FL2_PROXD		0x1	/* The BSS is for proximity detection */
#define WLC_BSSCFG_FL2_NAN		0x2	/* The BSS is for NAN service */
#define WLC_BSSCFG_FL2_AIBSS_PS		0x4	/* The BSS supports AIBSS PS */
#define WLC_BSSCFG_FL2_MCHAN_DISABLE	0x8	/* Do not create mchan chan_context */
#define WLC_BSSCFG_FL2_MODESW_BWSW	0x8	/* The BSS is for ModeSW bandwidth flag */
#define WLC_BSSCFG_FL2_DWDS_BRCM_IE_DISABLE	0x20	/* The BSS do NOT use DWDS cap in BRCM IE */
#define WLC_BSSCFG_FL2_SPILT_ASSOC_REQ	0x40	/* The BSS split assoc req into two parts */
#define WLC_BSSCFG_FL2_SPLIT_ASSOC_RESP	0x80	/* The BSS split assoc resp into two parts */
#define WLC_BSSCFG_FL2_CSA_IN_PROGRESS	0x100	/* The BSS is doing CSA */
#define WLC_BSSCFG_FL2_FBT_1X		0x1000	/**< The BSS is for FBT 802.1X */
#define WLC_BSSCFG_FL2_FBT_PSK		0x2000	/**< The BSS is for FBT PSK */

#define WLC_BSSCFG(wlc, idx) \
	(((idx) < WLC_MAXBSSCFG && ((int)(idx)) >= 0) ? ((wlc)->bsscfg[idx]) : NULL)

#define HWBCN_ENAB(cfg)		(((cfg)->flags & WLC_BSSCFG_HW_BCN) != 0)
#define HWPRB_ENAB(cfg)		(((cfg)->flags & WLC_BSSCFG_HW_PRB) != 0)
#define SOFTBCN_ENAB(cfg)	(((cfg)->flags & WLC_BSSCFG_SW_BCN) != 0)
#define SOFTPRB_ENAB(cfg)	(((cfg)->flags & WLC_BSSCFG_SW_PRB) != 0)
#define DYNBCN_ENAB(cfg)	(((cfg)->flags & WLC_BSSCFG_DYNBCN) != 0)

#define BSSCFG_HAS_NOIF(cfg)	(((cfg)->flags & WLC_BSSCFG_NOIF) != 0)
#define BSSCFG_HAS_NATIVEIF(cfg)	(((cfg)->flags & WLC_BSSCFG_NATIVEIF) != 0)

#ifdef WLDPT
#define BSSCFG_IS_DPT(cfg)	(((cfg)->flags & WLC_BSSCFG_DPT) != 0)
#else
#define BSSCFG_IS_DPT(cfg)	FALSE
#endif // endif

#ifdef WLTDLS
#define BSSCFG_IS_TDLS(cfg)	(((cfg)->flags & WLC_BSSCFG_TDLS) != 0)
#else
#define BSSCFG_IS_TDLS(cfg)	FALSE
#endif // endif

#define IBSS_HW_ENAB(cfg)	(BSSCFG_IBSS(cfg) && HWBCN_ENAB(cfg) && HWPRB_ENAB(cfg))

#ifdef WLAIBSS
#define BSSCFG_IS_AIBSS_PS_ENAB(cfg)	(((cfg)->flags2 & WLC_BSSCFG_FL2_AIBSS_PS) != 0)
#else
#define BSSCFG_IS_AIBSS_PS_ENAB(cfg)	FALSE
#endif // endif

#ifdef WLAIBSS
#define BSSCFG_AIBSS(wlc, cfg)	(AIBSS_ENAB((wlc)->pub) && (!(cfg)->BSS))
#else
#define BSSCFG_AIBSS(wlc, cfg)	(0)
#endif // endif

#ifdef WL_MODESW
#define BSSCFG_IS_MODESW_BWSW(cfg)	(((cfg)->flags2 & WLC_BSSCFG_FL2_MODESW_BWSW) != 0)
#else
#define BSSCFG_IS_MODESW_BWSW(cfg)	FALSE
#endif // endif

#define BSSCFG_IS_CSA_IN_PROGRESS(cfg)	(((cfg)->flags2 & WLC_BSSCFG_FL2_CSA_IN_PROGRESS) != 0)
#define BSSCFG_SET_CSA_IN_PROGRESS(cfg)	((cfg)->flags2 |= WLC_BSSCFG_FL2_CSA_IN_PROGRESS)
#define BSSCFG_CLR_CSA_IN_PROGRESS(cfg)	((cfg)->flags2 &= ~WLC_BSSCFG_FL2_CSA_IN_PROGRESS)
#ifdef WLFBT
#define WLC_BSSCFG_FL2_FBT_MASK (WLC_BSSCFG_FL2_FBT_1X | WLC_BSSCFG_FL2_FBT_PSK)
#ifdef WLFBT_DISABLED
#define BSSCFG_IS_FBT(cfg, pub) WLFBT_ENAB(pub)
#else
#define BSSCFG_IS_FBT(cfg, pub) (WLFBT_ENAB(pub) && (((cfg)->flags2 & WLC_BSSCFG_FL2_FBT_MASK) != 0))
#endif // endif
#define BSSCFG_IS_FBT_1X(cfg)   (((cfg)->flags2 & WLC_BSSCFG_FL2_FBT_1X) != 0)
#define BSSCFG_IS_FBT_PSK(cfg)  (((cfg)->flags2 & WLC_BSSCFG_FL2_FBT_PSK) != 0)
#else
#define BSSCFG_IS_FBT(cfg, pub) FALSE
#define BSSCFG_IS_FBT_1X(cfg)   FALSE
#define BSSCFG_IS_FBT_PSK(cfg)  FALSE
#endif /* WLFBT */

/* Iterator for all dpt bsscfgs:  (wlc_info_t *wlc, int idx, wlc_bsscfg_t *cfg) */
#define FOREACH_DPT(wlc, idx, cfg) \
	for (idx = 0; (int) idx < WLC_MAXBSSCFG; idx++) \
		if (((cfg = (wlc)->bsscfg[idx]) != NULL) && BSSCFG_IS_DPT(cfg))

/* Iterator for all tdls bsscfgs:  (wlc_info_t *wlc, int idx, wlc_bsscfg_t *cfg) */
#define FOREACH_TDLS(wlc, idx, cfg) \
	for (idx = 0; (int) idx < WLC_MAXBSSCFG; idx++) \
		if (((cfg = (wlc)->bsscfg[idx]) != NULL) && BSSCFG_IS_TDLS(cfg))

#ifdef WMF
#define WMF_ENAB(cfg)	((cfg)->wmf_enable)
#else
#define WMF_ENAB(cfg)	FALSE
#endif // endif

#ifdef DWDS
#define DWDS_ENAB(cfg)	((cfg)->_dwds)
#define DWDS_BRCM_IE(cfg) (!((cfg)->flags2 & WLC_BSSCFG_FL2_DWDS_BRCM_IE_DISABLE))
#else
#define DWDS_ENAB(cfg)	FALSE
#define DWDS_BRCM_IE(cfg)	FALSE
#endif // endif

#ifdef MULTIAP
#define MAP_ENAB(cfg)	((cfg)->_map)
#else
#define MAP_ENAB(cfg)	FALSE
#endif	/* MULTIAP */

#define SPLIT_ASSOC_REQ(cfg)	((cfg)->flags2 & WLC_BSSCFG_FL2_SPILT_ASSOC_REQ)
#define SPLIT_ASSOC_RESP(cfg)	((cfg)->flags2 & WLC_BSSCFG_FL2_SPLIT_ASSOC_RESP)

#ifdef MCAST_REGEN
#define MCAST_REGEN_ENAB(cfg)	((cfg)->mcast_regen_enable)
#else
#define MCAST_REGEN_ENAB(cfg)	FALSE
#endif // endif

#ifdef SMF_STATS
#define SMFS_ENAB(cfg) ((cfg)->smfs_info->enable)
#else
#define SMFS_ENAB(cfg) FALSE
#endif /* SMF_STATS */

/* TDLS capabilities */
#ifdef WLTDLS
#define BSS_TDLS_UAPSD_BUF_STA		0x1
#define BSS_TDLS_PEER_PSM			0x2
#define BSS_TDLS_CH_SW				0x4
#define BSS_TDLS_SUPPORT			0x8
#define BSS_TDLS_PROHIBIT			0x10
#define BSS_TDLS_CH_SW_PROHIBIT		0x20

#define BSS_TDLS_BUFFER_STA(a)	((a)->tdls && ((a)->tdls->tdls_cap & BSS_TDLS_UAPSD_BUF_STA))
#else
#define BSS_TDLS_BUFFER_STA(a)	FALSE
#endif /* WLTDLS */

struct wlc_prq_info_s {
	shm_mbss_prq_entry_t source;   /* To get ta addr and timestamp directly */
	bool is_directed;         /* Non-broadcast (has bsscfg associated with it) */
	bool directed_ssid;       /* Was request directed to an SSID? */
	bool directed_bssid;      /* Was request directed to a BSSID? */
	wlc_bsscfg_t *bsscfg;     /* The BSS Config associated with the request (if not bcast) */
	shm_mbss_prq_ft_t frame_type;  /* 0: cck; 1: ofdm; 2: mimo; 3 rsvd */
	bool up_band;             /* Upper or lower sideband of 40 MHz for MIMO phys */
	uint8 plcp0;              /* First byte of PLCP */
#ifdef WLLPRS
	bool is_htsta;		/* is from an HT sta */
#endif /* WLLPRS */
};

extern bsscfg_module_t *wlc_bsscfg_attach(wlc_info_t *wlc);
extern void wlc_bsscfg_detach(bsscfg_module_t *bcmh);

/** bsscfg cubby callback functions */
typedef int (*bsscfg_cubby_init_t)(void *ctx, wlc_bsscfg_t *cfg);
typedef void (*bsscfg_cubby_deinit_t)(void *ctx, wlc_bsscfg_t *cfg);
typedef void (*bsscfg_cubby_dump_t)(void *ctx, wlc_bsscfg_t *cfg, struct bcmstrbuf *b);

/**
 * This function allocates an opaque cubby of the requested size in the bsscfg container.
 * The callback functions fn_init/fn_deinit are called when a bsscfg is allocated/freed.
 * It returns a handle that can be used in macro BSSCFG_CUBBY to retrieve the cubby.
 * Function returns a negative number on failure
 */
extern int wlc_bsscfg_cubby_reserve(wlc_info_t *wlc, uint size,
	bsscfg_cubby_init_t fn_init, bsscfg_cubby_deinit_t fn_deinit,
	bsscfg_cubby_dump_t fn_dump,
	void *ctx);

/* macro to retrieve the pointer to module specific opaque data in bsscfg container */
#define BSSCFG_CUBBY(bsscfg, handle)	(void *)(((uint8 *)(bsscfg)) + (handle))

/** bsscfg up/down event data. */
typedef struct bsscfg_up_down_event_data_t
{
	/* BSSCFG instance data. */
	wlc_bsscfg_t	*bsscfg;

	/* TRUE for up event, FALSE for down event. */
	bool		up;

	/* Each client of the notification can increment this value. It is used
	 * to indicate the number of pending asynchronous callbacks in the driver
	 * down path.
	 */
	int callbacks_pending;
} bsscfg_up_down_event_data_t;

/** bsscfg up/down event callback function. */
typedef void (*bsscfg_up_down_fn_t)(void *ctx, bsscfg_up_down_event_data_t *evt_data);

/**
 * wlc_bsscfg_updown_register()
 *
 * This function registers a callback that will be invoked when either a bsscfg
 * up or down event occurs.
 *
 * Parameters
 *    wlc       Common driver context.
 *    callback  Callback function  to invoke on up/down events.
 *    arg       Client specified data that will provided as param to the callback.
 * Returns:
 *    BCME_OK on success, else BCME_xxx error code.
 */
extern int wlc_bsscfg_updown_register(wlc_info_t *wlc, bsscfg_up_down_fn_t fn, void *arg);

/**
 * wlc_bsscfg_updown_unregister()
 *
 * This function unregisters a bsscfg up/down event callback.
 *
 * Parameters
 *    wlc       Common driver context.
 *    callback  Callback function that was previously registered.
 *    arg       Client specified data that was previously registerd.
 * Returns:
 *    BCME_OK on success, else BCME_xxx error code.
 */
extern int wlc_bsscfg_updown_unregister(wlc_info_t *wlc, bsscfg_up_down_fn_t fn, void *arg);

/** bcn/prbresp template update notification */
typedef struct {
	wlc_bsscfg_t *cfg;
	int type;	/* FC_BEACON, or FC_PROBE_RESP */
} bss_tplt_upd_data_t;
typedef void (*bss_tplt_upd_fn_t)(void *arg, bss_tplt_upd_data_t *notif_data);
extern int wlc_bss_tplt_upd_register(wlc_info_t *wlc, bss_tplt_upd_fn_t fn, void *arg);
extern int wlc_bss_tplt_upd_unregister(wlc_info_t *wlc, bss_tplt_upd_fn_t fn, void *arg);
extern void wlc_bss_tplt_upd_notif(wlc_info_t *wlc, wlc_bsscfg_t *cfg, int type);

/** ibss mute request notification */
typedef struct {
	wlc_bsscfg_t *cfg;
	bool mute;
} ibss_mute_upd_data_t;
typedef void (*ibss_mute_upd_fn_t)(void *arg, ibss_mute_upd_data_t *notif_data);
extern int wlc_ibss_mute_upd_register(wlc_info_t *wlc, ibss_mute_upd_fn_t fn, void *arg);
extern int wlc_ibss_mute_upd_unregister(wlc_info_t *wlc, ibss_mute_upd_fn_t fn, void *arg);
extern void wlc_ibss_mute_upd_notif(wlc_info_t *wlc, wlc_bsscfg_t *cfg, bool mute);

/**
 * query bss specific pretbtt:
 * return in bss_pretbtt_query_data_t something bigger than 'min'
 * if 'min' doesn't meet your need
 */
typedef struct {
	wlc_bsscfg_t *cfg;
	uint pretbtt;
} bss_pretbtt_query_data_t;
typedef void (*bss_pretbtt_query_fn_t)(void *arg, bss_pretbtt_query_data_t *notif_data);
extern int wlc_bss_pretbtt_query_register(wlc_info_t *wlc,
	bss_pretbtt_query_fn_t fn, void *arg);
extern int wlc_bss_pretbtt_query_unregister(wlc_info_t *wlc,
	bss_pretbtt_query_fn_t fn, void *arg);
extern uint wlc_bss_pretbtt_query(wlc_info_t *wlc, wlc_bsscfg_t *cfg, uint min);

/** report assoc/roam state */
typedef struct {
	wlc_bsscfg_t *cfg;
	uint type;
	uint state;
} bss_assoc_state_data_t;
typedef void (*bss_assoc_state_fn_t)(void *arg, bss_assoc_state_data_t *notif_data);
extern int wlc_bss_assoc_state_register(wlc_info_t *wlc,
	bss_assoc_state_fn_t fn, void *arg);
extern int wlc_bss_assoc_state_unregister(wlc_info_t *wlc,
	bss_assoc_state_fn_t fn, void *arg);
extern void wlc_bss_assoc_state_notif(wlc_info_t *wlc, wlc_bsscfg_t *cfg,
	uint type, uint state);

extern wlc_bsscfg_t *wlc_bsscfg_primary(wlc_info_t *wlc);
extern int wlc_bsscfg_primary_init(wlc_info_t *wlc);
extern wlc_bsscfg_t *wlc_bsscfg_alloc(wlc_info_t *wlc, int idx, uint flags,
	struct ether_addr *ea, bool ap);
extern int wlc_bsscfg_vif_reset(wlc_info_t *wlc, int idx, uint flags,
	struct ether_addr *ea, bool ap);
extern int wlc_bsscfg_init(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg);
extern int wlc_bsscfg_reinit(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, bool ap);
extern void wlc_bsscfg_deinit(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg);
extern int wlc_bsscfg_bta_init(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg);
extern void wlc_bsscfg_dpt_init(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg);
#ifdef WLTDLS
extern int wlc_bsscfg_tdls_init(struct wlc_info *wlc, wlc_bsscfg_t *bsscfg, bool initiator);
#endif // endif
extern int wlc_bsscfg_vif_init(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg);
extern void wlc_bsscfg_free(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg);
extern void wlc_bsscfg_disablemulti(wlc_info_t *wlc);
extern int wlc_bsscfg_disable(wlc_info_t *wlc, wlc_bsscfg_t *cfg);
extern int wlc_bsscfg_down(wlc_info_t *wlc, wlc_bsscfg_t *cfg);
extern int wlc_bsscfg_up(wlc_info_t *wlc, wlc_bsscfg_t *cfg);
extern int wlc_bsscfg_enable(wlc_info_t *wlc, wlc_bsscfg_t *cfg);
extern int wlc_bsscfg_get_free_idx(wlc_info_t *wlc);
extern wlc_bsscfg_t *wlc_bsscfg_find(wlc_info_t *wlc, int idx, int *perr);
extern wlc_bsscfg_t *wlc_bsscfg_find_by_hwaddr(wlc_info_t *wlc, struct ether_addr *hwaddr);
extern wlc_bsscfg_t *wlc_bsscfg_find_by_bssid(wlc_info_t *wlc, const struct ether_addr *bssid);
extern wlc_bsscfg_t *wlc_bsscfg_find_by_hwaddr_bssid(wlc_info_t *wlc,
	const struct ether_addr *hwaddr, const struct ether_addr *bssid);
extern wlc_bsscfg_t *wlc_bsscfg_find_by_target_bssid(wlc_info_t *wlc,
	const struct ether_addr *bssid);
extern wlc_bsscfg_t *wlc_bsscfg_find_by_ssid(wlc_info_t *wlc, uint8 *ssid, int ssid_len);
extern wlc_bsscfg_t *wlc_bsscfg_find_by_wlcif(wlc_info_t *wlc, wlc_if_t *wlcif);
extern wlc_bsscfg_t *wlc_bsscfg_find_by_ID(wlc_info_t *wlc, uint16 id);
extern int wlc_ap_bss_up_count(wlc_info_t *wlc);
#ifdef PSTA
extern int wlc_psta_bss_count(wlc_info_t *wlc);
#endif /* PSTA */
#ifdef STA
extern int wlc_bsscfg_assoc_params_set(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg,
	wl_join_assoc_params_t *assoc_params, int assoc_params_len);
extern void wlc_bsscfg_assoc_params_reset(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg);
#define wlc_bsscfg_assoc_params(bsscfg)	(bsscfg)->assoc_params
extern int wlc_bsscfg_scan_params_set(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg,
	wl_join_scan_params_t *scan_params);
extern void wlc_bsscfg_scan_params_reset(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg);
#define wlc_bsscfg_scan_params(bsscfg)	(bsscfg)->scan_params
#endif // endif
extern void wlc_bsscfg_SSID_set(wlc_bsscfg_t *bsscfg, uint8 *SSID, int len);
extern void wlc_bsscfg_ID_assign(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg);

extern void wlc_bsscfg_bcn_disable(wlc_info_t *wlc, wlc_bsscfg_t *cfg);
extern void wlc_bsscfg_bcn_enable(wlc_info_t *wlc, wlc_bsscfg_t *cfg);
extern int wlc_bsscfg_rateset_init(wlc_info_t *wlc, wlc_bsscfg_t *cfg,
	uint8 rates, uint8 bw, uint8 mcsallow);
extern int wlc_bsscfg_set_infra_mode(wlc_info_t *wlc, wlc_bsscfg_t *cfg, bool infra);

#define WLC_BCMCSCB_GET(wlc, bsscfg) \
	(((bsscfg)->flags & WLC_BSSCFG_NOBCMC) ? \
	 NULL : \
	 (bsscfg)->bcmc_scb[CHSPEC_WLCBANDUNIT((bsscfg)->current_bss->chanspec)])
#define WLC_BSSCFG_IDX(bsscfg) ((bsscfg)->_idx)

typedef bool (*vndr_ie_filter_fn_t)(wlc_bsscfg_t *bsscfg, vndr_ie_t *ie);
extern int wlc_vndr_ie_getlen_ext(wlc_bsscfg_t *bsscfg, vndr_ie_filter_fn_t filter,
	uint32 pktflag, int *totie);
#define wlc_vndr_ie_getlen(cfg, pktflag, totie) \
		wlc_vndr_ie_getlen_ext(cfg, NULL, pktflag, totie)
typedef bool (*vndr_ie_write_filter_fn_t)(wlc_bsscfg_t *bsscfg, uint type, vndr_ie_t *ie);
extern uint8 *wlc_vndr_ie_write_ext(wlc_bsscfg_t *bsscfg, vndr_ie_write_filter_fn_t filter,
	uint type, uint8 *cp, int buflen, uint32 pktflag);
#define wlc_vndr_ie_write(cfg, cp, buflen, pktflag) \
		wlc_vndr_ie_write_ext(cfg, NULL, -1, cp, buflen, pktflag)
extern vndr_ie_listel_t *wlc_vndr_ie_add_elem(wlc_bsscfg_t *bsscfg, uint32 pktflag,
	vndr_ie_t *vndr_iep);
extern vndr_ie_listel_t *wlc_vndr_ie_mod_elem(wlc_bsscfg_t *bsscfg, vndr_ie_listel_t *old_listel,
	uint32 pktflag, vndr_ie_t *vndr_iep);
extern int wlc_vndr_ie_add(wlc_bsscfg_t *bsscfg, vndr_ie_buf_t *ie_buf, int len);
extern int wlc_vndr_ie_del(wlc_bsscfg_t *bsscfg, vndr_ie_buf_t *ie_buf, int len);
extern int wlc_vndr_ie_get(wlc_bsscfg_t *bsscfg, vndr_ie_buf_t *ie_buf, int len,
	uint32 pktflag);

extern int wlc_vndr_ie_mod_elem_by_type(wlc_bsscfg_t *bsscfg, uint8 type,
	uint32 pktflag, vndr_ie_t *vndr_iep);
extern int wlc_vndr_ie_del_by_type(wlc_bsscfg_t *bsscfg, uint8 type);
extern uint8 *wlc_vndr_ie_find_by_type(wlc_bsscfg_t *bsscfg, uint8 type);

extern uint8 *wlc_bsscfg_get_ie(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, uint8 ie_type);
extern int wlc_bsscfg_set_ie(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, uint8 *ie_data,
	bool *bcn_upd, bool *prbresp_upd);

extern void wlc_bsscfg_set_ext_cap(wlc_bsscfg_t *bsscfg, uint32 bit, bool val);
extern void wlc_bsscfg_set_ext_cap_spsmp(wlc_bsscfg_t *bsscfg, bool spsmp, uint8 si);
#ifdef WLAWDL
int wlc_bsscfg_awdl_rate_init(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, uint8 bw, uint8 minrate);
#endif // endif
#if defined(AP)
extern uint16 wlc_bsscfg_newaid(wlc_bsscfg_t *cfg);
#endif /* AP */

#ifdef SMF_STATS
extern int wlc_smfstats_update(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, uint8 smfs_type,
	uint16 code);
extern int wlc_bsscfg_smfsinit(wlc_info_t *wlc, wlc_bsscfg_t *cfg);
extern int wlc_bsscfg_get_smfs(wlc_bsscfg_t *cfg, int idx, char* buf, int len);
extern int wlc_bsscfg_clear_smfs(wlc_info_t *wlc, wlc_bsscfg_t *cfg);
#else
#define wlc_smfstats_update(a, b, c, d) do {} while (0)
#define wlc_bsscfg_smfsinit(a, b) do {} while (0)
#define wlc_bsscfg_get_smfs(a, b, c, d) do {} while (0)
#define wlc_bsscfg_clear_smfs(a, b) do {} while (0)
#endif // endif

#define WLC_BSSCFG_AUTH(cfg) ((cfg)->auth)

#define WLC_BSSCFG_DEFKEY(cfg, idx) \
	(((idx) < WLC_DEFAULT_KEYS && (int)(idx) >= 0) ? \
	((cfg)->bss_def_keys[idx]) : 0)

/* Extend WME_ENAB to per-BSS */
#define BSS_WME_ENAB(wlc, cfg) \
	(WME_ENAB((wlc)->pub) && !((cfg)->flags & WLC_BSSCFG_WME_DISABLE))
#define BSS_WME_AS(wlc, cfg) \
	(BSS_WME_ENAB(wlc, cfg) && ((cfg)->flags & WLC_BSSCFG_WME_ASSOC) != 0)

#ifdef WLBTAMP
/* Extend BTA_ENAB to per-BSS */
#define BSS_BTA_ENAB(wlc, cfg) \
	(BTA_ENAB((wlc)->pub) && (cfg) && ((cfg)->flags & WLC_BSSCFG_BTA))
#endif // endif

/* Extend N_ENAB to per-BSS */
#define BSS_N_ENAB(wlc, cfg) \
	(N_ENAB((wlc)->pub) && !((cfg)->flags & WLC_BSSCFG_11N_DISABLE))

/* Extend VHT_ENAB to per-BSS */
#define BSS_VHT_ENAB(wlc, cfg) \
	(VHT_ENAB_BAND((wlc)->pub, (wlc)->band->bandtype) &&\
		 !((cfg)->flags & WLC_BSSCFG_VHT_DISABLE))

/* Extend WL11H_ENAB to per-BSS */
#define BSS_WL11H_ENAB(wlc, cfg) \
	(WL11H_ENAB(wlc) && !((cfg)->flags & WLC_BSSCFG_11H_DISABLE))

/* Extend WLBSSLOAD_ENAB to per-BSS */
#define BSS_WLBSSLOAD_ENAB(wlc, cfg) \
	(WLBSSLOAD_ENAB((wlc)->pub) && !((cfg)->flags & WLC_BSSCFG_BSSLOAD_DISABLE))

/* Extend P2P_ENAB to per-BSS */
#ifdef WLP2P
#define BSS_P2P_ENAB(wlc, cfg) \
	(P2P_ENAB((wlc)->pub) && ((cfg)->flags & WLC_BSSCFG_P2P) != 0)
#define BSS_P2P_DISC_ENAB(wlc, cfg) \
	(P2P_ENAB((wlc)->pub) && ((cfg)->flags & WLC_BSSCFG_P2P_DISC) != 0)
#else /* !WLP2P */
#define BSS_P2P_ENAB(wlc, cfg)      FALSE
#define BSS_P2P_DISC_ENAB(wlc, cfg) FALSE
#endif /* !WLP2P */

#define BSS_EXCRX_ENAB(wlc, cfg) (((cfg)->flags & WLC_BSSCFG_EXCRX) != 0)

/* handy bsscfg type macros */
#ifdef WLP2P
#define P2P_DEV(wlc, cfg)	(BSS_P2P_DISC_ENAB(wlc, cfg))
#define P2P_GO(wlc, cfg)	(BSSCFG_AP(cfg) && BSS_P2P_ENAB(wlc, cfg))
#define P2P_CLIENT(wlc, cfg)	(BSSCFG_STA(cfg) && BSS_P2P_ENAB(wlc, cfg) && \
				 !BSS_P2P_DISC_ENAB(wlc, cfg))
#define P2P_IF(wlc, cfg)	(P2P_GO(wlc, cfg) || P2P_CLIENT(wlc, cfg))
#else
#define P2P_DEV(wlc, cfg)	FALSE
#define P2P_GO(wlc, cfg)	FALSE
#define P2P_CLIENT(wlc, cfg)	FALSE
#define P2P_IF(wlc, cfg)	FALSE
#endif /* !WLP2P */

#ifdef WL_BSSCFG_TX_SUPR
#define BSS_TX_SUPR_ENAB(cfg) ((cfg)->flags & WLC_BSSCFG_TX_SUPR_ENAB)
#define BSS_TX_SUPR(cfg) ((cfg)->flags & WLC_BSSCFG_TX_SUPR)
#else
#define BSS_TX_SUPR_ENAB(cfg)	FALSE
#define BSS_TX_SUPR(cfg)	0
#endif // endif

#ifdef WLTDLS
/* Extend TDLS_ENAB to per-BSS */
#define BSS_TDLS_ENAB(wlc, cfg) \
	(TDLS_ENAB((wlc)->pub) && (cfg) && ((cfg)->flags & WLC_BSSCFG_TDLS))
#define BSS_TDLS_AS(wlc, cfg) \
	((cfg) && (cfg)->tdls_scb && (cfg)->tdls_scb->state & ASSOCIATED)
#else
#define BSS_TDLS_ENAB(wlc, cfg) 	FALSE
#define BSS_TDLS_AS(wlc, cfg)		FALSE
#endif // endif

#ifdef WL_PROXDETECT
#define BSS_PROXD_ENAB(wlc, cfg) \
	(PROXD_ENAB((wlc)->pub) && (cfg) && ((cfg)->flags2 & WLC_BSSCFG_FL2_PROXD))
#else
#define BSS_PROXD_ENAB(wlc, cfg) 	FALSE
#endif // endif

#ifdef WLAWDL
/* Extend AWDL_ENAB to per-BSS */
#define BSS_AWDL_ENAB(wlc, cfg) \
	(AWDL_ENAB((wlc)->pub) && (cfg) && (cfg)->is_awdl)
#else
#define BSS_AWDL_ENAB(wlc, cfg) FALSE
#endif // endif

#ifdef WLMCHAN
#define BSS_MCHAN_ENAB(wlc, cfg) \
	(MCHAN_ENAB((wlc)->pub) && (cfg) && !((cfg)->flags2 & WLC_BSSCFG_FL2_MCHAN_DISABLE))
#else
#define BSS_MCHAN_ENAB(wlc, cfg) FALSE
#endif // endif

/* Macros related to Multi-BSS. */
#if defined(MBSS)
/* Define as all bits less than and including the msb shall be one's */
#define EADDR_TO_UC_IDX(eaddr, mask)	((eaddr).octet[5] & (mask))
#define WLC_BSSCFG_UCIDX(bsscfg)	((bsscfg)->_ucidx)
#define MBSS_BCN_ENAB(wlc, cfg)		(MBSS_ENAB((wlc)->pub) &&	\
					 BSSCFG_AP(cfg) &&		\
					 ((cfg)->flags & (WLC_BSSCFG_SW_BCN | \
					                  WLC_BSSCFG_HW_BCN | \
					                  0)))
#define MBSS_PRB_ENAB(wlc, cfg)		(MBSS_ENAB((wlc)->pub) &&	\
					 BSSCFG_AP(cfg) &&		\
					 ((cfg)->flags & (WLC_BSSCFG_SW_PRB | \
					                  WLC_BSSCFG_HW_PRB | \
					                  0)))

/**
 * BC/MC FID macros.  Only valid under MBSS
 *
 *    BCMC_FID_SHM_COMMIT  Committing FID to SHM; move driver's value to bcmc_fid_shm
 *    BCMC_FID_QUEUED	   Are any packets enqueued on the BC/MC fifo?
 */

/* XXX If we can guarantee no race condition reading SHM FID on dotxstatus of last
 * XXX fid, then we could eliminate recording fid_shm in the driver
 */
extern void bcmc_fid_shm_commit(wlc_bsscfg_t *bsscfg);
#define BCMC_FID_SHM_COMMIT(bsscfg) bcmc_fid_shm_commit(bsscfg)

#define BCMC_PKTS_QUEUED(bsscfg) \
	(((bsscfg)->bcmc_fid_shm != INVALIDFID) || ((bsscfg)->bcmc_fid != INVALIDFID))

extern int wlc_write_mbss_basemac(wlc_info_t *wlc, const struct ether_addr *addr);

#else

#define WLC_BSSCFG_UCIDX(bsscfg) 0
#define wlc_write_mbss_basemac(wlc, addr) (0)

#define BCMC_FID_SHM_COMMIT(bsscfg)
#define BCMC_PKTS_QUEUED(bsscfg) 0
#define MBSS_BCN_ENAB(wlc, cfg)  0
#define MBSS_PRB_ENAB(wlc, cfg)  0
#endif /* defined(MBSS) */

#define BSS_11H_SRADAR_ENAB(wlc, cfg)	(WL11H_ENAB(wlc) && BSSCFG_SRADAR_ENAB(cfg))
#define BSSCFG_SRADAR_ENAB(cfg)	((cfg)->flags & WLC_BSSCFG_SRADAR_ENAB)
#define BSS_11H_AP_NORADAR_CHAN_ENAB(wlc, cfg) (WL11H_ENAB(wlc) && BSSCFG_AP_NORADAR_CHAN_ENAB(cfg))
#define BSSCFG_AP_NORADAR_CHAN_ENAB(cfg)	((cfg)->flags & WLC_BSSCFG_AP_NORADAR_CHAN)
#define BSSCFG_AP_USR_RADAR_CHAN_SELECT(cfg)    ((cfg)->flags & WLC_BSSCFG_USR_RADAR_CHAN_SELECT)

#ifdef WLMCHAN
#define BSSCFG_AP_MCHAN_DISABLED(wlc, cfg)	(!MCHAN_ENAB(wlc->pub) || !P2P_GO(wlc, cfg))
#else
#define BSSCFG_AP_MCHAN_DISABLED(wlc, cfg)	TRUE
#endif // endif

#ifdef WL_BSSCFG_TX_SUPR
extern void wlc_bsscfg_tx_stop(wlc_bsscfg_t *bsscfg);
extern void wlc_bsscfg_tx_start(wlc_bsscfg_t *bsscfg);
extern bool wlc_bsscfg_tx_abs_enq(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, void *sdu, uint prec);
extern void wlc_bsscfg_tx_check(wlc_info_t *wlc);
extern void wlc_bsscfg_tx_supr_norm(wlc_info_t *wlc);
extern bool wlc_bsscfg_tx_supr_enq(wlc_info_t *wlc, wlc_bsscfg_t *cfg, void *pkt);
#else
#define wlc_bsscfg_tx_stop(a) do { } while (0)
#define wlc_bsscfg_tx_start(a) do { } while (0)
#define wlc_bsscfg_tx_abs_enq(a, b, c, d) FALSE
#define wlc_bsscfg_tx_check(a) do { } while (0)
#define wlc_bsscfg_tx_supr_norm(a) do { } while (0)
#define wlc_bsscfg_tx_supr_enq(a, b, c) FALSE
#endif // endif
#ifdef EXT_STA
#define BSSCFG_SAFEMODE(cfg)	((cfg) && (cfg)->safeModeEnabled)
#else
#define BSSCFG_SAFEMODE(cfg)	0
#endif /* EXT_STA */
#ifdef STA
extern void wlc_bsscfg_wsec_session_reset(wlc_info_t *wlc, wlc_bsscfg_t *cfg);
extern int wlc_bsscfg_wsec_key_buf_init(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg);
extern int wlc_bsscfg_wsec_key_buf_free(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg);
#endif /* STA */

extern bool wlc_bsscfg_mcastfilter(wlc_bsscfg_t *cfg, struct ether_addr *da);
extern void wlc_bsscfg_reprate_init(wlc_bsscfg_t *bsscfg);
extern void wlc_get_rate_histo_bsscfg(wlc_bsscfg_t *bsscfg, wl_mac_ratehisto_res_t *rhist,
	ratespec_t *most_used_ratespec, ratespec_t *highest_used_ratespec);
extern void wlc_bsscfg_add_wlif(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg);
extern bool wlc_bsscfg_is_intf_up(wlc_info_t *wlc, uint16 connID);
#ifdef AP
extern uint32 wlc_bsscfg_getdtim_count(wlc_info_t *wlc, wlc_bsscfg_t *cfg);
#endif // endif
#ifndef NO_ROAMOFFL_SUPPORT
extern bool wlc_bsscfg_chk_roamoffl_bssid(wlc_bsscfg_t *cfg, struct ether_addr *addr);
#endif /* !NO_ROAMOFFL_SUPPORT */
#ifdef BCMDBG_TXSTUCK
extern void wlc_bsscfg_print_txstuck(wlc_info_t *wlc, struct bcmstrbuf *b);
#endif /* BCMDBG_TXSTUCK */
#ifdef WL_GLOBAL_RCLASS
extern void wlc_bsscfg_update_rclass(wlc_info_t* wlc, wlc_bsscfg_t* bsscfg);
extern bool wlc_cur_opclass_global(wlc_info_t* wlc, wlc_bsscfg_t* bsscfg);
#endif /* WL_GLOBAL_RCLASS */
#endif	/* _WLC_BSSCFG_H_ */
