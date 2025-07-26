#!/usr/bin/env python3

###
# Generates build files for the project.
# This file also includes the project configuration,
# such as compiler flags and the object matching status.
#
# Usage:
#   python3 configure.py
#   ninja
#
# Append --help to see available options.
###

import argparse
import sys
from pathlib import Path
from typing import Any, Dict, List

from tools.project import (
    Object,
    ProgressCategory,
    ProjectConfig,
    calculate_progress,
    generate_build,
    is_windows,
)

# Game versions
DEFAULT_VERSION = 0
VERSIONS = [
    "GAMEID",  # 0
]

parser = argparse.ArgumentParser()
parser.add_argument(
    "mode",
    choices=["configure", "progress"],
    default="configure",
    help="script mode (default: configure)",
    nargs="?",
)
parser.add_argument(
    "-v",
    "--version",
    choices=VERSIONS,
    type=str.upper,
    default=VERSIONS[DEFAULT_VERSION],
    help="version to build",
)
parser.add_argument(
    "--build-dir",
    metavar="DIR",
    type=Path,
    default=Path("build"),
    help="base build directory (default: build)",
)
parser.add_argument(
    "--binutils",
    metavar="BINARY",
    type=Path,
    help="path to binutils (optional)",
)
parser.add_argument(
    "--compilers",
    metavar="DIR",
    type=Path,
    help="path to compilers (optional)",
)
parser.add_argument(
    "--map",
    action="store_true",
    help="generate map file(s)",
)
if not is_windows():
    parser.add_argument(
        "--wrapper",
        metavar="BINARY",
        type=Path,
        help="path to wibo or wine (optional)",
    )
parser.add_argument(
    "--dtk",
    metavar="BINARY | DIR",
    type=Path,
    help="path to decomp-toolkit binary or source (optional)",
)
parser.add_argument(
    "--objdiff",
    metavar="BINARY | DIR",
    type=Path,
    help="path to objdiff-cli binary or source (optional)",
)
parser.add_argument(
    "--sjiswrap",
    metavar="EXE",
    type=Path,
    help="path to sjiswrap.exe (optional)",
)
parser.add_argument(
    "--ninja",
    metavar="BINARY",
    type=Path,
    help="path to ninja binary (optional)"
)
parser.add_argument(
    "--verbose",
    action="store_true",
    help="print verbose output",
)
parser.add_argument(
    "--non-matching",
    dest="non_matching",
    action="store_true",
    help="builds equivalent (but non-matching) or modded objects",
)
parser.add_argument(
    "--warn",
    dest="warn",
    type=str,
    choices=["all", "off", "error"],
    help="how to handle warnings",
)
parser.add_argument(
    "--no-progress",
    dest="progress",
    action="store_false",
    help="disable progress calculation",
)
args = parser.parse_args()

config = ProjectConfig()
config.version = str(args.version)
version_num = VERSIONS.index(config.version)

# Apply arguments
config.build_dir = args.build_dir
config.dtk_path = args.dtk
config.objdiff_path = args.objdiff
config.binutils_path = args.binutils
config.compilers_path = args.compilers
config.generate_map = args.map
config.non_matching = args.non_matching
config.sjiswrap_path = args.sjiswrap
config.ninja_path = args.ninja
config.progress = args.progress
config.progress_modules = False
if not is_windows():
    config.wrapper = args.wrapper
# Don't build asm unless we're --non-matching
if not config.non_matching:
    config.asm_dir = None

# Tool versions
config.binutils_tag = "2.42-1"
config.compilers_tag = "20250520"
config.dtk_tag = "v1.6.1"
config.objdiff_tag = "v3.0.0-beta.8"
config.sjiswrap_tag = "v1.2.1"
config.wibo_tag = "0.6.16"

# Project
config.check_sha_path = Path("config") / config.version / "build.sha1"
config.asflags = [
    "-mgekko",
    "--strip-local-absolute",
    "-I include",
    f"-I build/{config.version}/include",
    f"--defsym BUILD_VERSION={version_num}",
]
config.ldflags = [
    "-fp hardware",
    "-nodefaults",
]
if args.map:
    config.ldflags.append("-mapunused")
    config.ldflags.append("-listclosure")  # For Wii linkers

# Use for any additional files that should cause a re-configure when modified
config.reconfig_deps = []

# Optional numeric ID for decomp.me preset
# Can be overridden in libraries or objects
config.scratch_preset_id = None

# Base flags, common to most GC/Wii games.
# Generally leave untouched, with overrides added below.
cflags_base = [
    "-proc gekko",
    "-fp hardware",
    "-lang c99",
    "-enum int",
    "-cpp_exceptions off",
    "-nosyspath",
    "-cwd include",
    "-i include",
    "-i include/stdlib",
    "-i source",
    "-enc mb",
    "-flag no-cats",
]

# Warning flags
if args.warn == "all":
    cflags_base.append("-W all")
elif args.warn == "off":
    cflags_base.append("-W off")
elif args.warn == "error":
    cflags_base.append("-W error")

cflags_opt_debug = [
    *cflags_base,
    "-opt off",
    "-inline off",
    # only add if you want line info
    # "-g",
]

cflags_opt_release = [
    *cflags_base,
    "-O4,p",
    "-ipa file",
    "-DNDEBUG",
]

config.linker_version = "Wii/1.0"

# Helper function for Revolution libraries
def RvlLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "Wii/1.0",
        "cflags_debug": cflags_opt_debug,
        "cflags_release": cflags_opt_release,
        "objects": objects,
    }

def AncientLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/3.0a5.2",
        "cflags_debug": cflags_opt_debug,
        "cflags_release": cflags_opt_release,
        "objects": objects,
    }

DebugMatching = True                   # Object matches and should be linked
DebugNonMatching = False               # Object does not match and should not be linked
DebugEquivalent = config.non_matching  # Object should be linked when configured with --non-matching

ReleaseMatching = True                   # Object matches and should be linked
ReleaseNonMatching = False               # Object does not match and should not be linked
ReleaseEquivalent = config.non_matching  # Object should be linked when configured with --non-matching


# Object is only matching for specific versions
def MatchingFor(*versions):
    return config.version in versions

config.warn_missing_config = True
config.warn_missing_source = False
config.libs = [
    RvlLib(
        "ai",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "ai/ai.c"),
        ],
    ),
    RvlLib(
        "amcExi",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "amcExi/AmcExi2Stubs.c"),
        ],
    ),
    RvlLib(
        "arc",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "arc/arc.c"),
        ],
    ),
    RvlLib(
        "ax",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "ax/AX.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "ax/AXAlloc.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "ax/AXAux.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "ax/AXCL.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "ax/AXOut.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "ax/AXSPB.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "ax/AXVPB.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "ax/AXProf.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "ax/AXComp.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "ax/DSPCode.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "ax/DSPADPCM.c"),
        ],
    ),
    RvlLib(
        "axart",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "axart/axart.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "axart/axartsound.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "axart/axartcents.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "axart/axartenv.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "axart/axartlfo.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "axart/axart3d.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "axart/axartlpf.c"),
        ],
    ),
    RvlLib(
        "axfx",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "axfx/AXFXReverbHi.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "axfx/AXFXReverbHiDpl2.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "axfx/AXFXReverbHiExp.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "axfx/AXFXReverbHiExpDpl2.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "axfx/AXFXDelay.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "axfx/AXFXDelayDpl2.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "axfx/AXFXDelayExp.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "axfx/AXFXDelayExpDpl2.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "axfx/AXFXReverbStd.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "axfx/AXFXReverbStdDpl2.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "axfx/AXFXReverbStdExp.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "axfx/AXFXReverbStdExpDpl2.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "axfx/AXFXChorus.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "axfx/AXFXChorusDpl2.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "axfx/AXFXChorusExp.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "axfx/AXFXChorusExpDpl2.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "axfx/AXFXLfoTable.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "axfx/AXFXSrcCoef.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "axfx/AXFXHooks.c"),
        ],
    ),
    RvlLib(
        "base",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "base/PPCArch.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "base/PPCPm.c"),
        ],
    ),
    RvlLib(
        "bte",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "bte/gki_buffer.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/gki_debug.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/gki_time.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/gki_ppc.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/hcisu_h2.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/uusb_ppc.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_aa_cfg.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_ag_cfg.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_av_cfg.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_bi_cfg.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_cg_cfg.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_ct_cfg.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_dg_cfg.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_dm_cfg.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_fs_cfg.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_ft_cfg.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_hd_cfg.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_hh_cfg.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_pbc_cfg.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_pbs_cfg.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_pr_cfg.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_ss_cfg.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_sys_cfg.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bte_disp.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bte_hcisu.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bte_init.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bte_load.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bte_logmsg.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bte_main.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bte_version.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/btu_task1.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bd.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_sys_conn.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_sys_main.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/ptim.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/utl.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_prm_api.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_prm_main.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_fs_ci.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_dm_act.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_dm_api.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_dm_main.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_dm_pm.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_hd_act.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_hd_api.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_hd_main.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_hh_act.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_hh_api.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_hh_main.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/bta_hh_utils.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/btm_acl.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/btm_dev.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/btm_devctl.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/btm_discovery.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/btm_inq.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/btm_main.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/btm_pm.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/btm_sco.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/btm_sec.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/btu_hcif.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/btu_init.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/wbt_ext.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/gap_api.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/gap_conn.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/gap_utils.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/goep_trace.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/goep_util.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/goep_fs.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/hcicmds.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/hidd_api.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/hidd_conn.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/hidd_mgmt.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/hidd_pm.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/hidh_api.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/hidh_conn.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/l2c_api.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/l2c_csm.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/l2c_link.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/l2c_main.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/l2c_utils.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/port_api.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/port_rfc.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/port_utils.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/rfc_l2cap_if.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/rfc_mx_fsm.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/rfc_port_fsm.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/rfc_port_if.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/rfc_ts_frames.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/rfc_utils.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/sdp_api.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/sdp_db.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/sdp_discovery.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/sdp_main.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/sdp_server.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/sdp_utils.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/xml_bld.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "bte/xml_parse.c"),
        ],
    ),
    RvlLib(
        "cx",
        [
            Object(DebugMatching, ReleaseMatching, "cx/CXCompression.c"),
            Object(DebugMatching, ReleaseMatching, "cx/CXStreamingUncompression.c"),
            Object(DebugMatching, ReleaseMatching, "cx/CXUncompression.c"),
            Object(DebugMatching, ReleaseMatching, "cx/CXSecureUncompression.c"),
        ],
    ),
    RvlLib(
        "darch",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "darch/darch.c"),
        ],
    ),
    RvlLib(
        "db",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "db/db.c"),
        ],
    ),
    RvlLib(
        "dsp",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "dsp/dsp.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "dsp/dsp_debug.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "dsp/dsp_perf.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "dsp/dsp_task.c"),
        ],
    ),
    RvlLib(
        "dvd",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "dvd/dvdfs.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "dvd/dvd.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "dvd/dvdqueue.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "dvd/dvderror.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "dvd/dvdidutils.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "dvd/dvdFatal.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "dvd/dvdDeviceError.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "dvd/dvd_broadway.c"),
        ],
    ),
    RvlLib(
        "enc",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "enc/encutility.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "enc/encunicode.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "enc/encjapanese.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "enc/enclatin.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "enc/encconvert.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "enc/encchinese.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "enc/enckorean.c"),
        ],
    ),
    RvlLib(
        "esp",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "esp/esp.c"),
        ],
    ),
    RvlLib(
        "euart",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "euart/euart.c"),
        ],
    ),
    RvlLib(
        "exi",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "exi/EXIBios.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "exi/EXIUart.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "exi/EXIAd16.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "exi/EXICommon.c"),
        ],
    ),
    RvlLib(
        "fs",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "fs/fs.c"),
        ],
    ),
    RvlLib(
        "gx",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "gx/GXInit.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "gx/GXFifo.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "gx/GXAttr.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "gx/GXMisc.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "gx/GXGeometry.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "gx/GXFrameBuf.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "gx/GXLight.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "gx/GXTexture.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "gx/GXBump.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "gx/GXTev.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "gx/GXPixel.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "gx/GXDraw.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "gx/GXDisplayList.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "gx/GXVert.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "gx/GXTransform.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "gx/GXVerify.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "gx/GXVerifXF.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "gx/GXVerifRAS.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "gx/GXSave.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "gx/GXPerf.c"),
        ],
    ),
    RvlLib(
        "hid",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "hid/hid_api.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "hid/hid_ios.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "hid/hid_client.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "hid/hid_device.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "hid/hid_interface.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "hid/hid_open_close.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "hid/hid_task.c"),
        ],
    ),
    RvlLib(
        "homebuttonLib",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/HBMFrameController.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/HBMAnmController.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/HBMGUIManager.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/HBMController.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/HBMRemoteSpk.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/HBMAxSound.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/HBMCommon.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/HBMBase.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/lyt_animation.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/lyt_arcResourceAccessor.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/lyt_bounding.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/lyt_common.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/lyt_drawInfo.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/lyt_group.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/lyt_init.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/lyt_layout.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/lyt_material.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/lyt_pane.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/lyt_picture.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/lyt_resourceAccessor.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/lyt_textBox.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/lyt_window.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/math_arithmetic.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/math_equation.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/math_geometry.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/math_triangular.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/math_types.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/ut_binaryFileFormat.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/ut_CharStrmReader.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/ut_CharWriter.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/ut_Font.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/ut_LinkList.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/ut_list.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/ut_ResFont.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/ut_ResFontBase.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/ut_RomFont.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/ut_TagProcessorBase.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/ut_TextWriterBase.cpp"),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/mix.cpp", release_second=True),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/syn.cpp", release_second=True),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/synctrl.cpp", release_second=True),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/synenv.cpp", release_second=True),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/synmix.cpp", release_second=True),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/synpitch.cpp", release_second=True),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/synsample.cpp", release_second=True),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/synvoice.cpp", release_second=True),
            Object(DebugNonMatching, ReleaseNonMatching, "homebuttonLib/seq.cpp", release_second=True),
        ],
    ),
    RvlLib(
        "ipc",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "ipc/ipcMain.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "ipc/ipcclt.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "ipc/memory.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "ipc/ipcProfile.c"),
        ],
    ),
    RvlLib(
        "kbd",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "kbd/kbd_lib.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "kbd/kbd_lib_led.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "kbd/kbd_lib_init.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "kbd/kbd_lib_maps_us.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "kbd/kbd_lib_maps_jp.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "kbd/kbd_lib_maps_eu.c"),
        ],
    ),
    RvlLib(
        "kpad",
        [
            Object(DebugMatching, ReleaseMatching, "kpad/KPAD.c"),
            Object(DebugMatching, ReleaseMatching, "kpad/KMPLS.c"),
            Object(DebugMatching, ReleaseMatching, "kpad/KZMplsTestSub.c"),
        ],
    ),
    RvlLib(
        "kpr",
        [
            Object(DebugMatching, ReleaseMatching, "kpr/kpr_lib.c", shift_jis = False),
        ],
    ),
    RvlLib(
        "mem",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "mem/mem_heapCommon.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "mem/mem_expHeap.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "mem/mem_frameHeap.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "mem/mem_unitHeap.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "mem/mem_allocator.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "mem/mem_list.c"),
        ],
    ),
    RvlLib(
        "midi",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "midi/MIDI.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "midi/MIDIRead.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "midi/MIDIXfer.c"),
        ],
    ),
    RvlLib(
        "mix",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "mix/mix.c", debug_second=True),
            Object(DebugNonMatching, ReleaseNonMatching, "mix/remote.c"),
        ],
    ),
    RvlLib(
        "mtx",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "mtx/mtx.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "mtx/mtxvec.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "mtx/mtxstack.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "mtx/mtx44.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "mtx/mtx44vec.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "mtx/vec.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "mtx/quat.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "mtx/psmtx.c"),
        ],
    ),
    RvlLib(
        "nand",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "nand/nand.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "nand/NANDOpenClose.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "nand/NANDCore.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "nand/NANDSecret.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "nand/NANDCheck.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "nand/NANDLogging.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "nand/NANDErrorMessage.c"),
        ],
    ),
    RvlLib(
        "NdevExi2A",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "NdevExi2A/DebuggerDriver.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "NdevExi2A/exi2.c"),
        ],
    ),
    RvlLib(
        "odenotstub",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "odenotstub/odenotstub.c"),
        ],
    ),
    RvlLib(
        "os",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "os/OS.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSAddress.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSAlarm.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSAlloc.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSArena.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSAudioSystem.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSCache.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSContext.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSError.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSExec.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSFatal.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSFont.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSInterrupt.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSLink.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSMessage.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSMemory.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSMutex.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSReboot.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSReset.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSRtc.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSSemaphore.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSStopwatch.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSSync.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSThread.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSTime.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSTimer.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSUtf.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSIpc.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSStateTM.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/__start.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSPlayRecord.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSStateFlags.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSNet.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSNandbootInfo.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSPlayTime.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSInstall.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSCrc.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/OSLaunch.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "os/__ppc_eabi_init.c"),
        ],
    ),
    RvlLib(
        "pad",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "pad/Padclamp.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "pad/Pad.c"),
        ],
    ),
    RvlLib(
        "rso",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "rso/RSOLink.c"),
        ],
    ),
    RvlLib(
        "sc",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "sc/scsystem.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "sc/scapi.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "sc/scapi_net.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "sc/scapi_prdinfo.c"),
        ],
    ),
    RvlLib(
        "seq",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "seq/seq.c", debug_second=True),
        ],
    ),
    RvlLib(
        "si",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "si/SIBios.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "si/SISamplingRate.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "si/SISteering.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "si/SISteeringXfer.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "si/SISteeringAuto.c"),
        ],
    ),
    RvlLib(
        "sp",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "sp/sp.c"),
        ],
    ),
    RvlLib(
        "syn",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "syn/syn.c", debug_second=True),
            Object(DebugNonMatching, ReleaseNonMatching, "syn/synctrl.c", debug_second=True),
            Object(DebugNonMatching, ReleaseNonMatching, "syn/synenv.c", debug_second=True),
            Object(DebugNonMatching, ReleaseNonMatching, "syn/synlfo.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "syn/synmix.c", debug_second=True),
            Object(DebugNonMatching, ReleaseNonMatching, "syn/synpitch.c", debug_second=True),
            Object(DebugNonMatching, ReleaseNonMatching, "syn/synsample.c", debug_second=True),
            Object(DebugNonMatching, ReleaseNonMatching, "syn/synvoice.c", debug_second=True),
            Object(DebugNonMatching, ReleaseNonMatching, "syn/synwt.c"),
        ],
    ),
    RvlLib(
        "thp",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "thp/THPDec.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "thp/THPStats.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "thp/THPAudio.c"),
        ],
    ),
    RvlLib(
        "tpl",
        [
            Object(DebugMatching, ReleaseMatching, "tpl/TPL.c"),
        ],
    ),
    RvlLib(
        "usb",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "usb/usb.c"),
        ],
    ),
    RvlLib(
        "vi",
        [
            Object(DebugNonMatching, ReleaseNonMatching, "vi/vi.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "vi/i2c.c"),
            Object(DebugNonMatching, ReleaseNonMatching, "vi/vi3in1.c"),
        ],
    ),
    AncientLib(
        "wenc",
        [
            Object(DebugMatching, ReleaseMatching, "wenc/wenc.c"),
        ],
    ),
    RvlLib(
        "wpad",
        [
            Object(DebugMatching, ReleaseMatching, "wpad/WPAD.c"),
            Object(DebugMatching, ReleaseMatching, "wpad/WPADHIDParser.c"),
            Object(DebugMatching, ReleaseMatching, "wpad/WPADEncrypt.c"),
            Object(DebugMatching, ReleaseMatching, "wpad/WPADClamp.c"),
            Object(DebugMatching, ReleaseMatching, "wpad/WPADMem.c"),
            Object(DebugMatching, ReleaseNonMatching, "wpad/lint.c"),
            Object(DebugMatching, ReleaseMatching, "wpad/WUD.c"),
            Object(DebugMatching, ReleaseMatching, "wpad/WUDHidHost.c"),
        ],
    ),
]


# Optional callback to adjust link order. This can be used to add, remove, or reorder objects.
# This is called once per module, with the module ID and the current link order.
#
# For example, this adds "dummy.c" to the end of the DOL link order if configured with --non-matching.
# "dummy.c" *must* be configured as a Matching (or Equivalent) object in order to be linked.
def link_order_callback(module_id: int, objects: List[str]) -> List[str]:
    # Don't modify the link order for matching builds
    if not config.non_matching:
        return objects
    if module_id == 0:  # DOL
        return objects + ["dummy.c"]
    return objects

# Uncomment to enable the link order callback.
# config.link_order_callback = link_order_callback


# Optional extra categories for progress tracking
# Adjust as desired for your project
config.progress_categories = [
    ProgressCategory("release", "Release"),
    ProgressCategory("debug", "Debug"),
]
config.progress_each_module = args.verbose
# Optional extra arguments to `objdiff-cli report generate`
config.progress_report_args = [
    # Marks relocations as mismatching if the target value is different
    # Default is "functionRelocDiffs=none", which is most lenient
    "--config functionRelocDiffs=data_value",
]

if args.mode == "configure":
    # Write build.ninja and objdiff.json
    generate_build(config)
elif args.mode == "progress":
    # Print progress information
    calculate_progress(config)
else:
    sys.exit("Unknown mode: " + args.mode)
