AC_INIT([tigame], [1.0], [])

AC_CANONICAL_TARGET

AC_PROG_CC
AC_PROG_INSTALL

AC_ARG_WITH([build-type],[--with-build-type=<build type>. Allows you to configure the type of build. Defaults to release. Acceptable values are debug, debug-sanitize, and release],
	[
		build_type=$withval
	],
	[
		build_type=release
	])
AC_SUBST([build_type])

AC_ARG_WITH([com-abi],[--with-com-abi=<number>. Allows you to set the TIGAME COM ABI Version. Extensions compiled for one version can only be loaded on that same version safely. This option currently does not have any effect, but may make breaking changes in future versions. It is recommened that you not specify this currently. The current abi version is 0],
[
	tigame_abi=$withval
],
[
	tigame_abi=0
])
AC_SUBST([tigame_abi])


os=${target_os}
AC_SUBST([os])


AC_CONFIG_FILES([Makefile include/tigame/ComAbi.h])



AC_OUTPUT
