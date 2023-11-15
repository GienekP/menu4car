[ -f apultra/Makefile ] && ! [ -f apultra/Makefile.menu4car ] &&
{
	IFS=''
	while read a; do
	[ "${a##CC=clang}" != "${a}" ] && echo "CC=gcc" && continue
	[ "${a##CFLAGS}" != "${a}" ] && echo "${a}" '$(ARCH)' && continue
	[ "${a##LDFLAGS}" != "${a}" ] && echo "${a}" '$(SUF) $(ARCH)' && continue
	[ "${a##OBJDIR}" != "${a}" ] && echo "${a}$(SUF)" && continue
	[ "${a##APP}" != "${a}" ] && echo "${a}$(SUF)" && continue
	[ "${a##LIB}" != "${a}" ] && [ "${a##LIBOBJS}" == "${a}" ] && t="${a}$(SUF).a" && echo "${t/.a.a/.a}" && continue
	#[ "${a##CFLAGS}" != "${a}" -a "${a%%(ARCH)}" != "${a}" ] && echo "'${a}'" '$(ARCH)' && continue
	#[ "${a##LDFLAGS}" != "${a}" -a "${a%%(ARCH)}" != "${a}" ] && echo "'${a}'" '$(ARCH)' && continue
	echo "${a}"
	done < apultra/Makefile >apultra/Makefile.menu4car
}
