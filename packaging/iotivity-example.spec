Name:           iotivity-example
Version: %{!?version:0}%{?version}
Release: %{!?release:0}%{?release}
License:        Apache-2.0
Summary:        Very minimalist example of IoTivity resource
Url:            http://git.s-osg.org/iotivity-example/plain/README.md
#X-Vc-Url:      http://git.s-osg.org/iotivity-example
Group:          Contrib

%{!?license: %define license %doc}

Source:         %{name}-%{version}.tar.gz
%if 0%{?manifest:1}
Source1001:     %{name}.manifest
%endif
BuildRequires:  make
BuildRequires:  pkgconfig(iotivity)
BuildRequires:  boost-devel
BuildRequires:  systemd
%if 0%{?tizen:1}
BuildRequires:  pkgconfig(dlog)
BuildRequires:  fdupes
%define PLATFORM PLATFORM=TIZEN
%endif
Requires:  iotivity
Requires(post): systemd
Requires(preun): systemd


%{!?extradir: %define extradir /opt/}

%define EXTRA_MAKE_FLAGS \\\
 %{?_smp_mflags} \\\
 extradir=%{extradir} \\\
 name=%{name} \\\
 DESTDIR=%{buildroot}/ \\\
 %{?PLATFORM}


%description
Mimimal client/server application,
that share an IoTivity resource.

%prep
%setup -q
%if 0%{?manifest:1}
cp %{SOURCE1001} .
%endif

%build


%__make %{EXTRA_MAKE_FLAGS}

%install
%__make install %{EXTRA_MAKE_FLAGS}

%__make rule/systemd/install  %{EXTRA_MAKE_FLAGS}

%if 0%{?install_service:1}
%install_service network.target.wants %{name}.service
%endif

%if 0%{?fdupes:1}
%fdupes %{buildroot}
%endif

%post
%if 0%{?install_service:1}
systemctl enable %{name}
systemctl daemon-reload
systemctl start %{name}
%endif

%preun
%if 0%{?install_service:1}
if [ 0 -eq $1 ] ; then
systemctl disable %{name}
systemctl daemon-reload
fi
%endif

%files
%defattr(-,root,root)
%license LICENSE
%doc *.md
%if 0%{?manifest:1}
%manifest %{name}.manifest
%endif
/opt/%{name}/*
%{_unitdir}/%{name}.service
%if 0%{?install_service:1}
%{_unitdir}/network.target.wants/%{name}.service
%endif
