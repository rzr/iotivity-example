Name:           iotivity-example
Version:        1.1.0
Release:        0
License:        Apache-2.0
Summary:        Very minimalist example of IoTivity resource
Url:            http://github.com/TizenTeam/iotivity-example
Group:          System/Libraries
#X-Vc-Url:      http://github.com/TizenTeam/iotivity-example
Group:          Contrib
Source:         %{name}-%{version}.tar.gz

BuildRequires:  make
BuildRequires:  fdupes
BuildRequires:  iotivity-devel
BuildRequires:  boost-devel
BuildRequires:  pkgconfig(dlog)
BuildRequires:  systemd
Requires:  iotivity


%description
Mimimal client/server application,
that share an IoTivity resource.

%prep
%setup -q

%build

%__make %{?_smp_mflags} \
    PLATFORM=TIZEN \
    config_pkgconfig=1 \
    #eol

%install
%__make install \
    DESTDIR=%{buildroot}/ \
    name=%{name} \
    PLATFORM=TIZEN \
    config_pkgconfig=1 \
    #eol

make %{name}.service

install -d %{buildroot}%{_unitdir}

install extra/iotivity-example.service \
  %{buildroot}%{_unitdir}/%{name}.service

%install_service network.target.wants %{name}.service


%fdupes %{buildroot}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%defattr(-,root,root)
/opt/%{name}/*
%{_unitdir}/%{name}.service
%{_unitdir}/network.target.wants/%{name}.service
