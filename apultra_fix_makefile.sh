[ -f apultra/Makefile ] && ! [ -f apultra/Makefile.menu4car ] &&
{
	IFS=''
	while read a; do
	[ "${a##CFLAGS}" != "${a}" ] && echo "${a}" '$(ARCH)' && continue
	[ "${a##LDFLAGS}" != "${a}" ] && echo "${a}" '$(ARCH)' && continue
	[ "${a##CFLAGS}" != "${a}" -a "${a%%(ARCH)}" != "${a}" ] && echo "'${a}'" '$(ARCH)' && continue
	[ "${a##LDFLAGS}" != "${a}" -a "${a%%(ARCH)}" != "${a}" ] && echo "'${a}'" '$(ARCH)' && continue
	echo "${a}"
	done < apultra/Makefile >apultra/Makefile.menu4car
}
