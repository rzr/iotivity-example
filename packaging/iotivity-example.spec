Name:           iotivity-example
Version:        0.9.0
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
BuildRequires:  mraa-devel
BuildRequires:  boost-devel


%description
Mimimal client/server application
that share a single gpio output as IoTivity resource.

%prep
%setup -q

%build

%__make %{?_smp_mflags} \
    PLATFORM=TIZEN \
    #eol

%install
%__make install \
    DESTDIR=%{buildroot}/opt/%{name}/ \
    PLATFORM=TIZEN \
    #eol

%fdupes %{buildroot}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%defattr(-,root,root)
/opt/%{name}/*
