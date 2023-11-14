[ -f apultra/Makefile ] && ! [ -f apultra/Makefile.menu4car ] &&
{
	IFS=''
	while read a; do
	[ "${a##CFLAGS}" != "${a}" ] && echo "${a}" '$(ARCH)' && continue
	[ "${a##LDFLAGS}" != "${a}" ] && echo "${a}" '$(SUF) $(ARCH)' && continue
	[ "${a##OBJDIR}" != "${a}" ] && echo "${a}$(SUF)" && continue
	[ "${a##APP}" != "${a}" ] && echo "${a}$(SUF)" && continue
	[ "${a##LIB}" != "${a}" ] && echo "${a}$(SUF).a" && continue
	#[ "${a##CFLAGS}" != "${a}" -a "${a%%(ARCH)}" != "${a}" ] && echo "'${a}'" '$(ARCH)' && continue
	#[ "${a##LDFLAGS}" != "${a}" -a "${a%%(ARCH)}" != "${a}" ] && echo "'${a}'" '$(ARCH)' && continue
	echo "${a}"
	done < apultra/Makefile >apultra/Makefile.menu4car
}
