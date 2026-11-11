# PX4-Autopilot DroneCAN LogMessage Format-String Vulnerability

## Description

PX4-Autopilot's `uavcan` module forwards the free-form text field of a DroneCAN
`uavcan.protocol.debug.LogMessage` directly into the logging API as its printf-style
format string, with no variadic arguments supplied
(`src/drivers/uavcan/logmessage.hpp:88`).

DroneCAN has no authentication. Any node on the bus can broadcast this message
(subject-id 16383), and the subscriber that handles it is started unconditionally when
the uavcan module runs. The text field is attacker-controlled data sitting in a
position where only a developer-controlled literal belongs.

Every conversion specifier in the supplied text consumes register or stack residue
instead of a real argument. A text containing `%s` makes NuttX's `vsnprintf`
dereference that residue, which faults the uavcan task. A text containing `%p` or `%x`
formats the same residue into the console output and the persistent log history.

NuttX's `vsnprintf` implements no `%n` conversion, so the defect provides no write
primitive. The demonstrated impact is limited to denial of service and information
disclosure.

The fix is one line, merged as pull request #29492 on 2026-10-30: pass the text as an
argument (`"%s", msg.text.c_str()`) instead of passing it as the format itself.

## PX4 Version Information

- Product: PX4-Autopilot (Dronecode Foundation)
- Platform: NuttX, ARM Cortex-M flight controller targets with a CAN stack
- Affected refs: `v1.14.0`, `v1.15.0`, `v1.16.0`, `v1.17.0`, `release/1.17`,
  `v1.18.0-rc1`
- Fixed in: `main` at `ccf6adae010c7b128ad5ef126c63b6cbe715900e`; first fixed release
  expected `v1.18.0`
- Latest published release as of 2026-11-11: `v1.18.0-rc1` (2026-09-10), still
  vulnerable. The tags `v1.18.0` and `v1.17.1` do not exist yet.
- Source: https://github.com/PX4/PX4-Autopilot/blob/v1.17.0/src/drivers/uavcan/logmessage.hpp
- Board coverage: `UavcanLogMessage` is present in 6 of 6 CAN-capable default targets
  checked (`px4_fmu-v3`, `v4pro`, `v5`, `v5x`, `v6c`, `v6x`). `px4_fmu-v2_default` has
  no CAN and is out of scope.

## Vulnerability information

### Root cause

Call chain:

    DroneCAN LogMessage (subject-id 16383)
      -> uavcan::Subscriber<uavcan::protocol::debug::LogMessage>
      -> UavcanLogMessage::logmessage_sub_cb
      -> px4_log_modulename(px4_level, module_name_buffer, msg.text.c_str())
      -> va_start(argptr, fmt) / vsnprintf(...)

- Module: `src/drivers/uavcan/logmessage.hpp`, class `UavcanLogMessage`, function
  `logmessage_sub_cb`, line 88
- Root: `msg.text` is passed as `fmt`; the call supplies zero variadic arguments
- Sink: `platforms/common/px4_log.cpp`, where the format is consumed at
  `va_start(argptr, fmt)` and handed to `vsnprintf`
- Fix boundary: pass the text as data, not as a format

The text field is free-form by DSDL definition, so this is not a caller mistake inside
a private helper. The pull request also removes `-Wno-format-security` from
`src/drivers/uavcan/CMakeLists.txt`, which documents that the module previously
suppressed exactly this diagnostic and that line 88 was the only site it covered.

### Trigger and payload encoding

A vehicle with `UAVCAN_ENABLE >= 1` has the log-message subscriber running. An attacker
node broadcasts `uavcan.protocol.debug.LogMessage` with `level = INFO` and:

- `text = "%s%s%s%s"`: the real NuttX `vsnprintf` dereferences the residue pointer, the
  uavcan task faults, and every DroneCAN peripheral is lost. With `UAVCAN_ENABLE = 3`
  (CAN ESCs) motor control is lost as well.
- `text = "LEAK-%08x.%08x.%08x"`: no fault; the residue values are written into the
  console output and the persistent log history.

Both are one frame, unauthenticated. No pairing, no user interaction, and no
particular vehicle state are required.

## Impact

- Vulnerability type: externally controlled format string (CWE-134)
- Attack type: remote, unauthenticated, one CAN frame
- Impact: deterministic denial of service of the uavcan task; disclosure of register
  and stack residue through the console and the persistent log
- Flight safety: with CAN ESCs configured, losing the uavcan task means losing motor
  control
- Write primitive: none (`%n` is not implemented by the NuttX `vsnprintf`)
- CVSS 3.1: `AV:A/AC:L/PR:N/UI:N/S:U/C:L/I:N/A:H` = 7.1 (High)

## Disclosure status

- Reported through public pull request #29492 and merged on 2026-10-30; the commit
  message credits the reporter as `Reported by @zhangzheng18`
- No vendor advisory and no CVE ID published as of 2026-11-11
- A CVE ID request has been prepared for submission through the MITRE CNA of last
  resort
- No executable reproducer is published in this bundle; it is available to the vendor
  on request

## Files

    README.md
    px4_uavcan_logmessage_fmt_string-attack/
      source_report.md             CVE candidate draft
      selection_summary.json       machine-readable case summary
      validation_result.json       per-ref verification results
      verification_log.txt         verification record
      source_snapshot.hpp          vulnerable and fixed code
