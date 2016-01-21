Name:           iotivity-example
Version:        0.0.0
Release:        20170331.114206philippe0+0
License:        Apache-2.0
Summary:        Very minimalist example of IoTivity resource
Url:            http://github.com/TizenTeam/iotivity-example
Group:          System/Libraries
#X-Vc-Url:      http://github.com/TizenTeam/iotivity-example
Group:          Contrib

%if ! 0%{?license:0}
%define license %doc
%endif

Source:         %{name}-%{version}.tar.gz
Source1001:     %{name}.manifest
BuildRequires:  make
BuildRequires:  fdupes
BuildRequires:  pkgconfig(iotivity)
BuildRequires:  boost-devel
BuildRequires:  pkgconfig(dlog)
BuildRequires:  systemd
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

%install_service network.target.wants %{name}.service


%fdupes %{buildroot}

%post
systemctl enable %{name}
systemctl daemon-reload
systemctl start %{name}

%preun
if [ 0 -eq $1 ] ; then
systemctl disable %{name}
systemctl daemon-reload
fi

%files
%defattr(-,root,root)
%license LICENSE
%manifest %{name}.manifest
/opt/%{name}/*
%{_unitdir}/%{name}.service
%{_unitdir}/network.target.wants/%{name}.service
