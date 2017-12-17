Name: pgscope
Vendor: Gustavo Junior Alves
Version: 0.0.2
Release: 1%{?dist}
Summary: PL/pgSQL source code tree search and browse tool

Group: Development/Tools
License: BSD-3-Clause
URL: https://github.com/gjalves/pgscope
Source0: pgscope-%{version}.tar.gz
BuildRoot: %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)

BuildRequires: coreutils

%define debug_package %{nil}

%changelog

* Sun Dec 16 2017 Gustavo Junior Alves <gjalves@gjalves.com.br> 0.0.2
- Autocomplete VIM script file

* Sun Dec 10 2017 Gustavo Junior Alves <gjalves@gjalves.com.br> 0.0.1
- Initial release

%install
install -D pgscope ${RPM_BUILD_ROOT}/usr/bin/pgscope
install -D sql.vim ${RPM_BUILD_ROOT}/usr/share/pgscope/sql.vim

%description
PL/pgSQL source code tree search and browse tool

%prep
%setup -q
make

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
/usr/bin/pgscope
/usr/share/pgscope/sql.vim

%post

%postun
