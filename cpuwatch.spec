Name: cpuwatch
Version: 1.1
Release: 1%{?dist}
Summary: Dynamic user niceing service

License: MIT
Source0: %{name}-%{version}.tar.gz
BuildRoot: %(mktemp -ud %{_tmppath}/%{name}-%{version}-XXXXXX)

BuildRequires: make

%description
cpuwatch allows a multi-user system to more fairly share its resources by dynamically applying a NICE value to all users based on the amount of CPU they are attempting to access. 

%global debug_package %{nil}

%prep
%setup -q

%build
make all

%install
install --directory $RPM_BUILD_ROOT/usr/share/doc/cpuwatch
install --directory $RPM_BUILD_ROOT/usr/sbin
install --directory $RPM_BUILD_ROOT/usr/lib/systemd/system

install -m 644 LICENSE $RPM_BUILD_ROOT/usr/share/doc/cpuwatch
install -m 644 README.md $RPM_BUILD_ROOT/usr/share/doc/cpuwatch
install -m 755 cpuwatch $RPM_BUILD_ROOT/usr/sbin
install -m 644 cpuwatch.service $RPM_BUILD_ROOT/usr/lib/systemd/system

%clean
rm -rf $RPM_BUILD_ROOT

%files
/usr/share/doc/cpuwatch/LICENSE
/usr/share/doc/cpuwatch/README.md
/usr/sbin/cpuwatch
/usr/lib/systemd/system/cpuwatch.service

%changelog
