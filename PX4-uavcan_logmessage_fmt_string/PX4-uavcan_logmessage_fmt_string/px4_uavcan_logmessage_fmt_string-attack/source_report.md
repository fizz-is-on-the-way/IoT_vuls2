# CVE Candidate Draft: PX4-Autopilot uavcan LogMessage format string

## Title

PX4-Autopilot uavcan externally controlled format string in DroneCAN log messages

## Affected Target / Commit / Config

- Target: PX4-Autopilot (Dronecode Foundation), NuttX build
- Affected refs: `v1.14.0`, `v1.15.0`, `v1.16.0`, `v1.17.0`, `release/1.17`, `v1.18.0-rc1`
- Affected file: `src/drivers/uavcan/logmessage.hpp`, line 88
- Fixed: `main` at `ccf6adae010c7b128ad5ef126c63b6cbe715900e` (pull request #29492)
- Required configuration: `UAVCAN_ENABLE >= 1`; flight-critical when `UAVCAN_ENABLE = 3`

## Vulnerability Summary

`UavcanLogMessage::logmessage_sub_cb` passes the DroneCAN-supplied `msg.text` value as
the printf-style format string of `px4_log_modulename()` and supplies no variadic
arguments. The value is remote, unauthenticated, and free-form, so every conversion
specifier in it consumes caller-saved register or stack residue at the `vsnprintf`
call. `%s` faults the uavcan task; `%p` and `%x` write residue into the console and
the persistent log.

## Root Cause

- Module: `src/drivers/uavcan/logmessage.hpp`
- Function: `UavcanLogMessage::logmessage_sub_cb`
- Root: `px4_log_modulename(px4_level, module_name_buffer, msg.text.c_str())` passes
  attacker text in the format position, with zero variadic arguments
- Sink: `platforms/common/px4_log.cpp`, `va_start(argptr, fmt)` followed by `vsnprintf`
- Fix boundary: pass the text as an argument, `"%s", msg.text.c_str()`

The pull request also removes `-Wno-format-security` from
`src/drivers/uavcan/CMakeLists.txt`, so the module had suppressed the compiler
diagnostic that covers this call, and line 88 was the only site it covered.

## Trigger Construction

An attacker node on the DroneCAN bus broadcasts one
`uavcan.protocol.debug.LogMessage` frame (subject-id 16383) with `level = INFO` and a
text field chosen to either fault the task (`%s` conversions) or format residue into
the log (`%p` / `%x`). The subscriber is started unconditionally at module start, so
no application-level state is needed on the vehicle side.

## Observed Behavior

- `text = "%s%s%s%s"`: the real NuttX `vsnprintf` follows the residue pointer; the
  uavcan task faults. All DroneCAN peripherals are lost; with CAN ESCs configured,
  motor control is lost too.
- `text = "LEAK-%08x.%08x.%08x"`: no fault; residue values appear in the console
  output and in the persistent log history.
- `%n` is not implemented by the NuttX `vsnprintf`, so no write primitive is exposed.

## Evidence Artifacts

- `source_snapshot.hpp`: the vulnerable callback at `v1.17.0` and the fixed line on
  `main`
- `validation_result.json`: per-ref verification of line 88
- `verification_log.txt`: verification record
- `../README.md`: disclosure write-up
- Public fix: https://github.com/PX4/PX4-Autopilot/pull/29492

## CVE Overlap / Dedup Argument

No public CVE or GitHub advisory describes this defect as of 2026-11-11. An NVD API
keyword search for `uavcan` and for `DroneCAN` returned zero results on 2026-11-10. The
PX4 repository has no published advisory for it.

It is also not CVE-2026-86097, which is a null-pointer defect in the parameter
selection code and was assigned through a different CNA. This case is a format-string
defect in the DroneCAN log path, with a different sink, trigger, and fix commit.

## Suggested Mitigation

Apply commit `ccf6adae` (or pull request #29492), or upgrade to `v1.18.0` once released.
Vehicles that do not use DroneCAN can set `UAVCAN_ENABLE = 0`; the log-message
subscriber cannot be disabled independently while DroneCAN is enabled.

## Disclosure Status

- Fix merged publicly on 2026-10-30, credited in the commit message
- No vendor advisory, no CVE ID as of 2026-11-11
- CVE ID request prepared for the MITRE CNA of last resort
- Reproducer withheld from this bundle until the vendor publishes an advisory

## Local Artifacts

- `validation_result.json`, `verification_log.txt`, `source_snapshot.hpp` in this
  directory
- Full disclosure write-up: `../README.md`
