/*
 *	This file is part of OGS Engine
 *	Copyright (C) 1996-1997 Id Software, Inc.
 *	Copyright (C) 2016-2017 OGS Dev Team
 *
 *	OGS Engine is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	OGS Engine is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OGS Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 *	In addition, as a special exception, the author gives permission to
 *	link the code of OGS Engine with the Half-Life Game Engine ("GoldSrc/GS
 *	Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *	L.L.C ("Valve").  You must obey the GNU General Public License in all
 *	respects for all of the code used other than the GoldSrc Engine and MODs
 *	from Valve.  If you modify this file, you may extend this exception
 *	to your version of the file, but you are not obligated to do so.  If
 *	you do not wish to do so, delete this exception statement from your
 *	version.
 */

/// @file

#pragma once

#include "maintypes.h"
#include "common/commontypes.h"
#include "common/crc.h"
#include "common/dll_state.h"
#include "common/entity_state.h"
#include "console/cvar.hpp"
#include "engine/custom.h"
#include "engine/progs.h"
#include "network/delta.hpp"
#include "network/delta_packet.hpp"
#include "network/filter.hpp"
#include "network/net.hpp"
#include "network/net_chan.hpp"
#include "network/usermsg.hpp"
#include "network/protocol.hpp"
#include "pm_shared/pm_defs.h"
#include "qlimits.h"
#include "rehlds/model.h"
#include "resources/consistency.hpp"
#include "system/server_static.hpp"
#include "system/servertypes.hpp"
#include "system/info.hpp"
#include "userid.h"
#include "world/event.hpp"
#include "world/inst_baseline.hpp"

class IGameClient;

typedef struct decalname_s decalname_t;

typedef struct server_s
{
	qboolean active;
	qboolean paused;
	qboolean loadgame; // client begins should reuse existing entity

	double time;
	double oldtime;

	int lastcheck;
	double lastchecktime;

	char name[64]; // MAX_QPATH // map name, or cinematic name
	char oldname[64];
	char startspot[64];
	char modelname[64];

	struct model_s *worldmodel;
	CRC32_t worldmapCRC;

	unsigned char clientdllmd5[16];

	resource_t resourcelist[MAX_RESOURCE_LIST];
	int num_resources;
	
	consistency_t consistency_list[MAX_CONSISTENCY_LIST];
	int num_consistency;
	
	const char *model_precache[MAX_MODELS];
	struct model_s *models[MAX_MODELS];
	unsigned char model_precache_flags[MAX_MODELS];
	
	struct event_s event_precache[MAX_EVENTS];
	
	const char *sound_precache[MAX_SOUNDS];
	short int sound_precache_hashedlookup[MAX_SOUND_HASHLOOKUP_SIZE];
	qboolean sound_precache_hashedlookup_built;
	
	const char *generic_precache[MAX_GENERIC];
	char generic_precache_names[MAX_GENERIC][64];
	int num_generic_names;
	
	char *lightstyles[MAX_LIGHTSTYLES];
	
	int num_edicts;
	int max_edicts;
	edict_t *edicts;
	
	struct entity_state_s *baselines;
	extra_baselines_t *instance_baselines;

	server_state_t state;

	sizebuf_t datagram;
	unsigned char datagram_buf[MAX_DATAGRAM];

	sizebuf_t reliable_datagram;
	unsigned char reliable_datagram_buf[MAX_DATAGRAM];
	
	// the multicast buffer is used to send a message to a set of clients
	// it is only used to marshall data until SV_Multicast is called
	sizebuf_t multicast;
	unsigned char multicast_buf[1024];

	sizebuf_t spectator;
	unsigned char spectator_buf[1024];

	sizebuf_t signon;
	unsigned char signon_data[32768];
} server_t;

struct rehlds_server_t
{
// map for sv.model_precache (for faster resolving of model index by its name)
#if defined(REHLDS_FIXES)
	CStringKeyStaticMap<int, 7, MAX_MODELS * 2>
	modelsMap; // case-sensitive keys for better performance
#elif defined(REHLDS_OPT_PEDANTIC)
	CICaseStringKeyStaticMap<int, 7, MAX_MODELS * 2>
	modelsMap; // use case-insensitive keys to conform original engine's
// behavior
#endif

#ifdef REHLDS_FIXES
	// Extended net buffers
	uint8_t reliableDatagramBuffer[NET_MAX_PAYLOAD];
	uint8_t signonData[NET_MAX_PAYLOAD];
	uint8_t spectatorBuffer[MAX_DATAGRAM];

	// Extended resource list
	resource_t resources[RESOURCE_MAX_COUNT];
	char precachedGenericResourceNames[RESOURCE_MAX_COUNT][MAX_QPATH];
	size_t precachedGenericResourceCount;
#endif
};

typedef struct client_frame_s
{
	double senttime;
	float ping_time;
	clientdata_t clientdata;
	weapon_data_t weapondata[64];
	packet_entities_t entities;
} client_frame_t;

typedef struct client_s
{
	qboolean active;
	qboolean spawned;
	qboolean fully_connected;
	qboolean connected;
	qboolean uploading;
	qboolean hasusrmsgs;
	qboolean has_force_unmodified;

	netchan_t netchan;

	int chokecount;
	int delta_sequence;

	qboolean fakeclient;
	qboolean proxy;

	usercmd_t lastcmd;

	double connecttime;
	double cmdtime;
	double ignorecmdtime;

	float latency;
	float packet_loss;

	double localtime;
	double nextping;
	double svtimebase;

	sizebuf_t datagram;
	byte datagram_buf[MAX_DATAGRAM];

	double connection_started;

	double next_messagetime;
	double next_messageinterval;

	qboolean send_message;
	qboolean skip_message;

	client_frame_t *frames;
	event_state_t events;
	edict_t *edict;
	const edict_t *pViewEntity;
	int userid;
	USERID_t network_userid;
	char userinfo[MAX_INFO_STRING];
	qboolean sendinfo;
	float sendinfo_time;
	char hashedcdkey[64];
	char name[32];
	int topcolor;
	int bottomcolor;
	int entityId;

	resource_t resourcesonhand;
	resource_t resourcesneeded;

	FileHandle_t upload;

	qboolean uploaddoneregistering;

	customization_t customdata;

	int crcValue;

	int lw;
	int lc;

	char physinfo[MAX_INFO_STRING];
	qboolean m_bLoopback;
	uint32 m_VoiceStreams[2];
	double m_lastvoicetime;
	int m_sendrescount;
} client_t;

typedef struct rcon_failure_s rcon_failure_t;
typedef struct challenge_s challenge_t;

typedef struct deltacallback_s
{
	int *numbase;
	int num;
	qboolean remove;
	qboolean custom;
	qboolean newbl;
	int newblindex;
	qboolean full;
	int offset;
} deltacallback_t;

#ifdef HOOK_ENGINE
#define pr_strings (*ppr_strings)
#define gNullString (*pgNullString)
#define scr_skipupdate (*pscr_skipupdate)
#define scr_centertime_off (*pscr_centertime_off)
#define g_LastScreenUpdateTime (*pg_LastScreenUpdateTime)

#define SV_UPDATE_BACKUP (*pSV_UPDATE_BACKUP)
#define SV_UPDATE_MASK (*pSV_UPDATE_MASK)

#define gGlobalVariables (*pgGlobalVariables)
#define g_psvs (*psvs)
#define g_psv (*psv)

#define sv_lan (*psv_lan)
#define sv_lan_rate (*psv_lan_rate)
#define sv_aim (*psv_aim)

#define sv_skycolor_r (*psv_skycolor_r)
#define sv_skycolor_g (*psv_skycolor_g)
#define sv_skycolor_b (*psv_skycolor_b)
#define sv_skyvec_x (*psv_skyvec_x)
#define sv_skyvec_y (*psv_skyvec_y)
#define sv_skyvec_z (*psv_skyvec_z)
#define sv_skyname (*psv_skyname)

#define sv_spectatormaxspeed (*psv_spectatormaxspeed)
#define sv_airaccelerate (*psv_airaccelerate)
#define sv_wateraccelerate (*psv_wateraccelerate)
#define sv_waterfriction (*psv_waterfriction)
#define sv_zmax (*psv_zmax)
#define sv_wateramp (*psv_wateramp)
#define mapcyclefile (*pmapcyclefile)
#define motdfile (*pmotdfile)
#define servercfgfile (*pservercfgfile)
#define lservercfgfile (*plservercfgfile)
#define logsdir (*plogsdir)
#define bannedcfgfile (*pbannedcfgfile)

#define sv_decalnames (*psv_decalnames)
#define sv_decalnamecount (*psv_decalnamecount)

#define sv_gpNewUserMsgs (*psv_gpNewUserMsgs)
#define sv_gpUserMsgs (*psv_gpUserMsgs)

#define g_svmove (*pg_svmove)

#define sv_lastnum (*psv_lastnum)
#define g_sv_instance_baselines (*pg_sv_instance_baselines)
#define g_bOutOfDateRestart (*pg_bOutOfDateRestart)
#define g_userid (*pg_userid)

#define g_sv_delta (*pg_sv_delta)
#define g_peventdelta (*pg_peventdelta)

#define rcon_password (*prcon_password)
#define sv_enableoldqueries (*psv_enableoldqueries)
#define sv_instancedbaseline (*psv_instancedbaseline)
#define sv_contact (*psv_contact)
#define sv_maxupdaterate (*psv_maxupdaterate)
#define sv_minupdaterate (*psv_minupdaterate)
#define sv_filterban (*psv_filterban)
#define sv_minrate (*psv_minrate)
#define sv_maxrate (*psv_maxrate)
#define sv_logrelay (*psv_logrelay)
#define violence_hblood (*pviolence_hblood)
#define violence_ablood (*pviolence_ablood)
#define violence_hgibs (*pviolence_hgibs)
#define violence_agibs (*pviolence_agibs)
#define sv_newunit (*psv_newunit)
#define sv_clienttrace (*psv_clienttrace)
#define sv_timeout (*psv_timeout)
#define sv_failuretime (*psv_failuretime)

#define sv_cheats (*psv_cheats)
#define sv_password (*psv_password)
#define sv_proxies (*psv_proxies)
#define sv_outofdatetime (*psv_outofdatetime)
#define mapchangecfgfile (*pmapchangecfgfile)

#define allow_cheats (*pallow_cheats)
#define mp_logecho (*pmp_logecho)
#define mp_logfile (*pmp_logfile)

#define sv_allow_download (*psv_allow_download)
#define sv_send_logos (*psv_send_logos)
#define sv_send_resources (*psv_send_resources)
#define sv_log_singleplayer (*psv_log_singleplayer)
#define sv_logsecret (*psv_logsecret)
#define sv_log_onefile (*psv_log_onefile)
#define sv_logbans (*psv_logbans)
#define sv_allow_upload (*psv_allow_upload)
#define sv_max_upload (*psv_max_upload)
#define hpk_maxsize (*phpk_maxsize)
#define sv_visiblemaxplayers (*psv_visiblemaxplayers)
#define max_queries_sec (*pmax_queries_sec)
#define max_queries_sec_global (*pmax_queries_sec_global)
#define max_queries_window (*pmax_queries_window)
#define sv_logblocks (*psv_logblocks)
#define sv_downloadurl (*psv_downloadurl)
#define sv_allow_dlfile (*psv_allow_dlfile)
#define sv_version (*psv_version)
#define sv_playermodel (*psv_playermodel)

#define outputbuf (*poutputbuf)
#define sv_redirected (*psv_redirected)
#define sv_redirectto (*psv_redirectto)

#define sv_rcon_minfailures (*psv_rcon_minfailures)
#define sv_rcon_maxfailures (*psv_rcon_maxfailures)
#define sv_rcon_minfailuretime (*psv_rcon_minfailuretime)
#define sv_rcon_banpenalty (*psv_rcon_banpenalty)

#define scr_downloading (*pscr_downloading)

#define g_bCS_CZ_Flags_Initialized (*pg_bCS_CZ_Flags_Initialized)
#define g_bIsCZero (*pg_bIsCZero)
#define g_bIsCZeroRitual (*pg_bIsCZeroRitual)
#define g_bIsTerrorStrike (*pg_bIsTerrorStrike)
#define g_bIsTFC (*pg_bIsTFC)
#define g_bIsHL1 (*pg_bIsHL1)
#define g_bIsCStrike (*pg_bIsCStrike)

#define fatbytes (*pfatbytes)
#define fatpvs (*pfatpvs)
#define fatpasbytes (*pfatpasbytes)
#define fatpas (*pfatpas)
#define gPacketSuppressed (*pgPacketSuppressed)
#define giNextUserMsg (*pgiNextUserMsg)
#define hashstrings_collisions (*phashstrings_collisions)

#define g_pplayerdelta (*pg_pplayerdelta)
#define g_pentitydelta (*pg_pentitydelta)
#define g_pcustomentitydelta (*pg_pcustomentitydelta)
#define g_pclientdelta (*pg_pclientdelta)
#define g_pweapondelta (*pg_pweapondelta)

#define localinfo (*plocalinfo)
#define localmodels (*plocalmodels)

#define ipfilters (*pipfilters)
#define numipfilters (*pnumipfilters)
#define userfilters (*puserfilters)
#define numuserfilters (*pnumuserfilters)

#define g_rg_sv_challenges (*pg_rg_sv_challenges)

#define g_svdeltacallback (*pg_svdeltacallback)

#endif // HOOK_ENGINE

extern char *pr_strings;
extern char *gNullString;
extern qboolean scr_skipupdate;
extern float scr_centertime_off;
extern float g_LastScreenUpdateTime;

extern int SV_UPDATE_BACKUP;
extern int SV_UPDATE_MASK;

extern globalvars_t gGlobalVariables;

extern server_static_t g_psvs;	// persistant server info
extern server_t g_psv;			// local server

extern rehlds_server_t g_rehlds_sv;

extern cvar_t sv_lan;
extern cvar_t sv_lan_rate;
extern cvar_t sv_aim;

extern cvar_t sv_skycolor_r;
extern cvar_t sv_skycolor_g;
extern cvar_t sv_skycolor_b;
extern cvar_t sv_skyvec_x;
extern cvar_t sv_skyvec_y;
extern cvar_t sv_skyvec_z;
extern cvar_t sv_skyname;

extern cvar_t sv_spectatormaxspeed;
extern cvar_t sv_airaccelerate;
extern cvar_t sv_wateraccelerate;
extern cvar_t sv_waterfriction;
extern cvar_t sv_zmax;
extern cvar_t sv_wateramp;
extern cvar_t mapcyclefile;
extern cvar_t motdfile;
extern cvar_t servercfgfile;
extern cvar_t lservercfgfile;
extern cvar_t logsdir;
extern cvar_t bannedcfgfile;
#ifdef REHLDS_FIXES
extern cvar_t listipcfgfile;
extern cvar_t syserror_logfile;
#endif

extern decalname_t sv_decalnames[MAX_BASE_DECALS];
extern int sv_decalnamecount;

extern UserMsg *sv_gpNewUserMsgs;
extern UserMsg *sv_gpUserMsgs;

extern playermove_t g_svmove;

extern int sv_lastnum;
extern extra_baselines_t g_sv_instance_baselines;
extern qboolean g_bOutOfDateRestart;
extern int g_userid;

extern delta_info_t *g_sv_delta;
extern delta_t *g_peventdelta;

extern cvar_t rcon_password;
extern cvar_t sv_enableoldqueries;
extern cvar_t sv_instancedbaseline;
extern cvar_t sv_contact;
extern cvar_t sv_maxupdaterate;
extern cvar_t sv_minupdaterate;
extern cvar_t sv_filterban;
extern cvar_t sv_minrate;
extern cvar_t sv_maxrate;
extern cvar_t sv_logrelay;
extern cvar_t violence_hblood;
extern cvar_t violence_ablood;
extern cvar_t violence_hgibs;
extern cvar_t violence_agibs;
extern cvar_t sv_newunit;
extern cvar_t sv_clienttrace;
extern cvar_t sv_timeout;
extern cvar_t sv_failuretime;

extern cvar_t sv_cheats;
extern cvar_t sv_password;
extern cvar_t sv_proxies;
extern cvar_t sv_outofdatetime;
extern cvar_t mapchangecfgfile;

extern qboolean allow_cheats;
extern cvar_t mp_logecho;
extern cvar_t mp_logfile;
extern cvar_t sv_allow_download;
extern cvar_t sv_send_logos;
extern cvar_t sv_send_resources;
extern cvar_t sv_log_singleplayer;
extern cvar_t sv_logsecret;
extern cvar_t sv_log_onefile;
extern cvar_t sv_logbans;
extern cvar_t sv_allow_upload;
extern cvar_t sv_max_upload;
extern cvar_t hpk_maxsize;
extern cvar_t sv_visiblemaxplayers;
extern cvar_t max_queries_sec;
extern cvar_t max_queries_sec_global;
extern cvar_t max_queries_window;
extern cvar_t sv_logblocks;
extern cvar_t sv_downloadurl;
extern cvar_t sv_allow_dlfile;
extern cvar_t sv_version;
#ifdef REHLDS_FIXES
extern cvar_t sv_echo_unknown_cmd;
extern cvar_t sv_auto_precache_sounds_in_models;
extern cvar_t sv_delayed_spray_upload;
extern cvar_t sv_rehlds_force_dlmax;
extern cvar_t sv_rehlds_hull_centering;
extern cvar_t sv_rcon_condebug;
extern cvar_t sv_rehlds_userinfo_transmitted_fields;
extern cvar_t sv_rehlds_attachedentities_playeranimationspeed_fix;
extern cvar_t sv_rehlds_local_gametime;
#endif
extern int sv_playermodel;

extern char outputbuf[MAX_ROUTEABLE_PACKET];
extern redirect_t sv_redirected;
extern netadr_t sv_redirectto;

extern cvar_t sv_rcon_minfailures;
extern cvar_t sv_rcon_maxfailures;
extern cvar_t sv_rcon_minfailuretime;
extern cvar_t sv_rcon_banpenalty;

#ifdef REHLDS_FIXES
enum GameType_e
{
	GT_Unitialized,
	GT_CZero,
	GT_CZeroRitual,
	GT_TerrorStrike,
	GT_TFC,
	GT_HL1,
	GT_CStrike
};

extern GameType_e g_eGameType;

//A crutch to prevent rewriting tons of code.
#define g_bIsCZero (g_eGameType == GT_CZero)
#define g_bIsCZeroRitual (g_eGameType == GT_CZeroRitual)
#define g_bIsTerrorStrike (g_eGameType == GT_TerrorStrike)
#define g_bIsTFC (g_eGameType == GT_TFC)
#define g_bIsHL1 (g_eGameType == GT_HL1)
#define g_bIsCStrike (g_eGameType == GT_CStrike)

#else
extern int g_bCS_CZ_Flags_Initialized;
extern int g_bIsCZero;
extern int g_bIsCZeroRitual;
extern int g_bIsTerrorStrike;
extern int g_bIsTFC;
extern int g_bIsHL1;
extern int g_bIsCStrike;
#endif // REHLDS_FIXES

extern int fatbytes;
extern int giNextUserMsg;
extern int hashstrings_collisions;

extern delta_t *g_pplayerdelta;
extern delta_t *g_pentitydelta;
extern delta_t *g_pcustomentitydelta;
extern delta_t *g_pclientdelta;
extern delta_t *g_pweapondelta;
#ifdef REHLDS_OPT_PEDANTIC
extern delta_t *g_pusercmddelta;
#endif

extern unsigned char fatpvs[1024];
extern int fatpasbytes;
extern unsigned char fatpas[1024];

extern int gPacketSuppressed;

extern char localinfo[MAX_LOCALINFO];
extern char localmodels[MAX_MODELS][5]; // inline model names for precache

extern ipfilter_t ipfilters[MAX_IPFILTERS];
extern int numipfilters;

extern userfilter_t userfilters[MAX_USERFILTERS];
extern int numuserfilters;

extern challenge_t g_rg_sv_challenges[MAX_CHALLENGES];

#ifdef HOOK_ENGINE
#define g_rgRconFailures (*pg_rgRconFailures)
#endif // HOOK_ENGINE

extern rcon_failure_t g_rgRconFailures[32];
extern deltacallback_t g_svdeltacallback;

delta_t *SV_LookupDelta(char *name);
NOXREF void SV_DownloadingModules();
void SV_GatherStatistics();
void SV_DeallocateDynamicData();
void SV_ReallocateDynamicData();
void SV_AllocClientFrames();
qboolean SV_IsPlayerIndex(int index);
qboolean SV_IsPlayerIndex_wrapped(int index);
void SV_ClearPacketEntities(client_frame_t *frame);
void SV_AllocPacketEntities(client_frame_t *frame, int numents);
void SV_ClearFrames(client_frame_t **frames);

void SV_Serverinfo_f();
void SV_Localinfo_f();
void SV_User_f();
void SV_Users_f();

void SV_CountPlayers(int *clients);
void SV_CountProxies(int *proxies);

void SV_FindModelNumbers();
void SV_StartParticle(const vec_t *org, const vec_t *dir, int color, int count);
void SV_StartSound(int recipients, edict_t *entity, int channel, const char *sample, int volume, float attenuation, int fFlags, int pitch);
void SV_StartSound_internal(int recipients, edict_t *entity, int channel, const char *sample, int volume, float attenuation, int fFlags, int pitch);
qboolean SV_BuildSoundMsg(edict_t *entity, int channel, const char *sample, int volume, float attenuation, int fFlags, int pitch, const float *origin, sizebuf_t *buffer);
int SV_HashString(const char *string, int iBounds);
int SV_LookupSoundIndex(const char *sample);
void SV_BuildHashedSoundLookupTable();
void SV_AddSampleToHashedLookupTable(const char *pszSample, int iSampleIndex);
qboolean SV_ValidClientMulticast(client_t *client, int soundLeaf, int to);
void SV_Multicast(edict_t *ent, vec_t *origin, int to, qboolean reliable);
void SV_WriteMovevarsToClient(sizebuf_t *message);
void SV_WriteDeltaDescriptionsToClient(sizebuf_t *msg);
void SV_SetMoveVars();
void SV_QueryMovevarsChanged();
void SV_SendServerinfo(sizebuf_t *msg, client_t *client);
void SV_SendServerinfo_internal(sizebuf_t *msg, client_t *client);
void SV_SendResources(sizebuf_t *msg);
void SV_WriteClientdataToMessage(client_t *client, sizebuf_t *msg);
void SV_WriteSpawn(sizebuf_t *msg);
void SV_SendUserReg(sizebuf_t *msg);

void SV_New_f();
void SV_SendRes_f();
void SV_Spawn_f();
void SV_Spawn_f_internal();

void SV_CheckUpdateRate(double *rate);

void SV_RejectConnection(netadr_t *adr, char *fmt, ...);
void SV_RejectConnectionForPassword(netadr_t *adr);

int SV_GetFragmentSize(void *state);

qboolean SV_FilterUser(USERID_t *userid);

int SV_CheckProtocol(netadr_t *adr, int nProtocol);
int SV_CheckProtocol_internal(netadr_t *adr, int nProtocol);

bool SV_CheckChallenge_api(const netadr_t &adr, int nChallengeValue);
int SV_CheckChallenge(netadr_t *adr, int nChallengeValue);

int SV_CheckIPRestrictions(netadr_t *adr, int nAuthProtocol);
int SV_CheckIPRestrictions_internal(netadr_t *adr, int nAuthProtocol);

int SV_CheckIPConnectionReuse(netadr_t *adr);

int SV_FinishCertificateCheck(netadr_t *adr, int nAuthProtocol, char *szRawCertificate, char *userinfo);
int SV_FinishCertificateCheck_internal(netadr_t *adr, int nAuthProtocol, char *szRawCertificate, char *userinfo);
int SV_CheckKeyInfo(netadr_t *adr, char *protinfo, unsigned short *port, int *pAuthProtocol, char *pszRaw, char *cdkey);
int SV_CheckKeyInfo_internal(netadr_t *adr, char *protinfo, unsigned short *port, int *pAuthProtocol, char *pszRaw, char *cdkey);
int SV_CheckForDuplicateSteamID(client_t *client);
int SV_CheckForDuplicateNames(char *userinfo, qboolean bIsReconnecting, int nExcludeSlot);
int SV_CheckUserInfo(netadr_t *adr, char *userinfo, qboolean bIsReconnecting, int nReconnectSlot, char *name);
int SV_FindEmptySlot(netadr_t *adr, int *pslot, client_t **ppClient);
void SV_ConnectClient();
void SV_ConnectClient_internal();
void SVC_Ping();
int SV_GetChallenge(const netadr_t &adr);
void SVC_GetChallenge();
void SVC_ServiceChallenge();
void SV_ResetModInfo();
int SV_GetFakeClientCount();
NOXREF qboolean SV_GetModInfo(char *pszInfo, char *pszDL, int *version, int *size, qboolean *svonly, qboolean *cldll, char *pszHLVersion);
NOXREF qboolean RequireValidChallenge(netadr_t *adr);
NOXREF qboolean ValidInfoChallenge(netadr_t *adr, const char *nugget);
NOXREF int GetChallengeNr(netadr_t *adr);
NOXREF qboolean CheckChallengeNr(netadr_t *adr, int nChallengeValue);
NOXREF void ReplyServerChallenge(netadr_t *adr);
NOXREF qboolean ValidChallenge(netadr_t *adr, int challengeNr);
NOXREF void SVC_InfoString();
NOXREF void SVC_Info(qboolean bDetailed);
NOXREF void SVC_PlayerInfo();
NOXREF void SVC_RuleInfo();
int SVC_GameDllQuery(const char *s);
void SV_FlushRedirect();
void SV_EndRedirect();
void SV_BeginRedirect(redirect_t rd, netadr_t *addr);
void SV_ResetRcon_f();
void SV_AddFailedRcon(netadr_t *adr);
qboolean SV_CheckRconFailure(netadr_t *adr);
int SV_Rcon_Validate();
void SV_Rcon(netadr_t *net_from_);
void SV_ConnectionlessPacket();
void SV_CheckRate(client_t *cl);
void SV_ProcessFile(client_t *cl, char *filename);
qboolean SV_FilterPacket();
void SV_SendBan();
void SV_ReadPackets();
// NOBODY int ntohl();
// NOBODY int htons();
void SV_CheckTimeouts();
int SV_CalcPing(client_t *cl);
void SV_SendFullClientUpdateForAll(client_t *client);
void SV_FullClientUpdate(client_t *cl, sizebuf_t *sb);
void SV_EmitEvents(client_t *cl, packet_entities_t *pack, sizebuf_t *msg);
void SV_EmitEvents_internal(client_t *cl, packet_entities_t *pack, sizebuf_t *msg);
void SV_AddToFatPVS(vec_t *org, mnode_t *node);
unsigned char *SV_FatPVS(float *org);
void SV_AddToFatPAS(vec_t *org, mnode_t *node);
unsigned char *SV_FatPAS(float *org);
int SV_PointLeafnum(vec_t *p);
void TRACE_DELTA(char *fmt, ...);
void SV_SetCallback(int num, qboolean remove, qboolean custom, int *numbase, qboolean full, int offset);
void SV_SetNewInfo(int newblindex);
void SV_WriteDeltaHeader(int num, qboolean remove, qboolean custom, int *numbase, qboolean newbl, int newblindex, qboolean full, int offset);
void SV_InvokeCallback();
int SV_FindBestBaseline(int index, entity_state_t **baseline, entity_state_t *to, int num, qboolean custom);
int SV_CreatePacketEntities(sv_delta_t type, client_t *client, packet_entities_t *to, sizebuf_t *msg);
int SV_CreatePacketEntities_internal(sv_delta_t type, client_t *client, packet_entities_t *to, sizebuf_t *msg);
void SV_EmitPacketEntities(client_t *client, packet_entities_t *to, sizebuf_t *msg);
qboolean SV_ShouldUpdatePing(client_t *client);
NOXREF qboolean SV_HasEventsInQueue(client_t *client);
void SV_GetNetInfo(client_t *client, int *ping, int *packet_loss);
int SV_CheckVisibility(edict_t *entity, unsigned char *pset);
void SV_EmitPings(client_t *client, sizebuf_t *msg);
void SV_WriteEntitiesToClient(client_t *client, sizebuf_t *msg);
void SV_CleanupEnts();
qboolean SV_SendClientDatagram(client_t *client);
void SV_UpdateUserInfo(client_t *client);
void SV_UpdateToReliableMessages();
void SV_SkipUpdates();
void SV_SendClientMessages();
void SV_ExtractFromUserinfo(client_t *cl);
int SV_ModelIndex(const char *name);
void SV_AddResource(resourcetype_t type, const char *name, int size, unsigned char flags, int index);
void SV_CreateGenericResources();
void SV_CreateResourceList();
void SV_ClearCaches();
void SV_PropagateCustomizations();
void SV_WriteVoiceCodec(sizebuf_t *pBuf);
void SV_WriteVoiceCodec_internal(sizebuf_t *pBuf);
void SV_CreateBaseline();
void SV_BroadcastCommand(char *fmt, ...);
void SV_BuildReconnect(sizebuf_t *msg);
NOXREF void SV_ReconnectAllClients();
void SetCStrikeFlags();
void SV_ActivateServer(int runPhysics);
void SV_ActivateServer_internal(int runPhysics);
void SV_ServerShutdown();
int SV_SpawnServer(qboolean bIsDemo, char *server, char *startspot);
void SV_LoadEntities();
void SV_ClearEntities();
int RegUserMsg(const char *pszName, int iSize);
qboolean StringToFilter(const char *s, ipfilter_t *f);
USERID_t *SV_StringToUserID(const char *str);

void SV_BanId_f();
void Host_Kick_f();
void SV_RemoveId_f();
void SV_WriteId_f();
void SV_ListId_f();
void SV_AddIP_f();
void SV_RemoveIP_f();
void SV_ListIP_f();
void SV_WriteIP_f();

void SV_KickPlayer(int nPlayerSlot, int nReason);
void SV_InactivateClients();
void SV_FailDownload(const char *filename);
const char *Q_stristr(const char *pStr, const char *pSearch);
qboolean IsSafeFileToDownload(const char *filename);
void SV_BeginFileDownload_f();
void SV_SetMaxclients();
void SV_HandleRconPacket();
void SV_CheckCmdTimes();
void SV_CheckForRcon();
qboolean SV_IsSimulating();
void SV_CheckMapDifferences();
void SV_Frame();
void SV_Drop_f();
void SV_RegisterDelta(char *name, char *loadfile);

void SV_InitDeltas();
void SV_InitEncoders();
void SV_Init();

void SV_Shutdown();

qboolean SV_CompareUserID(USERID_t *id1, USERID_t *id2);
qboolean SV_CompareUserID_internal(USERID_t *id1, USERID_t *id2);

char *SV_GetIDString(USERID_t *id);
char *SV_GetIDString_internal(USERID_t *id);

char *SV_GetClientIDString(client_t *client);

void SV_ClearClientStates();

void SV_DropClient_api(IGameClient *cl, bool crash, const char *fmt, ...);
void SV_DropClient(client_t *cl, qboolean crash, const char *fmt, ...);
void SV_DropClient_internal(client_t *cl, qboolean crash, const char *string);

void SV_ClientPrintf(const char *fmt, ...);
void SV_BroadcastPrintf(const char *fmt, ...);

qboolean Master_IsLanGame();
void Master_Heartbeat_f();