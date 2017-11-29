Name:           iotivity-example
Version: %{!?version:0.0.0}%{?version}
Release: %{!?release:0}%{?release}
License:        Apache-2.0
Summary:        Very minimalist example of IoTivity resource
Url:            http://git.s-osg.org/iotivity-example/plain/README.md
Group:          System/Libraries
#X-Vc-Url:      http://git.s-osg.org/iotivity-example
Group:          Contrib

%if ! 0%{?license:0}
%define license %doc
%endif

%{!?license: %define license %doc}

Source:         %{name}-%{version}.tar.gz
%if 0%{?manifest:1}
Source1001:     %{name}.manifest
%endif

BuildRequires:  make
BuildRequires:  pkgconfig(iotivity)
BuildRequires:  boost-devel
%if 0%{?tizen:1}
BuildRequires:  pkgconfig(dlog)
BuildRequires:  fdupes
%define EXTRA_MAKE_FLAGS PLATFORM=TIZEN
%endif
BuildRequires:  systemd
Requires:  iotivity
Requires(post): systemd
Requires(preun): systemd


%description
Mimimal client/server application,
that share an IoTivity resource.

%prep
%setup -q
%if 0%{?manifest:1}
cp %{SOURCE1001} .
%endif
%build


%__make %{?_smp_mflags} \
    name=%{name} \
    %EXTRA_MAKE_FLAGS \
    #eol

%install
%__make install \
    DESTDIR=%{buildroot}/ \
    name=%{name} \
    %EXTRA_MAKE_FLAGS \
    #eol

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
%if 0%{?manifest:1}
%manifest %{name}.manifest
%endif
/opt/%{name}/*
%{_unitdir}/%{name}.service
%if 0%{?install_service:1}
%{_unitdir}/network.target.wants/%{name}.service
%endif
