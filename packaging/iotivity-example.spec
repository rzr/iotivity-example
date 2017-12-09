Name:           iotivity-example
Version: %{!?version:0}%{?version}
Release: 20171209.050423philippe0+0
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
%if 0%{?tizen:1}
BuildRequires:  pkgconfig(dlog)
BuildRequires:  fdupes
%define PLATFORM PLATFORM=TIZEN
%endif
Requires:  iotivity

%define EXTRA_MAKE_FLAGS \\\
 %{?_smp_mflags} \\\
 extradir=/opt \\\
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

%if 0%{?fdupes:1}
%fdupes %{buildroot}
%endif

%post

%preun

%files
%defattr(-,root,root)
%license LICENSE
%doc *.md
%if 0%{?manifest:1}
%manifest %{name}.manifest
%endif
/opt/%{name}/*
