Name:           iotivity-example-switch
Version: %{!?version:0}%{?version}
Release: %{!?release:0}%{?release}
License:        Apache-2.0
Summary:        Very minimalist example of IoTivity resource
Url:            http://git.s-osg.org/iotivity-example/plain/README.md
Group:          System/Libraries
#X-Vc-Url:      http://git.s-osg.org/iotivity-example
Group:          Contrib

%{!?license: %define license %doc}
%{!?manifest: %define manifest %doc}


Source:         %{name}-%{version}.tar.gz
Source1001:     %{name}.manifest
BuildRequires:  make
BuildRequires:  pkgconfig(iotivity)
BuildRequires:  boost-devel
BuildRequires:  systemd
%if 0%{?tizen:1}
BuildRequires:  pkgconfig(dlog)
BuildRequires:  fdupes
%endif

Requires:  iotivity
Requires(post): systemd
Requires(preun): systemd


%description
Mimimal client/server application,
that share an IoTivity resource.

%prep
%setup -q
cp %{SOURCE1001} .

%build

%__make %{?_smp_mflags} \
 PLATFORM=TIZEN \
 #EOL


%install
%__make install \
 DESTDIR=%{buildroot}/ \
 name=%{name} \
 PLATFORM=TIZEN \
 #EOL

%__make rule/systemd/install \
 DESTDIR=%{buildroot}/ \
 name=%{name} \
 PLATFORM=TIZEN \
 #EOL

%if 0%{?install_service:1}
install_service network.target.wants %{name}.service
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
%manifest %{name}.manifest
/opt/%{name}/*
%{_unitdir}/%{name}.service
%if 0%{?install_service:1}
%{_unitdir}/network.target.wants/%{name}.service
%endif
