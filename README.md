# Overview

`cpuwatch` allows a multi-user system to more fairly share its resources, by dynamically applying a NICE value to all users based on the amount of CPU they are attempting to access. It is installed as a `systemd` service.

This is envisioned to be a superior system to placing all users into their own `cgroup`. Using a `cgroup` prevents a user from accessing more than a specified number of CPU cores, however many users each using multiple cores will eventually bog down the system. In addition, with small numbers of users and a many-core system it is possible that multiple cores will go unused when there is legitimate work to be done.

`cpuwatch` solves both of these problems by allowing all cores to be used at all times, but prioritising light users over heavy users through the standard system NICE settings. This gives every user the experience of a system which has only the load they are running - light users experience a responsive system, whereas heavy users get a slugish experience.

`cpuwatch` is released under an MIT license (see the LICENSE file for details) and was written by Duncan Tooke.

# Requirements

- `cpuwatch` is written for Linux, and relies on the `/proc` filesystem to function
- Compiling the code requires a C++ compiler with c++-20 support

# Building

- Ensure that you have both `rpm-build` and `rpmdevtools` installed
- If you haven't done so already, run `rpmdev-setuptree` 
- Download the tarball for one of the tagged releases; the file should have a name similar to `cpuwatch-1.0.tar.gz`
- Build the source and binary files with `rpmbuild -ta TARFILE`, substituting `TARFILE` for the path to the download
- Install the RPM found in `~/rpmbuild/RPMS/x86_64/` 

Alternatively, untar the source and run `make`, then manually install the files by hand.

# Files Installed

- **/usr/share/doc/cpuwatch/LICENSE**: the license for the software
- **/usr/share/doc/cpuwatch/README.md**: this document
- **/usr/sbin/cpuwatch**: the main `cpuwatch` binary
- **/usr/lib/systemd/service/cpuwatch.service**: the `systemd` service file

# How To Use

`cpuwatch` is a standard `systemd` service and can also be run manually for testing. It has no dynamically configurable options.

# Notes On The Design

- `cpuwatch` works by periodically calculating the total amount of CPU each user is consuming; this is done by comparing the last known CPU usage with the current CPU usage and finding the delta, in the same manner as `top` (but per user rather than per process)
- Once the delta (in %CPU) is known, this is converted into a NICE value 
- NICE = (delta / 100 / SAMPLE_RATE_SECONDS) * SCALING_FACTOR
- The scaling factor is currently set to 2; this allows a greater spread of NICE values so that users are more finely partitioned
- The NICE is then capped at 18, giving a final value between 0 and 18; if this is different to the user's current NICE value, they are reniced
- A NICE value of 19 is reserved; if `cpuwatch` finds that a user has a current NICE of 19 it will not change it, allowing a system administator to pin specific users on the highest possible value
- By default, the SAMPLE_RATE_SECONDS is set to 5; this is so that the maximum delay between a user logging in and the new NICE being applied is 10 seconds (up to 5 seconds for a first reading and 5 more before a delta can be calculated)
- All processes belonging to UIDs at or below MAX_SYSTEM_UID are ignored; this is set to 999 by default based on the guidance for Red Hat Enterprise Linux

# Contributing to cpuwatch

We'd be open to receiving contributions for any of the following:

- Bug fixes or simplicity/reliability/security improvements
- Packaging the code in non-RPM formats, e.g. DEB
- Improvements to default values